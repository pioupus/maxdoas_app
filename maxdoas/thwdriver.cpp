#include "thwdriver.h"
#include "crc8.h"
#include <QString>
#include <QTime>
#include <QWaitCondition>
#include <stdint.h>
#include <qmath.h>
#include <JString.h>
#include <QTest>

#define P0	2
#define P1	3
#define P2	4
#define P3	5
#define P4	6
#define P5	7
#define P6	8

#define TILT_MAX_8G                 14.83
#define TILT_MAX_4G                 30.79

#define CMD_TEMP_FIND_SENSORS		0xE3
#define CMD_TEMP_ASSIGN_SENSORS		0xE4
#define CMD_TEMP_READ_SENSORID		0xE5
#define CMD_TEMP_READ_SENSOR 		0xE6
#define CMD_TEMP_WRITE_SENSOR_RES	0xE7

#define CMD_TEMP_WRITE_SHOULD_TEMP	0xE8

#define CMD_MOT_SETACCELERATION_VEL	0x14
#define CMD_MOT_GETPOSITION 		0x20
#define CMD_MOT_CALIBRATION 		0x22
#define CMD_MOT_RAMP		 	0x23
#define CMD_MOT_IDLE_STATE 		0x50
#define CMD_MOT_GETACCELERATION_VEL	0x52
#define CMD_MOT_RESET			0xCC
#define CMD_MOT_STOP			0xA5

#define CMD_MOT_CLOSESHUTTER		0x60
#define CMD_MOT_OPENSHUTTER		0x61

#define CMD_MOT_HEATERPWM		0x68

#define CMD_MOT_INCLIN_SET_INIT		0x80
#define CMD_MOT_INCLIN_READ		0x81

#define CMD_MOT_COMPASS_READ		0x88
#define CMD_MOT_COMPASS_CAL_ENTER	0x89
#define CMD_MOT_COMPASS_CAL_EXIT	0x8A
#define CMD_MOT_COMPASS_SET_AVG		0x8B

#define CMD_MOT_PER_UART_TXRX		0x90

#define CMD_MOT_LIGHT_INIT		0x98
#define CMD_MOT_LIGHT_READ		0x99

#define CMD_TRNSMIT_OK			0xAA
#define CMD_TRNSMIT_FAIL		0x55

#define OMNI_ENABLED 1
const uint TimeOutData  =  3000; //ms
const uint TimeOutMotion  =  5000; //ms



static int sensorIDtoInt(THWTempSensorID id){
    switch(id){
    case tsPeltier:         return 0;
    case tsHeatSink:        return 1;
    case tsSpectrometer:    return 2;
    }
    return 0;
}

THWDriverThread::THWDriverThread()
{
    QMetaObject::invokeMethod(this,
                                         "init",
                                         Qt::QueuedConnection);//a "hack" for making all objects beeing in this thread
}

void THWDriverThread::init(){
    serial = new AbstractSerial(this);


    TiltADC_Steps = 1<<18;  //lets calculate with 12 bits;
    TiltADC_Gain = 8;  //1,2,4,8 Gains available
    TiltADC_RefVoltage = 2.048;
//    TiltCalValNegGX = -1;
//    TiltCalValPosGX = 1;
//    TiltCalValNegGY = -1;
//    TiltCalValPosGY = 1;

    //HeadingOffset = 0;
    mc = new TMirrorCoordinate();
    LastShutterCMD = scNone;
    wrapper = new Wrapper();
    NumberOfSpectrometers = 0;
    SpectrometerIndex = -1;
    NumOfPixels = 0;
    MutexSpectrBuffer.lockForWrite();
    {
        MutexMinIntegrationTime.lockForWrite();
        {
        SpectrMinIntegTime = -1;
        }
        MutexMinIntegrationTime.unlock();
        SpectrMaxIntensity = -1;
    }
    MutexSpectrBuffer.unlock();
    SpectrometerList = new QList<QString>;
    IntegTimeConf.autoenabled = false;
    IntegTimeConf.fixedIntegtime = 10000;
    SpectrometerSerial = "closed";
    #if !OMNI_ENABLED
        qsrand(time(NULL));
    #endif
}

void THWDriverThread::newOmniWrapper(){

}

void THWDriverThread::CloseEverythingForLeaving(){
    #if OMNI_ENABLED
    if (wrapper)
        wrapper->closeAllSpectrometers();
    delete wrapper;
    wrapper = NULL;
    #endif
    delete SpectrometerList;
    SpectrometerList = NULL;
    if (serial)
        serial->close();
    delete serial;
    serial = NULL;
    thread()->quit();
}

THWDriverThread::~THWDriverThread(){
    //hopefully stop() was called to close the spectrometer
    //CloseEverythingForLeaving();
}

void THWDriverThread::hwdtSloSetComPort(QString name)
{
    hwdtSloSerOpenClose(false);
    if(!name.isEmpty()){
        if (serial)
            serial->setDeviceName(name);
        hwdtSloSerOpenClose(true);
    }
}


void THWDriverThread::hwdtSloSetBaud(AbstractSerial::BaudRate baud)
{
    if (serial) {
        if (!serial->setBaudRate(baud)) {

            QString str;
            QTextStream(&str) << "Set baud rate " << baud << " error.";
            logger()->error( str );
        };
    }
}

void THWDriverThread::hwdtSloSerOpenClose(bool open){
    bool err = false;
    if (serial){

        if (open){
            /*
            Here using the open flag "Unbuffered".
            This flag disables the internal buffer class,
            and also disables the automatic data acquisition (disables asynchronous mode).
            In this case, we have disabled the asynchronous mode to read from port data using timeouts on the packet.
            Ie if we call, for example, read(5) and in buffer UART
            not yet available - then the method will wait for a time (total read timeout and/or char interval timeout) until the data arrive.

            Note: Behavior would be different if you open a port without a flag "Unbuffered".
                  I will not describe it - test/check it yourself. ;)
        */
            if ( !serial->open(AbstractSerial::ReadWrite | AbstractSerial::Unbuffered) ) {
                QString str;
                QTextStream(&str) << "Serial device: " << serial->deviceName() << " open fail.";
                logger()->error(str);
                err = true;
            }
            if (!serial->setBaudRate(AbstractSerial::BaudRate19200)) {
                QString str;
                QTextStream(&str) << "Set baud rate " << AbstractSerial::BaudRate19200 << " error.";
                logger()->error( str );
                err = true;
            };
            if (!serial->setDataBits(AbstractSerial::DataBits8)) {
                QString str;
                QTextStream(&str) << "Set data bits " <<  AbstractSerial::DataBits8 << " error.";
                logger()->error( str );
                err = true;
            }
            if (!serial->setParity(AbstractSerial::ParityNone)) {
                QString str;
                QTextStream(&str) << "Set parity " <<  AbstractSerial::ParityNone << " error.";
                logger()->error( str );
                err = true;
            }
            if (!serial->setStopBits(AbstractSerial::StopBits1)) {
                QString str;
                QTextStream(&str) << "Set stop bits " <<  AbstractSerial::StopBits1 << " error.";
                logger()->error( str );
                err = true;
            }
            if (!serial->setFlowControl(AbstractSerial::FlowControlOff)) {
                QString str;
                QTextStream(&str) << "Set flow " <<  AbstractSerial::FlowControlOff << " error.";
                logger()->error( str );
                err = true;
            }
            if (! err){
                QString str;
                QTextStream(&str) << "Serial device " << serial->deviceName() << " opened with " << serial->baudRate();
                logger()->info(str);

            }

        }else{
            serial->close();
            QString str;
            QTextStream(&str) << "Serial device: " << serial->deviceName() << " closed.";
            logger()->info(str);
        }
    }
    emit hwdtSigCOMPortChanged(serial->deviceName(),(open && !err),err);
}

