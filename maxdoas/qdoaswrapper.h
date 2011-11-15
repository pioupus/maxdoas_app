#ifndef QDOASWRAPPER_H
#define QDOASWRAPPER_H
#include "maxdoassettings.h"
#include "tretrievalimage.h"
#include "tspectralimage.h"
#include "tspectrum.h"
#include <QScriptEngine>
#include <QScriptContext>
#include <QScriptValue>
#include <QXmlStreamReader>
#include <QDomDocument>
#include <QStringList>

class QDoasConfigFile:public QObject
{
    Q_OBJECT
public:
    QDoasConfigFile(QString fn);
    ~QDoasConfigFile();
    QString getName();
public slots:
    bool load(QString fn);
    bool save(QString fn);


    bool setInputDirectory(QString fn);

    bool setOffset(TSpectrum *Offset);
    bool setXSRef(QString calibfn,TSpectrum *Spectrum);
    bool setCalRef(QString calibfn,TSpectrum *Spectrum);
    bool setUSAMPRef(QString calibfn,TSpectrum *Spectrum);

private:
    QDomDocument *qdoasfile;
};

class QDoasWrapper:public QObject, protected QScriptable
{
    Q_OBJECT
public slots:

    bool retrieve(TSpectrum *spectrum, QDoasConfigFile *cf);
    bool retrieve(TSpectralImage *specImage, QDoasConfigFile *cf);

    QScriptValue getRetrievalImage(QString symbol);
    double getRetrieval(QString symbol);

    void setOutPath(QString path);
public:

    TRetrievalImage* getRetrievalImage_(QString symbol);
    void appendTempFileNameToDeleteLater(QString s);
    QString retrieve(QString in,QString defname, QDoasConfigFile *cf);
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
    QStringList tempFiles;
    QString outPath;
    QString lastoutfile;
    TRetrievalImage *lastposImage;

};

#endif // QDOASWRAPPER_H
