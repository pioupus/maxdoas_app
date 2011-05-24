#ifndef THWDRIVER_H
#define THWDRIVER_H

#include <QString>
#include <QPointF>
#include <QList>

enum THWTempSensorID {tsNone,tsPeltier=0,tsHeatSink=1,tsSpectrometer=2};
enum THWDriverState {tsIDLE,tsGettingSensorData,tsMotorsCalibrating,tsMeasuringScanPixel,tsMeasuringSpectrum};
enum THWShutterCMD {scNone,scClose,scOpen};
//
//classes:
//TSpectrum
//    ARRAY of double
//    position
//    type(darkoffset,darknoise,sunspec,measurement)
//    integtime
//    avg
//
//TSignature
//    start
//    end
//    wl
//    spec
//
//    ppmm
//    correlation
//
//
//TRetrieval
//    list of TSignatures
//    correlation
//    TSpectrum
//
//TRetrievedImage
//    array[x][y] of TRetrieval
//
class THWDriver
{
public:
    THWDriver();
    void hwdSetComPort(QString name);
    void hwdSetBaud(int baud);

    float hwdGetTemperature(THWTempSensorID sensorID);
    void hwdAskTemperature(THWTempSensorID sensorID);
    void hwdSetTargetTemperature(int temperature);

    QPointF hwdGetTilt(void);
    void hwdAskTilt();
    void hwdSetTiltZero();

    float hwdCompass(void);
    void hwdAskCompass();
    void hwdStartCompassCal();
    void hwdStopCompassCal();
    void hwdSetCompassRealValue();

    void hwdGoMotorHome(void);

    void hwdSetShutter(bool close);

    void hwdMeasureDarkEffects(THWShutterCMD shutterCMD);

    void hwdMeasureScanPixel(QPoint pos,uint avg, uint integrTime);

    void hwdMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD);

    QList<QString> hwdGetListSpectrometer();
    void hwdOpenSpectrometer(uint index);
    void hwdCloseSpectrometer();
private:
};

#endif // THWDRIVER_H