QString THWDriverThread::DoRS485Direction(bool TempCtrler, uint size){
    QString trace;
    QTime rtstimer;
    if (TempCtrler){//lets keep the direction pin low -> controller will toggle direction
       serial->setRts(true);
       trace = "RTS cleared; ";
    }else{//for motion controll we have to controll direction pin
        serial->setRts(true);
        char activateDirection = 0xF0 | size;
        serial->write(&activateDirection,1);
        rtstimer.start();
        while ((serial->bytesAvailable() == 0) && (rtstimer.elapsed() <= 500))  {
            QTest::qSleep(1);
        }
        if(serial->bytesAvailable() > 0){
            activateDirection = 0;
            serial->read(&activateDirection, 1);
            if ((uint8_t)activateDirection == (0xF0 | size))
                trace = "Direction set(ok); ";
            else
                trace = "Direction set(fail); ";
        }else{
             trace = "Direction set(to); ";
        }
        serial->flush();
        //serial->setRts(false);//for sending

    }
    //            rtstimer.start();
    //            while(rtstimer.elapsed() <= 2){//lets be sure that send buffer sent completly
    //                QTest::qSleep(1);
    //            }
    return trace;
}

bool THWDriverThread::sendBuffer(char *TXBuffer,char *RXBuffer,uint size,uint timeout , bool TempCtrler, bool RetransmitAllowed){
    QString trace;
    QTime rtstimer;
    if (serial){
        if (serial->isOpen()){
           // serial->setRts(false);//for sending
            TXBuffer[size-1] = crc8(TXBuffer,size-1);
            trace = DoRS485Direction(TempCtrler,size);
            serial->write(TXBuffer,size);
            if (logger()->isTraceEnabled()){
                for (uint i=0; i< size;i++){
                    QString t;
                    t.sprintf("[0x%02X], ",(unsigned char )TXBuffer[i]);
                    trace = trace + t;
                }
                trace = "SentTX Buffer: " + trace;
                logger()->trace(trace);
            }
            return waitForAnswer(TXBuffer,RXBuffer,size,timeout,TempCtrler,RetransmitAllowed);
        }else{
            logger()->error("Try to use COM-Port, but it's closed");
            return false;
        }
    }
    logger()->error("HWClass not initialized(serial == NULL)");
    return false;
}

bool THWDriverThread::waitForAnswer(char *TXBuffer,char *RXBuffer,uint size,int timeout , bool TempCtrler, bool RetransmitAllowed){
    QTime timer,rtstimer;
    bool TransmissionOK = false;
    bool TransmissionError = false;
    uint RetransmissionCounter = 0;
    if (serial){
        uint BufferIndex = 0;
//        if (!TempCtrler){//for motion controller we have to controll dir pin
//            rtstimer.start();
//            while(rtstimer.elapsed() <= 10){//lets be sure that send buffer sent completly
//                 QTest::qSleep(5);
//            }
//            serial->setRts(true);//for receiving
//            logger()->trace("RTS cleared");
//        }
        timer.start();
        do{
            timer.restart();
            while((timer.elapsed() <= 50) && (!TransmissionOK)){
                QTest::qSleep(5);

                if (serial->bytesAvailable() > 0)  {
                    uint8_t rxbyte;
                    serial->read(&RXBuffer[0], 1);
                    rxbyte = RXBuffer[0];
                    if ((unsigned char)rxbyte == 0xF0+size){
                        logger()->warn(QString("DirectCMDEcho Received"));
                        serial->read(&RXBuffer[0], 1);
                        rxbyte = RXBuffer[0];
                    }
                    if ((unsigned char)rxbyte == CMD_TRNSMIT_OK){
                        TransmissionOK = true;
                    }else{
                        emit hwdtSigTransferDone(tsCheckSumError,RetransmissionCounter);
                        logger()->warn(QString("Transmission: Checksum error! tries: %1 ").arg(RetransmissionCounter));
                        break;
                    }
                }
            }
            if (!RetransmitAllowed){
                break;
            }
            if (!TransmissionOK){

                while (timer.elapsed() <= 50){
                    QTest::qSleep(5);
                }
                    //emit hwdtSigTransferDone(tsTimeOut,RetransmissionCounter);
                    logger()->warn(QString("Transmission: Timeout error! tries: %1 ").arg(RetransmissionCounter));

             //   timer.restart();
                //lets retransmit
                logger()->trace(DoRS485Direction(TempCtrler,size));
                serial->write(TXBuffer,size);
//                if (!TempCtrler){//for motion controller we have to controll dir pin
//                    rtstimer.start();
//                    while(rtstimer.elapsed() <= 10){//lets be sure that send buffer sent completly
//                        QTest::qSleep(5);
//                    }
//                    serial->setRts(true);//for receiving
//                    logger()->trace("RTS cleared");
//                }
                RetransmissionCounter++;
                QTest::qSleep(5);
            }

            if (RetransmissionCounter > 10){
                TransmissionError = true;
                logger()->error(QString("Transmission error tries: %1").arg(RetransmissionCounter));
            }

        }while(!TransmissionOK && !TransmissionError);

        if (TransmissionOK){
            timer.restart();
            while((timer.elapsed() <= timeout) && (BufferIndex < size)){
                QTest::qSleep(1);
                if (serial->bytesAvailable() > 0)  {
                    serial->read(&RXBuffer[BufferIndex], 1);
                    BufferIndex++;
                }
            }
            TransmissionOK = BufferIndex == size;
            if((!TransmissionOK) && (timer.elapsed() > timeout)){
                emit hwdtSigTransferDone(tsTimeOut,HW_TRANSMISSION_TIMEOUT_FINAL_PARAMETER);
                logger()->error("Transmission error Timeout or answer too small ");
            }
        }
        if (logger()->isTraceEnabled()){
            QString trace;
            for (uint i=0; i< BufferIndex;i++){
                QString t;
                t.sprintf("[0x%02X], ",(unsigned char)RXBuffer[i]);
                trace = trace + t;
            }
            trace = "Received Buffer: " + trace;
            logger()->trace(trace);
        }
        if (TransmissionOK){
            emit hwdtSigTransferDone(tsOK,RetransmissionCounter);
            logger()->debug("Transmission done and answer rxed");
        }
        serial->flush();
        return TransmissionOK;
    }
    logger()->error("HWClass not initialized(serial == NULL)");
    return false;
}



float THWDriverThread::sensorTempToCelsius(short int Temperature){
    return Temperature*0.0625;
}

short int THWDriverThread::CelsiusToSensorTemp(float Temperature){
    return round(Temperature/0.0625);
}

void THWDriverThread::hwdtSloAskTemperature(THWTempSensorID sensorID, bool byTimer)
{
#if 1
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    logger()->debug(QString("Asking for Temperature. SensorID: %1").arg(sensorIDtoInt(sensorID)));
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_TEMP_READ_SENSOR;
    txBuffer[P1] =  sensorIDtoInt(sensorID);
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,true,false)){
        int16_t siTemperature;
        uint16_t usiTemperature;
        float TemperaturePeltier,TemperatureSpectr,TemperatureHeatsink;
        usiTemperature = 0;
        usiTemperature |= (uint8_t)rxBuffer[P0];
        usiTemperature |= (uint8_t)rxBuffer[P1]<<8;
        siTemperature = usiTemperature;
        TemperaturePeltier =  sensorTempToCelsius(siTemperature);
        usiTemperature = 0;
        usiTemperature |= (uint8_t)rxBuffer[P2];
        usiTemperature |= (uint8_t)rxBuffer[P3]<<8;
        siTemperature = usiTemperature;
        TemperatureHeatsink =  sensorTempToCelsius(siTemperature);
        usiTemperature = 0;
        usiTemperature |= (uint8_t)rxBuffer[P4];
        usiTemperature |= (uint8_t)rxBuffer[P5]<<8;
        siTemperature = usiTemperature;
        TemperatureSpectr =  sensorTempToCelsius(siTemperature);
        logger()->debug(QString("Temperature of Sensor Peltier: %1C, Spectr: %2C, Heatsink: %3C").arg(TemperaturePeltier).arg(TemperatureSpectr).arg(TemperatureHeatsink));
        emit hwdtSigGotTemperature(sensorID, TemperaturePeltier,TemperatureSpectr,TemperatureHeatsink,byTimer);
    }
