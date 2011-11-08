#include "tretrieval.h"
#include <tmirrorcoordinate.h>

TRetrieval::TRetrieval()
{
    mirrorCoordinate = NULL;
}

TRetrieval::~TRetrieval(){
    delete mirrorCoordinate;
}

void TRetrieval::setMirrorCoordinate(TMirrorCoordinate *mc){
    if (mirrorCoordinate != NULL){
        delete mirrorCoordinate;
    }
    mirrorCoordinate = new TMirrorCoordinate(mc);
}
