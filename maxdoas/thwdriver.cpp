#include "thwdriver.h"
#include "crc8.h"
#include <QString>
#include <QTime>
#include <QWaitCondition>
#include <stdint.h>
#include <qmath.h>
#include <JString.h>
#include <QTest>
#include <limits>

#define P0	2
#define P1	3
#define P2	4
#define P3	5
#define P4	6
#define P5	7
#define P6	8

#define DT_2DSCANNER	0x01
#define DT_1DSCANNER	0x02
#define DT_SOLTRACKER	0x03

#define TILT_REF_VOLT               2.048
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
#define CMD_SHUTTERPOS			0x62

#define CMD_MOT_HEATERPWM		0x68

#define CMD_GET_DECL_MIN_U		0x83

#define CMD_GET_DECL_MAX_U		0x84

#define CMD_GET_DECL_ZENITH_U                   	0x85

#define CMD_START_DECL_CALIBRATION              	0xB6
//ok

#define CMD_STOP_DECL_CALIBRATION               	0xB7
//ok

#define CMD_SET_DECL_ZENITH				0xB8
//ok

#define CMD_GET_TEMPERATURE_SHUTTER_ERROR		0x73

#define CMD_GET_MOT_SETUP				0x74

#define CMD_GET_INFO					0x75

#define CMD_SET_GUID					0x76
//20 bit guid

#define CMD_SET_MAXDOAS_ZENITH_POS                      0x77
//ok
#define CMD_SET_SHUTTER_CLOSE_POS                       0x78
//ok

#define CMD_MOT_INCLIN_SET_INIT		0x80
#define CMD_MOT_INCLIN_READ         0x81
#define CMD_MOT_INCLIN_GET_INIT		0x82

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
#define MOT_ENABLED 1
#define TILT_AFTER_MOTMOVE_TIME_OUT 1000
const uint TimeOutData  =  3000; //ms
const uint TimeOutMotion  =  15000; //ms



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

    ShutterIsOpenBySwitch = sssUnknown;
    MotMovedThusNoTiltInfoAnymore = true;
    TiltADC_Steps = 1<<18;  //lets calculate with 12 bits;
    TiltADC_Gain = 8;  //1,2,4,8 Gains available


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
    MutexSpectrSerial.lockForWrite();
    {
        SpectrometerSerial = "closed";
    }MutexSpectrSerial.unlock();
    LastMotMovement.start();
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
        char activateDirection = (uint8_t)(0xF0 | size);
        if(serial->bytesAvailable() > 0)
            serial->reset();
        serial->write(&activateDirection,1);
        rtstimer.start();
        bool waiting = true;
        while (waiting){
            while(serial->bytesAvailable() > 0){
                rtstimer.restart();
                activateDirection = 0;
                serial->read(&activateDirection, 1);
                if ((uint8_t)activateDirection == (0xF0 | size)){
                    trace = trace+"Direction set(ok); ";
                    waiting = false;
                    break;
                }else{
                    QString t;
                    t.sprintf("0x%02X",(unsigned char )activateDirection);
                    trace = trace+"Direction set(fail( "+t+") ); ";
                }
            }
            if (waiting){
                if (rtstimer.elapsed() > 500){
                    waiting = false;
                    trace = trace+"Direction set(to); ";
                    break;
                }else{
                    QTest::qSleep(1);
                }
            }

        }

        serial->reset();
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
            if ((!MOT_ENABLED && TempCtrler) || (MOT_ENABLED)){
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
            }
            return false;
        }else{
            logger()->error("Try to use COM-Port, but it's closed");
            return false;
        }
    }
    logger()->error("HWClass not initialized(serial == NULL)");
    return false;
}

bool THWDriverThread::waitForAnswer(char *TXBuffer,char *RXBuffer,uint size,int timeout , bool TempCtrler, bool RetransmitAllowed){
    QTime timer;
    bool TransmissionOK = false;
    bool TransmissionError = false;
    bool firsttimeout = true;
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
            while((timer.elapsed() <= 100) && (!TransmissionOK)){
                QTest::qSleep(5);

                if (serial->bytesAvailable() > 0)  {
                    uint8_t rxbyte;
                    serial->read(&RXBuffer[0], 1);
                    rxbyte = RXBuffer[0];
                    if ((unsigned char)rxbyte == (0xF0 | size)){
                        logger()->warn(QString("DirectCMDEcho Received"));
                        serial->read(&RXBuffer[0], 1);
                        rxbyte = RXBuffer[0];
                    }
                    if ((unsigned char)rxbyte == CMD_TRNSMIT_OK){
                        TransmissionOK = true;
                        firsttimeout = false;
                    }else{
                        QString t;
                        t.sprintf("0x%02X",(unsigned char )rxbyte);
                        emit hwdtSigTransferDone(tsCheckSumError,RetransmissionCounter);
                        logger()->warn(QString("Transmission: Checksum error! (%1) tries: %2 ").arg(t).arg(RetransmissionCounter));
                        firsttimeout = false;
                        break;
                    }
                }
            }
            if(firsttimeout){
                logger()->warn(QString("CRC Response timeout"));
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
            logger()->trace("Transmission done and answer rxed");
        }
        serial->reset();
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
    logger()->debug(QString("Asking for Temperature."));
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
    case tcrNone:   return 0;
    case tcr12Bit:  return 0;
    case tcr14Bit:  return 1;
    case tcr16Bit:  return 2;
    case tcr18Bit:  return 3;
    }
    return 0;
}