#else
    emit hwdtSigGotTemperature(sensorID, 0,0,0,byTimer);
#endif

}

void THWDriverThread::hwdtSloSetTargetTemperature(float temperature)
{
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    logger()->debug(QString("Setting target temperature. Temperature: %1 C").arg(temperature));
    int16_t siTemperature = CelsiusToSensorTemp(temperature);
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_TEMP_WRITE_SHOULD_TEMP;
    txBuffer[P1] = (siTemperature >> 8) & 0xFF;
    txBuffer[P2] = siTemperature & 0xFF;
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,true,true)){

    }
}

static int TiltResolutionToConfBit(TTiltConfigRes Resolution){
    switch(Resolution){
    case tcr12Bit:  return 0;
    case tcr14Bit:  return 1;
    case tcr16Bit:  return 2;
    case tcr18Bit:  return 3;
    }
    return 0;
}

static int TiltGainToConfBit(TTiltConfigGain Gain){
    switch(Gain){
    case tcGain1:  return 0;
    case tcGain2:  return 1;
    case tcGain4:  return 2;
    case tcGain8:  return 3;
    }
    return 0;
}

//void THWDriverThread::hwdtSloOffsetTilt(float TiltX, float TiltY){
//    TiltX = sin(TiltX);
//    TiltY = sin(TiltY);
//    TiltCalValPosGX = 1 + TiltX;
//    TiltCalValNegGX = -1 + TiltX;
//    TiltCalValPosGY = 1 + TiltY;
//    TiltCalValNegGY = -1 + TiltY;
//}


void THWDriverThread::hwdtSloConfigTilt(TTiltConfigRes Resolution,TTiltConfigGain Gain){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    Resolution = tcr18Bit;
    Gain = tcGain8;
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_INCLIN_SET_INIT;
    txBuffer[P1] = TiltGainToConfBit(Gain);//gain
    txBuffer[P2] = TiltResolutionToConfBit(Resolution);//resolution
    logger()->debug(QString("Configuring Tilt Sensor. Resolution: %1 bits, Gain: %2").arg((int)Resolution).arg((int)Gain));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){
        TiltADC_Gain = (int) Gain;
        TiltADC_Steps = 1 << (int) Resolution;
    }
}

QPoint THWDriverThread::hwdtGetLastRawTilt(){
    QPoint result;
    MutexRawTiltPoint.lockForRead();
    {
        result = TiltRaw;
    }MutexRawTiltPoint.unlock();
    return result;
}

void THWDriverThread::hwdtSloSetTiltOffset(int x, int y){
    TiltRawOffset.setX(x);
    TiltRawOffset.setY(y);
}

void THWDriverThread::hwdtSloAskTilt()
{
#if 1
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_INCLIN_READ;
    logger()->debug("Asking Tilt Sensor");
    logger()->trace(QString("Tilt sensor Resolution: %1 steps, Gain: %2").arg((int)TiltADC_Steps).arg((int)TiltADC_Gain));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,false)){
        float TiltX,TiltY;
      //  float XCalOffset,YCalOffset;
      //  float XCalGain,YCalGain;
        int32_t liTiltX,liTiltY;
        uint32_t uliTiltX,uliTiltY;
        //canal 0
        uliTiltX = 0;
        uliTiltX |= (uint8_t)rxBuffer[P1];
        uliTiltX |= (uint8_t)rxBuffer[P2] << 8;
        uliTiltX |= (uint8_t)rxBuffer[P3] << 16;
        if(uliTiltX & 0x00800000)//sign bit set
            uliTiltX |= 0xFF000000;
        //canal 1
        uliTiltY = 0;
        uliTiltY |= (uint8_t)rxBuffer[P4];
        uliTiltY |= (uint8_t)rxBuffer[P5] << 8;
        uliTiltY |= (uint8_t)rxBuffer[P6] << 16;
        if(uliTiltY & 0x00800000)//sign bit set
            uliTiltY |= 0xFF000000;
        liTiltX = uliTiltX;
        liTiltY = uliTiltY;
        MutexRawTiltPoint.lockForWrite();
        {
            TiltRaw.setX(liTiltX);
            TiltRaw.setY(liTiltY);
            liTiltX = liTiltX - TiltRawOffset.x();
            liTiltY = liTiltY - TiltRawOffset.y();
        }
        MutexRawTiltPoint.unlock();
        logger()->trace(QString("Tilt sensor rxed X: %1 Y: %2").arg(liTiltX).arg(liTiltY));
        //steps are with including sign bit -> range is -refvolt till +refvolt -> factor 2
        TiltX = 2*TiltADC_RefVoltage*(float)liTiltX/((float)TiltADC_Steps*(float)TiltADC_Gain);
        TiltY = 2*TiltADC_RefVoltage*(float)liTiltY/((float)TiltADC_Steps*(float)TiltADC_Gain);
        logger()->trace(QString("Tilt g calced X: %1 Y: %2" ).arg(TiltX).arg(TiltY));

//        XCalOffset  = (TiltCalValPosGX + TiltCalValNegGX)/2;
//        XCalGain    = (TiltCalValPosGX - TiltCalValNegGX)/2;
//        YCalOffset  = (TiltCalValPosGY + TiltCalValNegGY)/2;
//        YCalGain    = (TiltCalValPosGY - TiltCalValNegGY)/2;
//        logger()->trace(QString("Tilt g OffsetError X: %1 Y: %2").arg(XCalOffset).arg(YCalOffset));
//      //  logger()->trace(QString("Tilt g GainError X: %1 Y %2" ).arg(XCalGain).arg(YCalGain));
//        TiltX = XCalGain*TiltX+XCalOffset;
//        TiltY = YCalGain*TiltY+YCalOffset;

        if(TiltX > 1)
            TiltX = 1.0;
        if(TiltX < -1)
            TiltX = -1.0;

        if(TiltY > 1)
            TiltY = 1.0;
        if(TiltY < -1)
            TiltY = -1.0;

        TiltX=asin((float)TiltX)*180/M_PI;
        TiltY=asin((float)TiltY)*180/M_PI;
        logger()->debug(QString("Tilt Sensor X: %1 Y: %2").arg(TiltX).arg(TiltY));
        emit hwdtSigGotTilt(TiltX,TiltY,TiltADC_Steps,TiltADC_Gain);
    }
#else
#endif
}

void THWDriverThread::hwdtSloAskCompass()
{


    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_COMPASS_READ;
    logger()->debug("Asking Compass");
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){

        float Heading;
        int16_t siHeading;

        siHeading = rxBuffer[P1];
        siHeading += rxBuffer[P2] << 8;

        Heading = siHeading;
        Heading /= 10;
        logger()->debug(QString("Compass Heading: %1").arg(Heading) );
        //Heading += HeadingOffset;
        emit hwdtSigGotCompassHeading(Heading);
    }
}

