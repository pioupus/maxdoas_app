#include "thwdriver.h"
#include "crc8.h"
#include <QString>
#include <QTime>
#include <QWaitCondition>
#include <stdint.h>
#include <qmath.h>
#include <JString.h>

#define P0	2
#define P1	3
#define P2	4
#define P3	5
#define P4	6
#define P5	7
#define P6	8

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
const uint TimeOutData  =  30; //ms
const uint TimeOutMotion  =  10000; //ms



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


    TiltADC_Steps = 1<<12;  //lets calculate with 12 bits;
    TiltADC_Gain = 1;  //1,2,4,8 Gains available
    TiltADC_RefVoltage = 2.5;
    TiltCalValNegGX = -1;
    TiltCalValPosGX = 1;
    TiltCalValNegGY = -1;
    TiltCalValPosGY = 1;

    //HeadingOffset = 0;

    LastShutterCMD = scNone;
    wrapper = new Wrapper();
    NumberOfSpectrometers = 0;
    SpectrometerIndex = -1;
    NumOfPixels = 0;
    MutexSpectrBuffer.lockForWrite();
    {
        SpectrMinIntegTime = -1;
        SpectrMaxIntensity = -1;
    }
    MutexSpectrBuffer.unlock();
    SpectrometerList = new QList<QString>;
    IntegTimeConf.autoenabled = false;
    IntegTimeConf.fixedIntegtime = 10000;
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
    if (serial)
        serial->setDeviceName(name);
    hwdtSloSerOpenClose(true);
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
    if (serial){
        bool err = false;
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
}

bool THWDriverThread::sendBuffer(char *TXBuffer,char *RXBuffer,uint size,uint timeout , bool TempCtrler){
    QString trace;
    if (serial){
        if (serial->isOpen()){
            TXBuffer[size-1] = crc8(TXBuffer,size-1);
            if (TempCtrler){//lets keep the direction pin low -> controller will toggle direction
                serial->setRts(false);
                trace = "RTS cleared; ";
            }else{//for motion controll we have to controll direction pin
                serial->setRts(true);//for sending
                trace = "RTS set; ";
            }
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
            return waitForAnswer(TXBuffer,RXBuffer,size,timeout,TempCtrler);
        }else{
            logger()->error("Try to use COM-Port, but it's closed");
            return false;
        }
    }
    logger()->error("HWClass not initialized(serial == NULL)");
    return false;
}

bool THWDriverThread::waitForAnswer(char *TXBuffer,char *RXBuffer,uint size,int timeout , bool TempCtrler){
    QTime timer;
    bool TransmissionOK = false;
    bool TransmissionError = false;
    uint RetransmissionCounter = 0;
    if (serial){
        uint BufferIndex = 0;
        timer.start();
        if (!TempCtrler){//for motion controller we have to controll dir pin
            while(timer.elapsed() <= 5){//lets be sure that send buffer sent completly

            }
            serial->setRts(false);//for receiving
            logger()->trace("RTS cleared");
        }
        timer.restart();
        do{
            while((timer.elapsed() <= 10) || (!TransmissionOK)){
                if (serial->bytesAvailable() > 0)  {
                    serial->read(&RXBuffer[0], 1);
                    if ((unsigned char)RXBuffer[0] == CMD_TRNSMIT_OK){
                        TransmissionOK = true;
                    }else{
                        emit hwdtSigTransferDone(tsCheckSumError,RetransmissionCounter);
                        logger()->warn(QString("Transmission: Checksum error! tries: %1 ").arg(RetransmissionCounter));
                        break;
                    }
                }
            }
            if (!TransmissionOK){
                if (timer.elapsed() > 10){
                    emit hwdtSigTransferDone(tsTimeOut,RetransmissionCounter);
                    logger()->warn(QString("Transmission: Timeout error! tries: %1 ").arg(RetransmissionCounter));
                }
                //lets retransmit
                serial->write(TXBuffer,size);
                RetransmissionCounter++;
                if (RetransmissionCounter > 10){
                    TransmissionError = true;
                    logger()->error(QString("Transmission error tries: %1").arg(RetransmissionCounter));
                }
            }
        }while(!TransmissionOK && !TransmissionError);

        if (TransmissionOK){
            timer.restart();
            while((timer.elapsed() <= timeout) || (BufferIndex < 9)){
                if (serial->bytesAvailable() > 0)  {
                    serial->read(&RXBuffer[BufferIndex], 1);
                    BufferIndex++;
                }
            }
            TransmissionOK = BufferIndex == 9;
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
        return TransmissionOK;
    }
    logger()->error("HWClass not initialized(serial == NULL)");
    return false;
}



float THWDriverThread::sensorTempToCelsius(short int Temperature){
    return Temperature;
}

short int THWDriverThread::CelsiusToSensorTemp(float Temperature){
    return Temperature;
}

void THWDriverThread::hwdtSloAskTemperature(THWTempSensorID sensorID, bool byTimer)
{
    const uint Bufferlength = 9;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    logger()->debug(QString("Asking for Temperature. SensorID: %1").arg(sensorIDtoInt(sensorID)));
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_TEMP_READ_SENSORID;
    txBuffer[P1] =  sensorIDtoInt(sensorID);
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,true)){
        int16_t siTemperature;
        float Temperature;
        siTemperature = rxBuffer[P2];
        siTemperature += rxBuffer[P1]>>8;
        Temperature =  sensorTempToCelsius(siTemperature);
        logger()->debug(QString("Temperature of Sensor ID: %1 is %2 C").arg(sensorIDtoInt(sensorID)).arg(Temperature));
        emit hwdtSigGotTemperature(sensorID, sensorTempToCelsius(Temperature),byTimer);
    }

}