TTiltConfigRes ConfBitToTiltResolution(int Resolution){
    switch(Resolution){
    case 0:  return tcr12Bit;
    case 1:  return tcr14Bit;
    case 2:  return tcr16Bit;
    case 3:  return tcr18Bit;
    default: return tcrNone;
    }
    return tcrNone;
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

void THWDriverThread::hwdtSloAskTiltConfig(){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_INCLIN_GET_INIT;

    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){
        TiltADC_Gain = 1 << (int) rxBuffer[P1];
        TiltADC_Steps = 1 << (int) ConfBitToTiltResolution(rxBuffer[P2]);
        logger()->debug(QString("Got Tilt Sensor Configuration. Resolution: %1 bits, Gain: %2").arg((int)TiltADC_Steps).arg((int)TiltADC_Gain));
        emit hwdtSigGotTiltConfig();
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

void THWDriverThread::hwdtSloSetTiltMinMaxCalibration(int min_x, int min_y,int max_x, int max_y){
    TiltRawMaxValue.setX(max_x);
    TiltRawMaxValue.setY(max_y);
    TiltRawMinValue.setX(min_x);
    TiltRawMinValue.setY(min_y);

}
void THWDriverThread::hwdtSloTiltStartCal(){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_START_DECL_CALIBRATION;

    logger()->debug(QString("Starting Tilt Min Max Calibration Mode"));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){

    }
}

void THWDriverThread::hwdtSloTiltStopCal(){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_STOP_DECL_CALIBRATION;

    logger()->debug(QString("Stopping Tilt Min Max Calibration Mode"));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){

    }
}

void THWDriverThread::hwdtSloTiltSetZenith(){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_SET_DECL_ZENITH;

    logger()->debug(QString("Setting Tilt Zenith angle"));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){

    }
}

// CMD_GET_DECL_MIN_U		0x83




// CMD_GET_TEMPERATURE_SHUTTER_ERROR		0x73

void THWDriverThread::hwdtSloAskScannerStatus(){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_GET_TEMPERATURE_SHUTTER_ERROR;

    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){

        uint16_t usiTemperature;
        int16_t siTemperature;

        usiTemperature = 0;
        usiTemperature |= (uint8_t)rxBuffer[P1];
        usiTemperature |= (uint8_t)rxBuffer[P2]<<8;
        siTemperature = usiTemperature;
        QString s="Closed",e="OK";
        MutexSpectrBuffer.lockForWrite();
        {
            ScannerTemperature =  sensorTempToCelsius(siTemperature);




            if (rxBuffer[P3] & 0x01){
                ShutterIsOpenBySwitch = sssClosed;
                s = "Opened";
            }else
                ShutterIsOpenBySwitch = sssOpened;

            if (rxBuffer[P3] & 0x02){
                EndSwitchErrorState = eseERROR;
                e = "ERROR";
            }else
                EndSwitchErrorState = eseOK;
        }MutexSpectrBuffer.unlock();
        logger()->debug(QString("Got Scannerstatus: Temperature: %1 C, ShutterSwitch: %2, EndPosSwitch: %3").arg(ScannerTemperature).arg(s).arg(e));
        emit hwdtSigGotScannerStatus(ScannerTemperature,ShutterIsOpenBySwitch==sssOpened,EndSwitchErrorState==eseERROR);
    }
}

// CMD_GET_MOT_SETUP				0x74

void THWDriverThread::hwdtSloAskMotorSetup(){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_GET_MOT_SETUP;

    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){
#if 0
uint32_t maxdoaszenith=ee_getMaxdoasZenithPos();
        data->ProtocolBuffer[P0] = maxdoaszenith;
        data->ProtocolBuffer[P1] = (maxdoaszenith >> 8);
        data->ProtocolBuffer[P2] = (maxdoaszenith >> 16);

        uint16_t shutterclose=ee_getShutterClosePos();
        data->ProtocolBuffer[P3] = shutterclose;
        data->ProtocolBuffer[P4] = (shutterclose >> 8);

        data->ProtocolBuffer[P5] = (MAXDOAS_MICROSTEP << 4)|SHUTTER_MICROSTEP;
        data->ProtocolBuffer[P6] = SHUTTER_TYPE;
        task_sendanswer(data->ProtocolBuffer);
#endif

        uint32_t MaxDoasZenithPos;
        uint16_t ShutterClosePos;

        MaxDoasZenithPos = 0;
        MaxDoasZenithPos |= (uint8_t)rxBuffer[P0];
        MaxDoasZenithPos |= (uint8_t)rxBuffer[P1]<<8;
        MaxDoasZenithPos |= (uint8_t)rxBuffer[P2]<<16;

        ShutterClosePos = 0;
        ShutterClosePos |= (uint8_t)rxBuffer[P3];
        ShutterClosePos |= (uint8_t)rxBuffer[P4]<<8;

        int ShutterMicrosteps,MaxDoasMicrosteps;
        if (rxBuffer[P5]>>4 == 0)
            MaxDoasMicrosteps = 1;
        else if (rxBuffer[P5]>>4 == 1)
            MaxDoasMicrosteps = 2;
        else if (rxBuffer[P5]>>4 == 2)
            MaxDoasMicrosteps = 4;
        else if (rxBuffer[P5]>>4 == 3)
            MaxDoasMicrosteps = 8;
        else if (rxBuffer[P5]>>4 == 7)
            MaxDoasMicrosteps = 16;
        else
            MaxDoasMicrosteps = 1;

        if ((rxBuffer[P5] & 0x0F) == 0)
            ShutterMicrosteps = 1;
        else if ((rxBuffer[P5] & 0x0F) == 1)
            ShutterMicrosteps = 2;
        else if ((rxBuffer[P5] & 0x0F) == 2)
            ShutterMicrosteps = 4;
        else if ((rxBuffer[P5] & 0x0F) == 3)
            ShutterMicrosteps = 8;
        else if ((rxBuffer[P5] & 0x0F) == 7)
            ShutterMicrosteps = 16;
        else
            ShutterMicrosteps = 1;

        logger()->debug(QString("Got Scannermotorsetup: ZenithPos: %1 , ShutterClosePos: %2, MaxDoasMicrosteps: %3, ShutterMicrosteps: %4, Shuttertype: %5").arg(MaxDoasZenithPos).arg(ShutterClosePos).arg(MaxDoasMicrosteps).arg(ShutterMicrosteps).arg(rxBuffer[P6]));
        emit hwdtSigGotMotorSetup(MaxDoasZenithPos,ShutterClosePos,MaxDoasMicrosteps,ShutterMicrosteps,rxBuffer[P6]);
    }
}

