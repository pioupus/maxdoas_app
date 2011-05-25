#ifndef THWDRIVER_H
#define THWDRIVER_H

#include <QString>
#include <QPointF>
#include <QList>
#include <QThread>
#include <QReadWriteLock>

#include "tspectrum.h"

#define MAXWAVELEGNTH_BUFFER_ELEMTENTS 4096

enum THWTempSensorID {tsNone,tsPeltier=0,tsHeatSink=1,tsSpectrometer=2};
enum THWDriverState {tsIDLE,tsGettingSensorData,tsMotorsCalibrating,tsMeasuringScanPixel,tsMeasuringSpectrum};
enum THWShutterCMD {scNone,scClose,scOpen};
//
//classes:


class QThreadEx : public QThread
{
protected:
    void run() {
        exec();
    }
};


class THWDriverThread : public QObject
{
    Q_OBJECT
public:
    THWDriverThread();

    void hwdtSetComPort(QString name);
    void hwdtSetBaud(int baud);

    void hwdtGetLastSpectrumBuffer(double *Spectrum, uint size);

public slots:
    void hwdtSloAskTemperature(THWTempSensorID sensorID);
    void hwdtSloSetTargetTemperature(int temperature);

    void hwdtSloAskTilt();

    void hwdtSloAskCompass();
    void hwdtSloStartCompassCal();
    void hwdtSloStopCompassCal();

    void hwdtSloGoMotorHome(void);

    void hwdtSloSetShutter(THWShutterCMD ShutterCMD);

    void hwdtSloMeasureScanPixel(QPoint pos,uint avg, uint integrTime);

    void hwdtSloMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD);

    void hwdtSloOpenSpectrometer(uint index);
    void hwdtSloCloseSpectrometer();

signals:
    void hwdtSigGotTemperature(THWTempSensorID sensorID, float Temperature);
    void hwdtSigGotCompassHeading(float Heading);
    void hwdtSigCompassStartedCalibrating();
    void hwdtSigCompassStoppedCalibrating();
    void hwdtSigMotorIsHome();
    void hwdtSigShutterStateChanged(THWShutterCMD LastShutterCMD);
    void hwdtSigScanPixelMeasured();
    void hwdtSigGotSpectrum();
    void hwdtSigSpectrumeterOpened();

private:
    QReadWriteLock MutexSpectrBuffer;
    double LastSpectr[MAXWAVELEGNTH_BUFFER_ELEMTENTS];
    uint SpectrBufferSize;
    uint integTimer;
    uint SpectrAvgCount;
};




class THWDriver : private QObject
{
        Q_OBJECT
public:
    THWDriver();

    void hwdSetWavelengthBuffer(double* wlBuffer,uint Bufferlength);
    void hwdOverwriteWLCoefficients(TSPectrWLCoefficients* SpectrCoefficients);
    TSPectrWLCoefficients hwdGetWLCoefficients();

    void hwdSetComPort(QString name);
    void hwdSetBaud(int baud);

    float hwdGetTemperature(THWTempSensorID sensorID);
    void hwdAskTemperature(THWTempSensorID sensorID);
    void hwdSetTargetTemperature(int temperature);

    QPointF hwdGetTilt(void);
    void hwdAskTilt();
    void hwdSetTiltZero();

    float hwdGetCompassHeading(void);
    void hwdAskCompass();
    void hwdStartCompassCal();
    void hwdStopCompassCal();
    void hwdSetCompassRealValue();

    void hwdGoMotorHome(void);

    void hwdSetShutter(THWShutterCMD ShutterCMD);


    void hwdMeasureScanPixel(QPoint pos,uint avg, uint integrTime);

    void hwdMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD);

    uint hwdGetSpectrum(TSpectrum *Spectrum);

    QList<QString> hwdGetListSpectrometer();
    void hwdOpenSpectrometer(uint index);
    void hwdCloseSpectrometer();

private slots:
    void hwdSloGotTemperature(THWTempSensorID sensorID, float Temperature);
    void hwdSloGotCompassHeading(float Heading);
    void hwdSloCompassStartedCalibrating();
    void hwdSloCompassStoppedCalibrating();
    void hwdSloMotorIsHome();
    void hwdSloShutterStateChanged(THWShutterCMD LastShutterCMD);
    void hwdSloScanPixelMeasured();
    void hwdSloGotSpectrum();
    void hwdSloSpectrumeterOpened();

signals:
    void hwdSigGotTemperature(THWTempSensorID sensorID, float Temperature);
    void hwdSigGotCompassHeading(float Heading);
    void hwdSigCompassStartedCalibrating();
    void hwdSigCompassStoppedCalibrating();
    void hwdSigMotorIsHome();
    void hwdSigShutterStateChanged(THWShutterCMD LastShutterCMD);
    void hwdSigScanPixelMeasured();
    void hwdSigGotSpectrum();
    void hwdSigSpectrumeterOpened();

private:

    THWDriverThread *HWDriverObject;
    QThreadEx *HWDriverThread;
    TSPectrWLCoefficients SpectrCoefficients;

    float Temperatures[3];
    float CompassHeading;
    float TiltOffset;
    float CompassOffset;

};

#endif // THWDRIVER_H
