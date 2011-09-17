#include "tmirrorcoordinate.h"

TMirrorCoordinate::TMirrorCoordinate(QObject *parent)
{
}

//TMirrorCoordinate::TMirrorCoordinate()
//{
//}

TMirrorCoordinate::TMirrorCoordinate(QPointF c){
    AngleCoordinate = c;
}

QPoint TMirrorCoordinate::getMotorCoordinate(){
    return QPoint(0,0);
}

QPointF TMirrorCoordinate::getAngleCoordinate(){
    return AngleCoordinate;
}
