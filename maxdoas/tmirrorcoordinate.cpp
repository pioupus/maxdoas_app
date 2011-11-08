#include "tmirrorcoordinate.h"
#include <QDebug>
//<QDebug>
int constructioncounter=0;
//#define STATIONARY_OFFSET 142.875
#define STATIONARY_OFFSET 71.4375
#define STATIONARY_STEPS 32

#define MIRROR_OFFSET 36
#define MIRROR_STEPS 32

TMirrorCoordinate::TMirrorCoordinate(QObject *parent)
{
    constructioncounter++;
    pixelIndexX = -1;
    pixelIndexY = -1;
    qDebug() << "mc construction: " << constructioncounter;
}

TMirrorCoordinate::TMirrorCoordinate(QPointF c){
    AngleCoordinate = c;
    pixelIndexX = -1;
    pixelIndexY = -1;
    constructioncounter++;
    qDebug() << "mc construction: " << constructioncounter;
}

TMirrorCoordinate::TMirrorCoordinate(TMirrorCoordinate * other){
    AngleCoordinate = other->getAngleCoordinate();
    pixelIndexX = other->pixelIndexX;
    pixelIndexY = other->pixelIndexY;
    pixelIndex = other->pixelIndex;
    constructioncounter++;
    qDebug() << "mc construction: " << constructioncounter;
}

TMirrorCoordinate::~TMirrorCoordinate(){
    constructioncounter--;
    qDebug() << "mc destruction: " << constructioncounter;
}

QPoint TMirrorCoordinate::getMotorCoordinate(){
    QPoint result;
    //result.setX(AngleCoordinate.x()*1000);
    result.setX(STATIONARY_STEPS*(AngleCoordinate.x()+STATIONARY_OFFSET)/1.8);
    //result.setY(AngleCoordinate.y()*1000);
    result.setY((MIRROR_STEPS*MIRROR_OFFSET/1.8) - ((MIRROR_STEPS/2)*AngleCoordinate.y()/1.8));
    //result.setY(AngleCoordinate.y()*1);
    return result;
}

QPointF TMirrorCoordinate::getAngleCoordinate(){
    return AngleCoordinate;
}

void TMirrorCoordinate::setAngleCoordinate(QPointF ac){
    AngleCoordinate = ac;
}

void TMirrorCoordinate::setMotorCoordinate(int X, int Y){
    AngleCoordinate.setX((1.8*X/STATIONARY_STEPS)-STATIONARY_OFFSET);

    //result.setY((MIRROR_STEPS*MIRROR_OFFSET/1.8) - ((MIRROR_STEPS/2)*AngleCoordinate.y()/1.8));
    Y -= (MIRROR_STEPS*MIRROR_OFFSET/1.8);
    float y = (2*1.8*Y/MIRROR_STEPS);
    AngleCoordinate.setY(y);
   // AngleCoordinate.setY(Y);
}

void TMirrorCoordinate::setMotorCoordinate(QPoint mc){
    setMotorCoordinate(mc.x(),mc.y());
}
