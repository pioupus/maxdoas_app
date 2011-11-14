#ifndef QDOASWRAPPER_H
#define QDOASWRAPPER_H
#include "maxdoassettings.h"
#include "tretrievalimage.h"
#include "tspectralimage.h"
#include "tspectrum.h"
#include <QXmlStreamReader>

class QDoasConfigFile:public QObject
{
    Q_OBJECT
public:
    QDoasConfigFile(QString fn);
    ~QDoasConfigFile();

    bool load(QString fn);
    bool save(QString fn);

    bool setOffset(QString fn);
    bool setReference(QString fn);
    bool setInputDirectory(QString fn);

    bool setOffset(TSpectrum *Offset);
    bool setReference(TSpectrum *Spectrum);

private:
    QXmlStreamReader *qdoasfile;
};

class QDoasWrapper:public QObject
{
    Q_OBJECT
public:
    bool retrieve(TSpectrum spectrum, QDoasConfigFile cf);
    bool retrieve(TSpectralImage specImage, QDoasConfigFile cf);

    TRetrievalImage* getRetrievalImage(QString symbol);
    double getRetrieval(QString symbol);

    static QDoasWrapper* instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();

            if (!m_Instance){
                m_Instance = new QDoasWrapper();
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
private:
    QDoasWrapper();
    ~QDoasWrapper();
    QDoasWrapper(const QDoasWrapper &); // hide copy constructor
    QDoasWrapper& operator=(const QDoasWrapper &); // hide assign op
                                 // we leave just the declarations, so the compiler will warn us
                                 // if we try to use those two functions by accident

    static QDoasWrapper* m_Instance;
    TMaxdoasSettings* ms;
};

#endif // QDOASWRAPPER_H
