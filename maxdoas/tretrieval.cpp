#include "tretrieval.h"
#include <tmirrorcoordinate.h>

TRetrieval::TRetrieval()
{
    mirrorCoordinate = NULL;
    WindVector = QPointF(0,0);
}

TRetrieval::TRetrieval(TRetrieval * other){
    mirrorCoordinate = NULL;
    setMirrorCoordinate(other->mirrorCoordinate);
    val = other->val;
    symbName = other->symbName;
    WindVector = other->WindVector;
    weight = other->weight;
    origval = other->origval;
}

TRetrieval::~TRetrieval(){
    delete mirrorCoordinate;
}

QPointF TRetrieval::getWindVector(){
    return WindVector;
}

void TRetrieval::setWindVector(QPointF v){
    WindVector = v;
}

void TRetrieval::setMirrorCoordinate(TMirrorCoordinate *mc){
    if (mirrorCoordinate != NULL){
        delete mirrorCoordinate;
        mirrorCoordinate = NULL;
    }
    if (mc != NULL)
        mirrorCoordinate = new TMirrorCoordinate(mc);
}
