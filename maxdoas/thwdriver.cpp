#include "thwdriver.h"
#include "crc8.h"
#include <QString>
#include <QTime>
#include <QWaitCondition>
#include <stdint.h>
#include <qmath.h>
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
    serial = new AbstractSerial(this);
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
        qDebug() << "Serial device by default: " << serial->deviceName() << " open fail.";
    }
    if (!serial->setBaudRate(AbstractSerial::BaudRate19200)) {
        qDebug() << "Set baud rate " <<  AbstractSerial::BaudRate19200 << " error.";
    };
    if (!serial->setDataBits(AbstractSerial::DataBits8)) {
        qDebug() << "Set data bits " <<  AbstractSerial::DataBits8 << " error.";
    }
    if (!serial->setParity(AbstractSerial::ParityNone)) {
        qDebug() << "Set parity " <<  AbstractSerial::ParityNone << " error.";
    }
    if (!serial->setStopBits(AbstractSerial::StopBits1)) {
        qDebug() << "Set stop bits " <<  AbstractSerial::StopBits1 << " error.";
    }
    if (!serial->setFlowControl(AbstractSerial::FlowControlOff)) {
        qDebug() << "Set flow " <<  AbstractSerial::FlowControlOff << " error.";
    }
    TiltADC_Steps = 1<<12;  //lets calculate with 12 bits;
    TiltADC_Gain = 1;  //1,2,4,8 Gains available
    TiltADC_RefVoltage = 2.5;
    TiltCalValNegGX = -1;
    TiltCalValPosGX = 1;
    TiltCalValNegGY = -1;
    TiltCalValPosGY = 1;

    HeadingOffset = 0;

    LastShutterCMD = scNone;
}

void THWDriverThread::hwdtSloSetComPort(QString name)
{
    serial->setDeviceName(name);
}

void THWDriverThread::hwdtSloSetBaud(AbstractSerial::BaudRate baud)
{
    if (!serial->setBaudRate(baud)) {
        qDebug() << "Set baud rate " <<  baud << " error.";
    };
}

bool THWDriverThread::sendBuffer(char *TXBuffer,char *RXBuffer,uint size,uint timeout , bool TempCtrler){
    TXBuffer[size-1] = crc8(TXBuffer,size-1);
    if (TempCtrler){//lets keep the direction pin low -> controller will toggle direction
        serial->setRts(false);
    }else{//for motion controll we have to controll direction pin
        serial->setRts(true);//for sending
    }
    serial->write(TXBuffer,size);
    return waitForAnswer(TXBuffer,RXBuffer,size,timeout,TempCtrler);
}

bool THWDriverThread::waitForAnswer(char *TXBuffer,char *RXBuffer,uint size,int timeout , bool TempCtrler){
    QTime timer;
    bool TransmissionOK = false;
    bool TransmissionError = false;
    uint RetransmissionCounter = 0;

    uint BufferIndex = 0;
    timer.start();
    if (!TempCtrler){//for motion controller we have to controll dir pin
        while(timer.elapsed() <= 5){//lets be sure that send buffer sent completly

        }
        serial->setRts(false);//for receiving
    }
    timer.restart();
    do{
        while((timer.elapsed() <= 10) || (!TransmissionOK)){
            if (serial->bytesAvailable() > 0)  {
                serial->read(&RXBuffer[0], 1);
                if ((unsigned char)RXBuffer[0] == CMD_TRNSMIT_OK)
                    TransmissionOK = true;
            }
        }
        if (!TransmissionOK){
            //lets retransmit
            serial->write(TXBuffer,size);
            RetransmissionCounter++;
            if (RetransmissionCounter > 10)
                TransmissionError = true;
        }
    }while(!TransmissionOK || !TransmissionError);

    if (TransmissionOK){
        timer.restart();
        while((timer.elapsed() <= timeout) || (BufferIndex < 9)){
            if (serial->bytesAvailable() > 0)  {
                serial->read(&RXBuffer[BufferIndex], 1);
                BufferIndex++;
            }
        }
        TransmissionOK = BufferIndex == 9;
    }
    return TransmissionOK;
}


void THWDriverThread::hwdtGetLastSpectrumBuffer(double *Spectrum, uint size)
{
    int i = SpectrBufferSize;
    if (size < SpectrBufferSize)
        i = size;
    MutexSpectrBuffer.lockForRead();
    memcpy(Spectrum,LastSpectr,sizeof(double)*i);
    MutexSpectrBuffer.unlock();
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
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_TEMP_READ_SENSORID;
    txBuffer[P1] =  sensorIDtoInt(sensorID);
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,true)){
        int16_t Temperature;
        Temperature = rxBuffer[P2];
        Temperature += rxBuffer[P1]>>8;
        emit hwdtSigGotTemperature(sensorID, sensorTempToCelsius(Temperature),byTimer);
    }

}

void THWDriverThread::hwdtSloSetTargetTemperature(float temperature)
{
    const uint Bufferlength = 9;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
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

        //steps are with including sign bit -> range is -refvolt till +refvolt -> factor 2
        TiltX = 2*TiltADC_RefVoltage*(float)liTiltX/((float)TiltADC_Steps*(float)TiltADC_Gain);
        TiltY = 2*TiltADC_RefVoltage*(float)liTiltY/((float)TiltADC_Steps*(float)TiltADC_Gain);

        XCalOffset  = (TiltCalValPosGX + TiltCalValNegGX)/2;
        XCalGain    = (TiltCalValPosGX - TiltCalValNegGX)/2;
        YCalOffset  = (TiltCalValPosGY + TiltCalValNegGY)/2;
        YCalGain    = (TiltCalValPosGY - TiltCalValNegGY)/2;

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
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false)){

        float Heading;
        int16_t siHeading;

        siHeading = rxBuffer[P1];
        siHeading += rxBuffer[P2] << 8;

        Heading = siHeading;
        Heading /= 10;
        Heading += HeadingOffset;

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
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutData,false)){
        emit hwdtSigCompassStoppedCalibrating();
    }
}

