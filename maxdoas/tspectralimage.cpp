#include "tspectralimage.h"
#include <QHashIterator>
#include <QFile>
#include <QFileInfo>
#include <QScriptEngine>
#include <QScriptContext>
#include <QScriptValue>
#include <tspectrumplotter.h>
#include <math.h>

TSpectralImage::TSpectralImage(QObject *parent) :
    QObject(parent)
{
  //  FirstDate = 0;
    meanSpectrum = NULL;
    rmsSpectrum = NULL;
    stdDevSpectrum = NULL;
    maxRMSPos = NULL;
}

TSpectralImage::~TSpectralImage(){
    QHashIterator<QPair<int,int>, QPair<TSpectrum*,double> > i(spectrumtable);
    QPair<int,int> key;
    TSpectrum* spektrum;
    int c = spectrumtable.count();
    if(c==0)
        key.first = 1;
    while (i.hasNext()) {
        QPair<TSpectrum*,double> val;
        val = i.next().value();
        spektrum = val.first;
        delete spektrum;
    }
    spectrumtable.clear();
    spectrumlist.clear();
}

void TSpectralImage::add(TMirrorCoordinate* coord, TSpectrum* spektrum){
    QPair<int,int> key;

    QPair<TSpectrum*,double> val;

    key.first = coord->getMotorCoordinate().x();
    key.second = coord->getMotorCoordinate().y();

    val.first = spektrum;
    val.second = -1;
    spectrumtable.insert(key,val);

    if (spectrumtable.count() == 1){
        FirstDate = spektrum->GetDateTime();
    }else{
        if (spektrum->GetDateTime() < FirstDate){
            FirstDate = spektrum->GetDateTime() ;
        }
    }
    spectrumlist.append(spektrum);
    delete meanSpectrum;
    delete rmsSpectrum;
    delete stdDevSpectrum;
    delete maxRMSPos;
    stdDevSpectrum = NULL;
    rmsSpectrum = NULL;
    meanSpectrum = NULL;
    maxRMSPos = NULL;
}

void TSpectralImage::save(QString FileName){
    QFile data(FileName);
    QFile meta(fnToMetafn(FileName));
    if (data.open(QFile::WriteOnly | QFile::Truncate) && meta.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream datastream(&data);
        QTextStream metastream(&meta);
        QHashIterator<QPair<int,int>, QPair<TSpectrum*,double> > i(spectrumtable);
        TSpectrum* spektrum;
        metastream << "time" << '\t';

        metastream << "Fixed Axis" << '\t';
        metastream << "Mirror Axis" << '\t';

        metastream << "IntegStyle" << '\t';
        metastream << "targetPeak" << '\t';
        metastream << "targetCorridor" << '\t';
        metastream << "maxIntegTime" << '\t';

        metastream << "IntegTime" << '\t';
        metastream << "AvgCount" << '\t';
        metastream << "Temperature" << '\t';
        metastream << "Max possible value\n";
        while (i.hasNext()) {
             QPair<TSpectrum*,double> val;
            val = i.next().value();
            spektrum = val.first;
            spektrum->SaveSpectrum(datastream,metastream);
        }
        data.close();
    }
}

void TSpectralImage::save(QString Directory,QString BaseName,int SequenceNumber){
    QString fn;
    fn = DefaultFileNameFromSeqNumber(Directory,BaseName,SequenceNumber,FirstDate)+".spe";
    save(fn);
}

bool TSpectralImage::Load(QString Directory, QString BaseName,int seqnumber,uint groupindex){
    QString filename = GetSequenceFileName(Directory,BaseName,seqnumber,groupindex);
    return Load(filename);
}

bool TSpectralImage::Load(QString fn){
    bool result=false;
    if (fn == "")
        return false;
    QFile dataf(fn);
    QFile metaf(fnToMetafn(fn));
    if ((dataf.open(QIODevice::ReadOnly | QIODevice::Text))&&(metaf.open(QIODevice::ReadOnly | QIODevice::Text))){
        bool ok;
        TSpectrum *spec;
        QTextStream data(&dataf);
        QTextStream meta(&metaf);
        meta.readLine();
        ok = true;
        while (ok){
            spec = new TSpectrum();
            ok = spec->LoadSpectrum(data,meta);
            if(ok){
                add(spec->getMirrorCoordinate(), spec);
                result = true;
            }else{
                delete spec;
            }
        }
        dataf.close();
        metaf.close();

    }

    return result;
}

bool TSpectralImage::isChanged(){
    bool result = false;
    TSpectrum* spektrum;
    QHashIterator<QPair<int,int>,  QPair<TSpectrum*,double> > i(spectrumtable);

    while (i.hasNext()) {
        QPair<TSpectrum*,double> val;
        val = i.next().value();
        spektrum = val.first;
        if (spektrum->getHash() != val.second){
            i.toFront();    //for not needing to iterate whole hash next time
            val.second = -1;
            spectrumtable[i.next().key()] = val;
            result = true;
            delete meanSpectrum;
            delete rmsSpectrum;
            delete stdDevSpectrum;
            delete maxRMSPos;
            stdDevSpectrum = NULL;
            rmsSpectrum = NULL;
            meanSpectrum = NULL;
            maxRMSPos = NULL;
            break;
        }
    }
    return result;
}

