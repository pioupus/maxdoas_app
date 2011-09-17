#include "tspectralimage.h"
#include <QHashIterator>

TSpectralImage::TSpectralImage(QObject *parent) :
    QObject(parent)
{
  //  FirstDate = 0;
}

TSpectralImage::~TSpectralImage(){
    QHashIterator<QPair<int,int>, TSpectrum*> i(spectrumtable);
    TSpectrum* spektrum;
    while (i.hasNext()) {
        QPair<int,int> key;
        spektrum = i.next().value();
        key  = i.key();
        spectrumtable[key] = NULL;
        delete spektrum;
    }
}

void TSpectralImage::add(TMirrorCoordinate* coord, TSpectrum* spektrum){
    QPair<int,int> key;
    key.first = coord->getMotorCoordinate().x();
    key.second = coord->getMotorCoordinate().y();
    spectrumtable.insert(key,spektrum);
    if (spectrumtable.count() == 1){
        FirstDate = spektrum->GetDateTime();
    }else{
        if (spektrum->GetDateTime() < FirstDate){
            FirstDate = spektrum->GetDateTime() ;
        }
    }
}

void TSpectralImage::save(QString Directory,QString BaseName){

}