void THWDriverThread::hwdtSloSetTargetTemperature(float temperature)
{
    const uint Bufferlength = 9;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    logger()->debug(QString("Setting target temperature. Temperature: %1 C").arg(temperature));
    int16_t siTemperature = CelsiusToSensorTemp(temperature);
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_TEMP_WRITE_SHOULD_TEMP;
    txBuffer[P1] = (siTemperature >> 8) & 0xFF;
    txBuffer[P2] = siTemperature & 0xFF;
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,true)){

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

void THWDriverThread::hwdtSloOffsetTilt(float TiltX, float TiltY){
    TiltX = sin(TiltX);
    TiltY = sin(TiltY);
    TiltCalValPosGX = 1 + TiltX;
    TiltCalValNegGX = -1 + TiltX;
    TiltCalValPosGY = 1 + TiltY;
    TiltCalValNegGY = -1 + TiltY;
}


void THWDriverThread::hwdtSloConfigTilt(TTiltConfigRes Resolution,TTiltConfigGain Gain){
    const uint Bufferlength = 9;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_INCLIN_SET_INIT;
    txBuffer[P1] = TiltGainToConfBit(Gain);//gain
    txBuffer[P2] = TiltResolutionToConfBit(Resolution);//resolution
    logger()->debug(QString("Configuring Tilt Sensor. Resolution: %1 bits, Gain: %2").arg((int)Resolution).arg((int)Gain));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false)){
        TiltADC_Gain = (int) Gain;
        TiltADC_Steps = 1 << (int) Resolution;
    }
}

void THWDriverThread::hwdtSloAskTilt()
{

    const uint Bufferlength = 9;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_INCLIN_READ;
    logger()->debug("Asking Tilt Sensor");
    logger()->trace(QString("Tilt sensor Resolution: %1 steps, Gain: %2").arg((int)TiltADC_Steps).arg((int)TiltADC_Gain));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false)){
        float TiltX,TiltY;
        float XCalOffset,YCalOffset;
        float XCalGain,YCalGain;
        int32_t liTiltX,liTiltY;

        //canal 0
        liTiltX = rxBuffer[P1];
        liTiltX += rxBuffer[P2] << 8;
        liTiltX += rxBuffer[P3] << 16;
        if(liTiltX & 0x00800000)//sign bit set
            liTiltX |= 0xFF000000;
        //canal 1
        liTiltY = rxBuffer[P4];
        liTiltY += rxBuffer[P5] << 8;
        liTiltY += rxBuffer[P6] << 16;
        if(liTiltY & 0x00800000)//sign bit set
            liTiltY |= 0xFF000000;
        logger()->trace(QString("Tilt sensor rxed X: %1 Y: %2").arg(liTiltY).arg(liTiltX));
        //steps are with including sign bit -> range is -refvolt till +refvolt -> factor 2
        TiltX = 2*TiltADC_RefVoltage*(float)liTiltX/((float)TiltADC_Steps*(float)TiltADC_Gain);
        TiltY = 2*TiltADC_RefVoltage*(float)liTiltY/((float)TiltADC_Steps*(float)TiltADC_Gain);
        logger()->trace(QString("Tilt g calced X: %1 Y: %2" ).arg(TiltX).arg(TiltY));

        XCalOffset  = (TiltCalValPosGX + TiltCalValNegGX)/2;
        XCalGain    = (TiltCalValPosGX - TiltCalValNegGX)/2;
        YCalOffset  = (TiltCalValPosGY + TiltCalValNegGY)/2;
        YCalGain    = (TiltCalValPosGY - TiltCalValNegGY)/2;
        logger()->trace(QString("Tilt g OffsetError X: %1 Y: %2").arg(XCalOffset).arg(YCalOffset));
        logger()->trace(QString("Tilt g GainError X: %1 Y %2" ).arg(XCalGain).arg(YCalGain));
        TiltX = XCalGain*TiltX+XCalOffset;
        TiltY = YCalGain*TiltY+YCalOffset;

        if(TiltX > 1)
            TiltX = 1.0;
        if(TiltX < -1)
            TiltX = -1.0;

        if(TiltY > 1)
            TiltY = 1.0;
        if(TiltY < -1)
            TiltY = -1.0;

        TiltX=asin((float)TiltX);
        TiltY=asin((float)TiltY);
        logger()->debug(QString("Tilt Sensor X: %1 Y: %2").arg(TiltX).arg(TiltY));
        emit hwdtSigGotTilt(TiltX,TiltY);
    }
}

