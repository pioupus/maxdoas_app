#ifndef TSCANPATH_H
#define TSCANPATH_H

#include <QObject>
#include <QPointF>
#include <QRectF>
#include "tmirrorcoordinate.h"
#include <QScriptEngine>
#include <QScriptContext>
#include <QScriptValue>
#include <QScriptable>
#include <QTextStream>
#include <QFile>

enum TPatternStyle {psNone,psRect,psEllipse,psLine};

class TPatternType{
public:
    TPatternType(TPatternType *other);
    TPatternType();

    void save(QTextStream &meta);
    bool load(QFile &meta);

    void setLine(QPointF p1,QPointF p2, uint Divisions);
    void setRectangle(QPointF p1,QPointF p2, QPointF p3, QPointF p4, QPoint Divisions);
    void setEllipse(QPointF center,float MinorAxis, float MajorAxis, float angle, uint Divisions);


    TPatternStyle Patternstyle;
    QPointF edge1;
    QPointF edge2;
    QPointF edge3;
    QPointF edge4;

    int divx;
    int divy;

    QPointF center;
    float MinorAxis;
    float MajorAxis;
    float angle;

};

class TParamLine
{
public:
    TParamLine(QPointF P1,QPointF P2);
    TParamLine();
    TParamLine(TParamLine* other);
    void iniDiff(QPointF Offset,QPointF Diff);
    void ini(QPointF P1,QPointF P2);
    float getCollisionParam(TParamLine *line);
    QPointF getPointbyParam(float p);
    float getLength();
    float getLength(float Param);
    float containsPoint(QPointF P, bool &contains);
    TParamLine* getOrthoLine(QPointF P);
    QPointF getOffset();
    QPointF getDiffVec();
    float GetDistanceToPoint(QPointF Point);
    QPointF getRectCollisionPoint(QRectF rect, bool reverse);

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
    ~TScanPath();
    int AddRect(QPointF p1,QPointF p2, QPointF p3, QPointF p4, QPoint Divisions);
    int AddLine(QPointF p1,QPointF p2, uint Divisions);
    int AddEllipseOutline(QPointF center,float MinorAxis, float MajorAxis, float angle, uint Divisions);
    TMirrorCoordinate *getPoint_(int index);
    QList<TPatternType*> getPatternSources();
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
    QList<TPatternType*> Patternsources;
};

#endif // TSCANPATH_H
