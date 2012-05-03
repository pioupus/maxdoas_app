#ifndef THWDRIVER_H
#define THWDRIVER_H

#include <QTimer>
#include <QString>
#include <QPointF>
#include <QList>
#include <QThread>
#include <QReadWriteLock>
#include <QStringList>
#include <abstractserial.h>
#include "wavelengthbuffer.h"
#include "log4qt/consoleappender.h"
#include "log4qt/logger.h"
#include "log4qt/ttcclayout.h"

#include "tspectrum.h"

#include <Wrapper.h>
#include <ArrayTypes.h>
#include <iostream>
#include <iomanip>
#include "serialdeviceenumerator.h"

#define INVALID_COMPASS_HEADING 400

#define INVALID_LIGHTSENSOR_VAL -1

#define HW_TRANSMISSION_TIMEOUT_FINAL_PARAMETER 100
#define TEMPERATURE_BUFFER_COUNT 100

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


//classes:


class QThreadEx : public QThread
{
protected:
    //void run() {
    //    exec();
    //}
};


class THWDriverThread : public QObject
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    THWDriverThread();
    ~THWDriverThread();

    void hwdtGetLastSpectrumBuffer(double *Spectrum,int *NumberOfSpecPixels, TSPectrWLCoefficients *SpectrCoefficients, uint size, double *MaxPossibleValue,uint *Integrationtime, uint *avg,TMirrorCoordinate *mc,QString *SpectrSerial);
    void hwdtSetIntegrationConfiguration(TAutoIntegConf *autoIntConf);
    QList<QString> hwdtGetSpectrometerList();
    uint hwdtGetMinimumIntegrationTime();
    QPoint hwdtGetLastRawTilt();
    QString getSpectrSerial();
public slots:

    void hwdtSloSetComPort(QString name);
    void hwdtSloSetBaud(AbstractSerial::BaudRate baud);
    void hwdtSloSerOpenClose(bool open);

    void hwdtSloAskTemperature(THWTempSensorID sensorID,bool byTimer);
    void hwdtSloSetTargetTemperature(float temperature);

    void hwdtSloAskTilt();
    void hwdtSloSetTiltMinMaxCalibration(int min_x, int min_y,int max_x, int max_y);
    void hwdtSloConfigTilt(TTiltConfigRes Resolution,TTiltConfigGain Gain);
    void hwdtSloTiltStartCal();
    void hwdtSloTiltStopCal();
    void hwdtSloAskTiltMinValue();
    void hwdtSloAskTiltMaxValue();
    void hwdtSloAskTiltZenithValue();

    bool hwdtSloShutterGoto(int Shutterposition);
    void hwdtSloTiltSetZenith();
    void hwdtSloSetMaxdoasZenithPos();
    void hwdtSloSetStepperShutterClosePos();
    void hwdtSloAskScannerStatus();
    void hwdtSloAskMotorSetup();
    void hwdtSloAskDeviceInfo();
    void hwdtSloSetGUID(int guid);

    void hwdtSloAskCompass();
    void hwdtSloStartCompassCal();
    void hwdtSloStopCompassCal();

    void hwdtSloConfigLightSensor(TLightSensorGain Gain, TLightSensorIntegTime LightSensorIntegTime);
    void hwdtSloAskLightSensor();

    void hwdtSloGoMotorHome();
    bool SetShutter(THWShutterCMD ShutterCMD);
    void hwdtSloSetShutter(THWShutterCMD ShutterCMD);

    void hwdtSloMeasureScanPixel(float PosX, float PosY ,uint avg, uint integrTime);

    bool hwdtSloMotGoto(float PosX, float PosY);
    bool hwdtSloMotGotoSteps(int PosX, int PosY);

    void hwdtSloMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD);

    void hwdtSloDiscoverSpectrometers();
    void hwdtSloOpenSpectrometer(QString Serialnumber);


    void hwdtSloCloseSpectrometer();

    void CloseEverythingForLeaving();

    void hwdtSloMotIdleState(bool idle);


private slots:
   // void threadTerminated();
    void init();
