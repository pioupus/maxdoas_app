#include "qdoaswrapper.h"
#include <QProcess>

QDoasConfigFile::QDoasConfigFile(){
    qdoasfile = new QXmlStreamReader();
}

QDoasConfigFile::~QDoasConfigFile(){
    delete qdoasfile;
}


bool QDoasConfigFile::load(QString fn){

}

bool QDoasConfigFile::save(QString fn){

}

bool QDoasConfigFile::setOffset(QString fn){

}

bool QDoasConfigFile::setReference(QString fn){

}

bool QDoasConfigFile::setInputDirectory(QString fn){
}




QDoasWrapper::QDoasWrapper()
{
    ms = TMaxdoasSettings::instance();
}

bool QDoasWrapper::retrieve(TSpectrum spectrum, QDoasConfigFile cf){

}

bool QDoasWrapper::retrieve(TSpectralImage specImage, QDoasConfigFile cf){

}

TRetrievalImage* QDoasWrapper::getRetrieval(QString symbol){

}