void THWDriverThread::hwdtSloAskCompass()
{


    const uint Bufferlength = 9;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_COMPASS_READ;
    logger()->debug("Asking Compass");
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false)){

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
    const uint Bufferlength = 9;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_COMPASS_CAL_ENTER;
    logger()->debug("Compass Calibration start");
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false)){
        emit hwdtSigCompassStartedCalibrating();
    }
}

void THWDriverThread::hwdtSloStopCompassCal()
{
    const uint Bufferlength = 9;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_COMPASS_CAL_EXIT;
    logger()->debug("Compass Calibration stop");
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false)){
        emit hwdtSigCompassStoppedCalibrating();
    }
}

void THWDriverThread::hwdtSloConfigLightSensor(TLightSensorGain Gain, TLightSensorIntegTime LightSensorIntegTime){
    int g;
    float integ=0;
    const uint Bufferlength = 9;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
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
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false)){
        LightSensorGain = Gain;
        this->LightSensorIntegTime = LightSensorIntegTime;
    }
}

void THWDriverThread::hwdtSloAskLightSensor(){
    const uint Bufferlength = 9;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_LIGHT_READ;
    logger()->debug("Ask Light Sensor");
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false)){
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
    const uint Bufferlength = 9;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    txBuffer[0] = 0;
    txBuffer[1] = CMD_MOT_CALIBRATION;
    logger()->debug("Calibrate Motor");
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutMotion,false)){
        emit hwdtSigCompassStoppedCalibrating();
    }
}

bool THWDriverThread::SetShutter(THWShutterCMD ShutterCMD){
    const uint Bufferlength = 9;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    txBuffer[0] =  0;
    if ((ShutterCMD == scNone) || (LastShutterCMD == ShutterCMD)){
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
        if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutMotion,false)){
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
void THWDriverThread::hwdtGetLastSpectrumBuffer(double *Spectrum, int *NumberOfSpecPixels, TSPectrWLCoefficients *SpectrCoefficients ,uint size, double *MaxPossibleValue)
{
    uint i = NumOfPixels;
    if (size < i)
        i = size;
    MutexSpectrBuffer.lockForRead();
    {
        memcpy(Spectrum,LastSpectr,sizeof(double)*i);
        *MaxPossibleValue = SpectrMaxIntensity;
        *NumberOfSpecPixels = NumOfPixels;
        memcpy(SpectrCoefficients,&(this->SpectrCoefficients),sizeof(TSPectrWLCoefficients));
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

    double *spectrpointer;
    int i,n;//,m;
    JString s;
    logger()->debug("Fetch Spectra");
    #if OMNI_ENABLED
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
        logger()->error("Trying to read from Spectrometer even though its not opened yet.");
    }
#else
#endif
}

void THWDriverThread::hwdtSloMeasureScanPixel(int PosX, int PosY ,uint avg, uint integrTime)
{
    const uint Bufferlength = 9;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    int32_t PosStationary = PosX;
    int32_t PosMirror = PosY;
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_RAMP;

    txBuffer[P1] = PosMirror & 0xFF;
    txBuffer[P2] = (PosMirror >> 8) & 0xFF;
    txBuffer[P3] = (PosMirror >> 16) & 0xFF;

    txBuffer[P4] = PosStationary & 0xFF;
    txBuffer[P5] = (PosStationary >> 8) & 0xFF;
    txBuffer[P6] = (PosStationary >> 16) & 0xFF;
    logger()->debug(QString("Motors go to. Mirror: %1 Stationary: %2").arg(PosX).arg(PosY));
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutMotion,false)){
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
            emit hwdtSigGotSpectrum();
        }
        if (SetShutter(shutterCMDtmp)){
            emit hwdtSigShutterStateChanged(shutterCMDtmp);
        }
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

