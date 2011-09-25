#include "tspectrum.h"
#include "tspectrumplotter.h"
#include <QSettings>
#include <QDir>
#include <QObject>
#include <QDateTime>

QString DefaultFileNameFromSeqNumber(QString Directory, QString BaseName,int seqnumber,QDateTime datetime){
    QString seq = QString::number(seqnumber).rightJustified(5, '0');
    return Directory+'/'+BaseName+'_'+datetime.toString("dd_MM_yyyy__hh_mm_ss")+"_seq"+seq;
}

TSpectrum::TSpectrum(QObject* parent){
    setParent(parent);
    Wavelength = TWavelengthbuffer::instance();
    type = stNone;
    memset(&IntegConf,0,sizeof(IntegConf));
    memset(&WLCoefficients,0,sizeof(WLCoefficients));
    WLCoefficients.Coeff1 = 1;
    IntegTime = 0;
    AvgCount = 0;
    SequenceNumber = -1;
    datetime = QDateTime::currentDateTime();
    rmsval = -1;
    hash = -1;
    meanval = -1;
    stddevval = -1;
    BaseName = "";
    MirrorCoordinate = NULL;

}

TSpectrum::TSpectrum(TSpectrum * other){
    Wavelength = TWavelengthbuffer::instance();
    type = other->type;
    IntegTime = other->IntegTime;
    IntegConf = other->IntegConf;
    AvgCount = other->AvgCount;
    SequenceNumber = other->SequenceNumber;
    datetime = other->datetime;
    rmsval = other->rmsval;
    hash = other->hash;
    meanval = other->meanval;
    stddevval = other->stddevval;
    BaseName = other->BaseName;
    MirrorCoordinate = NULL;
    Temperature = other->Temperature;
    WLCoefficients = other->WLCoefficients;
    NumOfSpectrPixels = other->NumOfSpectrPixels;
    MaxPossibleValue = other->MaxPossibleValue;
    setMirrorCoordinate(other->getMirrorCoordinate());
    memcpy(spectrum,other->spectrum,sizeof(double)*NumOfSpectrPixels);
}

TSpectrum::~TSpectrum(){
    if (MirrorCoordinate != NULL){
        delete MirrorCoordinate;
    }
}

void TSpectrum::setZero(){
    if (NumOfSpectrPixels==0){
        NumOfSpectrPixels = MAXWAVELEGNTH_BUFFER_ELEMTENTS;
    }
    memset(spectrum,0,sizeof(double)*NumOfSpectrPixels);
}

QDateTime TSpectrum::GetDateTime(){
    return datetime;
}

QString TSpectrum::GetSequenceFileName(QString Directory, QString BaseName, uint Sequence){
    (void)Directory;
    (void)BaseName;
    (void)Sequence;
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
    return "";
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

QString TSpectrum::getDefaultFileName(QString Directory, QString BaseName,int seqnumber){


    if (SequenceNumber==-1)
        SequenceNumber = seqnumber;

    if (seqnumber==-1)
        seqnumber = SequenceNumber;

    if (BaseName=="")
        BaseName = this->BaseName;

    if (this->BaseName=="")
        this->BaseName = BaseName;

    return DefaultFileNameFromSeqNumber(Directory,BaseName, seqnumber,datetime);

}


void TSpectrum::SaveSpectrumDefName(QString Directory, QString BaseName,int seqnumber){

    SaveSpectrum(getDefaultFileName( Directory, BaseName, seqnumber)+".spe");
}

void TSpectrum::setMirrorCoordinate(TMirrorCoordinate* mc){
    if (MirrorCoordinate != NULL){
        delete MirrorCoordinate;
    }
    if (mc == NULL)
        MirrorCoordinate = NULL;
    else
        MirrorCoordinate = new TMirrorCoordinate(mc);
}

TMirrorCoordinate * TSpectrum::getMirrorCoordinate(){
    return MirrorCoordinate;
}

bool TSpectrum::LoadSpectrSTD(QString fn){
    (void)fn;
    hash = -1;
    return false;
}

bool TSpectrum::LoadSpectrDefaultName(QString Directory, QString BaseName,int seqnumber){
    QString seq;
    if (SequenceNumber==-1)
        SequenceNumber = seqnumber;
    seq = QString::number(SequenceNumber).rightJustified(5, '0');
    return LoadSpectrSTD(Directory+'/'+BaseName+'_'+datetime.toString("dd_MM_yyyy__hh_mm_ss")+"_seq"+seq+".spe");

}


bool TSpectrum::LoadSpectrEMT(QString fn){
    (void) fn;
    return false;
}

void TSpectrum::plot(int index){
    TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(0);
    SpectrumPlotter->plotSpectrum(this,index);
}

void TSpectrum::add(QObject *spect){
    TSpectrum *spec = dynamic_cast<TSpectrum*>(spect);
    if (spec != NULL){
        if (NumOfSpectrPixels == spec->NumOfSpectrPixels){
            for(int i = 0;i<NumOfSpectrPixels;i++){
                this->spectrum[i] = this->spectrum[i] + spec->spectrum[i];
            }
            hash = -1;
            MaxPossibleValue += spec->MaxPossibleValue;
        }

    }
}

void TSpectrum::add(double val){
    for(int i = 0;i<NumOfSpectrPixels;i++){
        this->spectrum[i] = this->spectrum[i] + val;
    }
    MaxPossibleValue += val;
    hash = -1;
}

void TSpectrum::sub(QObject *spect){
    TSpectrum *spec = dynamic_cast<TSpectrum*>(spect);
    if (spec != NULL){
        if (NumOfSpectrPixels == spec->NumOfSpectrPixels){
            for(int i = 0;i<NumOfSpectrPixels;i++){
                this->spectrum[i] = this->spectrum[i] - spec->spectrum[i];
            }
            hash = -1;
        }
    }
}

void TSpectrum::sub(double val){
    for(int i = 0;i<NumOfSpectrPixels;i++){
        this->spectrum[i] = this->spectrum[i] - val;
    }
    hash = -1;
}

void TSpectrum::mul(QObject *spect){
    TSpectrum *spec = dynamic_cast<TSpectrum*>(spect);
    if (spec != NULL){
        if (NumOfSpectrPixels == spec->NumOfSpectrPixels){
            for(int i = 0;i<NumOfSpectrPixels;i++){
                this->spectrum[i] = this->spectrum[i] * spec->spectrum[i];
            }
            hash = -1;
        }
    }
}

void TSpectrum::mul(double val){
    for(int i = 0;i<NumOfSpectrPixels;i++){
        this->spectrum[i] = this->spectrum[i] * val;
    }
    hash = -1;
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
            hash = -1;

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
        MaxPossibleValue /= val;
    }
    hash = -1;
}

double TSpectrum::getHash(){
    double result=0;
    for(int i = (NumOfSpectrPixels>>2)-20;i<(NumOfSpectrPixels>>2)+20;i++){
        if (i>-1 && i<NumOfSpectrPixels)
            result+=spectrum[i];
    }
    return result;
}

bool TSpectrum::isSpectrumChanged(){
    double hash=getHash();
    bool result;

    result = hash != hash;
    if (result){
       rmsval = -1;
       meanval = -1;
       stddevval = -1;
   }
    this->hash = hash;
    return result;
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
        stddevval = sqrt(stddevval);
        stddevval /= (NumOfSpectrPixels-1);

    }
    return stddevval;
}
