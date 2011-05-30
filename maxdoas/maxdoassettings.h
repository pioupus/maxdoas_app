#ifndef MAXDOASSETTINGS_H
#define MAXDOASSETTINGS_H
#include <QString>
#include <QSettings>

#include <QMutex>

class TMaxdoasSettings
{
public:

    QString getPreferredSpecSerial();
    void setPreferredSpecSerial(QString s);

    int getRetrievalAvgCount();
    void setRetrievalAvgCount(int s);

    int getRetrievalIntegTimeuS();
    void setRetrievalIntegTimeuS(int uS);

    bool getRetrievalAutoEnabled();
    void setRetrievalAutoEnabled(bool i);

    int getRetrievalAutoTargetPeak();
    void setRetrievalAutoTargetPeak(int i);

    int getRetrievalAutoTargetPeakCorridor();
    void setRetrievalAutoTargetPeakCorridor(int i);

    int getRetrievalAutoMaxIntegTime();
    void setRetrievalAutoMaxIntegTime(int i);

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