void THWDriverThread::hwdtSloStartCompassCal()
{
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_COMPASS_CAL_ENTER;
    logger()->debug("Compass Calibration start");
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){
        emit hwdtSigCompassStartedCalibrating();
    }
}

void THWDriverThread::hwdtSloStopCompassCal()
{
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_COMPASS_CAL_EXIT;
    logger()->debug("Compass Calibration stop");
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){
        emit hwdtSigCompassStoppedCalibrating();
    }
}

void THWDriverThread::hwdtSloConfigLightSensor(TLightSensorGain Gain, TLightSensorIntegTime LightSensorIntegTime){
    int g;
    float integ=0;
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_LIGHT_INIT;
    txBuffer[P1] =  1; //switch on
    txBuffer[P2] =  (int)Gain; //switch on
    txBuffer[P3] =  (int)LightSensorIntegTime; //switch on
    g = 1;
    if (Gain==lsGain16)
        g = 16;

    switch(LightSensorIntegTime){
    case lsInteg13_7ms: integ *= 13.7; break;
    case lsInteg101ms:  integ *= 101;  break;
    case lsInteg402ms:  integ *= 402;  break;
    }
    logger()->debug(QString("Config Light Sensor Gain: %1 Integrationtime(ms): %2").arg(g).arg(integ));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){
        LightSensorGain = Gain;
        this->LightSensorIntegTime = LightSensorIntegTime;
    }
}

void THWDriverThread::hwdtSloAskLightSensor(){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_LIGHT_READ;
    logger()->debug("Ask Light Sensor");
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){
        uint16_t sivalue;
        float value;
        sivalue = rxBuffer[P2];
        sivalue += rxBuffer[P3] << 8;
        value = sivalue;
        switch(LightSensorIntegTime){
        case lsInteg13_7ms: value *= 29.34; break;
        case lsInteg101ms:  value *= 3.98;  break;
        case lsInteg402ms:  value *= 1;     break;
        }

        if (LightSensorGain == lsGain16)
            value /= 16;
        logger()->debug(QString("Light Sensor value: %1").arg(value) );
        emit hwdtSigGotLightSensorValue(value);
    }
}

void THWDriverThread::hwdtSloGoMotorHome(void)
{
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] = 0;
    txBuffer[1] = CMD_MOT_CALIBRATION;
    logger()->debug("Calibrate Motor");
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutMotion,false,true)){
        mc->setMotorCoordinate(0,0);
        emit hwdtSigMotorIsHome();
    }
}

bool THWDriverThread::SetShutter(THWShutterCMD ShutterCMD){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    if ((ShutterCMD == scNone) || (LastShutterCMD == ShutterCMD)){
        if (ShutterCMD != scNone)
            LastShutterCMD = ShutterCMD;
        return true;
    }else{
        if (ShutterCMD == scClose){
            txBuffer[1] =  CMD_MOT_CLOSESHUTTER;
            logger()->debug("Close shutter");
        }else{
            txBuffer[1] =  CMD_MOT_OPENSHUTTER;
            logger()->debug("Open shutter");
        }
        if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutMotion,false,true)){
            //QTest::qWait(200);
            LastShutterCMD = ShutterCMD;
            return true;
        }else{
            return false;
        }
    }
}

void THWDriverThread::hwdtSloSetShutter(THWShutterCMD ShutterCMD)
{
    if (SetShutter(ShutterCMD)){
        emit hwdtSigShutterStateChanged(ShutterCMD);
    }
}

//called from other threads!!!
void THWDriverThread::hwdtGetLastSpectrumBuffer(double *Spectrum,
                                                int *NumberOfSpecPixels,
                                                TSPectrWLCoefficients *SpectrCoefficients,
                                                uint size,
                                                double *MaxPossibleValue,
                                                uint *Integrationtime,
                                                uint *avg,
                                                TMirrorCoordinate *mc,
                                                QString *SpectrSerial)
{
    #if ! OMNI_ENABLED
        NumOfPixels = 2048;
    #endif
    uint i = NumOfPixels;
    if (size < i)
        i = size;
    MutexSpectrBuffer.lockForRead();
    {
        *avg = SpectrAvgCount;
        *NumberOfSpecPixels = NumOfPixels;
        memcpy(SpectrCoefficients,&(this->SpectrCoefficients),sizeof(TSPectrWLCoefficients));
        mc->setAngleCoordinate(this->mc->getAngleCoordinate());
        #if ! OMNI_ENABLED
            for (uint n=0;n<i;n++){
                Spectrum[n] = (float)(qrand() % 1000)/1000.0;
            }
            *MaxPossibleValue = 1;

            *Integrationtime = 10;
            *SpectrSerial = "random";
        #else
            memcpy(Spectrum,LastSpectr,sizeof(double)*i);
            *MaxPossibleValue = SpectrMaxIntensity;
            *Integrationtime = LastSpectrIntegTime;
            *SpectrSerial = SpectrometerSerial;
        #endif
    }
    MutexSpectrBuffer.unlock();
}

double THWDriverThread::getMaxIntensityOfLastSpect(){
    double ret=0;
    //MutexSpectrBuffer.lockForRead();  called withing a lockforwrite(from takespectrum())
    {
        for (int i=0;i<NumOfPixels;i++){
            if (ret < LastSpectr[i])
                ret = LastSpectr[i];
        }
    }
    //MutexSpectrBuffer.unlock();
    return ret;
}

bool THWDriverThread::CalcAndSetAutoIntegTime(){
    double PeakVal;
    bool ret = false;
    float m = 0;
    float TargetVal;
    float TargetCorridor;

    MutexintegTime.lockForRead();
    {
        //MutexSpectrBuffer.lockForRead();called withing a lockforwrite(from takespectrum())
        {
            TargetVal= SpectrMaxIntensity*IntegTimeConf.targetPeak/100;
            TargetCorridor= SpectrMaxIntensity*IntegTimeConf.targetCorridor/100;
        }
        //MutexSpectrBuffer.unlock();
        if (IntegTimeConf.autoenabled){
            PeakVal = getMaxIntensityOfLastSpect();
            if ((PeakVal < TargetVal-TargetCorridor) || (PeakVal > TargetVal+TargetCorridor)){
                m = PeakVal/(float)LastSpectrIntegTime;
                LastSpectrIntegTime = round(TargetVal/m);
                if (LastSpectrIntegTime > IntegTimeConf.maxIntegTime)
                    LastSpectrIntegTime = IntegTimeConf.maxIntegTime;
                wrapper->setIntegrationTime(SpectrometerIndex,LastSpectrIntegTime);
                ret = true;
            }
        }else{
            if (LastSpectrIntegTime != IntegTimeConf.fixedIntegtime){
                LastSpectrIntegTime = IntegTimeConf.fixedIntegtime;
                wrapper->setIntegrationTime(SpectrometerIndex,LastSpectrIntegTime);
                ret = true;
            }else{
                ret = false;
            }
        }
    }
    MutexintegTime.unlock();
    return ret;
}



