#ifndef WAVELEGNTHBUFFER_H
#define WAVELEGNTHBUFFER_H
#include <QString>
#include <QSettings>

#include "maxdoassettings.h"

#include <QMutex>

class TWavelengthbuffer
{
public:

    double buf[MAXWAVELEGNTH_BUFFER_ELEMTENTS];

    static TWavelengthbuffer* instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();

            if (!m_Instance)
                m_Instance = new TWavelengthbuffer;

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
    TWavelengthbuffer(){};
    ~TWavelengthbuffer();
    TWavelengthbuffer(const TWavelengthbuffer &); // hide copy constructor
    TWavelengthbuffer& operator=(const TWavelengthbuffer &); // hide assign op
                                 // we leave just the declarations, so the compiler will warn us
                                 // if we try to use those two functions by accident

    static TWavelengthbuffer* m_Instance;
};
#endif // WAVELEGNTHBUFFER_H
