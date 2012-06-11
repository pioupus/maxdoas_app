#include "tmirrorcoordinate.h"
#include <QDebug>
#include <math.h>
#include "maxdoassettings.h"

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
    (void)parent;
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
    TMaxdoasSettings* ms = TMaxdoasSettings::instance();
    //result.setX(AngleCoordinate.x()*1000);

    result.setX(((float)ms->getMicrostepping().x()*AngleCoordinate.x()/ms->getMotorStepAngle().x())+(float)ms->getZenithSteps().x());

    //result.setX(STATIONARY_STEPS*(AngleCoordinate.x()+STATIONARY_OFFSET)/1.8);
    //result.setY(AngleCoordinate.y()*1000);
    result.setY((float)ms->getZenithSteps().y() - (((float)ms->getMicrostepping().y()/2)*AngleCoordinate.y()/ms->getMotorStepAngle().y()));
    //result.setY((MIRROR_STEPS*MIRROR_OFFSET/1.8) - ((MIRROR_STEPS/2)*AngleCoordinate.y()/1.8));
    //result.setY(AngleCoordinate.y()*1);
    return result;
}

QPointF TMirrorCoordinate::getAngleCoordinate(){
    //X -> Stationary   -> Longitude
    //Y -> Mirror       -> Lattidude
    return AngleCoordinate;
}

QPointF TMirrorCoordinate::getElevationCoordinate(){
    //X -> Azimuth
    //Y -> Zenith
    QPointF result;
    double Azimuth;
    double Elevation;
    //Zenith = 90+(MirrorMotorAngle-90)*cos(StationaryMotorAngle)
    //Azimuth = (90-MirrorMotorAngle)*sin(StationaryMotorAngle) for MirrorMotorAngle a => 0
    //Azimuth = 180 - (90+MirrorMotorAngle)*sin(StationaryMotorAngle) for MirrorMotorAngle a < 0


    Elevation = 90 - fabs(AngleCoordinate.y());
    Elevation *= cos(AngleCoordinate.x()*M_PI/180.0);




    Azimuth = 90-fabs(AngleCoordinate.y());
    Azimuth *= sin(AngleCoordinate.x()*M_PI/180.0);

    if (AngleCoordinate.y() >= 0){
    }else{
        if (AngleCoordinate.x() >= 0)
            Azimuth = 180-Azimuth;
        else
            Azimuth = -180-Azimuth;
    }

    result.setX(Azimuth);
    result.setY(Elevation);

    return result;
}
void TMirrorCoordinate::setAngleCoordinate(QPointF ac){
    AngleCoordinate = ac;
}

void TMirrorCoordinate::setMotorCoordinate(int X, int Y){
//result.setX(((float)ms->getMicrostepping().x()*AngleCoordinate.x()/ms->getMotorStepAngle().x())+(float)ms->getZenithSteps().x());
    TMaxdoasSettings* ms = TMaxdoasSettings::instance();
    AngleCoordinate.setX(ms->getMotorStepAngle().x()*(X-ms->getZenithSteps().x())/(float)ms->getMicrostepping().x());

    //result.setY((float)ms->getZenithSteps().y() - (((float)ms->getMicrostepping().y()/2)*AngleCoordinate.y()/ms->getMotorStepAngle().y()));

    AngleCoordinate.setY((2*ms->getMotorStepAngle().y()/ms->getMicrostepping().y())*(ms->getZenithSteps().y()-Y));

    //Y -= (MIRROR_STEPS*MIRROR_OFFSET/1.8);
    //float y = (2*1.8*Y/MIRROR_STEPS);
    //AngleCoordinate.setY(-y);
   // AngleCoordinate.setY(Y);
}

void TMirrorCoordinate::setMotorCoordinate(QPoint mc){
    setMotorCoordinate(mc.x(),mc.y());
}
