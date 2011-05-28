#ifndef THWDRIVER_H
#define THWDRIVER_H

#include <QTimer>
#include <QString>
#include <QPointF>
#include <QList>
#include <QThread>
#include <QReadWriteLock>
#include <abstractserial.h>

#include "log4qt/consoleappender.h"
#include "log4qt/logger.h"
#include "log4qt/ttcclayout.h"

#include "tspectrum.h"

#define MAXWAVELEGNTH_BUFFER_ELEMTENTS 4096

#define INVALID_COMPASS_HEADING 400

#define INVALID_LIGHTSENSOR_VAL -1

#define HW_TRANSMISSION_TIMEOUT_FINAL_PARAMETER 100

enum THWTempSensorID {tsNone,tsPeltier=0,tsHeatSink=1,tsSpectrometer=2};
Q_DECLARE_METATYPE(THWTempSensorID);
enum THWDriverState {tsIDLE,tsGettingSensorData,tsMotorsCalibrating,tsMeasuringScanPixel,tsMeasuringSpectrum};
enum THWShutterCMD {scNone,scClose,scOpen};
Q_DECLARE_METATYPE(THWShutterCMD);
enum THWCompassState {csNone,csCalibrating,csReady};

enum THWTransferState {tsOK,tsCheckSumError,tsTimeOut};
Q_DECLARE_METATYPE(THWTransferState);

enum TTiltConfigRes {tcr12Bit=12,tcr14Bit=14,tcr16Bit=16,tcr18Bit=18};
Q_DECLARE_METATYPE(TTiltConfigRes);
enum TTiltConfigGain {tcGain1=1,tcGain2=2,tcGain4=4,tcGain8=8};
Q_DECLARE_METATYPE(TTiltConfigGain);

Q_DECLARE_METATYPE(AbstractSerial::BaudRate);
enum TLightSensorGain {lsGain1=0,lsGain16=1};
enum TLightSensorIntegTime {lsInteg13_7ms=0,lsInteg101ms=1,lsInteg402ms=2};
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
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    THWDriverThread();
    ~THWDriverThread();

    void hwdtGetLastSpectrumBuffer(double *Spectrum, uint size);

public slots:
    void hwdtSloSetComPort(QString name);
    void hwdtSloSetBaud(AbstractSerial::BaudRate baud);
    void hwdtSloSerOpenClose(bool open);

    void hwdtSloAskTemperature(THWTempSensorID sensorID,bool byTimer);
    void hwdtSloSetTargetTemperature(float temperature);

    void hwdtSloAskTilt();
    void hwdtSloConfigTilt(TTiltConfigRes Resolution,TTiltConfigGain Gain);
    void hwdtSloOffsetTilt(float TiltX, float TiltY);

    void hwdtSloAskCompass();
    void hwdtSloStartCompassCal();
    void hwdtSloStopCompassCal();

    void hwdtSloConfigLightSensor(TLightSensorGain Gain, TLightSensorIntegTime LightSensorIntegTime);
    void hwdtSloAskLightSensor();

    void hwdtSloGoMotorHome();
    bool SetShutter(THWShutterCMD ShutterCMD);
    void hwdtSloSetShutter(THWShutterCMD ShutterCMD);

    void hwdtSloMeasureScanPixel(int PosX, int PosY ,uint avg, uint integrTime);

    void hwdtSloMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD);

    void hwdtSloAskWLCoefficients();

    void hwdtSloOpenSpectrometer(uint index);
    void hwdtSloCloseSpectrometer();

signals:
    void hwdtSigTransferDone(THWTransferState TransferState, uint ErrorParameter);

    void hwdtSigGotTemperature(THWTempSensorID sensorID, float Temperature, bool byTimer);
    void hwdtSigGotTilt(float TiltX,float TiltY);

    void hwdtSigGotCompassHeading(float Heading);
    void hwdtSigCompassStartedCalibrating();
    void hwdtSigCompassStoppedCalibrating();

    void hwdtSigGotLightSensorValue(float lightValue);
    void hwdtSigMotorIsHome();
    void hwdtSigShutterStateChanged(THWShutterCMD LastShutterCMD);
    void hwdtSigScanPixelMeasured();

    void hwdtSigGotWLCoefficients();
    void hwdtSigGotSpectrum();
    void hwdtSigSpectrumeterOpened();
