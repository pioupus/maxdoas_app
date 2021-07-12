#ifndef TEMISSIONRATE_H
#define TEMISSIONRATE_H
#include "tscanpath.h"
#include <QRectF>
#include <QPointF>
#include <QList>
#include <QDateTime>
#include <QObject>
#include <tretrievalimage.h>

typedef struct{
    float time;
    float Emission;
    float PrevEmission;
    float NextEmission;
    QPointF CorridorPoint;
    QPointF CorridorDirection;
    QPointF CorridorPointMeters;
    float CorridorLength;
    float correlation;
    float PrevEmissionForCorrelation;
    float NextEmissionForCorrelation;
}tEmissionentry;

class TEmissionrate: public QObject
{
Q_OBJECT
public:
    TEmissionrate();
    ~TEmissionrate();
    void AddEmmision(float datetime, float emission, TParamLine* corridor,QPointF PositionInMeters);
    void prependEmmision(float datetime, float emission, TParamLine* corridor,QPointF PositionInMeters);
    void setTimeOffset(float TimeOffset);

    void setMainDirectionLine(QPointF startpoint, QPointF direction,QRectF PlotRegion);

    void setImageSize(QRectF rect);
    QRectF getImageSize();
    int count();
    int countCorrelation();
    tEmissionentry* EmissionAt(int index);
    QPointF CorrelationAt(int index);
    QPointF MainDirectionLineP1;
    QPointF MainDirectionLineP2;
    void setTimeDiff(float dt);
    void setPixelCounts(QPoint pc);

    float correlationLineFitParameterPrev_a0;
    float correlationLineFitParameterPrev_a1;
    float correlationLineFitParameterNext_a0;
    float correlationLineFitParameterNext_a1;

private:
    QList<tEmissionentry*> EmissionList;
    bool corridorpainted;
    QRectF ImageSize;
    void calcEmission_(TRetrievalImage* retimage, bool Prev,bool Next);
    QList<QPointF> CorrelationList;
    float timeDiff;
    float meanDistance;
    float emissionFactor;
    QPoint PixelCounts;

    float maxCorridorLengthEnd;
    float maxCorridorLengthStart;
    float corridornumberlimit;
    float corridornumberStart;


public slots:
    void plot(int plotindex);
    void oplot(int plotindex);
    void oplotEmissionRaster(int plotindex);
    void calcEmission(TRetrievalImage* retimage);
    void calcEmissionPrev(TRetrievalImage* retimage);
    void calcEmissionNext(TRetrievalImage* retimage);
    void calcCorrelation();
    float getMaxCorrelationPos();
    float getMaxCorrelationVal();
    void plotCorrelation(int plotindex);
    float getCorrelationCorrectionfactor();
    void setMeanDistance(float distance);
    void setEmissionFactor(float ef);
    void save(QString fn);

    void setMaxCorridorLength(float start, float end);
    void setCorridorNumberLimit(float start, float end);

};

#endif // TEMISSIONRATE_H
