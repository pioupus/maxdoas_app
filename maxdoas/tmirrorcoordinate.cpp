#include "tmirrorcoordinate.h"

TMirrorCoordinate::TMirrorCoordinate(QObject *parent)
{
}

TMirrorCoordinate::TMirrorCoordinate(QPointF c){
    AngleCoordinate = c;
}

TMirrorCoordinate::TMirrorCoordinate(TMirrorCoordinate * other){
    AngleCoordinate = other->getAngleCoordinate();
}

QPoint TMirrorCoordinate::getMotorCoordinate(){
    QPoint result;
    result.setX(AngleCoordinate.x()*1000);
    result.setY(AngleCoordinate.y()*1000);
    return result;
}

QPointF TMirrorCoordinate::getAngleCoordinate(){
    return AngleCoordinate;
}
