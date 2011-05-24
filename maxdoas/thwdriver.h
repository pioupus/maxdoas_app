#ifndef THWDRIVER_H
#define THWDRIVER_H

#include <QString>
#include <QPointF>

enum THWTempSensorID {tsNone,tsPeltier=0,tsHeatSink=1,tsSpectrometer=2};

class THWDriver
{
public:
    THWDriver();
    void hwdSetComPort(QString name);
    void hwdSetBaud(int baud);

    float hwdGetTemperature(THWTempSensorID sensorID);
    void hwdAskTemperature(THWTempSensorID sensorID);

    QPointF hwdGetTilt(void);
    void hwdAskTilt();

    float hwdCompass(void);
    void hwdAskCompass();

    void hwdGoMotorHome(void);

    void hwdMeasureDarkEffects();

    void hwdMeasureScanPixel(QPoint pos,uint avg, uint integrTime);

private:
};

#endif // THWDRIVER_H