void THWDriverThread::TakeSpectrum(int avg, uint IntegrTime){


    JString s;
    //logger()->debug("Fetch Spectra");
    MutexSpectrBuffer.lockForWrite();
    {
        SpectrAvgCount = avg;
    }MutexSpectrBuffer.unlock();
    #if OMNI_ENABLED

    double *spectrpointer;
    int i,n;//,m;

    s = wrapper->getLastException();
    if (s.getLength()==0)
        SpectrometerIndex=0;
    if (SpectrometerIndex > -1){
         if ((LastSpectrIntegTime != IntegrTime) && (IntegrTime != 0))
             wrapper->setIntegrationTime(SpectrometerIndex,IntegrTime);
         if (IntegrTime == 0)
             LastSpectrIntegTime = wrapper->getIntegrationTime(SpectrometerIndex);
         else
             LastSpectrIntegTime = IntegrTime;
//        for (m = 1;m<2;m++){
//            if (m == 0){
//                wrapper->setIntegrationTime(SpectrometerIndex,wrapper->getMinimumIntegrationTime(SpectrometerIndex)*integtimetest);
//                integtimetest+=10;
//                if (integtimetest > 100)
//                    integtimetest = 1;
//            }else
//                wrapper->setIntegrationTime(SpectrometerIndex,wrapper->getMinimumIntegrationTime(SpectrometerIndex)*400);

            MutexSpectrBuffer.lockForWrite();
            {
                bool alreadyAutoCalced = false;
                for (i=0;i<avg;i++){
                    {
                        DoubleArray Spectrbuf;
                        Spectrbuf = wrapper->getSpectrum(SpectrometerIndex,0);
                        spectrpointer = Spectrbuf.getDoubleValues();
                        if (i == 0){
                            memcpy(LastSpectr,spectrpointer,sizeof(double)*NumOfPixels);
                            if (IntegrTime == 0){
                                if (!alreadyAutoCalced){
                                    if (CalcAndSetAutoIntegTime()){ //if we adjusted integrtime
                                        i = -1; //lets run loop from beginning
                                        alreadyAutoCalced = true;
                                    }
                                }
                            }
                        }else{
                            for (n = 0; n< NumOfPixels;n++){
                                LastSpectr[n] += spectrpointer[n];
                            }
                        }
                    }
                }
                if (avg > 1){
                    for (n = 0; n< NumOfPixels;n++){
                        LastSpectr[n] /= (double)avg;
                    }
                }
            }
            MutexSpectrBuffer.unlock();
       // }
    }else{
        //logger()->error("Trying to read from Spectrometer even though its not opened yet.");
    }
#else
    (void)IntegrTime;
#endif
}

bool THWDriverThread::hwdtSloMotGoto(int PosX, int PosY){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    int32_t PosStationary = PosX;
    int32_t PosMirror = PosY;
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_RAMP;

    txBuffer[P1] = PosMirror & 0xFF;
    txBuffer[P2] = (PosMirror >> 8) & 0xFF;
    txBuffer[P3] = (PosMirror >> 16) & 0xFF;

    txBuffer[P4] = PosStationary & 0xFF;
    txBuffer[P5] = (PosStationary >> 8) & 0xFF;
    txBuffer[P6] = (PosStationary >> 16) & 0xFF;
    logger()->debug(QString("Motors go to. Mirror: %1 Stationary: %2").arg(PosX).arg(PosY));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutMotion,false,true)){
        mc->setMotorCoordinate(PosStationary,PosMirror);
        emit hwdtSigMotMoved();
        return true;

    }else{
        return false;
    }
}

void THWDriverThread::hwdtSloMotIdleState(bool idle){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];

    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_IDLE_STATE;
    if (idle)
        txBuffer[P1] = 0;
    else
        txBuffer[P1] = 1;


    logger()->debug(QString("Motors idlestate: %1").arg(idle));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){


    }else{

    }
}

void THWDriverThread::hwdtSloMeasureScanPixel(int PosX, int PosY ,uint avg, uint integrTime)
{

    if (hwdtSloMotGoto(PosX,PosY)){
        //now we should measure;
        TakeSpectrum( avg, integrTime);

        emit hwdtSigScanPixelMeasured();
    }
}


void THWDriverThread::hwdtSloMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD)
{
    THWShutterCMD shutterCMDtmp = LastShutterCMD;
    if (SetShutter(shutterCMD)){


        emit hwdtSigShutterStateChanged(shutterCMD);

        if (wrapper){
            TakeSpectrum(avg,integrTime);
            if (SpectrometerIndex > -1)
                QTest::qWait(10);
            emit hwdtSigGotSpectrum();
        }
        if (SetShutter(shutterCMDtmp)){
            emit hwdtSigShutterStateChanged(shutterCMDtmp);
        }
    }else{
        QTest::qWait(10);
        emit hwdtSigGotSpectrum();
    }
}

//called from other threads!!!
void THWDriverThread::hwdtSetIntegrationConfiguration(TAutoIntegConf *autoIntConf){
    MutexintegTime.lockForWrite();
    {
        memcpy(&this->IntegTimeConf,autoIntConf,sizeof(TAutoIntegConf));
    }
    MutexintegTime.unlock();
}

QList<QString> THWDriverThread::hwdtGetSpectrometerList(){
    QList<QString> tmp;
    MutexSpectrList.lockForRead();
    {
        tmp = *SpectrometerList;
    }
    MutexSpectrList.unlock();
    return tmp;
}

void THWDriverThread::hwdtSloDiscoverSpectrometers(){
    #if OMNI_ENABLED
    newOmniWrapper();
    wrapper->closeAllSpectrometers();
    NumberOfSpectrometers = wrapper->openAllSpectrometers();
    MutexSpectrList.lockForWrite();
    {
        SpectrometerList->clear();
        for ( int i=0; i < NumberOfSpectrometers; i++){
            QString s(wrapper->getSerialNumber(i).getASCII());
            SpectrometerList->append(s);
            logger()->debug("Spectrometer "+s+" found.");
        }
    }
    MutexSpectrList.unlock();
    #endif
    emit hwdtSigSpectrometersDicovered();
}

uint THWDriverThread::hwdtGetMinimumIntegrationTime(){
    uint minintegtime;
    MutexMinIntegrationTime.lockForRead();
    {
        minintegtime = SpectrMinIntegTime;

    }
    MutexMinIntegrationTime.unlock();
    return minintegtime;
}

void THWDriverThread::hwdtSloOpenSpectrometer(QString Serialnumber)
{
#if OMNI_ENABLED
    newOmniWrapper();
    MutexSpectrBuffer.lockForWrite();
    {
        MutexMinIntegrationTime.lockForWrite();
        {
        SpectrMinIntegTime = -1;
        }
        MutexMinIntegrationTime.unlock();
        SpectrMaxIntensity = -1;
    }
    MutexSpectrBuffer.unlock();
    SpectrometerIndex = SpectrometerList->indexOf(Serialnumber);
    if (SpectrometerIndex == -1)
        hwdtSloDiscoverSpectrometers();
    SpectrometerIndex = SpectrometerList->indexOf(Serialnumber);
    if (SpectrometerIndex > -1){
        SpectrometerSerial = Serialnumber;
        Coefficients Coef;
        LastSpectrIntegTime = -1;
        MutexSpectrBuffer.lockForWrite();
        {
            NumOfPixels = wrapper->getNumberOfPixels(SpectrometerIndex,0);
            SpectrMaxIntensity = wrapper->getMaximumIntensity(SpectrometerIndex);
            MutexMinIntegrationTime.lockForWrite();
            {
            SpectrMinIntegTime = wrapper->getMinimumIntegrationTime(SpectrometerIndex);
            }
            MutexMinIntegrationTime.unlock();
            Coef = wrapper->getCalibrationCoefficientsFromBuffer(SpectrometerIndex);
            SpectrCoefficients.Offset = Coef.getWlIntercept();
            SpectrCoefficients.Coeff1 = Coef.getWlFirst() ;
            SpectrCoefficients.Coeff2 = Coef.getWlSecond();
            SpectrCoefficients.Coeff3 = Coef.getWlThird();
            SpectrCoefficients.overWrittenFromFile = false;
            SpectrCoefficients.uninitialized = false;
        }
        MutexSpectrBuffer.unlock();
        emit hwdtSigSpectrumeterOpened();
    }
  //   SpectrometerIndex =0 ;
#else
    (void)Serialnumber;
    memset(&SpectrCoefficients,0,sizeof(SpectrCoefficients));
    SpectrCoefficients.Coeff1 = 1;
    SpectrCoefficients.uninitialized = false;
#endif
}

