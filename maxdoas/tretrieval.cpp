#include "tretrieval.h"
#include <tmirrorcoordinate.h>

TRetrieval::TRetrieval()
{
    mirrorCoordinate = NULL;
}

TRetrieval::TRetrieval(TRetrieval * other){
    mirrorCoordinate = NULL;
    setMirrorCoordinate(other->mirrorCoordinate);
    val = other->val;
    symbName = other->symbName;
    WindVector = other->WindVector;
}

TRetrieval::~TRetrieval(){
    delete mirrorCoordinate;
}

void TRetrieval::setMirrorCoordinate(TMirrorCoordinate *mc){
    if (mirrorCoordinate != NULL){
        delete mirrorCoordinate;
        mirrorCoordinate = NULL;
    }
    if (mc != NULL)
        mirrorCoordinate = new TMirrorCoordinate(mc);
}
