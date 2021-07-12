#ifndef TPATHGENERATOR_H
#define TPATHGENERATOR_H

#include <QObject>
#include <QPointF>
#include "tmirrorcoordinate.h"

class TParamLine
{

public:
    TParamLine(QPointF P1,QPointF P2);
    void ini(QPointF P1,QPointF P2);

    float getCollisionParam(TParamLine *line);
    QPointF getPointbyParam(float p);
    float getLength();

    QPointF getOffset();
    QPointF getDiffVec();
private:
    QPointF Offset;
    QPointF d;
};


class TPathGenerator : public QObject
{
    Q_OBJECT
public:
    explicit TPathGenerator(QObject *parent = 0);

    void AddRect(QPointF p1,QPointF p2,QPointF p3,QPointF p4, QPoint Divisions);
    void AddLine(QPointF p1,QPointF p2, uint Divisions);
    void AddEllipseOutline(QPointF center,float MinorAxis, float MajorAxis, float angle,uint Divisions);
    void clear();
    TMirrorCoordinate getPoint(int index);
private:
    bool testColl(QPointF p1,QPointF p2,QPointF p3,QPointF p4);
    QList<TMirrorCoordinate*> pointlist;

};

#endif // TPATHGENERATOR_H
