#include "tscanpath.h"
#include <math.h>
#include <QScriptEngine>
#include <QScriptContext>
#include <QScriptValue>

TParamLine::TParamLine(QPointF P1,QPointF P2){
    ini(P1,P2);
}

void TParamLine::ini(QPointF P1,QPointF P2){
    Offset = P1;
    d = P2-P1;
}

float TParamLine::getCollisionParam(TParamLine *line){
    QPointF Offset2 = line->getOffset();
    QPointF d2 = line->getDiffVec();
    float denum = d2.x()*d.y()-d.x()*d2.y();
    if(denum == 0.0)
        return NAN;
    float num = d2.y()*(Offset.x()-Offset2.x()) + d2.x()*(Offset2.y()-Offset.y());
    return num/denum;
}

QPointF TParamLine::getPointbyParam(float p){
    return Offset+d*p;
}

float TParamLine::getLength(){
    return sqrt(pow(d.x(),2.0)+pow(d.y(),2.0));
}

QPointF TParamLine::getOffset(){
    return Offset;
}

QPointF TParamLine::getDiffVec(){
    return d;
}


TScanPath::TScanPath(QObject *parent) :
    QObject(parent)
{

}

bool TScanPath::testColl(QPointF p1,QPointF p2,QPointF p3,QPointF p4){
    bool result;
    TParamLine* line1 = new TParamLine(p1,p3);
    TParamLine* line2 = new TParamLine(p2,p4);
    float coll1 = line2->getCollisionParam(line1);
    float coll2;
    if (!isnan(coll1)){
        coll2 = line1->getCollisionParam(line2);
    }else{
        coll2 = 2;
    }
    delete line1;
    delete line2;
    result = (coll1 > 0)&&(coll1 < 1)&&(coll2 > -1)&&(coll2 < 1);
    return result;
}


int TScanPath::AddRect(QPointF p1,QPointF p2, QPointF p3, QPointF p4, QPoint Divisions){
    QPointF p[4];
    bool ok = false;
    //lets test if all angles are < 180 if so, even the correct order is adjusted for getting the outline
    //(both diagonals have a common point within the rectangle)

    if (testColl(p1,p2,p3,p4)){
//        p1--------p4
//        |         |
//        |         |
//        p2--------p3
        p[0] = p1;  p[1] = p2;  p[2] = p4;   p[3] = p3;
        ok = true;
    }else if (testColl(p1,p3,p2,p4)){
//        p1--------p4
//        |         |
//        |         |
//        p3--------p2
        p[0] = p1;  p[1] = p3; p[2] = p4;  p[3] = p2;
        ok = true;
    }else if (testColl(p1,p2,p4,p3)){
//        p1--------p3
//        |         |
//        |         |
//        p2--------p4
        p[0] = p1;   p[1] = p2; p[2] = p3;   p[3] = p4;
        ok = true;
    }
    if (ok){
//        p[0]------p[2]
//        |--line3--|
//        |         |
//        p[1]------p[3]
        TParamLine* line1 = new TParamLine(p[0],p[1]);
        TParamLine* line2 = new TParamLine(p[2],p[3]);
        TParamLine* line3 = new TParamLine(p[0],p[2]);
        for(int n=0;n<Divisions.x();n++){
            QPointF lp1 = line1->getPointbyParam((float)n/((float)Divisions.x()-1));
            QPointF lp2 = line2->getPointbyParam((float)n/((float)Divisions.x()-1));
            line3->ini(lp1,lp2);
            for(int m=0;m<Divisions.y();m++){
                TMirrorCoordinate *coord = new TMirrorCoordinate(line3->getPointbyParam((float)m/((float)Divisions.y()-1)));
                pointlist.append(coord);
            }
        }
        delete line3;
        delete line2;
        delete line1;
    }
    return pointlist.count();
}

int TScanPath::AddRect(){
    int res=0;
    int c = argumentCount();
    if (c==10){

        QPointF p1(argument(0).toNumber(),argument(1).toNumber());
        QPointF p2(argument(2).toNumber(),argument(3).toNumber());
        QPointF p3(argument(4).toNumber(),argument(5).toNumber());
        QPointF p4(argument(6).toNumber(),argument(7).toNumber());
        QPoint Divisions(argument(8).toNumber(),argument(9).toNumber());
        res = AddRect(p1,p2,p3,p4,Divisions);
    }
    return res;
}

int TScanPath::AddLine(QPointF p1,QPointF p2, uint Divisions){
    TParamLine* line = new TParamLine(p1,p2);
    for(uint i=0;i<Divisions;i++){
        TMirrorCoordinate *coord = new TMirrorCoordinate(line->getPointbyParam((float)i/(float)Divisions));
        pointlist.append(coord);
    }
    delete line;
    return pointlist.count();
}

int TScanPath::AddLine(){
    int res=0;
    int c = argumentCount();
    if (c==5){

        QPointF p1(argument(0).toNumber(),argument(1).toNumber());
        QPointF p2(argument(2).toNumber(),argument(3).toNumber());
        int Divisions = argument(4).toNumber();
        res = AddLine(p1,p2,Divisions);
    }
    return res;
}

int TScanPath::AddEllipseOutline(QPointF center,float MinorAxis, float MajorAxis, float angle, uint Divisions){
    float xaxis = MajorAxis;
    float yaxis = MinorAxis;
    float transform[4];
    //  0   1
    //  2   3
    transform[0] = cos(angle);
    transform[1] = -sin(angle);
    transform[2] = -transform[1];//sin()
    transform[3] = transform[0]; //cos()

    for (uint i=0;i<Divisions;i++){
        QPointF p;
        float param=(float)i*2*M_PI/(float)Divisions;
        p.setX(cos(param)*xaxis);
        p.setY(sin(param)*yaxis);

        p.setX(p.x()*transform[0]+p.y()*transform[1]);
        p.setY(p.x()*transform[2]+p.y()*transform[3]);
        p = p + center;
        TMirrorCoordinate *c = new TMirrorCoordinate(p);
        pointlist.append(c);
    }
}

int TScanPath::AddEllipseOutline(){
    int res=0;
    int c = argumentCount();
    if (c==6){

        QPointF center(argument(0).toNumber(),argument(1).toNumber());
        float  MinorAxis = argument(2).toNumber();
        float MajorAxis = argument(3).toNumber();
        float angle = argument(4).toNumber();
        int Divisions = argument(5).toNumber();
        res = AddEllipseOutline(center,MinorAxis,MajorAxis,angle,Divisions);
    }
    return res;
}

void TScanPath::clear(){
    for(int i=0;i<pointlist.count();i++){
        TMirrorCoordinate *coord = pointlist.at(i);
        delete coord;
    }
    pointlist.clear();
}
//
//QScriptValue TScanPath::count(){
//    return count_();
//}

int TScanPath::count(){
    return pointlist.count();
}

TMirrorCoordinate *TScanPath::getPoint_(int index){
    return pointlist.at(index);
}

QScriptValue TScanPath::getPoint(int index)
{
    TMirrorCoordinate * mc = getPoint_(index);
    return engine()->newQObject(mc);
}

