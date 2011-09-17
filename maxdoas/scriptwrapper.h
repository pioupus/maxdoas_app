#ifndef SCRIPTWRAPPER_H
#define SCRIPTWRAPPER_H
#include <QScriptEngine>
#include <QScriptValue>
#include <QScriptContext>
#include <QMutex>
#include <QObject>
#include "thwdriver.h"

class TScanner: public QObject
{
    Q_OBJECT
public:
    void WaitForSpectrum();
    THWDriver *getHWDriver();
    uint    IntegrationTime;
    static TScanner* instance(THWDriver* hwdriver)
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();

            if (!m_Instance){
                m_Instance = new TScanner();
                m_Instance->setHWDriver(hwdriver);
            }
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
private slots:
    void on_GotSpectrum();
private:
    TScanner();
    ~TScanner();
    TScanner(const TScanner &); // hide copy constructor
    TScanner& operator=(const TScanner &); // hide assign op
                                 // we leave just the declarations, so the compiler will warn us
                                 // if we try to use those two functions by accident

    static TScanner* m_Instance;
    void setHWDriver(THWDriver* hwdriver);
    THWDriver* hwdriver;
    bool GotSpectrum;
};

class TScriptWrapper
{
public:
    TScriptWrapper(THWDriver* hwdriver);
    ~TScriptWrapper();
    QScriptEngine * getScriptEngine();
    void startScriptFile(QString fn);
    void startScript(QString test);
    void abort();

private:
  //  QScriptValue DoSleep(QScriptContext *context, QScriptEngine *engine);
    QScriptEngine *ScriptEngine;
    bool aborting;
    TScanner* scanner;
};




#endif // SCRIPTWRAPPER_H
