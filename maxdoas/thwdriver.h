#ifndef THWDRIVER_H
#define THWDRIVER_H

#include <QString>
#include <QPointF>
#include <QList>
#include <QThread>
#include <QReadWriteLock>
#include <abstractserial.h>

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


    void hwdtGetLastSpectrumBuffer(double *Spectrum, uint size);

public slots:
    void hwdtSloSetComPort(QString name);
    void hwdtSloSetBaud(AbstractSerial::BaudRate baud);

    void hwdtSloAskTemperature(THWTempSensorID sensorID);
    void hwdtSloSetTargetTemperature(int temperature);

    void hwdtSloAskTilt();

    void hwdtSloAskCompass();
    void hwdtSloStartCompassCal();
    void hwdtSloStopCompassCal();

    void hwdtSloGoMotorHome();

    void hwdtSloSetShutter(THWShutterCMD ShutterCMD);

    void hwdtSloMeasureScanPixel(QPoint pos,uint avg, uint integrTime);

    void hwdtSloMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD);

    void hwdtSloAskWLCoefficients();

    void hwdtSloOpenSpectrometer(uint index);
    void hwdtSloCloseSpectrometer();

signals:
    void hwdtSigGotTemperature(THWTempSensorID sensorID, float Temperature);
    void hwdtSigGotTilt(float TiltX,float TiltY);

    void hwdtSigGotCompassHeading(float Heading);
    void hwdtSigCompassStartedCalibrating();
    void hwdtSigCompassStoppedCalibrating();
    void hwdtSigMotorIsHome();
    void hwdtSigShutterStateChanged(THWShutterCMD LastShutterCMD);
    void hwdtSigScanPixelMeasured();

    void hwdtSigGotWLCoefficients();
    void hwdtSigGotSpectrum();
    void hwdtSigSpectrumeterOpened();
private:
    bool sendBuffer(char *TXBuffer,char *RXBuffer,uint size,uint timeout , bool TempCtrler); //returns true if ok
    bool waitForAnswer(char *TXBuffer,char *RXBuffer,uint size,uint timeout, bool TempCtrler); //returns true if ok
    float sensorTempToCelsius(short int Temperature);
    short int CelsiusToSensorTemp(float Temperature);
    int CRCError;
    QReadWriteLock MutexSpectrBuffer;
    double LastSpectr[MAXWAVELEGNTH_BUFFER_ELEMTENTS];
    uint SpectrBufferSize;
    uint integTimer;
    uint SpectrAvgCount;
    AbstractSerial *serial;
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
    void hwdSetBaud(AbstractSerial::BaudRate baud);

    float hwdGetTemperature(THWTempSensorID sensorID);
    void hwdAskTemperature(THWTempSensorID sensorID);
    void hwdSetTargetTemperature(int temperature);

    QPointF hwdGetTilt();
    void hwdAskTilt();
    void hwdSetTiltZero();

    float hwdGetCompassHeading();
    void hwdAskCompass();
    void hwdStartCompassCal();
    void hwdStopCompassCal();
    void hwdSetCompassRealValue();

    void hwdGoMotorHome();

    void hwdSetShutter(THWShutterCMD ShutterCMD);


    void hwdMeasureScanPixel(QPoint pos,uint avg, uint integrTime);

    void hwdMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD);

    uint hwdGetSpectrum(TSpectrum *Spectrum);

    QList<QString> hwdGetListSpectrometer();
    void hwdOpenSpectrometer(uint index);
    void hwdCloseSpectrometer();

private slots:  //coming from thread
    void hwdSloGotTemperature(THWTempSensorID sensorID, float Temperature);
    void hwdSloGotTilt(float TiltX,float TiltY);
    void hwdSloGotCompassHeading(float Heading);
    void hwdSloCompassStartedCalibrating();
    void hwdSloCompassStoppedCalibrating();
    void hwdSloMotorIsHome();
    void hwdSloShutterStateChanged(THWShutterCMD LastShutterCMD);
    void hwdSloScanPixelMeasured();
    void hwdSloGotSpectrum();
    void hwdSloSpectrumeterOpened();
    void hwdSloGotWLCoefficients();
signals: //thread -> outside
    void hwdSigGotTemperature(THWTempSensorID sensorID, float Temperature);
    void hwdSigGotTilt(float TiltX,float TiltY);
    void hwdSigGotCompassHeading(float Heading);
    void hwdSigCompassStartedCalibrating();
    void hwdSigCompassStoppedCalibrating();
    void hwdSigMotorIsHome();
    void hwdSigShutterStateChanged(THWShutterCMD LastShutterCMD);
    void hwdSigScanPixelMeasured();
    void hwdSigGotSpectrum();
    void hwdSigSpectrumeterOpened();


    //commands for controlling thread
    void hwdtSigSetComPort(QString name);
    void hwdtSigSetBaud(AbstractSerial::BaudRate baud);

    void hwdtSigAskTemperature(THWTempSensorID sensorID);
    void hwdtSigSetTargetTemperature(int temperature);

    void hwdtSigAskTilt();

    void hwdtSigAskCompass();
    void hwdtSigStartCompassCal();
    void hwdtSigStopCompassCal();

    void hwdtSigGoMotorHome();

    void hwdtSigSetShutter(THWShutterCMD ShutterCMD);

    void hwdtSigMeasureScanPixel(QPoint pos,uint avg, uint integrTime);

    void hwdtSigMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD);
    void hwdtSigAskWLCoefficients();
    void hwdtSigOpenSpectrometer(uint index);
    void hwdtSigCloseSpectrometer();
private:

    THWDriverThread *HWDriverObject;
    QThreadEx *HWDriverThread;
    TSPectrWLCoefficients SpectrCoefficients;

    double *WavelengthBuffer; //for storing inside TSpectrum
    uint WavelengthBufferSize;
    float Temperatures[3];
    float CompassHeading;
    float TiltOffset;
    float CompassOffset;

};

#endif // THWDRIVER_H
