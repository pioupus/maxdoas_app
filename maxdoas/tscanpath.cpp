#include "tscanpath.h"
#include <math.h>
#include <QScriptEngine>
#include <QScriptContext>
#include <QScriptValue>
#include "maxdoassettings.h"

TPatternType::TPatternType(){
    this->Patternstyle = psNone;
    this->edge1 = QPointF(0,0);
    this->edge2 = QPointF(0,0);
    this->edge3 = QPointF(0,0);
    this->edge4 = QPointF(0,0);

    this->divx = 0;
    this->divy = 0;

    this->center = QPointF(0,0);
    this->MinorAxis = 0;
    this->MajorAxis = 0;
    this->angle = 0;
}

TPatternType::TPatternType(TPatternType *other){
    this->Patternstyle = other->Patternstyle;
    this->edge1 = other->edge1;
    this->edge2 = other->edge2;
    this->edge3 = other->edge3;
    this->edge4 = other->edge4;

    this->divx = other->divx;
    this->divy = other->divy;

    this->center = other->center;
    this->MinorAxis = other->MinorAxis;
    this->MajorAxis = other->MajorAxis;
    this->angle = other->angle;
}

void TPatternType::setLine(QPointF p1,QPointF p2, uint Divisions){
    edge1 = p1;
    edge2 = p2;
    divx = Divisions;
    divy = 1;
    Patternstyle = psLine;
}

void TPatternType::setRectangle(QPointF p1,QPointF p2, QPointF p3, QPointF p4, QPoint Divisions){
    edge1 = p1;
    edge2 = p2;
    edge3 = p3;
    edge4 = p4;
    divx = Divisions.x();
    divy = Divisions.y();
    Patternstyle = psRect;
}

void TPatternType::setEllipse(QPointF center,float MinorAxis, float MajorAxis, float angle, uint Divisions){
    center = center;
    MajorAxis = MajorAxis;
    MinorAxis = MinorAxis;
    angle = angle;
    divx = Divisions;
    divy = 1;
    Patternstyle = psEllipse;
}

void TPatternType::save(QTextStream &meta){
    switch(Patternstyle){
        case psRect:
           meta << "Rectangle\n";
           meta << "edge1:\t"       << edge1.x()    << "\t" << edge1.y()    << "\n";
           meta << "edge2:\t"       << edge2.x()    << "\t" << edge2.y()    << "\n";
           meta << "edge3:\t"       << edge3.x()    << "\t" << edge3.y()    << "\n";
           meta << "edge4:\t"       << edge4.x()    << "\t" << edge4.y()    << "\n";
           meta << "div:\t"         << divx         << "\t" << divy         << "\n\n";
           break;
        case psEllipse:
           meta << "Ellipse\n";
           meta << "center:\t"      << center.x()    << "\t" << center.y()  << "\n";
           meta << "MinorAxis:\t"   << MinorAxis                            << "\n";
           meta << "MajorAxis:\t"   << MajorAxis                            << "\n";
           meta << "angle:\t"       << angle                                << "\n";
           meta << "div:\t"         << divx                                 << "\n\n";
            break;
        case psLine:
            meta << "Line\n";
            meta << "edge1:\t"       << edge1.x()    << "\t" << edge1.y()    << "\n";
            meta << "edge2:\t"       << edge2.x()    << "\t" << edge2.y()    << "\n";
            meta << "div:\t"         << divx                                 << "\n\n";
            break;
        case psNone:
            break;
    }
}

bool TPatternType::load(QFile &meta){
    int fields = 0;
    float f;
    QString line_str = meta.readLine();
    QTextStream line(&line_str);
    QString type;
    line >> type;
    Patternstyle = psNone;
    if (type.startsWith("Rectangle")){
        Patternstyle = psRect;
        while(!meta.atEnd() && (fields<5)){
            QString line_str = meta.readLine();
            QTextStream line(&line_str);
            QString t;
            line >> t;
            if (t.startsWith("edge1")){
                line >> f;  edge1.setX(f);
                line >> f;  edge1.setY(f);
                fields++;
            }
            if (t.startsWith("edge2")){
                line >> f;  edge2.setX(f);
                line >> f;  edge2.setY(f);
                fields++;
            }
            if (t.startsWith("edge3")){
                line >> f;  edge3.setX(f);
                line >> f;  edge3.setY(f);
                fields++;
            }
            if (t.startsWith("edge4")){
                line >> f;  edge4.setX(f);
                line >> f;  edge4.setY(f);
                fields++;
            }
            if (t.startsWith("div")){
                line >> divx;
                line >> divy;
                fields++;
            }
        }
    }
    if (type.startsWith("Ellipse")){
        Patternstyle = psEllipse;
        while(!meta.atEnd() && (fields<5)){
            QString line_str = meta.readLine();
            QTextStream line(&line_str);
            QString t;
            line >> t;
            if (t.startsWith("center")){
                line >> f;  edge1.setX(f);
                line >> f;  edge1.setY(f);
                fields++;
            }
            if (t.startsWith("MinorAxis")){
                line >> MinorAxis;
                fields++;
            }
            if (t.startsWith("MajorAxis")){
                line >> MajorAxis;
                fields++;
            }
            if (t.startsWith("angle")){
                line >> angle;
                fields++;
            }
            if (t.startsWith("div")){
                line >> divx;
                divy  = 1;
                fields++;
            }
        }
    }
    if (type.startsWith("Line")){
        Patternstyle = psLine;
        while(!meta.atEnd() && (fields<3)){
            QString line_str = meta.readLine();
            QTextStream line(&line_str);
            QString t;
            line >> t;
            if (t.startsWith("edge1")){
                line >> f;  edge1.setX(f);
                line >> f;  edge1.setY(f);
                fields++;
            }
            if (t.startsWith("edge2")){
                line >> f;  edge2.setX(f);
                line >> f;  edge2.setY(f);
                fields++;
            }
            if (t.startsWith("div")){
                line >> divx;
                divy = 1;
                fields++;
            }
        }
    }
    return Patternstyle != psNone;
}