// CMD_GET_INFO					0x75

    void THWDriverThread::hwdtSloAskDeviceInfo(){
        const uint Bufferlength = 10;
        char txBuffer[Bufferlength];
        char rxBuffer[Bufferlength];
        for (uint i = 0; i < Bufferlength;i++){
            txBuffer[i] =  0;
            rxBuffer[i] =  0;
        }
        txBuffer[0] =  0;
        txBuffer[1] =  CMD_GET_INFO;

        if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){
#if 0
    //git rev (4Byte)
    //serialnum (3Byte)	//20 bit=guid + 4 bit=type
    data->ProtocolBuffer[P0] = (uint8_t)(0xFF & (GITHASH));
    data->ProtocolBuffer[P1] = (uint8_t)(0xFF & (GITHASH >> 8));
    data->ProtocolBuffer[P2] = (uint8_t)(0xFF & (GITHASH >> 16));
    data->ProtocolBuffer[P3] = (uint8_t)(0xFF & (GITHASH >> 24));


    uint32_t res = ee_getguid();

    data->ProtocolBuffer[P4] = (uint8_t)(0xFF & (res));
    data->ProtocolBuffer[P5] = (uint8_t)(0xFF & (res >> 8));
    data->ProtocolBuffer[P6] = (uint8_t)(0xFF & (res >> 16));
    data->ProtocolBuffer[P6] &= 0x0F;
    data->ProtocolBuffer[P6] |= DEVICE_TYPE << 4;

    task_sendanswer(data->ProtocolBuffer);
#endif

            uint32_t MaxdoasGitHash;
            uint32_t guid;
            uint8_t devicetype;

            MaxdoasGitHash = 0;
            MaxdoasGitHash |= (uint8_t)rxBuffer[P0];
            MaxdoasGitHash |= (uint8_t)rxBuffer[P1]<<8;
            MaxdoasGitHash |= (uint8_t)rxBuffer[P2]<<16;
            MaxdoasGitHash |= (uint8_t)rxBuffer[P3]<<24;

            guid = 0;
            guid |= (uint8_t)rxBuffer[P4];
            guid |= (uint8_t)rxBuffer[P5]<<8;
            guid |= ((uint8_t)rxBuffer[P6] & 0x0F)<<16;




            devicetype = (uint8_t)rxBuffer[P6] >> 4;
            ScannerDeviceType = sdtUnknown;
            if(devicetype == DT_2DSCANNER)
                ScannerDeviceType = sdtWindField;
            if(devicetype == DT_1DSCANNER)
                ScannerDeviceType = sdtMAXDOAS;
            if(devicetype == DT_SOLTRACKER)
                ScannerDeviceType = sdtSolTracker;
            logger()->debug(QString("Got Scannerinfo: Versionshash: %1, DeviceSerialNum: %2, DeviceType: %3").arg(QString::number(MaxdoasGitHash,16)).arg(rawDataToSerialNumber(guid,devicetype)).arg(devicetype));
            emit hwdtSigGotDeviceInfo(MaxdoasGitHash,guid, devicetype);
        }
    }

// CMD_SET_GUID					0x76

void THWDriverThread::hwdtSloSetGUID(int guid){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    uint32_t guid_ = guid;
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_SET_GUID;
#if 0
    guid = data->ProtocolBuffer[P3];
    guid = guid << 8;
    guid |= data->ProtocolBuffer[P2];
    guid = guid << 8;
    guid |= data->ProtocolBuffer[P1];
    guid &= 0x000FFFFF;
#endif
    txBuffer[P1] = (uint8_t)(guid_ & 0xFF);
    txBuffer[P2] = (uint8_t)(guid_ >> 8);
    txBuffer[P3] = (uint8_t)(guid_ >> 16);

    logger()->debug(QString("Write SerialNumbertoDevice: %1").arg(rawDataToSerialNumber(guid,-1)));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){

    }
}

void THWDriverThread::hwdtSloSetMotAccVel(int MaxDoasAcc, int MaxDoasVel, int ShutterAcc, int ShutterVel){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_SETACCELERATION_VEL;
#if 0
    acc = ((data->ProtocolBuffer[P1] & 0x0F) << 8) | data->ProtocolBuffer[P0];
    vel = ((data->ProtocolBuffer[P1] & 0xF0) << 4) | data->ProtocolBuffer[P2];

    MotorDatas.MotMaxdoas->confAcc = acc;
    MotorDatas.MotMaxdoas->confDec = acc;
    MotorDatas.MotMaxdoas->confVel = vel;

    acc = ((data->ProtocolBuffer[P4] & 0x0F) << 8) | data->ProtocolBuffer[P3];
    vel = ((data->ProtocolBuffer[P4] & 0xF0) << 4) | data->ProtocolBuffer[P5];
    MotorDatas.MotShutter->confAcc = acc;
    MotorDatas.MotShutter->confDec = acc;
    MotorDatas.MotShutter->confVel = vel;
#endif
    txBuffer[P0] = (uint8_t)MaxDoasAcc;
    txBuffer[P1] = (uint8_t)(MaxDoasAcc >> 8)&(0x0F);
    txBuffer[P2] = (uint8_t)MaxDoasVel;
    txBuffer[P1] |= (uint8_t)(MaxDoasVel >> 4)&(0xF0);

    txBuffer[P3] = (uint8_t)ShutterAcc;
    txBuffer[P4] = (uint8_t)(ShutterAcc >> 8)&(0x0F);
    txBuffer[P5] = (uint8_t)ShutterVel;
    txBuffer[P4] |= (uint8_t)(ShutterVel >> 4)&(0xF0);

    logger()->debug(QString("Write MotorVelocity: maxdoas acc:%1, maxdoas vel:%2, shutter acc:%3, shutter vel:%4").arg(MaxDoasAcc).arg(MaxDoasVel).arg(ShutterAcc).arg(ShutterVel));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){

    }
}


void THWDriverThread::hwdtSloSetMaxdoasZenithPos(){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_SET_MAXDOAS_ZENITH_POS;

    logger()->debug(QString("Definend Motor Zenith pos"));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){

    }
}

void THWDriverThread::hwdtSloSetStepperShutterClosePos(){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_SET_SHUTTER_CLOSE_POS;

    logger()->debug(QString("Definend Stepper shutter close position"));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){

    }
}


