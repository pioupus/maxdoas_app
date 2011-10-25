#ifndef TSCANPATH_H
#define TSCANPATH_H

#include <QObject>
#include <QPointF>
#include "tmirrorcoordinate.h"
#include <QScriptEngine>
#include <QScriptContext>
#include <QScriptValue>
#include <QScriptable>

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
    QPointF d;
    QPointF Offset;
};

class TScanPath : public QObject,
    protected QScriptable
{
     Q_OBJECT
public:
    TScanPath(QObject *parent);
    int AddRect(QPointF p1,QPointF p2, QPointF p3, QPointF p4, QPoint Divisions);
    int AddLine(QPointF p1,QPointF p2, uint Divisions);
    int AddEllipseOutline(QPointF center,float MinorAxis, float MajorAxis, float angle, uint Divisions);
    TMirrorCoordinate *getPoint_(int index);
public slots:
   // QScriptValue  AddRect(QScriptContext *context, QScriptEngine *engine);
    int AddLine();
    int AddRect();
    int AddEllipseOutline();
    void clear();
    int count();
    //QScriptValue count();

    QScriptValue getPoint(int index);



private:
    QList<TMirrorCoordinate*> pointlist;
    bool testColl(QPointF p1,QPointF p2,QPointF p3,QPointF p4);
};

#endif // TSCANPATH_H