TParamLine::TParamLine(QPointF P1,QPointF P2){
    ini(P1,P2);
}

TParamLine::TParamLine(){

}

TParamLine::TParamLine(TParamLine* other){
    Offset = other->Offset;
    d = other->d;
}

void TParamLine::iniDiff(QPointF Offset,QPointF Diff){
    this->Offset = Offset;
    d = Diff;
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

QPointF TParamLine::getRectCollisionPoint(QRectF rect, bool reverse){
    QPointF result;
    TParamLine edge;
    float l;
    if (reverse)
        d= -d;
    if (d.x() > 0){
        float l1,l2;
        edge.ini(rect.bottomRight(),rect.topRight());
        l1 = getCollisionParam(&edge);
        if (d.y() < 0){
            edge.ini(rect.topRight(),rect.topLeft());
            l2 = getCollisionParam(&edge);
        }else{
            edge.ini(rect.bottomRight(),rect.bottomLeft());
            l2 = getCollisionParam(&edge);
        }
        if (l1 < l2)
            l = l1;
        else
            l = l2;
    }else{
        float l1,l2;
        edge.ini(rect.bottomLeft(),rect.topLeft());
        l1 = getCollisionParam(&edge);
        if (d.y() < 0){
            edge.ini(rect.topRight(),rect.topLeft());
            l2 = getCollisionParam(&edge);
        }else{
            edge.ini(rect.bottomRight(),rect.bottomLeft());
            l2 = getCollisionParam(&edge);
        }
        if (l1 < l2)
            l = l1;
        else
            l = l2;
    }
    result = getPointbyParam(l);
    if (reverse)
        d= -d;
    return result;
}

float TParamLine::containsPoint(QPointF P, bool &contains){
    float dPx = P.x()-Offset.x();
    float dPy = P.y()-Offset.y();
    float a,b;
    a = dPx*d.y();
    b = dPy*d.x();
    contains = a == b;

    a = d.x(); if (a < 0) a = -a;
    b = d.y(); if (b < 0) b = -b;
    if (a > b)
        a = dPx / d.x();
    else
        a = dPy / d.y();
    return a;
}


QPointF TParamLine::getPointbyParam(float p){
    return Offset+d*p;
}

float TParamLine::getLength(){
    return sqrt(pow(d.x(),2.0)+pow(d.y(),2.0));
}

float TParamLine::getLength(float Param){
    float result = 0;
    QPointF P = getPointbyParam(Param);
    result = sqrt(pow(Offset.x()-P.x(),2.0)+pow(Offset.y()-P.y(),2.0));
    return result;
}

QPointF TParamLine::getOffset(){
    return Offset;
}

QPointF TParamLine::getDiffVec(){
    return d;
}

TParamLine* TParamLine::getOrthoLine(QPointF P){
    TParamLine* result = new TParamLine();
    QPointF Direction = QPointF(-d.y(),d.x());
    result->iniDiff(P,Direction);
    return result;
}

float TParamLine::GetDistanceToPoint(QPointF Point){
    float result;
    TParamLine* Distance = getOrthoLine(Point);
    result = Distance->getCollisionParam(this);
    result = Distance->getLength(result);
    delete Distance;
    return result;
}

TScanPath::TScanPath(QObject *parent) :
    QObject(parent)
{

}

TScanPath::~TScanPath(){
    TMirrorCoordinate *mc;
    for(int i = 0;i<pointlist.count();i++){
        mc = pointlist.at(i);
        delete mc;
    }
    pointlist.clear();
    TPatternType *pt;
    for(int i = 0;i<Patternsources.count();i++){
        pt = Patternsources.at(i);
        delete pt;
    }
    Patternsources.clear();
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
        TPatternType *pt = new TPatternType();
        pt->setRectangle(p[0],p[1],p[2],p[3],Divisions);
        Patternsources.append(pt);
        TParamLine* line1 = new TParamLine(p[0],p[1]);
        TParamLine* line2 = new TParamLine(p[2],p[3]);
        TParamLine* line3 = new TParamLine(p[0],p[2]);
        for(int n=0;n<Divisions.y();n++){
            QPointF lp1 = line1->getPointbyParam((float)n/((float)Divisions.y()-1));
            QPointF lp2 = line2->getPointbyParam((float)n/((float)Divisions.y()-1));
            line3->ini(lp1,lp2);
            for(int m=0;m<Divisions.x();m++){
                TMirrorCoordinate *coord = new TMirrorCoordinate(line3->getPointbyParam((float)m/((float)Divisions.x()-1)));
                coord->pixelIndexX = m;
                coord->pixelIndexY = n;
                coord->pixelIndex = pointlist.count();
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
    TMaxdoasSettings *ms;
    ms = TMaxdoasSettings::instance();
    if ((ms->getAttachedScanningDevice()==sdtWindField)||(ms->getAttachedScanningDevice()==sdtUnknown)){
        if (c==10){

            QPointF p1(argument(0).toNumber(),argument(1).toNumber());
            QPointF p2(argument(2).toNumber(),argument(3).toNumber());
            QPointF p3(argument(4).toNumber(),argument(5).toNumber());
            QPointF p4(argument(6).toNumber(),argument(7).toNumber());
            QPoint Divisions(argument(8).toNumber(),argument(9).toNumber());
            res = AddRect(p1,p2,p3,p4,Divisions);
        }
    }else if (ms->getAttachedScanningDevice()==sdtMAXDOAS){
        logger()->error("try to create a Rectpattern even though its in MAXDOAS mode");
    }
    return res;
}

int TScanPath::AddLine(QPointF p1,QPointF p2, uint Divisions){
    TPatternType *pt = new TPatternType();
    pt->setLine( p1, p2,  Divisions);
    Patternsources.append(pt);

    TParamLine* line = new TParamLine(p1,p2);
    for(uint i=0;i<Divisions;i++){
        TMirrorCoordinate *coord = new TMirrorCoordinate(line->getPointbyParam((float)i/((float)Divisions-1)));
        coord->pixelIndexX = i;
        coord->pixelIndexY = 0;
        coord->pixelIndex = pointlist.count();
        pointlist.append(coord);
    }
    delete line;
    return pointlist.count();
}

int TScanPath::AddLine(){
    int res=0;
    int c = argumentCount();
    TMaxdoasSettings *ms = TMaxdoasSettings::instance();
    if ((ms->getAttachedScanningDevice()==sdtWindField)||(ms->getAttachedScanningDevice()==sdtUnknown)){
        if (c==5){

            QPointF p1(argument(0).toNumber(),argument(1).toNumber());
            QPointF p2(argument(2).toNumber(),argument(3).toNumber());
            int Divisions = argument(4).toNumber();
            res = AddLine(p1,p2,Divisions);
        }
    }else if (ms->getAttachedScanningDevice()==sdtMAXDOAS){
        if (c==3){

            QPointF p1(argument(0).toNumber(),0);
            QPointF p2(argument(1).toNumber(),0);
            int Divisions = argument(2).toNumber();
            res = AddLine(p1,p2,Divisions);
        }
    }
    return res;
}

int TScanPath::AddEllipseOutline(QPointF center,float MinorAxis, float MajorAxis, float angle, uint Divisions){
    TPatternType *pt = new TPatternType();
    pt->setEllipse(center,MinorAxis, MajorAxis, angle, Divisions);
    Patternsources.append(pt);

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
        c->pixelIndexX = i;
        c->pixelIndexY = 0;
        pointlist.append(c);
    }
    return pointlist.count();
}

int TScanPath::AddEllipseOutline(){
    int res=0;
    int c = argumentCount();
    TMaxdoasSettings *ms = TMaxdoasSettings::instance();
    if ((ms->getAttachedScanningDevice()==sdtWindField)||(ms->getAttachedScanningDevice()==sdtUnknown)){

        if (c==6){

            QPointF center(argument(0).toNumber(),argument(1).toNumber());
            float  MinorAxis = argument(2).toNumber();
            float MajorAxis = argument(3).toNumber();
            float angle = argument(4).toNumber();
            int Divisions = argument(5).toNumber();
            res = AddEllipseOutline(center,MinorAxis,MajorAxis,angle,Divisions);
        }
    }else if (ms->getAttachedScanningDevice()==sdtMAXDOAS){
        logger()->error("try to create a Ellipsepattern even though its in MAXDOAS mode ");
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

QList<TPatternType*> TScanPath::getPatternSources(){
    return Patternsources;
}
