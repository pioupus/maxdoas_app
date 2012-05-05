#ifndef MAXDOASSETTINGS_H
#define MAXDOASSETTINGS_H
#include <QString>
#include <QSettings>

#include <QMutex>
#include "wavelengthbuffer.h"




struct TAutoIntegConf{
    bool autoenabled;
    float targetPeak;//in percent
    float targetCorridor;//in percent(targetPeak-targetCorridor/2 < signal < targetPeak+targetCorridor/2)
    uint maxIntegTime;
    uint fixedIntegtime;
};

struct TCOMPortConf{
    bool valid;
    bool ByName;
    QString Name;
    QString SysPath;
};

typedef enum{sdtUnknown,sdtWindField,sdtMAXDOAS,sdtSolTracker} TScannerDeviceType;
typedef enum{smtUnknown,smtDCMotor,smtStepper,smtServo} TShutterMotorType;
typedef enum{sssUnknown,sssOpened,sssClosed} TShutterBySwitchState;
typedef enum{eseUnknown,eseOK,eseERROR} TEndSwitchErrorState;

QString rawDataToSerialNumber(int guid, int devicetype);

void serialNumberToRawData(QString serial, int* guid, int* devicetype);

class TMaxdoasSettings
{
public:

    void workwiththisserial(QString serial);
    QString getLastScannerSerial();

    bool isInConfigMode();

    QString getPreferredSpecSerial();
    void setPreferredSpecSerial(QString s);

    int getRetrievalAvgCount();
    void setRetrievalAvgCount(int s);

    TSPectrWLCoefficients getWaveLengthCoefficients(QString serial);
    void setWaveLengthCoefficients(QString serial, TSPectrWLCoefficients coef,bool lock, bool alwaysUseTheseCoef);

    TAutoIntegConf getAutoIntegrationRetrievalConf();
    void setAutoIntegrationRetrievalConf(TAutoIntegConf AutoIntegrationConf);

    TCOMPortConf getComPortConfiguration();
    void setComPortConfiguration(TCOMPortConf ComPortConfiguration);

    float getTargetTemperature();
    void setTargetTemperature(float Temperature);

    QPoint getTiltOffset();
    void setTiltOffset(QPoint TiltOffset);

    QString getQDoasPath();
    void setQDoasPath(QString path);

    QPoint getTiltMaxValue();
    void setTiltMaxValue(QPoint MaxVal);

    QPoint getTiltMinValue();
    void setTiltMinValue(QPoint Minval);

//    float getTiltZenithDirection();
//    void setTiltZenithDirection(float TiltZenithDirection);

    QPoint getTiltZenith();
    void setTiltZenith(QPoint TiltZenith);

    TScannerDeviceType getAttachedScanningDevice(void);
    void setAttachedScanningDevice(TScannerDeviceType sdt);

    TScannerDeviceType askAttachedScanningDevice(void);

    QPoint getZenithSteps();
    void setZenithSteps(QPoint ZenithPosition);

    QPoint getMicrostepping();
    void setMicrostepping(QPoint Microsteps);

    QPointF getMotorStepAngle();
    void setMotorStepAngle(QPointF MotorStepAngle);

    int getShutterClosePos();
    void setShutterClosePos(int ClosePosition);

    QString getSiteName();
    void setSiteName(QString SiteName);

    float getScannerDirection();
    void setScannerDirection(float ScannerDirection);

    float getSiteLatitude();
    void setSiteLatitude(float SiteLatitude);

    float getSiteLongitude();
    void setSiteLongitude(float SiteLongitude);

    static TMaxdoasSettings* instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();

            if (!m_Instance)
                m_Instance = new TMaxdoasSettings;

            mutex.unlock();
        }

        return m_Instance;
    }

    static void drop()
    {
        static QMutex mutex;
        mutex.lock();
        delete m_Instance;
        m_Instance = 0;
        mutex.unlock();
    }

private:
    TMaxdoasSettings();
    ~TMaxdoasSettings();
    TMaxdoasSettings(const TMaxdoasSettings &); // hide copy constructor
    TMaxdoasSettings& operator=(const TMaxdoasSettings &); // hide assign op
                                 // we leave just the declarations, so the compiler will warn us
                                 // if we try to use those two functions by accident

    static TMaxdoasSettings* m_Instance;
    QSettings *settings;
    QString serialnum;
};





#endif // MAXDOASSETTINGS_H