void THWDriverThread::hwdtSloCloseSpectrometer()
{
    SpectrometerSerial = "closed";
    MutexSpectrBuffer.lockForWrite();
    {
        LastSpectrIntegTime = -1;
        SpectrometerIndex = -1;
        MutexMinIntegrationTime.lockForWrite();
        {
        SpectrMinIntegTime = -1;
        }
        MutexMinIntegrationTime.unlock();
        SpectrMaxIntensity = -1;
    }
    MutexSpectrBuffer.unlock();
#if OMNI_ENABLED
    if (wrapper)
        wrapper->closeAllSpectrometers();
#endif
}



THWDriver::THWDriver()
{
    TemperatureTimer = new QTimer();
    TiltTimer  = new QTimer();
    ActualTilt = new QPointF(0,0);
    HWDriverObject = new THWDriverThread();
    CompassState = csNone;
    CompassHeading = INVALID_COMPASS_HEADING;
    CompassOffset = 0;
    TempBufferPointer = 0;
    LightSensorVal = INVALID_LIGHTSENSOR_VAL;
    this->m_sde = new SerialDeviceEnumerator(this);
    ComPortConf.valid = false;
    COMPortOpened = false;
    ComPortList = new QStringList();
    connect(this->m_sde, SIGNAL(hasChanged(QStringList)),
            this, SLOT(slotCOMPorts(QStringList)));
    this->m_sde->setEnabled(true);

    HWDriverThread = new QThreadEx();
    HWDriverThread->setObjectName("DriverThread");

    qRegisterMetaType<THWTransferState>( "THWTransferState" );
    qRegisterMetaType<THWTempSensorID>( "THWTempSensorID" );
    qRegisterMetaType<THWShutterCMD>( "THWShutterCMD" );
    qRegisterMetaType<TTiltConfigRes>( "TTiltConfigRes" );
    qRegisterMetaType<TTiltConfigGain>( "TTiltConfigGain" );
    qRegisterMetaType<AbstractSerial::BaudRate>( "AbstractSerial::BaudRate" );

    //Connection of outgoing signals..
    {
        connect(HWDriverObject,SIGNAL(hwdtSigGotTemperature(THWTempSensorID , float,float,float ,bool)),
                    this,SLOT (hwdSloGotTemperature(THWTempSensorID , float,float,float ,bool)),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigGotTilt(float,float,int,int)),
                    this,SLOT (hwdSloGotTilt(float,float,int,int)),Qt::QueuedConnection);
        //connect(HWDriverObject,SIGNAL(hwdtSigGotTilt(float,float,int,int)),
        //            this,SIGNAL (hwdSigGotTilt(float,float,int,int)),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigGotCompassHeading( float )),
                    this,SLOT (hwdSloGotCompassHeading( float )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigGotCompassHeading( float )),
                    this,SIGNAL (hwdSigGotCompassHeading( float )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigCompassStartedCalibrating( )),
                    this,SLOT (hwdSloCompassStartedCalibrating( )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigCompassStartedCalibrating( )),
                    this,SIGNAL (hwdSigCompassStartedCalibrating( )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigCompassStoppedCalibrating( )),
                    this,SLOT (hwdSloCompassStoppedCalibrating( )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigCompassStoppedCalibrating( )),
                    this,SIGNAL (hwdSigCompassStoppedCalibrating( )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigGotLightSensorValue( float )),
                    this,SLOT (hwdSloGotLightSensorValue( float )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigGotLightSensorValue( float)),
                    this,SIGNAL (hwdSigGotLightSensorValue( float)),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigMotorIsHome( )),
                    this,SLOT (hwdSloMotorIsHome( )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigMotorIsHome( )),
                    this,SIGNAL (hwdSigMotorIsHome( )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigShutterStateChanged( THWShutterCMD )),
                    this,SLOT (hwdSloShutterStateChanged( THWShutterCMD )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigShutterStateChanged( THWShutterCMD )),
                    this,SIGNAL (hwdSigShutterStateChanged( THWShutterCMD )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigScanPixelMeasured( )),
                    this,SLOT (hwdSloScanPixelMeasured( )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigScanPixelMeasured( )),
                    this,SIGNAL (hwdSigScanPixelMeasured( )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigGotSpectrum( )),
                    this,SLOT (hwdSloGotSpectrum( )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigGotSpectrum( )),
                    this,SIGNAL (hwdSigGotSpectrum( )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigMotMoved( )),
                    this,SIGNAL (hwdSigMotMoved( )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigGotWLCoefficients( )),
                    this,SLOT (hwdSloGotWLCoefficients( )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigSpectrometersDicovered( )),
                    this,SIGNAL (hwdSigSpectrometersDiscovered( )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigSpectrumeterOpened( )),
                    this,SLOT (hwdSloSpectrumeterOpened( )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigSpectrumeterOpened( )),
                    this,SIGNAL (hwdSigSpectrumeterOpened( )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigTransferDone(THWTransferState , uint )),
                    this,SIGNAL (hwdSigTransferDone(THWTransferState , uint )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigCOMPortChanged(QString, bool,bool )),
                    this,SLOT (hwdSloCOMPortChanged(QString, bool,bool )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigCOMPortChanged(QString, bool,bool )),
                    this,SIGNAL (hwdSigCOMPortChanged(QString, bool,bool )),Qt::QueuedConnection);

    }
    //for messages giong from this to thread..
    {
        connect(this,SIGNAL(hwdtSigSetComPort(QString )),
                    HWDriverObject,SLOT (hwdtSloSetComPort(QString )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigSetBaud(AbstractSerial::BaudRate )),
                    HWDriverObject,SLOT (hwdtSloSetBaud(AbstractSerial::BaudRate  )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigAskTemperature(THWTempSensorID ,bool)),
                    HWDriverObject,SLOT (hwdtSloAskTemperature(THWTempSensorID,bool )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigSetTargetTemperature(float )),
                    HWDriverObject,SLOT (hwdtSloSetTargetTemperature(float )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigConfigTilt(TTiltConfigRes, TTiltConfigGain)),
                    HWDriverObject,SLOT (hwdtSloConfigTilt(TTiltConfigRes, TTiltConfigGain)),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigAskTilt( )),
                    HWDriverObject,SLOT (hwdtSloAskTilt( )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigSetTiltOffset(int,int)),
                    HWDriverObject,SLOT (hwdtSloSetTiltOffset(int,int)),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigAskCompass( )),
                    HWDriverObject,SLOT (hwdtSloAskCompass( )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigStartCompassCal( )),
                    HWDriverObject,SLOT (hwdtSloStartCompassCal( )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigStopCompassCal( )),
                    HWDriverObject,SLOT (hwdtSloStopCompassCal( )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigAskLightSensor( )),
                    HWDriverObject,SLOT (hwdtSloAskLightSensor( )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigGoMotorHome( )),
                    HWDriverObject,SLOT (hwdtSloGoMotorHome( )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigSetShutter(THWShutterCMD )),
                    HWDriverObject,SLOT (hwdtSloSetShutter(THWShutterCMD )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigMeasureScanPixel(int , int  ,uint , uint)),
                    HWDriverObject,SLOT (hwdtSloMeasureScanPixel(int , int  ,uint , uint)),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtMotGoto(int , int )),
                    HWDriverObject,SLOT (hwdtSloMotGoto(int , int  )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtMotIdleState(bool )),
                    HWDriverObject,SLOT (hwdtSloMotIdleState(bool)),Qt::QueuedConnection);



        connect(this,SIGNAL(hwdtSigMeasureSpectrum(uint , uint ,THWShutterCMD  )),
                    HWDriverObject,SLOT (hwdtSloMeasureSpectrum(uint , uint ,THWShutterCMD )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigDiscoverSpectrometers()),
                    HWDriverObject,SLOT (hwdtSloDiscoverSpectrometers()),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigOpenSpectrometer(QString )),
                    HWDriverObject,SLOT (hwdtSloOpenSpectrometer(QString )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigCloseSpectrometer( )),
                    HWDriverObject,SLOT (hwdtSloCloseSpectrometer( )),Qt::QueuedConnection);

        connect(TemperatureTimer,SIGNAL(timeout()),
                    this,SLOT (hwdSlotTemperatureTimer( )));

        connect(TiltTimer,SIGNAL(timeout()),
                    this,SLOT (hwdSlotTiltTimer()));

        connect(this,SIGNAL(hwdtQuitThred()),
                            HWDriverObject,SLOT(CloseEverythingForLeaving()),Qt::QueuedConnection);


    }
    HWDriverObject->moveToThread(HWDriverThread);
    HWDriverThread->start();
    connect(HWDriverThread,SIGNAL(finished()),this,SLOT(hwdSlothreadFinished()));
    //TemperatureTimer->start(1000);
    TiltTimer->start(1000);
   // hwdSetComPort("/dev/ttyUSB0");
    WavelengthBuffer = TWavelengthbuffer::instance();
    memset(&SpectrCoefficients,0,sizeof(SpectrCoefficients));
    SpectrCoefficients.uninitialized = true;
    SpectrCoefficients.Coeff1 = 1;
}