bool THWDriverThread::hwdtSloShutterGoto(int Shutterposition){

#if MOT_ENABLED
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    int32_t PosShutter = Shutterposition;

    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_SHUTTERPOS;

    txBuffer[P4] = PosShutter & 0xFF;
    txBuffer[P5] = (PosShutter >> 8) & 0xFF;
    txBuffer[P6] = (PosShutter >> 16) & 0xFF;
    logger()->debug(QString("Shutter go to: %1").arg(PosShutter));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutMotion,false,true)){
        //emit hwdtSigShutterStateChanged()
        return true;

    }else{
        logger()->error(QString("Moving Shutter failed"));
        //emit hwdtSigMotFailed();
        return false;
    }
#else
    logger()->error(QString("Moving Motor failed"));
    emit hwdtSigMotFailed();
    return false;
#endif
}

QPointF DoTiltMinMaxCalCalculation(QPoint TiltRaw, QPoint Minv,QPoint Maxv,int Gain,int Resolution,float RefV){
    QPointF result;
    QPoint TiltRawOffset = (Minv + Maxv)/2;
    QPoint TiltRawRangeIs = Maxv - Minv;
    QPointF TiltRawRangeShould = QPointF((float)Gain*Resolution/(RefV),(float)Gain*Resolution/(RefV));
    QPointF RawTiltCorrectionfactor;
    if ((TiltRawRangeIs.x()==0)||(TiltRawRangeIs.y()==0)){
        RawTiltCorrectionfactor = QPointF(1,1);
    }else{
        RawTiltCorrectionfactor.setX((float)TiltRawRangeIs.x() / TiltRawRangeShould.x());
        RawTiltCorrectionfactor.setY((float)TiltRawRangeIs.y() / TiltRawRangeShould.y());
    }

    result = TiltRaw - TiltRawOffset;

    result.setX(result.x() / RawTiltCorrectionfactor.x());
    result.setY(result.y() / RawTiltCorrectionfactor.y());

    result = result*2*RefV/((float)Resolution*(float)Gain);

    if(result.x() > 1)   result.setX(1.0);
    if(result.x() < -1)  result.setX(-1.0);

    if(result.y() > 1)   result.setY(1.0);
    if(result.y() < -1)  result.setY(-1.0);
    return result;
}


void THWDriverThread::hwdtSloAskTiltMinValue(){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_GET_DECL_MIN_U;

    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){
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
            TiltRawMin_byDevice.setX(liTiltX);
            TiltRawMin_byDevice.setY(liTiltY);
        }MutexRawTiltPoint.unlock();
        logger()->debug(QString("Tilt Sensor min val X: %1 Y: %2").arg(liTiltX).arg(liTiltY));
        emit hwdtSigGotTiltMinVal(liTiltX,liTiltY,TiltADC_Steps,TiltADC_Gain);
    }
}
// CMD_GET_DECL_MAX_U		0x84
void THWDriverThread::hwdtSloAskTiltMaxValue(){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_GET_DECL_MAX_U;

    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){
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
            TiltRawMax_byDevice.setX(liTiltX);
            TiltRawMax_byDevice.setY(liTiltY);
        }MutexRawTiltPoint.unlock();
        logger()->debug(QString("Tilt Sensor max val X: %1 Y: %2").arg(liTiltX).arg(liTiltY));
        emit hwdtSigGotTiltMaxVal(liTiltX,liTiltY,TiltADC_Steps,TiltADC_Gain);
    }
}



// CMD_GET_DECL_ZENITH_U                   	0x85
void THWDriverThread::hwdtSloAskTiltZenithValue(){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    for (uint i = 0; i < Bufferlength;i++){
        txBuffer[i] =  0;
        rxBuffer[i] =  0;
    }
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_GET_DECL_ZENITH_U;

    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false,true)){
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
            TiltRawZenith_byDevice.setX(liTiltX);
            TiltRawZenith_byDevice.setY(liTiltY);
        }MutexRawTiltPoint.unlock();
        logger()->debug(QString("Tilt Sensor zenith val X: %1 Y: %2").arg(liTiltX).arg(liTiltY));
        emit hwdtSigGotTiltZenith(liTiltX,liTiltY,TiltADC_Steps,TiltADC_Gain);
    }
}

void THWDriverThread::hwdtSloAskTilt()
{
#if 1
    if ((LastMotMovement.elapsed()>TILT_AFTER_MOTMOVE_TIME_OUT)||(ScannerDeviceType != sdtMAXDOAS)){
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
            MotMovedThusNoTiltInfoAnymore = false;
            int32_t liTiltX,liTiltY;
            QPointF TiltCalibrated;
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
                TiltCalibrated = DoTiltMinMaxCalCalculation(TiltRaw, TiltRawMinValue,TiltRawMaxValue,TiltADC_Gain,TiltADC_Steps,TILT_REF_VOLT);


            }
            MutexRawTiltPoint.unlock();

            logger()->debug(QString("Tilt Sensor X: %1 Y: %2").arg(TiltCalibrated.x()).arg(TiltCalibrated.y()));


            emit hwdtSigGotTilt(TiltCalibrated.x(),TiltCalibrated.y(),TiltADC_Steps,TiltADC_Gain);
        }
    }else{
        MutexRawTiltPoint.lockForWrite();
        {
            TiltRaw.setX(100);
            TiltRaw.setY(100);


        }
        MutexRawTiltPoint.unlock();
        emit hwdtSigGotTilt(100,100,TiltADC_Steps,TiltADC_Gain);
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
    MotMovedThusNoTiltInfoAnymore = true;
#if MOT_ENABLED
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
        ScannerStepPos.setX(0);
        ScannerStepPos.setY(0);
        emit hwdtSigMotorIsHome(0,0);
    }else{
        logger()->error(QString("Moving Motor failed"));
        LastMotMovement.start();
        emit hwdtSigMotFailed();
    };
#else
    logger()->error(QString("Moving Motor failed"));
    emit hwdtSigMotFailed();
#endif
}

bool THWDriverThread::SetShutter(THWShutterCMD ShutterCMD){
#if MOT_ENABLED
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
#else
    return false;
#endif
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
                                                QString *SpectrSerial,
                                                TShutterBySwitchState *shutterstate,
                                                float *ScannerTemperature)
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
        *shutterstate = ShutterIsOpenBySwitch;
        *ScannerTemperature = this->ScannerTemperature;
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
            MutexSpectrSerial.lockForRead();
            {
                *SpectrSerial = SpectrometerSerial;
            }
            MutexSpectrSerial.unlock();
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
    if (ScannerDeviceType == sdtMAXDOAS){
        hwdtSloAskScannerStatus();
        //if ( LastMotMovement.elapsed()>1000){
            hwdtSloAskTilt();
        //}
    }
}

