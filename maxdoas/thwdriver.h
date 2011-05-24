#ifndef THWDRIVER_H
#define THWDRIVER_H

#include <QString>
#include <QPointF>

enum THWTempSensorID {tsNone,tsPeltier=0,tsHeatSink=1,tsSpectrometer=2};
enum THWDriverState {tsNone,tsGettingSensorData,tsMeasuringScanPixel,tsMeasuringSpectrum};
enum THWShutterCMD {scNone,scClose,scOpen};

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

    float hwdCompass(void);
    void hwdAskCompass();
    void hwdStartCompassCal();
    void hwdStopCompassCal();

    void hwdGoMotorHome(void);

    void hwdSetShutter(bool close);

    void hwdMeasureDarkEffects(THWShutterCMD shutterCMD);

    void hwdMeasureScanPixel(QPoint pos,uint avg, uint integrTime);

    void hwdMeasureSpectrum(uint avg, uint integrTime,THWShutterCMD shutterCMD);
private:
};

#endif // THWDRIVER_H