THWDriver::~THWDriver(){
    //hopefully stop() was called for not deleting a running thread
    delete TiltTimer;
    delete TemperatureTimer;
    delete ActualTilt;
    HWDriverObject->deleteLater();
    HWDriverThread->deleteLater();
    delete m_sde;
    delete ComPortList;
}

void THWDriver::hwdSetTiltInterval(int ms){
    TiltTimer->setInterval(ms);
}

void THWDriver::hwdSetComPort(TCOMPortConf ComConf)
{
    QString name;

    memcpy(&ComPortConf,&ComConf,sizeof(TCOMPortConf));
    if (ComConf.valid){
        if (ComConf.ByName){
            name = ComConf.Name;
            emit hwdtSigSetComPort(name);
        }else{
            QString SysPath;
            for(int i = 0;i<ComPortList->count();i++){
                m_sde->setDeviceName(ComPortList->at(i));
                SysPath = m_sde->systemPath();
                SysPath = SysPath.left(SysPath.indexOf("tty"));
                if(SysPath == ComConf.SysPath){
                    name = ComPortList->at(i);
                    emit hwdtSigSetComPort(name);
                    break;
                }
            }
        }

    }
}

void THWDriver::hwdSloCOMPortChanged(QString name, bool opened, bool error){
    (void)name;
    (void)error;
    COMPortOpened = opened;
}

void THWDriver::slotCOMPorts(const QStringList &list){
    ComPortList->clear();
    ComPortList->append(list);
    QTest::qWait(1000);
    if(!COMPortOpened){
        hwdSetComPort(ComPortConf);
    }else{//if connected but now unplugged we should update state for
        if (!ComPortConf.ByName){//actual COMPort-> close Comport
            bool found=false;
            QString SysPath;
            for(int i = 0;i<ComPortList->count();i++){
                m_sde->setDeviceName(ComPortList->at(i));
                SysPath = m_sde->systemPath();
                SysPath = SysPath.left(SysPath.indexOf("tty"));
                if(SysPath == ComPortConf.SysPath){
                    found=true;
                    break;
                }
            }
            if(!found){
                emit hwdtSigSetComPort("");
            }
        }
    }
}

void THWDriver::stop(){
    emit hwdtQuitThred();
}

void THWDriver::hwdSlothreadFinished(){
        emit hwdSigHWThreadFinished();
}

TSPectrWLCoefficients THWDriver::hwdGetWLCoefficients()
{
    return this->SpectrCoefficients;
}




void THWDriver::hwdSetBaud(AbstractSerial::BaudRate baud)
{
    emit hwdtSigSetBaud(baud);
}


float THWDriver::hwdGetTemperature(THWTempSensorID sensorID)
{
    int i = sensorIDtoInt(sensorID);
    return Temperatures[i];
}

void THWDriver::hwdSlotTemperatureTimer(){
    emit hwdtSigAskTemperature(tsPeltier,true);
}

void THWDriver::hwdSloGotTemperature(THWTempSensorID sensorID, float TemperaturePeltier, float TemperatureSpectr,float TemperatureHeatsink,bool byTimer)
{
    (void)byTimer;
    (void)sensorID;
    Temperatures[sensorIDtoInt(tsPeltier)] = TemperaturePeltier;
    Temperatures[sensorIDtoInt(tsSpectrometer)] = TemperatureSpectr;
    Temperatures[sensorIDtoInt(tsHeatSink)] = TemperatureHeatsink;

    if (byTimer){

        if (TempBufferPointer == TEMPERATURE_BUFFER_COUNT){
            for (int i=1;i<TEMPERATURE_BUFFER_COUNT;i++){
                TempBufferPeltier[i-1] = TempBufferPeltier[i];
                TempBufferSpectr[i-1] = TempBufferSpectr[i];
                TempBufferHeatSink[i-1] = TempBufferHeatSink[i];
            }
            TempBufferPointer = TEMPERATURE_BUFFER_COUNT-1;
        }
        if ((TemperaturePeltier < 100) && (TemperaturePeltier > -100))
            TempBufferPeltier[TempBufferPointer] = TemperaturePeltier;
        else if (TempBufferPointer > 0)
            TempBufferPeltier[TempBufferPointer] = TempBufferPeltier[TempBufferPointer-1];
        else
            TempBufferPeltier[TempBufferPointer] = 0;

        if ((TemperatureHeatsink < 100) && (TemperatureHeatsink > -100))
            TempBufferHeatSink[TempBufferPointer] = TemperatureHeatsink;
        else if (TempBufferPointer > 0)
            TempBufferHeatSink[TempBufferPointer] = TempBufferHeatSink[TempBufferPointer-1];
        else
            TempBufferHeatSink[TempBufferPointer] = 0;

        if ((TemperatureSpectr < 100) && (TemperatureSpectr > -100))
            TempBufferSpectr[TempBufferPointer] = TemperatureSpectr;
        else if (TempBufferPointer > 0)
            TempBufferSpectr[TempBufferPointer] = TempBufferSpectr[TempBufferPointer-1];
        else
            TempBufferSpectr[TempBufferPointer] = 0;
        TempBufferPointer++;
    }


    emit hwdSigGotTemperatures(TemperaturePeltier,TemperatureSpectr,TemperatureHeatsink);
}

void THWDriver::hwdAskTemperature(THWTempSensorID sensorID)
{
    emit hwdtSigAskTemperature(sensorID,false);
}

void THWDriver::hwdSetTargetTemperature(float temperature)
{
    emit hwdtSigSetTargetTemperature(temperature);
}


QPointF THWDriver::hwdGetTilt(void)
{
    return QPointF(*ActualTilt);
}

void THWDriver::hwdAskTilt()
{
    emit hwdtSigAskTilt();
}

void THWDriver::hwdSlotTiltTimer(){
    emit hwdAskTilt();
}


