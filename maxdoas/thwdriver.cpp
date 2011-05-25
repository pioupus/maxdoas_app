#include "thwdriver.h"

 #include <QString>

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


void THWDriverThread::hwdtGetLastSpectrumBuffer(double *Spectrum, uint size)
{
    int i = SpectrBufferSize;
    if (size < SpectrBufferSize)
        i = size;
    MutexSpectrBuffer.lockForRead();
    memcpy(Spectrum,LastSpectr,sizeof(double)*i);
    MutexSpectrBuffer.unlock();
}


void THWDriverThread::hwdtSloAskTemperature(THWTempSensorID sensorID)
{
}

void THWDriverThread::hwdtSloSetTargetTemperature(int temperature)
{
}


void THWDriverThread::hwdtSloAskTilt()
{
}


void THWDriverThread::hwdtSloAskCompass()
{
}

void THWDriverThread::hwdtSloStartCompassCal()
{
}

void THWDriverThread::hwdtSloStopCompassCal()
{
}


void THWDriverThread::hwdtSloGoMotorHome(void)
{
}


void THWDriverThread::hwdtSloSetShutter(THWShutterCMD ShutterCMD)
{
}


void THWDriverThread::hwdtSloMeasureScanPixel(QPoint pos,uint avg, uint integrTime)
{
}


void THWDriverThread::hwdtSloMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD)
{
}

void THWDriverThread::hwdtSloAskWLCoefficients()
{

}

void THWDriverThread::hwdtSloOpenSpectrometer(uint index)
{
}

void THWDriverThread::hwdtSloCloseSpectrometer()
{
}



THWDriver::THWDriver()
{
    HWDriverObject = new THWDriverThread();
    //Connection of outgoing signals..
    {
        connect(HWDriverObject,SIGNAL(hwdSigGotTemperature(THWTempSensorID , float )),
                    this,SLOT (hwdSloGotTemperature(THWTempSensorID , float )),Qt::QueuedConnection);
        connect(HWDriverObject,SIGNAL(hwdSigGotTemperature(THWTempSensorID , float )),
                    this,SIGNAL (hwdSigGotTemperature(THWTempSensorID , float )),Qt::QueuedConnection);

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
                    HWDriverObject,SLOT (hwdtSloSetBaud(QString )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigAskTemperature(THWTempSensorID )),
                    HWDriverObject,SLOT (hwdtSloAskTemperature(QString )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigSetTargetTemperature(int )),
                    HWDriverObject,SLOT (hwdtSloSetTargetTemperature(QString )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigAskTilt( )),
                    HWDriverObject,SLOT (hwdtSloAskTilt( )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigAskCompass( )),
                    HWDriverObject,SLOT (hwdtSloAskCompass( )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigStartCompassCal( )),
                    HWDriverObject,SLOT (hwdtSloStartCompassCal( )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigStopCompassCal( )),
                    HWDriverObject,SLOT (hwdtSloStopCompassCal( )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigGoMotorHome( )),
                    HWDriverObject,SLOT (hwdtSloGoMotorHome( )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigSetShutter(THWShutterCMD )),
                    HWDriverObject,SLOT (hwdtSloSetShutter(THWShutterCMD )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigMeasureScanPixel(QPoint,uint,uint )),
                    HWDriverObject,SLOT (hwdtSloMeasureScanPixel(QPoint,uint,uint )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigMeasureScanPixel(uint,uint,THWShutterCMD )),
                    HWDriverObject,SLOT (hwdtSloMeasureScanPixel(uint,uint,THWShutterCMD )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigAskWLCoefficients( )),
                    HWDriverObject,SLOT (hwdtSloAskWLCoefficients( )),Qt::QueuedConnectio

        connect(this,SIGNAL(hwdtSigOpenSpectrometer(uint )),
                    HWDriverObject,SLOT (hwdtSloOpenSpectrometer(uint )),Qt::QueuedConnection);

        connect(this,SIGNAL(hwdtSigCloseSpectrometer( )),
                    HWDriverObject,SLOT (hwdtSloCloseSpectrometer( )),Qt::QueuedConnection);
    }
    HWDriverThread = new QThreadEx();
    HWDriverObject->moveToThread(HWDriverThread);
    HWDriverThread->start();
}


void THWDriver::hwdSetWavelengthBuffer(double* wlBuffer,uint Bufferlength)
{
    WavelengthBuffer = wlBuffer;
    WavelengthBufferSize = Bufferlength;
}

void THWDriver::hwdOverwriteWLCoefficients(TSPectrWLCoefficients* SpectrCoefficients)
{

}

TSPectrWLCoefficients THWDriver::hwdGetWLCoefficients()
{

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

}

void THWDriver::hwdAskTemperature(THWTempSensorID sensorID)
{

}

void THWDriver::hwdSetTargetTemperature(int temperature)
{

}


QPointF THWDriver::hwdGetTilt(void)
{

}

void THWDriver::hwdAskTilt()
{

}

void THWDriver::hwdSetTiltZero()
{

}


float THWDriver::hwdGetCompassHeading(void)
{

}

void THWDriver::hwdAskCompass()
{

}

void THWDriver::hwdStartCompassCal()
{

}

void THWDriver::hwdStopCompassCal()
{

}

void THWDriver::hwdSetCompassRealValue()
{

}


void THWDriver::hwdGoMotorHome(void)
{

}


void THWDriver::hwdSetShutter(THWShutterCMD ShutterCMD)
{

}



void THWDriver::hwdMeasureScanPixel(QPoint pos,uint avg, uint integrTime)
{

}


void THWDriver::hwdMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD)
{

}


uint THWDriver::hwdGetSpectrum(TSpectrum *Spectrum)
{

}


QList<QString> THWDriver::hwdGetListSpectrometer()
{

}

void THWDriver::hwdOpenSpectrometer(uint index)
{

}

void THWDriver::hwdCloseSpectrometer()
{

}


void THWDriver::hwdSloGotTemperature(THWTempSensorID sensorID, float Temperature)
{

}

void THWDriver::hwdSloGotCompassHeading(float Heading)
{

}

void THWDriver::hwdSloCompassStartedCalibrating()
{

}

void THWDriver::hwdSloCompassStoppedCalibrating()
{

}

void THWDriver::hwdSloMotorIsHome()
{

}

void THWDriver::hwdSloShutterStateChanged(THWShutterCMD LastShutterCMD)
{

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