//inStep Coordinates!
bool THWDriverThread::hwdtSloMotGotoSteps(int PosX, int PosY){
#if MOT_ENABLED
    MotMovedThusNoTiltInfoAnymore = true;
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    TMirrorCoordinate mc_;
    mc_.setMotorCoordinate(PosX,PosY);
    int32_t PosStationary = PosX;
    int32_t PosMirror =PosY;
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
    logger()->debug(QString("Motors go to. Mirror: %1 Stationary: %2").arg(mc_.getAngleCoordinate().x()).arg(mc_.getAngleCoordinate().y()));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutMotion,false,true)){
        mc->setAngleCoordinate(mc_.getAngleCoordinate());
        ScannerStepPos.setX(PosStationary);
        ScannerStepPos.setY(PosMirror);
        emit hwdtSigMotMoved(PosStationary,PosMirror);
        LastMotMovement.start();
        return true;

    }else{
        logger()->error(QString("Moving Motor failed"));
        emit hwdtSigMotFailed();
        return false;
    }
#else
    logger()->error(QString("Moving Motor failed"));
    emit hwdtSigMotFailed();
    return false;
#endif
}

//inAngle Coordinates!
bool THWDriverThread::hwdtSloMotGoto(float PosX, float PosY){
    TMirrorCoordinate mc_;
    QPointF ac;
    bool ok;
    ac.setX(PosX);
    ac.setY(PosY);
    mc_.setAngleCoordinate(ac);
    ok = hwdtSloMotGotoSteps(mc_.getMotorCoordinate().x(),mc_.getMotorCoordinate().y());
    if (ok){
        mc->setAngleCoordinate(ac);
    }
    return ok;
}

void THWDriverThread::hwdtSloMotIdleState(bool idle){
    const uint Bufferlength = 10;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    MotMovedThusNoTiltInfoAnymore = true;
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
         LastMotMovement.start();

    }else{

    }
}

void THWDriverThread::hwdtSloMeasureScanPixel(float PosX, float PosY ,uint avg, uint integrTime)
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
QString THWDriverThread::getSpectrSerial(){
    QString result;
    MutexSpectrSerial.lockForRead();{
        result = SpectrometerSerial;
    }
    return result;
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
        MutexSpectrSerial.lockForWrite();
        {
            SpectrometerSerial = Serialnumber;
        }MutexSpectrSerial.unlock();
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
    MutexSpectrSerial.lockForWrite();
    {
    SpectrometerSerial = "closed";
    }MutexSpectrSerial.unlock();
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
    GetScannerConfigTimer = new QTimer();
    ActualTilt = new QPointF(0,0);
    TiltMin = new QPoint(0,0);
    TiltMax = new QPoint(0,0);
    TiltZenith = new QPoint(0,0);
    ScannerStepPos = QPoint(0,0);

    GotTiltConfig = false;
    this->ScannerTemperature = 0.0;
    this->ShutterOpenedBySwitch = sssUnknown;
    this->EndSwitchError = eseUnknown;
    ShutterClosePosition = -1;
    ShutterPosition = 0;
    MaxDoasZenithPosition = 0;
    ShutterClosePosition = 0;
    MaxDoasMicrosteps = 1;
    ShutterMicrosteps = 1;
    ShutterType = smtUnknown;

    GitHash = "";
    ScannerSerialNumber = "";
    deviceType = sdtUnknown;

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

        connect(HWDriverObject,SIGNAL(hwdtSigGotTiltConfig()),
                    this,SLOT (hwdtSloGotTiltConfig()),Qt::QueuedConnection);


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

        connect(HWDriverObject,SIGNAL(hwdtSigMotorIsHome(int,int )),
                    this,SLOT (hwdSloMotorIsHome(int, int )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigMotorIsHome(int,int )),
                    this,SIGNAL (hwdSigMotorIsHome(int,int )),Qt::QueuedConnection);

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

        connect(HWDriverObject,SIGNAL(hwdtSigMotMoved(int, int )),
                    this,SLOT (hwdSloMotMoved(int,int )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigMotMoved(int, int )),
                    this,SIGNAL (hwdSigMotMoved(int,int )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigMotFailed( )),
                    this,SIGNAL (hwdSigMotFailed( )),Qt::QueuedConnection);

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

        connect(HWDriverObject,SIGNAL(hwdtSigGotTiltMinVal(int, int,int,int )),
                    this,SLOT (hwdSloGotTiltMinVal(int, int,int,int )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigGotTiltMinVal(int, int,int,int  )),
                    this,SIGNAL (hwdSigGotTiltMinVal(int, int,int,int  )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigGotTiltMaxVal(int, int,int,int )),
                    this,SLOT (hwdSloGotTiltMaxVal(int, int,int,int )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigGotTiltMaxVal(int, int,int,int  )),
                    this,SIGNAL (hwdSigGotTiltMaxVal(int, int,int,int  )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigGotTiltZenith(int, int,int,int )),
                    this,SLOT (hwdSloGotTiltZenith(int, int,int,int )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigGotTiltZenith(int, int,int,int  )),
                    this,SIGNAL (hwdSigGotTiltZenith(int, int,int,int  )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigGotScannerStatus(float, bool,bool )),
                    this,SLOT (hwdSloGotScannerStatus(float, bool,bool )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigGotScannerStatus(float, bool,bool  )),
                    this,SIGNAL (hwdSigGotScannerStatus(float, bool,bool )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigGotMotorSetup(int, int,int,int,int )),
                    this,SLOT (hwdSloGotMotorSetup(int, int,int,int,int )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigGotMotorSetup(int, int,int,int,int )),
                    this,SIGNAL (hwdSigGotMotorSetup(int, int,int,int,int )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigGotDeviceInfo(int, int,int )),
                    this,SLOT (hwdSloGotDeviceInfo(int, int,int  )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigGotDeviceInfo(int, int,int )),
                    this,SIGNAL (hwdSigGotDeviceInfo( int, int,int)),Qt::QueuedConnection);

    }
    //for messages giong from this to thread..
    {
        connect(this,SIGNAL(hwdtSigAskTiltConfig( )),
                    HWDriverObject,SLOT (hwdtSloAskTiltConfig( )),Qt::QueuedConnection);

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

        connect(this,SIGNAL(hwdtSigSetTiltMinMaxCalibration(int,int,int,int)),
                    HWDriverObject,SLOT (hwdtSloSetTiltMinMaxCalibration(int,int,int,int)),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigShutterGoto(int)),
                    HWDriverObject,SLOT (hwdtSloShutterGoto(int)),Qt::QueuedConnection);

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

        connect(this,SIGNAL(hwdtSigMeasureScanPixel(float , float  ,uint , uint)),
                    HWDriverObject,SLOT (hwdtSloMeasureScanPixel(float , float  ,uint , uint)),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtMotGoto(float , float )),
                    HWDriverObject,SLOT (hwdtSloMotGoto(float , float  )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtMotGotoSteps(int , int )),
                    HWDriverObject,SLOT (hwdtSloMotGotoSteps(int , int  )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtMotIdleState(bool )),
                    HWDriverObject,SLOT (hwdtSloMotIdleState(bool)),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigSetMotAccVel(int , int, int, int )),
                    HWDriverObject,SLOT (hwdtSloSetMotAccVel(int , int, int, int  )),Qt::QueuedConnection);




        connect(this,SIGNAL(hwdtSigTiltStartCal( )),
                    HWDriverObject,SLOT (hwdtSloTiltStartCal( )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigTiltStopCal( )),
                    HWDriverObject,SLOT (hwdtSloTiltStopCal()),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigAskTiltMinValue( )),
                    HWDriverObject,SLOT (hwdtSloAskTiltMinValue()),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigAskTiltMaxValue( )),
                    HWDriverObject,SLOT (hwdtSloAskTiltMaxValue()),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigAskTiltZenithValue( )),
                    HWDriverObject,SLOT (hwdtSloAskTiltZenithValue()),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigTiltSetZenith( )),
                    HWDriverObject,SLOT (hwdtSloTiltSetZenith()),Qt::QueuedConnection);



        connect(this,SIGNAL(hwdtSigSetMaxdoasZenithPos( )),
                    HWDriverObject,SLOT (hwdtSloSetMaxdoasZenithPos()),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigSetStepperShutterClosePos( )),
                    HWDriverObject,SLOT (hwdtSloSetStepperShutterClosePos()),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigAskScannerStatus( )),
                    HWDriverObject,SLOT (hwdtSloAskScannerStatus()),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigAskMotorSetup( )),
                    HWDriverObject,SLOT (hwdtSloAskMotorSetup()),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigAskDeviceInfo( )),
                    HWDriverObject,SLOT (hwdtSloAskDeviceInfo()),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigSetGUID(int )),
                    HWDriverObject,SLOT (hwdtSloSetGUID(int)),Qt::QueuedConnection);



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

        connect(GetScannerConfigTimer,SIGNAL(timeout()),
                    this,SLOT (hwdSlotScannerConfigTimer()));

        connect(this,SIGNAL(hwdtQuitThred()),
                            HWDriverObject,SLOT(CloseEverythingForLeaving()),Qt::QueuedConnection);


    }
    HWDriverObject->moveToThread(HWDriverThread);
    HWDriverThread->start();
    connect(HWDriverThread,SIGNAL(finished()),this,SLOT(hwdSlothreadFinished()));
    TemperatureTimer->start(5000);
    TiltTimer->start(5000);
    FetchScannerInfos();
    GetScannerConfigTimer->start(500);
   // hwdSetComPort("/dev/ttyUSB0");
    WavelengthBuffer = TWavelengthbuffer::instance();

    TiltZenithDirection = 0;
   // memset(&SpectrCoefficients,0,sizeof(SpectrCoefficients));
