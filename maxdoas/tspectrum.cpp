#include "tspectrum.h"
#include <QSettings>
#include <QDir>
#include <QObject>

TSpectrum::TSpectrum(QObject* parent){
    setParent(parent);
    Wavelength = TWavelengthbuffer::instance();
    type = stNone;
    position.setX(0);
    position.setY(0);
    IntegTime = 0;
    AvgCount = 0;
}


TSpectrum::~TSpectrum(){

}

QDateTime TSpectrum::GetDateTime(){
    return datetime;
}

QString TSpectrum::GetSequenceFileName(QString Directory, QString BaseName, uint Sequence){
//    QDir dir(Directory);
//    dir.setFilter(QDir::Files);
//    dir.setSorting(QDir::Time);
//
//    QFileInfoList list = dir.entryInfoList();
//    std::cout << "     Bytes Filename" << std::endl;
//    for (int i = 0; i < list.size(); ++i) {
//        QFileInfo fileInfo = list.at(i);
//        std::cout << qPrintable(QString("%1 %2").arg(fileInfo.size(), 10)
//                                                .arg(fileInfo.fileName()));
//        std::cout << std::endl;
//    }

}

void TSpectrum::SaveSpectrSTD(QString fn){

}

void TSpectrum::SaveSpectrMetaAscii(QString fn){

}

void TSpectrum::SaveSpectrSTD_DefaultName(QString Directory, QString BaseName, uint SquenceNr){

}

void TSpectrum::SaveSpectrMetaAscii_DefaultName(QString Directory, QString BaseName, uint SquenceNr){

}



bool TSpectrum::LoadSpectrSTD(QString fn){

}

bool TSpectrum::LoadSpectrMetaAscii(QString fn){

}

bool TSpectrum::LoadSpectrSTD_Sequ(QString Directory, QString BaseName, uint Sequence){
    QSettings settings(GetSequenceFileName(Directory, BaseName,Sequence),QSettings::IniFormat);

}


bool TSpectrum::LoadSpectrEMT(QString fn){

}