void THWDriver::hwdSloGotTilt(float TiltX, float TiltY, int Gain, int Resolution){
//#define TILT_MAX_8G                 14.83
//#define TILT_MAX_4G                 30.79

    (void)Resolution;
    float Border;
    float MaxTilt;
//    if (Gain < 8){
//        MaxTilt = TILT_MAX_4G; // for bubble spirit use
//        Border = TILT_MAX_8G*0.75;
//        float MinTilt = TiltX;
//        if (MinTilt > TiltY){
//            MinTilt = TiltY;
//        }
//        if (MinTilt < TILT_MAX_8G*0.75){
//            hwdtSigConfigTilt(tcr18Bit,tcGain8);
//            //switch gain to 8
//        }
//    }else{
        MaxTilt = TILT_MAX_4G*0.75;
        Border = TILT_MAX_8G;
//        float MaxTilt = TiltX;
//        if (MaxTilt < TiltY){
//            MaxTilt = TiltY;
//        }
//        if (MaxTilt >= TILT_MAX_8G){
//            hwdtSigConfigTilt(tcr18Bit,tcGain4);
//            //switch gain to 4
//        }
//    }
    ActualTilt->setX(TiltX);
    ActualTilt->setY(TiltY);
    emit hwdSigGotTilt(ActualTilt->x(),ActualTilt->y(),Gain,Resolution,Border,MaxTilt);
}

QPoint THWDriver::hwdGetRawTilt(){
    return HWDriverObject->hwdtGetLastRawTilt();
}

void THWDriver::hwdSetTiltOffset(QPoint Offset)
{
    emit hwdtSigSetTiltOffset(Offset.x(),Offset.y());
}


float THWDriver::hwdGetCompassHeading(void)
{
    return CompassHeading;
}

void THWDriver::hwdAskCompass()
{
    emit hwdtSigAskCompass();
}

void THWDriver::hwdSloGotCompassHeading(float Heading)
{
    CompassHeading = Heading - CompassOffset;
    CompassHeading = fmod(CompassHeading , 360.0);
    CompassState = csReady;
}

void THWDriver::hwdStartCompassCal()
{
    emit hwdtSigStartCompassCal();
}

void THWDriver::hwdSloCompassStartedCalibrating()
{
    CompassState = csCalibrating;
}

void THWDriver::hwdStopCompassCal()
{
    emit hwdtSigStopCompassCal();
}

void THWDriver::hwdSloCompassStoppedCalibrating()
{
    if (CompassHeading == INVALID_COMPASS_HEADING)
        CompassState = csNone;
    else
        CompassState = csReady;
}

void THWDriver::hwdSetCompassRealValue(float RealHeading)
{
    float Offset = CompassHeading-RealHeading;
    CompassOffset = fmod(Offset , 360.0);
}

THWCompassState THWDriver::hwdGetCompassState(){
    return CompassState;
}

void THWDriver::hwdSloGotLightSensorValue(float lightValue){
    LightSensorVal = lightValue;
}

void THWDriver::hwdAskLightSensor(){
    emit hwdAskLightSensor();
}

float THWDriver::hwdGetLightsensorValue(){
    return LightSensorVal;
}


void THWDriver::hwdGoMotorHome(void)
{
    emit hwdtSigGoMotorHome();
}


void THWDriver::hwdSetShutter(THWShutterCMD ShutterCMD)
{
    emit hwdtSigSetShutter(ShutterCMD);
}


void THWDriver::hwdMeasureScanPixel(QPoint pos,uint avg, uint integrTime)
{
    emit hwdtSigMeasureScanPixel(pos.x(),pos.y(),avg,integrTime);
}

void THWDriver::hwdMotMove(QPoint pos)
{
    emit hwdtMotGoto(pos.x(),pos.y());
}

void THWDriver:: hwdMotIdleState(bool idle){
    emit hwdtMotIdleState(idle);
}

void THWDriver::hwdMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD)
{
    emit hwdtSigMeasureSpectrum(avg,integrTime,shutterCMD);
}


void THWDriver::hwdOverwriteWLCoefficients(TSPectrWLCoefficients* WlCoefficients)
{
    if (!WlCoefficients->uninitialized){
        if (WavelengthBuffer != NULL){
            for (uint i = 0;i<MAXWAVELEGNTH_BUFFER_ELEMTENTS;i++){
                WavelengthBuffer->buf[i] = WlCoefficients->Offset+
                                           (double)i*WlCoefficients->Coeff1+
                                           pow((double)i,2.0)*WlCoefficients->Coeff2+
                                           pow((double)i,3.0)*WlCoefficients->Coeff3;
            }
        }
        memcpy(&this->SpectrCoefficients,WlCoefficients,sizeof(TSPectrWLCoefficients));
        this->SpectrCoefficients.overWrittenFromFile = true;
        this->SpectrCoefficients.uninitialized = false;
    }
}

uint THWDriver::hwdGetMinimumIntegrationTime(){
    return HWDriverObject->hwdtGetMinimumIntegrationTime();
}

uint THWDriver::hwdGetSpectrum(TSpectrum *Spectrum)
{
    TSPectrWLCoefficients coef;
    TMirrorCoordinate mc;

    HWDriverObject->hwdtGetLastSpectrumBuffer(Spectrum->spectrum,
                                              &Spectrum->NumOfSpectrPixels,
                                              &coef,
                                              MAXWAVELEGNTH_BUFFER_ELEMTENTS,
                                              &Spectrum->MaxPossibleValue,
                                              &Spectrum->IntegTime,
                                              &Spectrum->AvgCount,
                                              &mc,
                                              &Spectrum->SpectrometerSerialNumber);
    Spectrum->setMirrorCoordinate(&mc);
    memcpy(&Spectrum->IntegConf,&IntegTimeConf,sizeof(TAutoIntegConf));
    if (SpectrCoefficients.uninitialized){
        memcpy(&SpectrCoefficients,&coef,sizeof(TSPectrWLCoefficients));
        if (WavelengthBuffer != NULL){
            for (uint i = 0;i<MAXWAVELEGNTH_BUFFER_ELEMTENTS;i++){
                WavelengthBuffer->buf[i] = SpectrCoefficients.Offset+
                                           (double)i*SpectrCoefficients.Coeff1+
                                           pow((double)i,2.0)*SpectrCoefficients.Coeff2+
                                           pow((double)i,3.0)*SpectrCoefficients.Coeff3;
            }
        }
    }

    return Spectrum->NumOfSpectrPixels;
}

void THWDriver::setIntegrationConfiguration(TAutoIntegConf *autoIntConf){
    memcpy(&IntegTimeConf,autoIntConf,sizeof(IntegTimeConf));
    HWDriverObject->hwdtSetIntegrationConfiguration(autoIntConf);
}

void THWDriver::hwdDiscoverSpectrometers()
{
    emit hwdtSigDiscoverSpectrometers();
}

QList<QString> THWDriver::hwdGetListSpectrometer()
{

  //    emit hwdtQuitThred();
    emit hwdtSigDiscoverSpectrometers();
    return HWDriverObject->hwdtGetSpectrometerList();
}

void THWDriver::hwdOpenSpectrometer(QString SerialNumber)
{
    SpectrCoefficients.uninitialized = true;
    emit hwdtSigOpenSpectrometer(SerialNumber);
}

void THWDriver::hwdCloseSpectrometer()
{
    emit hwdtSigCloseSpectrometer();
}


void THWDriver::hwdSloMotorIsHome()
{

}

void THWDriver::hwdSloShutterStateChanged(THWShutterCMD LastShutterCMD)
{
    (void)LastShutterCMD;
}

void THWDriver::hwdSloScanPixelMeasured()
{

}

void THWDriver::hwdSloGotSpectrum()
{

}

void THWDriver::hwdSloSpectrumeterOpened()
{

}

void THWDriver::hwdSloGotWLCoefficients(){

}
