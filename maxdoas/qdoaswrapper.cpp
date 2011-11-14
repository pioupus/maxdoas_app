#include "qdoaswrapper.h"
#include <QProcess>

QDoasConfigFile::QDoasConfigFile(QString fn){
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

QDoasWrapper* QDoasWrapper::m_Instance = 0;


QDoasWrapper::QDoasWrapper()
{
    ms = TMaxdoasSettings::instance();
}

QDoasWrapper::~QDoasWrapper(){

}

bool QDoasWrapper::retrieve(TSpectrum spectrum, QDoasConfigFile cf){

}

bool QDoasWrapper::retrieve(TSpectralImage specImage, QDoasConfigFile cf){

}

TRetrievalImage* QDoasWrapper::getRetrievalImage(QString symbol){

}

double QDoasWrapper::getRetrieval(QString symbol){

}