void THWDriverThread::hwdtSloOpenSpectrometer(QString Serialnumber)
{
#if OMNI_ENABLED
    newOmniWrapper();
    MutexSpectrBuffer.lockForWrite();
    {
        SpectrMinIntegTime = -1;
        SpectrMaxIntensity = -1;
    }
    MutexSpectrBuffer.unlock();
    SpectrometerIndex = SpectrometerList->indexOf(Serialnumber);
    if (SpectrometerIndex == -1)
        hwdtSloDiscoverSpectrometers();
    SpectrometerIndex = SpectrometerList->indexOf(Serialnumber);
    if (SpectrometerIndex > -1){
        Coefficients Coef;
        LastSpectrIntegTime = -1;
        MutexSpectrBuffer.lockForWrite();
        {
            NumOfPixels = wrapper->getNumberOfPixels(SpectrometerIndex,0);
            SpectrMinIntegTime = wrapper->getNumberOfPixels(SpectrometerIndex,0);
            SpectrMaxIntensity = wrapper->getMaximumIntensity(SpectrometerIndex);
            SpectrMinIntegTime = wrapper->getMinimumIntegrationTime(SpectrometerIndex);
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
#endif
}

void THWDriverThread::hwdtSloCloseSpectrometer()
{
    MutexSpectrBuffer.lockForWrite();
    {
        LastSpectrIntegTime = -1;
        SpectrometerIndex = -1;
        SpectrMinIntegTime = -1;
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
    ActualTilt = new QPointF(100,100);
    HWDriverObject = new THWDriverThread();
    CompassState = csNone;
    CompassHeading = INVALID_COMPASS_HEADING;
    CompassOffset = 0;
    LightSensorVal = INVALID_LIGHTSENSOR_VAL;
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
        connect(HWDriverObject,SIGNAL(hwdtSigGotTemperature(THWTempSensorID , float ,bool)),
                    this,SLOT (hwdSloGotTemperature(THWTempSensorID , float ,bool)),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigGotTemperature(THWTempSensorID , float, bool )),
                    this,SIGNAL (hwdSigGotTemperature(THWTempSensorID , float )),Qt::QueuedConnection);

        connect(HWDriverObject,SIGNAL(hwdtSigGotTilt(float,float)),
                    this,SLOT (hwdSloGotTilt(float,float)),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigGotTilt(float,float)),
                    this,SIGNAL (hwdSigGotTilt(float,float)),Qt::QueuedConnection);

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

        connect(this,SIGNAL(hwdtSigOffsetTilt(float,float)),
                    HWDriverObject,SLOT (hwdtSloOffsetTilt(float,float)),Qt::QueuedConnection);

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

        connect(this,SIGNAL(hwdtQuitThred()),
                            HWDriverObject,SLOT(CloseEverythingForLeaving()),Qt::QueuedConnection);


    }
    HWDriverObject->moveToThread(HWDriverThread);
    HWDriverThread->start();
    connect(HWDriverThread,SIGNAL(finished()),this,SLOT(hwdSlothreadFinished()));
    TemperatureTimer->start(200);
   // hwdSetComPort("/dev/ttyUSB0");
    WavelengthBuffer = TWavelengthbuffer::instance();
    SpectrCoefficients.uninitialized = true;
}

THWDriver::~THWDriver(){
    //hopefully stop() was called for not deleting a running thread
    HWDriverObject->deleteLater();
    HWDriverThread->deleteLater();
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


void THWDriver::hwdSetComPort(QString name)
{
    emit hwdtSigSetComPort(name);
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

void THWDriver::hwdSloGotTemperature(THWTempSensorID sensorID, float Temperature, bool byTimer)
{
    int i = sensorIDtoInt(sensorID);
    Temperatures[i] = Temperature;
    if (byTimer){
        if (sensorID != tsSpectrometer){
            if (sensorID == tsHeatSink)
                sensorID = tsSpectrometer;
            if (sensorID == tsPeltier)
                sensorID = tsHeatSink;
            emit hwdtSigAskTemperature(sensorID,byTimer);
        }
    }
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

void THWDriver::hwdSloGotTilt(float TiltX,float TiltY){
    ActualTilt->setX(TiltX);
    ActualTilt->setY(TiltY);
}

void THWDriver::hwdSetTiltZero()
{
    emit hwdtSigOffsetTilt(ActualTilt->x(),ActualTilt->y());
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

uint THWDriver::hwdGetSpectrum(TSpectrum *Spectrum)
{
    TSPectrWLCoefficients coef;
    HWDriverObject->hwdtGetLastSpectrumBuffer(Spectrum->spectrum,
                                              &Spectrum->NumOfSpectrPixels,
                                              &coef,
                                              MAXWAVELEGNTH_BUFFER_ELEMTENTS,
                                              &Spectrum->MaxPossibleValue
                                              );
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
