#include "qdoaswrapper.h"
#include <QProcess>

QDoasConfigFile::QDoasConfigFile(){
    qdoasfile = new QXmlStreamReader();
}

QDoasConfigFile::~QDoasConfigFile(){
    delete qdoasfile;
}


QDoasWrapper::QDoasWrapper()
{
    ms = TMaxdoasSettings::instance();
}