void THWDriverThread::hwdtSloConfigLightSensor(TLightSensorGain Gain, TLightSensorIntegTime LightSensorIntegTime){
    const uint Bufferlength = 9;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    txBuffer[0] =  0;
    txBuffer[1] =  CMD_MOT_LIGHT_INIT;
    txBuffer[P1] =  1; //switch on
    txBuffer[P2] =  (int)Gain; //switch on
    txBuffer[P3] =  (int)LightSensorIntegTime; //switch on
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
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutMotion,false)){
        emit hwdtSigCompassStoppedCalibrating();
    }
}

bool THWDriverThread::SetShutter(THWShutterCMD ShutterCMD){
    const uint Bufferlength = 9;
    char txBuffer[Bufferlength];
    char rxBuffer[Bufferlength];
    txBuffer[0] =  0;
    if (ShutterCMD == scClose){
        txBuffer[1] =  CMD_MOT_CLOSESHUTTER;
    }else{
        txBuffer[1] =  CMD_MOT_OPENSHUTTER;
    }
    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutMotion,false)){
        return true;
    }else{
        return false;
    }
}

void THWDriverThread::hwdtSloSetShutter(THWShutterCMD ShutterCMD)
{
    if (SetShutter(ShutterCMD)){
        emit hwdtSigShutterStateChanged(ShutterCMD);
        LastShutterCMD = ShutterCMD;
    }
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

    if (sendBuffer(txBuffer,rxBuffer,Bufferlength,TimeOutMotion,false)){
        //now we should measure;
        (void)avg;
        (void)integrTime;
        emit hwdtSigScanPixelMeasured();
    }
}


void THWDriverThread::hwdtSloMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD)
{

    if (SetShutter(shutterCMD)){
        emit hwdtSigShutterStateChanged(shutterCMD);
        //Measure Spectr;
        (void)avg;
        (void)integrTime;
        if (SetShutter(LastShutterCMD)){
            emit hwdtSigShutterStateChanged(LastShutterCMD);
        }
    }
}

void THWDriverThread::hwdtSloAskWLCoefficients()
{

}

void THWDriverThread::hwdtSloOpenSpectrometer(uint index)
{
    (void)index;
}

void THWDriverThread::hwdtSloCloseSpectrometer()
{

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
    //Connection of outgoing signals..
    {
        connect(HWDriverObject,SIGNAL(hwdSigGotTemperature(THWTempSensorID , float ,bool)),
                    this,SLOT (hwdSloGotTemperature(THWTempSensorID , float ,bool)),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdSigGotTemperature(THWTempSensorID , float )),
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

        connect(HWDriverObject,SIGNAL(hwdtSigSpectrumeterOpened( )),
                    this,SLOT (hwdSloSpectrumeterOpened( )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdtSigSpectrumeterOpened( )),
                    this,SIGNAL (hwdSigSpectrumeterOpened( )),Qt::QueuedConnection);
    }
    //for messages giong from this to thread..
    {
        connect(this,SIGNAL(hwdtSigSetComPort(QString )),
                    HWDriverObject,SLOT (hwdtSloSetComPort(QString )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigSetBaud(AbstractSerial::BaudRate )),
                    HWDriverObject,SLOT (hwdtSloSetBaud(AbstractSerial::BaudRate  )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigAskTemperature(THWTempSensorID )),
                    HWDriverObject,SLOT (hwdtSloAskTemperature(THWTempSensorID )),Qt::QueuedConnection);

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

        connect(this,SIGNAL(hwdtSigAskWLCoefficients( )),
                    HWDriverObject,SLOT (hwdtSloAskWLCoefficients( )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigOpenSpectrometer(uint )),
                    HWDriverObject,SLOT (hwdtSloOpenSpectrometer(uint )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigCloseSpectrometer( )),
                    HWDriverObject,SLOT (hwdtSloCloseSpectrometer( )),Qt::QueuedConnection);

        connect(TemperatureTimer,SIGNAL(timeout()),
                    this,SLOT (hwdSlotTemperatureTimer( )));

    }
    HWDriverThread = new QThreadEx();
    HWDriverObject->moveToThread(HWDriverThread);
    HWDriverThread->start();
    TemperatureTimer->start(200);
}



void THWDriver::hwdSetWavelengthBuffer(double* wlBuffer,uint Bufferlength)
{
    WavelengthBuffer = wlBuffer;
    WavelengthBufferSize = Bufferlength;
}

void THWDriver::hwdOverwriteWLCoefficients(TSPectrWLCoefficients* SpectrCoefficients)
{
    (void)SpectrCoefficients;
}

TSPectrWLCoefficients THWDriver::hwdGetWLCoefficients()
{
    TSPectrWLCoefficients Coefficients;
    return Coefficients;
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


uint THWDriver::hwdGetSpectrum(TSpectrum *Spectrum)
{
    (void)Spectrum;
    return 0;
}


QList<QString> THWDriver::hwdGetListSpectrometer()
{

}

void THWDriver::hwdOpenSpectrometer(uint index)
{
    emit hwdOpenSpectrometer(index);
}

void THWDriver::hwdCloseSpectrometer()
{
    emit hwdCloseSpectrometer();
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