signals:
    void hwdtSigTransferDone(THWTransferState TransferState, uint ErrorParameter);

    void hwdtSigGotTemperature(THWTempSensorID sensorID, float TemperaturePeltier,float TemperatureSpectr,float TemperatureHeatsink, bool byTimer);
    void hwdtSigGotTilt(float TiltX, float TiltY, int Gain, int Resolution);

    void hwdtSigGotTiltMinVal(int TiltX, int TiltY, int Gain, int Resolution);
    void hwdtSigGotTiltMaxVal(int TiltX, int TiltY, int Gain, int Resolution);
    void hwdtSigGotTiltZenith(int TiltX, int TiltY, int Gain, int Resolution);

    void hwdtSigGotScannerStatus(float ScannerTemperature,bool ShutterOpenedBySwitch, bool EndSwitchError);

    void hwdtSigGotMotorSetup(int MaxDoasZenithPosition,int ShutterClosePosition, int MaxDoasMicrosteps, int ShutterMicrosteps, int ShutterType);
    void hwdtSigGotDeviceInfo(int GitHash,int guid, int deviceType);

    void hwdtSigGotCompassHeading(float Heading);
    void hwdtSigCompassStartedCalibrating();
    void hwdtSigCompassStoppedCalibrating();

    void hwdtSigGotLightSensorValue(float lightValue);
    void hwdtSigMotorIsHome(int x, int y);
    void hwdtSigShutterStateChanged(THWShutterCMD LastShutterCMD);
    void hwdtSigScanPixelMeasured();

    void hwdtSigGotWLCoefficients();
    void hwdtSigGotSpectrum();
    void hwdtSigSpectrumeterOpened();
    void hwdtSigSpectrometersDicovered();
    void hwdtSigCOMPortChanged(QString name, bool opened, bool error);
    void hwdtSigMotMoved(int x, int y);
    void hwdtSigMotFailed();
private:
    bool sendBuffer(char *TXBuffer,char *RXBuffer,uint size,uint timeout , bool TempCtrler,bool RetransmitAllowed); //returns true if ok
    bool waitForAnswer(char *TXBuffer,char *RXBuffer,uint size,int timeout, bool TempCtrler,bool RetransmitAllowed); //returns true if ok
    float sensorTempToCelsius(short int Temperature);
    short int CelsiusToSensorTemp(float Temperature);
    void TakeSpectrum(int avg, uint IntegrTime);
    void newOmniWrapper();
    bool CalcAndSetAutoIntegTime();
    double getMaxIntensityOfLastSpect();

    QString DoRS485Direction(bool TempCtrler,uint size);
    int CRCError;

    //float HeadingOffset;
   // uint integtimetest;
    uint TiltADC_Steps;
    uint TiltADC_Gain;
    float TiltADC_RefVoltage;
    QPoint TiltRaw;
    QPoint TiltRawMin_byDevice;
    QPoint TiltRawMax_byDevice;
    QPoint TiltRawZenith_byDevice;
    //QPoint TiltRawOffset;
    QPoint TiltRawMaxValue;
    QPoint TiltRawMinValue;
    TShutterBySwitchState ShutterIsOpenBySwitch;
    TEndSwitchErrorState EndSwitchErrorState;
//    float TiltCalValNegGX;
//    float TiltCalValPosGX;
//    float TiltCalValNegGY;
//    float TiltCalValPosGY;

    TLightSensorGain LightSensorGain;
    TLightSensorIntegTime LightSensorIntegTime;

    QReadWriteLock MutexSpectrBuffer;
    QReadWriteLock MutexMinIntegrationTime;
    QReadWriteLock MutexRawTiltPoint;
    QReadWriteLock MutexSpectrSerial;
    double LastSpectr[MAXWAVELEGNTH_BUFFER_ELEMTENTS];

    uint SpectrAvgCount;
    TSPectrWLCoefficients SpectrCoefficients;
    Wrapper *wrapper; //Spectrometer;
    int NumberOfSpectrometers;
    int SpectrometerIndex;
    uint LastSpectrIntegTime;
    int NumOfPixels;
    double SpectrMinIntegTime;
    double SpectrMaxIntensity;
    QString SpectrometerSerial;
    QReadWriteLock MutexintegTime;
    TAutoIntegConf IntegTimeConf;
    AbstractSerial *serial;
    THWShutterCMD LastShutterCMD;
    QReadWriteLock MutexSpectrList;
    QList<QString> *SpectrometerList;
    TMirrorCoordinate *mc;
    QPoint ScannerStepPos;


};




class THWDriver : public QObject
{
        Q_OBJECT
public:
    THWDriver();
    ~THWDriver();
    void hwdOverwriteWLCoefficients(TSPectrWLCoefficients* WlCoefficients);
    //TSPectrWLCoefficients hwdGetWLCoefficients();

    void hwdSetComPort(TCOMPortConf ComConf);
    void hwdSetBaud(AbstractSerial::BaudRate baud);

    float hwdGetTemperature(THWTempSensorID sensorID);
    void hwdAskTemperature(THWTempSensorID sensorID);
    void hwdSetTargetTemperature(float temperature);

    QPointF hwdGetTilt();
    void hwdAskTilt();
    void hwdSetTiltMinMaxCalib(QPoint Min, QPoint Max);
    QPoint hwdGetRawTilt();
    void hwdSetTiltInterval(int ms);
    float hwdGetCompassHeading();
    void hwdAskCompass();
    void hwdStartCompassCal();
    void hwdStopCompassCal();
    void hwdSetCompassRealValue(float RealHeading);
    THWCompassState hwdGetCompassState();

