#ifndef WAVELEGNTHBUFFER_H
#define WAVELEGNTHBUFFER_H
#include <QString>
#include <QSettings>

//#include "maxdoassettings.h"

#include <QMutex>

#define MAXWAVELEGNTH_BUFFER_ELEMTENTS 4096

struct TSPectrWLCoefficients {
    double Offset;
    double Coeff1;
    double Coeff2;
    double Coeff3;
    bool overWrittenFromFile;
    bool uninitialized;
} ;

class TWavelengthbuffer
{
public:
    void setCoefficients(TSPectrWLCoefficients* coeff);
    TSPectrWLCoefficients hwdGetWLCoefficients();

    void setUnintialised();
    bool isInitialised();


    double buf[MAXWAVELEGNTH_BUFFER_ELEMTENTS];

    static TWavelengthbuffer* instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();

            if (!m_Instance)
                m_Instance = new TWavelengthbuffer();

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


    TWavelengthbuffer();
    ~TWavelengthbuffer();
    TWavelengthbuffer(const TWavelengthbuffer &); // hide copy constructor
    TWavelengthbuffer& operator=(const TWavelengthbuffer &); // hide assign op
                                 // we leave just the declarations, so the compiler will warn us
                                 // if we try to use those two functions by accident

    static TWavelengthbuffer* m_Instance;
    TSPectrWLCoefficients SpectrCoefficients;
    bool initialised;
};
#endif // WAVELEGNTHBUFFER_H