private:
    bool sendBuffer(char *TXBuffer,char *RXBuffer,uint size,uint timeout , bool TempCtrler); //returns true if ok
    bool waitForAnswer(char *TXBuffer,char *RXBuffer,uint size,int timeout, bool TempCtrler); //returns true if ok
    float sensorTempToCelsius(short int Temperature);
    short int CelsiusToSensorTemp(float Temperature);
    int CRCError;

    //float HeadingOffset;

    uint TiltADC_Steps;
    uint TiltADC_Gain;
    float TiltADC_RefVoltage;
    float TiltCalValNegGX;
    float TiltCalValPosGX;
    float TiltCalValNegGY;
    float TiltCalValPosGY;

    TLightSensorGain LightSensorGain;
    TLightSensorIntegTime LightSensorIntegTime;

    QReadWriteLock MutexSpectrBuffer;
    double LastSpectr[MAXWAVELEGNTH_BUFFER_ELEMTENTS];
    uint SpectrBufferSize;
    uint integTimer;
    uint SpectrAvgCount;

    AbstractSerial *serial;
    THWShutterCMD LastShutterCMD;

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
    void hwdSetTargetTemperature(float temperature);

    QPointF hwdGetTilt();
    void hwdAskTilt();
    void hwdSetTiltZero();

    float hwdGetCompassHeading();
    void hwdAskCompass();
    void hwdStartCompassCal();
    void hwdStopCompassCal();
    void hwdSetCompassRealValue(float RealHeading);
    THWCompassState hwdGetCompassState();

    void hwdAskLightSensor();
    float hwdGetLightsensorValue();

    void hwdGoMotorHome();

    void hwdSetShutter(THWShutterCMD ShutterCMD);


    void hwdMeasureScanPixel(QPoint pos,uint avg, uint integrTime);

    void hwdMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD);

    uint hwdGetSpectrum(TSpectrum *Spectrum);

    QList<QString> hwdGetListSpectrometer();
    void hwdOpenSpectrometer(uint index);
    void hwdCloseSpectrometer();

private slots:  //coming from thread
    void hwdSloGotTemperature(THWTempSensorID sensorID, float Temperature,bool byTimer);
    void hwdSloGotTilt(float TiltX,float TiltY);
    void hwdSloGotCompassHeading(float Heading);
    void hwdSloCompassStartedCalibrating();
    void hwdSloCompassStoppedCalibrating();
    void hwdSloGotLightSensorValue(float lightValue);
    void hwdSloMotorIsHome();
    void hwdSloShutterStateChanged(THWShutterCMD LastShutterCMD);
    void hwdSloScanPixelMeasured();
    void hwdSloGotSpectrum();
    void hwdSloSpectrumeterOpened();
    void hwdSloGotWLCoefficients();
private slots:  //internal signals
    void hwdSlotTemperatureTimer();
signals: //thread -> outside
    void hwdSigGotTemperature(THWTempSensorID sensorID, float Temperature);
    void hwdSigGotTilt(float TiltX,float TiltY);
    void hwdSigGotCompassHeading(float Heading);
    void hwdSigCompassStartedCalibrating();
    void hwdSigCompassStoppedCalibrating();
    void hwdSigGotLightSensorValue(float lightValue);
    void hwdSigMotorIsHome();
    void hwdSigShutterStateChanged(THWShutterCMD LastShutterCMD);
    void hwdSigScanPixelMeasured();
    void hwdSigGotSpectrum();
    void hwdSigSpectrumeterOpened();
    void hwdSigTransferDone(THWTransferState TransferState, uint ErrorParameter);

    //commands for controlling thread
    void hwdtSigSetComPort(QString name);
    void hwdtSigSetBaud(AbstractSerial::BaudRate baud);

    void hwdtSigAskTemperature(THWTempSensorID sensorID, bool byTimer);
    void hwdtSigSetTargetTemperature(float temperature);

    void hwdtSigConfigTilt(TTiltConfigRes Resolution,TTiltConfigGain Gain);
    void hwdtSigAskTilt();
    void hwdtSigOffsetTilt(float TiltX, float TiltY);

    void hwdtSigAskCompass();
    void hwdtSigStartCompassCal();
    void hwdtSigStopCompassCal();
    void hwdtSigAskLightSensor();
    void hwdtSigGoMotorHome();

    void hwdtSigSetShutter(THWShutterCMD ShutterCMD);

    void hwdtSigMeasureScanPixel(int PosX, int PosY ,uint avg, uint integrTime);

    void hwdtSigMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD);
    void hwdtSigAskWLCoefficients();
    void hwdtSigOpenSpectrometer(uint index);
    void hwdtSigCloseSpectrometer();
private:

    THWDriverThread *HWDriverObject;
    QThreadEx *HWDriverThread;
    TSPectrWLCoefficients SpectrCoefficients;

    QTimer *TemperatureTimer;

    double *WavelengthBuffer; //for storing inside TSpectrum
    uint WavelengthBufferSize;
    THWTempSensorID LastSensorID;
    float Temperatures[3];
    QPointF *ActualTilt;
    float CompassHeading;
    float CompassOffset;
    THWCompassState CompassState;

    float LightSensorVal;

};

#endif // THWDRIVER_H