//    SpectrCoefficients.uninitialized = true;
//    SpectrCoefficients.Coeff1 = 1;
}

THWDriver::~THWDriver(){
    //hopefully stop() was called for not deleting a running thread
    delete TiltTimer;
    delete TemperatureTimer;
    delete GetScannerConfigTimer;
    delete ActualTilt;
    delete TiltMin;
    delete TiltMax;
    delete TiltZenith;
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

void THWDriver::hwdSloMotMoved(int StepX, int StepY){
    ScannerStepPos.setX(StepX);
    ScannerStepPos.setY(StepY);
}

QPoint THWDriver::getStepperPos(){
    return ScannerStepPos;
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


void THWDriver::hwdTiltStartCal(){
    emit  hwdtSigTiltStartCal();
}

void THWDriver::hwdTiltStopCal(){
    emit hwdtSigTiltStopCal();
}

void THWDriver::hwdAskTiltMinValue(){
    emit hwdtSigAskTiltMinValue();
}

void THWDriver::hwdAskTiltMaxValue(){
    emit hwdtSigAskTiltMaxValue();
}

void THWDriver::hwdAskTiltZenithValue(){
    emit hwdtSigAskTiltZenithValue();
}

void THWDriver::hwdTiltSetZenith(){
    emit hwdtSigTiltSetZenith();
}

void THWDriver::hwdSetMaxdoasZenithPos(){
    emit hwdtSigSetMaxdoasZenithPos();
}

void THWDriver::hwdSetStepperShutterClosePos(){
    emit hwdtSigSetStepperShutterClosePos();
}

void THWDriver::hwdAskScannerStatus(){
    emit hwdtSigAskScannerStatus();
}

void THWDriver::hwdAskMotorSetup(){
    emit hwdtSigAskMotorSetup();
}

void THWDriver::hwdAskDeviceInfo(){
    emit hwdtSigAskDeviceInfo();
}

void THWDriver::hwdSetGUID(int guid){
    emit hwdtSigSetGUID(guid);
}


void THWDriver::hwdSloGotScannerStatus(float ScannerTemperature,bool ShutterOpenedBySwitch, bool EndSwitchError){
    this->ScannerTemperature = ScannerTemperature;

    if (ShutterOpenedBySwitch){
        this->ShutterOpenedBySwitch = sssOpened;
        ShutterPosition=0;
    }else
        this->ShutterOpenedBySwitch = sssClosed;
    if (EndSwitchError)
        this->EndSwitchError = eseERROR;
    else
        this->EndSwitchError = eseOK;
}

void THWDriver::hwdSloGotMotorSetup(int MaxDoasZenithPosition,int ShutterClosePosition, int MaxDoasMicrosteps, int ShutterMicrosteps, int ShutterType){
    #define ST_DCMOTOR			0x00
    #define ST_STEPPERMOTOR		0x01
    #define ST_SERVOMOTOR		0x02
    GotMotorSetup = true;
    this->MaxDoasZenithPosition = MaxDoasZenithPosition;
    this->ShutterClosePosition = ShutterClosePosition;
    this->MaxDoasMicrosteps = MaxDoasMicrosteps;
    this->ShutterMicrosteps = ShutterMicrosteps;

    switch(ShutterType){
        case ST_DCMOTOR:        this->ShutterType = smtDCMotor;
            break;
        case ST_STEPPERMOTOR:   this->ShutterType = smtStepper;
            break;
        case ST_SERVOMOTOR:     this->ShutterType = smtServo;
            break;
        default:                this->ShutterType = smtUnknown;
    }
    TMaxdoasSettings *ms = TMaxdoasSettings::instance();
    ms->setZenithSteps(QPoint(this->MaxDoasZenithPosition,0));
    ms->setShutterClosePos(this->ShutterClosePosition);
    ms->setMicrostepping(QPoint(this->MaxDoasMicrosteps,this->ShutterMicrosteps));
}

void THWDriver::hwdSloGotDeviceInfo(int GitHash,int guid, int deviceType){
    GotDeviceInfo = true;
    this->GitHash.number(GitHash,16);
    this->ScannerSerialNumber = rawDataToSerialNumber(guid,deviceType);
    switch(deviceType){
        case DT_2DSCANNER:   this->deviceType = sdtWindField;
            break;
        case DT_1DSCANNER:   this->deviceType = sdtMAXDOAS;
            break;
        case DT_SOLTRACKER:  this->deviceType = sdtSolTracker;
            break;
        default:             this->deviceType = sdtUnknown;
    }
    TMaxdoasSettings *ms = TMaxdoasSettings::instance();
    ms->workwiththisserial(ScannerSerialNumber);
    ms->setAttachedScanningDevice(this->deviceType);

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

void THWDriver::FetchScannerInfos()
{
    GotDeviceInfo = false;
    GotMotorSetup = false;
    GotTiltMaxValue = false;
    GotTiltMinValue = false;
    GotTiltZenithValue = false;
}


QPointF THWDriver::hwdGetTilt(void)
{
    return QPointF(*ActualTilt);
}

void THWDriver::hwdSlotScannerConfigTimer(){

    if(!GotDeviceInfo){
        hwdAskDeviceInfo();
        emit hwdtSigSetMotAccVel(100 , 70, 30, 5 );
    }
    if(!GotMotorSetup)
        hwdAskMotorSetup();
    if(!GotTiltMaxValue)
        hwdAskTiltMaxValue();
    if(!GotTiltMinValue)
        hwdAskTiltMinValue();
    if(!GotTiltZenithValue)
        hwdAskTiltZenithValue();
}

void THWDriver::hwdtAskTiltConfig(){
    emit hwdtSigAskTiltConfig();
}


void THWDriver::hwdAskTilt()
{
    if(!GotTiltConfig)
        hwdtAskTiltConfig();
    emit hwdtSigAskTilt();
}

void THWDriver::hwdSlotTiltTimer(){
    hwdAskTilt();

}

float CalcTiltDirection(float TiltX, float TiltY){
    if (TiltX > 1)
        return NAN;
    float TiltDirection = atan(TiltX/TiltY)*180/M_PI;
    if((TiltY < 0))
        if(TiltX > 0)
            TiltDirection += 180;
        else
            TiltDirection -= 180;
    else
        TiltDirection += 00;
    return TiltDirection;
}

void THWDriver::hwdSloGotTilt(float TiltX, float TiltY, int Resolution , int Gain ){
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

    QPointF TiltCalibrated = DoTiltMinMaxCalCalculation(*TiltZenith,*TiltMin,*TiltMax,Gain,Resolution,TILT_REF_VOLT);

    TiltZenithDirection = CalcTiltDirection(TiltCalibrated.x(),TiltCalibrated.y());
    //ms->setTiltZenithDirection(TiltZenithDirection);

    TiltDirection = CalcTiltDirection(TiltX,TiltY)-TiltZenithDirection;
    while(TiltDirection < -180.0)
        TiltDirection+=360;

    while(TiltDirection > 180.0)
        TiltDirection-=360;


    if (TiltX > 1){
        TiltX = NAN;
        TiltY = NAN;
    }else{
        TiltX=asin((float)TiltX)*180/M_PI;
        TiltY=asin((float)TiltY)*180/M_PI;
    }
    ActualTilt->setX(TiltX);
    ActualTilt->setY(TiltY);
    emit hwdSigGotTiltDirection(TiltDirection);
    emit hwdSigGotTilt(ActualTilt->x(),ActualTilt->y(),Gain,Resolution,Border,MaxTilt);
}


void THWDriver::hwdSloGotTiltMinVal(int TiltX, int TiltY, int Resolution, int Gain){
    (void)Gain;
    (void)Resolution;
    GotTiltMinValue = true;
    TiltMin->setX(TiltX);
    TiltMin->setY(TiltY);
    TMaxdoasSettings *ms = TMaxdoasSettings::instance();
    ms->setTiltMinValue(QPoint(TiltX,TiltY));
    QPointF TiltCalibrated = DoTiltMinMaxCalCalculation(*TiltZenith,*TiltMin,*TiltMax,Gain,Resolution,TILT_REF_VOLT);

    TiltZenithDirection = CalcTiltDirection(TiltCalibrated.x(),TiltCalibrated.y());
    //ms->setTiltZenithDirection(TiltZenithDirection);
}

void THWDriver::hwdSloGotTiltMaxVal(int TiltX, int TiltY, int  Resolution , int Gain){
    (void)Gain;
    (void)Resolution;
    GotTiltMaxValue = true;
    TiltMax->setX(TiltX);
    TiltMax->setY(TiltY);
    TMaxdoasSettings *ms = TMaxdoasSettings::instance();
    ms->setTiltMaxValue(QPoint(TiltX,TiltY));
    QPointF TiltCalibrated = DoTiltMinMaxCalCalculation(*TiltZenith,*TiltMin,*TiltMax,Gain,Resolution,TILT_REF_VOLT);

    TiltZenithDirection = CalcTiltDirection(TiltCalibrated.x(),TiltCalibrated.y());
    //ms->setTiltZenithDirection(TiltZenithDirection);
}

void THWDriver::hwdSloGotTiltZenith(int TiltX, int TiltY, int Resolution , int Gain){
    (void)Gain;
    (void)Resolution;
    GotTiltZenithValue = true;
    TiltZenith->setX(TiltX);
    TiltZenith->setY(TiltY);
    TMaxdoasSettings *ms = TMaxdoasSettings::instance();
    ms->setTiltZenith(QPoint(TiltX,TiltY));
    QPointF TiltCalibrated = DoTiltMinMaxCalCalculation(*TiltZenith,*TiltMin,*TiltMax,Gain,Resolution,TILT_REF_VOLT);

    TiltZenithDirection = CalcTiltDirection(TiltCalibrated.x(),TiltCalibrated.y());
    //ms->setTiltZenithDirection(TiltZenithDirection);
}

void THWDriver::hwdtSloGotTiltConfig()
{
    GotTiltConfig = true;
}

QPoint THWDriver::hwdGetRawTilt(){
    return HWDriverObject->hwdtGetLastRawTilt();
}

void THWDriver::hwdSetTiltMinMaxCalib(QPoint Min, QPoint Max)
{
    emit hwdtSigSetTiltMinMaxCalibration(Min.x(),Min.y(),Max.x(),Max.y());
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
    if (ShutterCMD == scClose)
        ShutterPosition = ShutterClosePosition;
    if (ShutterCMD == scOpen)
        ShutterPosition = 0;

    emit hwdtSigSetShutter(ShutterCMD);
}

void THWDriver::hwdSetShuttPos(int Shutterpos){
    ShutterPosition = Shutterpos;
    emit hwdtSigShutterGoto(Shutterpos);
}

void THWDriver::hwdMeasureScanPixel(QPointF AngleCoordinate,uint avg, uint integrTime)
{
    emit hwdtSigMeasureScanPixel(AngleCoordinate.x(),AngleCoordinate.y(),avg,integrTime);
}

void THWDriver::hwdMotMove(QPointF AngleCoordinate)
{
    emit hwdtMotGoto(AngleCoordinate.x(),AngleCoordinate.y());
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
//    if (!WlCoefficients->uninitialized){
//        if (WavelengthBuffer != NULL){
//            for (uint i = 0;i<MAXWAVELEGNTH_BUFFER_ELEMTENTS;i++){
//                WavelengthBuffer->buf[i] = WlCoefficients->Offset+
//                                           (double)i*WlCoefficients->Coeff1+
//                                           pow((double)i,2.0)*WlCoefficients->Coeff2+
//                                           pow((double)i,3.0)*WlCoefficients->Coeff3;
//            }
//        }
//        memcpy(&this->SpectrCoefficients,WlCoefficients,sizeof(TSPectrWLCoefficients));
//        this->SpectrCoefficients.overWrittenFromFile = true;
//        this->SpectrCoefficients.uninitialized = false;
//        WavelengthBuffer->setCoefficients(WlCoefficients);
//    }
    WlCoefficients->overWrittenFromFile = true;
    WavelengthBuffer->setCoefficients(WlCoefficients);
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
                                              &Spectrum->SpectrometerSerialNumber,
                                              &Spectrum->ShutterState,
                                              &Spectrum->ScannerTemperature);


    Spectrum->setMirrorCoordinate(&mc);
    memcpy(&Spectrum->IntegConf,&IntegTimeConf,sizeof(TAutoIntegConf));
    memcpy(&Spectrum->WLCoefficients,&coef,sizeof(TSPectrWLCoefficients));
    Spectrum->Tilt = *ActualTilt;
    Spectrum->Temperature = (Temperatures[sensorIDtoInt(tsPeltier)]+Temperatures[sensorIDtoInt(tsSpectrometer)])/2;
    Spectrum->TiltDirection = TiltDirection;
    WavelengthBuffer->setCoefficients(&coef);
//    if (SpectrCoefficients.uninitialized){
//        memcpy(&SpectrCoefficients,&coef,sizeof(TSPectrWLCoefficients));
//        if (WavelengthBuffer != NULL){
//            for (uint i = 0;i<MAXWAVELEGNTH_BUFFER_ELEMTENTS;i++){
//                WavelengthBuffer->buf[i] = SpectrCoefficients.Offset+
//                                           (double)i*SpectrCoefficients.Coeff1+
//                                           pow((double)i,2.0)*SpectrCoefficients.Coeff2+
//                                           pow((double)i,3.0)*SpectrCoefficients.Coeff3;
//            }
//        }
//    }

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

QString THWDriver::getSpectrSerial(){
    return HWDriverObject->getSpectrSerial();
}

QList<QString> THWDriver::hwdGetListSpectrometer()
{

  //    emit hwdtQuitThred();
    emit hwdtSigDiscoverSpectrometers();
    return HWDriverObject->hwdtGetSpectrometerList();
}

void THWDriver::hwdOpenSpectrometer(QString SerialNumber)
{
    WavelengthBuffer->setUnintialised();
    //SpectrCoefficients.uninitialized = true;
    emit hwdtSigOpenSpectrometer(SerialNumber);
}

void THWDriver::hwdCloseSpectrometer()
{
    emit hwdtSigCloseSpectrometer();
}


void THWDriver::hwdSloMotorIsHome(int x, int y)
{
    ScannerStepPos = QPoint(x,y);
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

QString THWDriver::getFirmwarehash(){
    return GitHash;
}

QPointF THWDriver::getRawTiltMin(){
    return *TiltMin;
}

QPointF THWDriver::getRawTiltMax(){
    return *TiltMax;
}

int THWDriver::hwdGetShuttPos(){
    return ShutterPosition;
}

void THWDriver::hwdMotMoveBySteps(QPoint MotCoordinates){
    emit hwdtMotGotoSteps(MotCoordinates.x(),MotCoordinates.y());
}
