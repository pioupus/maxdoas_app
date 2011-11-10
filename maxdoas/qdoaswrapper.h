#ifndef QDOASWRAPPER_H
#define QDOASWRAPPER_H
#include "maxdoassettings.h"
#include "tretrievalimage.h"
#include <QXmlStreamReader>

class QDoasConfigFile
{
public:
    QDoasConfigFile();
    ~QDoasConfigFile();
private:
    QXmlStreamReader *qdoasfile;
};

class QDoasWrapper
{
public:
    QDoasWrapper();
    bool run(QDoasConfigFile cf);
    TRetrievalImage* getRetrieval(QString symbol);
private:
    TMaxdoasSettings* ms;
};

#endif // QDOASWRAPPER_H
