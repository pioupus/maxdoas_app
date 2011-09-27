#ifndef TSPECTRALIMAGE_H
#define TSPECTRALIMAGE_H

#include <QObject>
#include <QHash>
#include <QPair>
#include <QDateTime>
#include <QHashIterator>
#include <QFile>
#include <QScriptEngine>
#include <QScriptContext>
#include <QScriptValue>
#include <QScriptable>
#include "tspectrum.h"
#include "tmirrorcoordinate.h"

class TSpectralImage : public QObject, protected QScriptable
{
Q_OBJECT
public:
    explicit TSpectralImage(QObject *parent = 0);
    ~TSpectralImage();

    TSpectrum* getMean_();
    TSpectrum* getRms_();
    TSpectrum* getStdDev_();

    TMirrorCoordinate* getMaxRMSPos_();
    TMirrorCoordinate * getMirrorCoordinate(int index);
    TSpectrum * getSpectrum(int index);

private:
    QHash<QPair<int,int>, QPair<TSpectrum*,double> > spectrumtable;
    QDateTime FirstDate;
    QList<TSpectrum*> spectrumlist;
    TSpectrum* meanSpectrum;
    TSpectrum* rmsSpectrum;
    TSpectrum* stdDevSpectrum;
    TMirrorCoordinate *maxRMSPos;
    double maxRMSVal;
    bool isChanged();

public slots:
    void plot(int plotIndex,int Pixelsize=10);

    void add(TMirrorCoordinate* coord, TSpectrum* spektrum);
    void save(QString Directory,QString BaseName,int SequenceNumber);
    void save(QString FileName);
    int count();
    QScriptValue getMean();
    QScriptValue getRms();
    QScriptValue getStdDev();

    QScriptValue getMaxRMSPos();

    double getMaxRMSVal();
};

#endif // TSPECTRALIMAGE_H
