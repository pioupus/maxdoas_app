#include "thwdriver.h"

 #include <QString>

THWDriverThread::THWDriverThread()
{

}

void THWDriverThread::hwdtSetComPort(QString name)
{
}

void THWDriverThread::hwdtSetBaud(int baud)
{
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


void THWDriverThread::hwdtSloOpenSpectrometer(uint index)
{
}

void THWDriverThread::hwdtSloCloseSpectrometer()
{
}



THWDriver::THWDriver()
{
    HWDriverObject = new THWDriverThread();

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

    connect(HWDriverObject,SIGNAL(hwdtSigSpectrumeterOpened( )),
                this,SLOT (hwdSloSpectrumeterOpened( )),Qt::QueuedConnection);
    connect(HWDriverObject,SIGNAL(hwdtSigSpectrumeterOpened( )),
                this,SIGNAL (hwdSigSpectrumeterOpened( )),Qt::QueuedConnection);


    HWDriverThread = new QThreadEx();
    HWDriverObject->moveToThread(HWDriverThread);
    HWDriverThread->start();
}


void THWDriver::hwdSetWavelengthBuffer(double* wlBuffer,uint Bufferlength)
{

}

void THWDriver::hwdOverwriteWLCoefficients(TSPectrWLCoefficients* SpectrCoefficients)
{

}

TSPectrWLCoefficients THWDriver::hwdGetWLCoefficients()
{

}


void THWDriver::hwdSetComPort(QString name)
{

}

void THWDriver::hwdSetBaud(int baud)
{

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
