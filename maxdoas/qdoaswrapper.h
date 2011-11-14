#ifndef QDOASWRAPPER_H
#define QDOASWRAPPER_H
#include "maxdoassettings.h"
#include "tretrievalimage.h"
#include "tspectralimage.h"
#include "tspectrum.h"
#include <QXmlStreamReader>

class QDoasConfigFile
{
public:
    QDoasConfigFile();
    ~QDoasConfigFile();

    bool load(QString fn);
    bool save(QString fn);
    bool setOffset(QString fn);
    bool setReference(QString fn);
    bool setInputDirectory(QString fn);

private:
    QXmlStreamReader *qdoasfile;
};

class QDoasWrapper:QObject
{
    Q_OBJECT
public:
    QDoasWrapper();
    bool retrieve(TSpectrum spectrum, QDoasConfigFile cf);
    bool retrieve(TSpectralImage specImage, QDoasConfigFile cf);

    TRetrievalImage* getRetrieval(QString symbol);
private:
    TMaxdoasSettings* ms;
};

#endif // QDOASWRAPPER_H
