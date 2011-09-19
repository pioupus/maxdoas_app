#include "tspectrum.h"
#include "tspectrumplotter.h"
#include <QSettings>
#include <QDir>
#include <QObject>
#include <QDateTime>

TSpectrum::TSpectrum(QObject* parent){
    setParent(parent);
    Wavelength = TWavelengthbuffer::instance();
    type = stNone;
    position.setX(0);
    position.setY(0);
    IntegTime = 0;
    AvgCount = 0;
    SequenceNumber = -1;
    datetime = QDateTime::currentDateTime();
    rmsval = -1;
    changeindicator = -1;
    meanval = -1;
    stddevval = -1;
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

void TSpectrum::SaveSpectrum(QTextStream &file){
    file.setRealNumberPrecision(6);
    for(int i = 0;i<NumOfSpectrPixels;i++){
        file << spectrum[i] << '\t';
    }
    file << '\n';
}

void TSpectrum::SaveSpectrum(QString fn){
    QFile data(fn);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&data);
        SaveSpectrum(out);
        data.close();
    }
}

void TSpectrum::SaveSpectrumDefName(QString Directory, QString BaseName,int seqnumber){
    QString seq;
    if (SequenceNumber==-1)
        SequenceNumber = seqnumber;
    seq = QString::number(SequenceNumber).rightJustified(5, '0');
    SaveSpectrum(Directory+'/'+BaseName+'_'+datetime.toString("dd_MM_yyyy__hh_mm_ss")+"_seq"+seq+".spe");
}


bool TSpectrum::LoadSpectrSTD(QString fn){
    changeindicator = -1;
}

bool TSpectrum::LoadSpectrDefaultName(QString Directory, QString BaseName,int seqnumber){
    QString seq;
    if (SequenceNumber==-1)
        SequenceNumber = seqnumber;
    seq = QString::number(SequenceNumber).rightJustified(5, '0');
    LoadSpectrSTD(Directory+'/'+BaseName+'_'+datetime.toString("dd_MM_yyyy__hh_mm_ss")+"_seq"+seq+".spe");

}


bool TSpectrum::LoadSpectrEMT(QString fn){

}

void TSpectrum::plot(int index){
    TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance();
    SpectrumPlotter->plotSpectrum(this,index);
}

void TSpectrum::add(QObject *spect){
    TSpectrum *spec = dynamic_cast<TSpectrum*>(spect);
    if (spec != NULL){
        if (NumOfSpectrPixels == spec->NumOfSpectrPixels){
            for(int i = 0;i<NumOfSpectrPixels;i++){
                this->spectrum[i] = this->spectrum[i] + spec->spectrum[i];
            }
            changeindicator = -1;
            MaxPossibleValue += spec->MaxPossibleValue;
        }

    }
}

void TSpectrum::add(double val){
    for(int i = 0;i<NumOfSpectrPixels;i++){
        this->spectrum[i] = this->spectrum[i] + val;
    }
    MaxPossibleValue += val;
    changeindicator = -1;
}

void TSpectrum::sub(QObject *spect){
    TSpectrum *spec = dynamic_cast<TSpectrum*>(spect);
    if (spec != NULL){
        if (NumOfSpectrPixels == spec->NumOfSpectrPixels){
            for(int i = 0;i<NumOfSpectrPixels;i++){
                this->spectrum[i] = this->spectrum[i] - spec->spectrum[i];
            }
            changeindicator = -1;
        }
    }
}

void TSpectrum::sub(double val){
    for(int i = 0;i<NumOfSpectrPixels;i++){
        this->spectrum[i] = this->spectrum[i] - val;
    }
    changeindicator = -1;
}

void TSpectrum::mul(QObject *spect){
    TSpectrum *spec = dynamic_cast<TSpectrum*>(spect);
    if (spec != NULL){
        if (NumOfSpectrPixels == spec->NumOfSpectrPixels){
            for(int i = 0;i<NumOfSpectrPixels;i++){
                this->spectrum[i] = this->spectrum[i] * spec->spectrum[i];
            }
            changeindicator = -1;
        }
    }
}

void TSpectrum::mul(double val){
    for(int i = 0;i<NumOfSpectrPixels;i++){
        this->spectrum[i] = this->spectrum[i] * val;
    }
    changeindicator = -1;
}


void TSpectrum::div(QObject *spect){
    TSpectrum *spec = dynamic_cast<TSpectrum*>(spect);
    if (spec != NULL){
        if (NumOfSpectrPixels == spec->NumOfSpectrPixels){
            for(int i = 0;i<NumOfSpectrPixels;i++){
                if (spec->spectrum[i] != 0)
                    this->spectrum[i] = this->spectrum[i] / spec->spectrum[i];
                else
                     this->spectrum[i] = NAN;
            }
            changeindicator = -1;
        }
    }
}
void TSpectrum::div(double val){
    if (val == 0){
        for(int i = 0;i<NumOfSpectrPixels;i++){
            this->spectrum[i] = NAN;
        }
    }else{
        for(int i = 0;i<NumOfSpectrPixels;i++){
            this->spectrum[i] = this->spectrum[i] / val;
        }
    }
    changeindicator = -1;
}

bool TSpectrum::isSpectrumChanged(){
    double key=0;
    for(int i = (NumOfSpectrPixels>>2)-20;i<(NumOfSpectrPixels>>2)+20;i++){
        if (i>-1 && i<NumOfSpectrPixels)
            key+=spectrum[i];
    }
    if (key != changeindicator){
       rmsval = -1;
       meanval = -1;
       stddevval = -1;
   }
    changeindicator = key;
}

double TSpectrum::rms(){
    isSpectrumChanged();
    if (rmsval == -1){
        rmsval = 0;
        for(int i = 0;i<NumOfSpectrPixels;i++){
            rmsval += pow(this->spectrum[i],2);
        }
        rmsval /= NumOfSpectrPixels;
        rmsval = sqrt(rmsval);
    }
    return rmsval;
}

double TSpectrum::mean(){
    isSpectrumChanged();
    if (meanval == -1){
        meanval = 0;
        for(int i = 0;i<NumOfSpectrPixels;i++){
            meanval += this->spectrum[i];
        }
        meanval /= NumOfSpectrPixels;
    }
    return meanval;
}

double TSpectrum::stddev(){
    isSpectrumChanged();
    if (stddevval == -1){
        double m=mean();
        stddevval = 0;
        for(int i = 0;i<NumOfSpectrPixels;i++){
            stddevval += pow(this->spectrum[i]-m,2);
        }
        stddevval /= (NumOfSpectrPixels-1);
        stddevval = sqrt(stddevval);
    }
    return stddevval;
}