    void hwdAskLightSensor();
    float hwdGetLightsensorValue();

    QString getFirmwarehash();

    void hwdTiltStartCal();
    void hwdTiltStopCal();
    void hwdAskTiltMinValue();
    void hwdAskTiltMaxValue();
    void hwdAskTiltZenithValue();

    QPointF getRawTiltMin();

    QPointF getRawTiltMax();

    void hwdTiltSetZenith();
    void hwdSetMaxdoasZenithPos();
    void hwdSetStepperShutterClosePos();
    void hwdSetShuttPos(int Shutterpos);
    int hwdGetShuttPos();
    void hwdAskScannerStatus();
    void hwdAskMotorSetup();
    void hwdAskDeviceInfo();
    void hwdSetGUID(int guid);

    void hwdGoMotorHome();

    void hwdSetShutter(THWShutterCMD ShutterCMD);


    void hwdMeasureScanPixel(QPointF AngleCoordinate,uint avg, uint integrTime);
    void hwdMotMove(QPointF AngleCoordinate);
    void hwdMotMoveBySteps(QPoint MotCoordinates);
    void hwdMotIdleState(bool idle);
    void hwdMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD);

    uint hwdGetSpectrum(TSpectrum *Spectrum);
    void setIntegrationConfiguration(TAutoIntegConf *autoIntConf);
    void hwdDiscoverSpectrometers();
    QList<QString> hwdGetListSpectrometer();
    void hwdOpenSpectrometer(QString SerialNumber);
    void hwdCloseSpectrometer();
    void stop();
    uint hwdGetMinimumIntegrationTime();

    double TempBufferPeltier[TEMPERATURE_BUFFER_COUNT];
    double TempBufferSpectr[TEMPERATURE_BUFFER_COUNT];
    double TempBufferHeatSink[TEMPERATURE_BUFFER_COUNT];
    int TempBufferPointer;
    QString getSpectrSerial();
    QPoint getStepperPos();
private slots:  //coming from thread
    void hwdSloGotTemperature(THWTempSensorID sensorID, float TemperaturePeltier, float TemperatureSpectr,float TemperatureHeatsink,bool byTimer);
    void hwdSloGotTilt(float TiltX, float TiltY, int Gain, int Resolution);
    void hwdSloGotCompassHeading(float Heading);
    void hwdSloCompassStartedCalibrating();
    void hwdSloCompassStoppedCalibrating();
    void hwdSloGotLightSensorValue(float lightValue);
    void hwdSloMotorIsHome(int x, int y);
    void hwdSloShutterStateChanged(THWShutterCMD LastShutterCMD);
    void hwdSloScanPixelMeasured();
    void hwdSloGotSpectrum();
    void hwdSloSpectrumeterOpened();
    void hwdSloGotWLCoefficients();
    void hwdSlothreadFinished();
    void hwdSloCOMPortChanged(QString name, bool opened, bool error);

    void hwdSloMotMoved(int StepX, int StepY);
    void hwdSloGotTiltMinVal(int TiltX, int TiltY, int Gain, int Resolution);
    void hwdSloGotTiltMaxVal(int TiltX, int TiltY, int Gain, int Resolution);
    void hwdSloGotTiltZenith(int TiltX, int TiltY, int Gain, int Resolution);

    void hwdSloGotScannerStatus(float ScannerTemperature,bool ShutterOpenedBySwitch, bool EndSwitchError);

    void hwdSloGotMotorSetup(int MaxDoasZenithPosition,int ShutterClosePosition, int MaxDoasMicrosteps, int ShutterMicrosteps, int ShutterType);
    void hwdSloGotDeviceInfo(int GitHash,int guid, int deviceType);
private slots:  //internal signals
    void hwdSlotTemperatureTimer();
    void hwdSlotTiltTimer();
    void slotCOMPorts(const QStringList &list);