TSpectrum* TSpectralImage::getMean_(){
    TSpectrum* spektrum;
    int cnt=0;
    QHashIterator<QPair<int,int>,  QPair<TSpectrum*,double> > i(spectrumtable);
    bool changed=isChanged();
    if (changed || meanSpectrum == NULL){
        meanSpectrum = new TSpectrum();
        while (i.hasNext()) {
            QPair<TSpectrum*,double> val;
            val = i.next().value();
            spektrum = val.first;
            if (cnt == 0){
                meanSpectrum->NumOfSpectrPixels = spektrum->NumOfSpectrPixels;
                meanSpectrum->MaxPossibleValue = spektrum->MaxPossibleValue;
                meanSpectrum->setZero();
            }
            meanSpectrum->add(spektrum);
            if (changed){
                val.second = spektrum->getHash();
                spectrumtable[i.key()] = val;
            }
            cnt++;
        }
        meanSpectrum->div(cnt);
    }
    return new TSpectrum(meanSpectrum);
}

QScriptValue TSpectralImage::getMean(){
    TSpectrum * spektrum = getMean_();
    return engine()->newQObject(spektrum);
}

TSpectrum* TSpectralImage::getRms_(){
    TSpectrum* spektrum;
    int cnt=0;
    QHashIterator<QPair<int,int>,  QPair<TSpectrum*,double> > i(spectrumtable);
    bool changed=isChanged();
    if (changed || rmsSpectrum == NULL){
        rmsSpectrum = new TSpectrum();
        while (i.hasNext()) {
            QPair<TSpectrum*,double> val;
            val = i.next().value();
            spektrum = val.first;
            if (cnt == 0){
                rmsSpectrum->NumOfSpectrPixels = spektrum->NumOfSpectrPixels;
                rmsSpectrum->MaxPossibleValue = spektrum->MaxPossibleValue;
                rmsSpectrum->setZero();
            }
            for (int n = 0;n<spektrum->NumOfSpectrPixels;n++){
                double v = spektrum->spectrum[n];
                v = pow(v,2);
                rmsSpectrum->spectrum[n] += v;
            }
            rmsSpectrum->MaxPossibleValue += pow(spektrum->MaxPossibleValue,2);
            if (changed){
                val.second = spektrum->getHash();
                spectrumtable[i.key()] = val;
            }
            cnt++;
        }
        rmsSpectrum->div(cnt);
        for (int n = 0;n<spektrum->NumOfSpectrPixels;n++){
            rmsSpectrum->spectrum[n] = sqrt(rmsSpectrum->spectrum[n]);
        }
        rmsSpectrum->MaxPossibleValue = sqrt(rmsSpectrum->MaxPossibleValue);
    }
    return new TSpectrum(rmsSpectrum);
}


QScriptValue TSpectralImage::getRms(){
    TSpectrum * spektrum = getRms_();
    return engine()->newQObject(spektrum);
}

TSpectrum* TSpectralImage::getStdDev_(){
    TSpectrum* spektrum;
    int cnt=0;
    QHashIterator<QPair<int,int>,  QPair<TSpectrum*,double> > i(spectrumtable);
    bool changed=isChanged();
    getMean();
    if (changed || stdDevSpectrum == NULL){
        stdDevSpectrum = new TSpectrum();
        while (i.hasNext()) {
            QPair<TSpectrum*,double> val;
            val = i.next().value();
            spektrum = val.first;
            if (cnt == 0){
                stdDevSpectrum->NumOfSpectrPixels = spektrum->NumOfSpectrPixels;
                stdDevSpectrum->MaxPossibleValue = spektrum->MaxPossibleValue;
                stdDevSpectrum->setZero();
            }
            for (int n = 0;n<spektrum->NumOfSpectrPixels;n++){
                stdDevSpectrum->spectrum[n] += pow((spektrum->spectrum[n] - meanSpectrum->spectrum[n]),2);
            }
            stdDevSpectrum->MaxPossibleValue += pow(spektrum->MaxPossibleValue - spektrum->mean(),2);
            if (changed){
                val.second = spektrum->getHash();
                spectrumtable[i.key()] = val;
            }
            cnt++;
        }
        stdDevSpectrum->div(cnt);
        for (int n = 0;n<spektrum->NumOfSpectrPixels;n++){
            stdDevSpectrum->spectrum[n] = sqrt(rmsSpectrum->spectrum[n]);
        }
        stdDevSpectrum->MaxPossibleValue = sqrt(stdDevSpectrum->MaxPossibleValue);

    }
    return new TSpectrum(stdDevSpectrum);
}


QScriptValue TSpectralImage::getStdDev(){
    TSpectrum * spektrum = getStdDev_();
    return engine()->newQObject(spektrum);
}

TMirrorCoordinate* TSpectralImage::getMaxRMSPos_(){
    TSpectrum* spektrum;
    QHashIterator<QPair<int,int>,  QPair<TSpectrum*,double> > i(spectrumtable);
    bool changed=isChanged();
    if (changed || maxRMSPos == NULL){
        maxRMSVal = 0;
        while (i.hasNext()) {
            QPair<TSpectrum*,double> val;
            val = i.next().value();
            spektrum = val.first;
            if (spektrum->rms() > maxRMSVal){
                maxRMSVal = spektrum->rms();
                maxRMSPos = spektrum->getMirrorCoordinate();
            }
        }
    }
    return maxRMSPos;
}

QScriptValue TSpectralImage::getMaxRMSPos(){
    TMirrorCoordinate * MirrorCoordinate = getMaxRMSPos_();
    return engine()->newQObject(MirrorCoordinate);
}

double TSpectralImage::getMaxRMSVal(){
    getMaxRMSPos();
    return maxRMSVal;
}

int TSpectralImage::count(){
    return spectrumtable.count();
}

TMirrorCoordinate * TSpectralImage::getMirrorCoordinate(int index){
    return spectrumlist[index]->getMirrorCoordinate();
}

TSpectrum * TSpectralImage::getSpectrum(int index){
    return spectrumlist[index];
}

void TSpectralImage::plot(int plotIndex,int Pixelsize){
    TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(0);
    SpectrumPlotter->plotSpectralImage(this,plotIndex,Pixelsize);
}
