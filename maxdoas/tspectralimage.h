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
#include "tretrievalimage.h"
#include "tmirrorcoordinate.h"
#include "tscanpath.h"

#include "log4qt/consoleappender.h"
#include "log4qt/logger.h"
#include "log4qt/ttcclayout.h"

class TSpectralImage : public QObject, protected QScriptable
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit TSpectralImage(TScanPath *parent);
    ~TSpectralImage();

    TSpectrum* getMean_();
    TSpectrum* getRms_();
    TSpectrum* getStdDev_();

    TMirrorCoordinate* getMaxRMSPos_();
    TMirrorCoordinate * getMirrorCoordinate(int index);
    TSpectrum * getSpectrum(int index);
    int getxCount();
    int getyCount();
    bool getPositionArray(TRetrieval* **buffer, int cntX, int cntY);
    TRetrievalImage* getIntensityImage();
    int getPixelIndex(TMirrorCoordinate* mc);
    QString getFileName();
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
    QList<TPatternType*> Patternsources;
    bool getPositionLine(QPointF P1, QPointF P2, QList<TMirrorCoordinate*> &Points, int div, QMap<float, TMirrorCoordinate*> &line,bool PermitRemovePoints);
    bool getPositionArrayRect(TPatternType *pt, TRetrieval* **buffer, int cntX, int cntY);
    bool getPositionArrayLine(TPatternType *pt, TRetrieval* **buffer, int cntX, int cntY);
    QString fn;
public slots:
    void plot(int plotIndex,int Pixelsize=10);

    void add(TMirrorCoordinate* coord, TSpectrum* spektrum);
    void save(QString Directory,QString BaseName,int SequenceNumber);
    void save(QString FileName);
    void saveTmp(QString FileName);
    bool Load(QString Directory, QString BaseName,int seqnumber,uint startindex,uint groupindex);
    bool Load(QString fn);

    int count();
    QScriptValue getMean();
    QScriptValue getRms();
    QScriptValue getStdDev();

    QScriptValue getMaxRMSPos();

    QScriptValue getIntensityArray();

    double getMaxRMSVal();
};

#endif // TSPECTRALIMAGE_H
