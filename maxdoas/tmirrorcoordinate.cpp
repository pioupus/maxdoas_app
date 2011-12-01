#include "tmirrorcoordinate.h"
#include <QDebug>
#include <math.h>
//<QDebug>
#define PRINTCONSTR_DESTR 0
int constructioncounter=0;
//#define STATIONARY_OFFSET 142.875
#define STATIONARY_OFFSET 71.4375
#define STATIONARY_STEPS 32

#define MIRROR_OFFSET 36
#define MIRROR_STEPS 32

TMirrorCoordinate::TMirrorCoordinate(QObject *parent)
{
    pixelIndexX = -1;
    pixelIndexY = -1;
    #if PRINTCONSTR_DESTR
        constructioncounter++;
        qDebug() << "mc construction: " << constructioncounter;
    #endif
}

TMirrorCoordinate::TMirrorCoordinate(QPointF c){
    AngleCoordinate = c;
    pixelIndexX = -1;
    pixelIndexY = -1;
    #if PRINTCONSTR_DESTR
        constructioncounter++;
        qDebug() << "mc construction: " << constructioncounter;
    #endif
}

TMirrorCoordinate::TMirrorCoordinate(TMirrorCoordinate * other){
    AngleCoordinate = other->getAngleCoordinate();
    pixelIndexX = other->pixelIndexX;
    pixelIndexY = other->pixelIndexY;
    pixelIndex = other->pixelIndex;
    #if PRINTCONSTR_DESTR
        constructioncounter++;
        qDebug() << "mc construction: " << constructioncounter;
    #endif
}

TMirrorCoordinate::~TMirrorCoordinate(){
    #if PRINTCONSTR_DESTR
        constructioncounter--;
         qDebug() << "mc destruction: " << constructioncounter;
    #endif
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
    //X -> Stationary   -> Longitude
    //Y -> Mirror       -> Lattidude
    return AngleCoordinate;
}

QPointF TMirrorCoordinate::getZenithCoordinate(){
    //X -> Azimuth
    //Y -> Zenith
    QPointF result;
    double Zenith;
    double Azimuth;
    //Zenith = 90+(MirrorMotorAngle-90)*cos(StationaryMotorAngle)
    //Azimuth = (90-MirrorMotorAngle)*sin(StationaryMotorAngle) for MirrorMotorAngle a => 0
    //Azimuth = 180 - (90+MirrorMotorAngle)*sin(StationaryMotorAngle) for MirrorMotorAngle a < 0

    Zenith = 90.0+(AngleCoordinate.y()-90.0);
    Zenith *= cos(AngleCoordinate.x()*M_PI/180.0);

    if (AngleCoordinate.y() >= 0){
        Azimuth = 90-AngleCoordinate.y();
    }else{
        Azimuth = 180 - (90+AngleCoordinate.y());
    }
    Azimuth *= sin(AngleCoordinate.x()*M_PI/180.0);

    result.setX(Azimuth);
    result.setY(Zenith);

    return result;
}
void TMirrorCoordinate::setAngleCoordinate(QPointF ac){
    AngleCoordinate = ac;
}

void TMirrorCoordinate::setMotorCoordinate(int X, int Y){
    AngleCoordinate.setX((1.8*X/STATIONARY_STEPS)-STATIONARY_OFFSET);

    //result.setY((MIRROR_STEPS*MIRROR_OFFSET/1.8) - ((MIRROR_STEPS/2)*AngleCoordinate.y()/1.8));
    Y -= (MIRROR_STEPS*MIRROR_OFFSET/1.8);
    float y = (2*1.8*Y/MIRROR_STEPS);
    AngleCoordinate.setY(-y);
   // AngleCoordinate.setY(Y);
}

void TMirrorCoordinate::setMotorCoordinate(QPoint mc){
    setMotorCoordinate(mc.x(),mc.y());
}