signals: //thread -> outside
    void hwdSigHWThreadFinished();
    void hwdSigGotTemperatures(float TemperaturePeltier,float TemperatureSpectr,float TemperatureHeatsink);
    void hwdSigGotTilt(float TiltX,float TiltY,int Gain, int Resolution,float ResolutionBorder,float MaxTilt);

    void hwdSigGotTiltDirection(float Direction);

    void hwdSigGotTiltMinVal(int TiltX, int TiltY, int Gain, int Resolution);
    void hwdSigGotTiltMaxVal(int TiltX, int TiltY, int Gain, int Resolution);
    void hwdSigGotTiltZenith(int TiltX, int TiltY, int Gain, int Resolution);

    void hwdSigGotScannerStatus(float ScannerTemperature,bool ShutterOpenedBySwitch, bool EndSwitchError);

    void hwdSigGotMotorSetup(int MaxDoasZenithPosition,int ShutterClosePosition, int MaxDoasMicrosteps, int ShutterMicrosteps, int ShutterType);
    void hwdSigGotDeviceInfo(int GitHash,int guid, int deviceType);


    void hwdSigGotCompassHeading(float Heading);
    void hwdSigCompassStartedCalibrating();
    void hwdSigCompassStoppedCalibrating();
    void hwdSigGotLightSensorValue(float lightValue);
    void hwdSigMotorIsHome(int x, int y);
    void hwdSigShutterStateChanged(THWShutterCMD LastShutterCMD);
    void hwdSigScanPixelMeasured();
    void hwdSigGotSpectrum();
    void hwdSigMotMoved(int StepX, int StepY);
    void hwdSigMotFailed();
    void hwdSigSpectrumeterOpened();
    void hwdSigTransferDone(THWTransferState TransferState, uint ErrorParameter);
    void hwdSigSpectrometersDiscovered();
    void hwdSigCOMPortChanged(QString name,bool opened,bool error);
    //commands for controlling thread
    void hwdtSigSetComPort(QString name);
    void hwdtSigSetBaud(AbstractSerial::BaudRate baud);

    void hwdtSigAskTemperature(THWTempSensorID sensorID, bool byTimer);
    void hwdtSigSetTargetTemperature(float temperature);

    void hwdtSigConfigTilt(TTiltConfigRes Resolution,TTiltConfigGain Gain);
    void hwdtSigAskTilt();
    void hwdtSigSetTiltMinMaxCalibration(int minX, int minY,int maxX, int maxY);

    bool hwdtSigShutterGoto(int Shutterposition);
    void hwdtSigAskCompass();
    void hwdtSigStartCompassCal();
    void hwdtSigStopCompassCal();
    void hwdtSigAskLightSensor();
    void hwdtSigGoMotorHome();
    void hwdtSigMotoIdleState(bool idle);

    void hwdtSigTiltStartCal();
    void hwdtSigTiltStopCal();
    void hwdtSigAskTiltMinValue();
    void hwdtSigAskTiltMaxValue();
    void hwdtSigAskTiltZenithValue();

    void hwdtSigTiltSetZenith();
    void hwdtSigSetMaxdoasZenithPos();
    void hwdtSigSetStepperShutterClosePos();
    void hwdtSigAskScannerStatus();
    void hwdtSigAskMotorSetup();
    void hwdtSigAskDeviceInfo();
    void hwdtSigSetGUID(int guid);

    void hwdtSigSetShutter(THWShutterCMD ShutterCMD);

    bool hwdtMotGoto(float PosX, float PosY);
    bool hwdtMotGotoSteps(int PosX, int PosY);
    bool hwdtMotIdleState(bool idle);

    void hwdtSigMeasureScanPixel(float PosX, float PosY ,uint avg, uint integrTime);

    void hwdtSigMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD);
    void hwdtSigDiscoverSpectrometers();
    void hwdtSigOpenSpectrometer(QString SerialNumber);
    void hwdtSigCloseSpectrometer();
    void hwdtQuitThred();

private:

    THWDriverThread *HWDriverObject;
    QThreadEx *HWDriverThread;
   // TSPectrWLCoefficients SpectrCoefficients;
    QStringList *ComPortList;
    QTimer *TemperatureTimer;
    QTimer *TiltTimer;
    TCOMPortConf ComPortConf;
    TWavelengthbuffer *WavelengthBuffer; //for storing inside TSpectrum
    THWTempSensorID LastSensorID;
    float Temperatures[3];
    QPointF *ActualTilt;
    QPointF *TiltMin;
    QPointF *TiltMax;
    QPointF *TiltZenith;

    int ShutterPosition;
    float ScannerTemperature;
    TShutterBySwitchState ShutterOpenedBySwitch;
    TEndSwitchErrorState EndSwitchError;


    int MaxDoasZenithPosition;
    int ShutterClosePosition;
    int MaxDoasMicrosteps;
    int ShutterMicrosteps;
    TShutterMotorType ShutterType;


    QString GitHash;
    QString ScannerSerialNumber;
    TScannerDeviceType deviceType;

    float CompassHeading;
    float CompassOffset;
    float TiltResolutionBorder;
    THWCompassState CompassState;
    bool COMPortOpened;
    SerialDeviceEnumerator *m_sde;
    float LightSensorVal;
    TAutoIntegConf IntegTimeConf;
    QPoint ScannerStepPos;
};

#endif // THWDRIVER_H
