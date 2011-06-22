#ifndef MAXDOASSETTINGS_H
#define MAXDOASSETTINGS_H
#include <QString>
#include <QSettings>

#include <QMutex>
struct TSPectrWLCoefficients{
    double Offset;
    double Coeff1;
    double Coeff2;
    double Coeff3;
    bool overWrittenFromFile;
    bool uninitialized;
};

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

class TMaxdoasSettings
{
public:

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
};





#endif // MAXDOASSETTINGS_H
