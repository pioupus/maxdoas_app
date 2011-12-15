#include "wavelengthbuffer.h"
#include <math.h>

TWavelengthbuffer* TWavelengthbuffer::m_Instance = 0;

TWavelengthbuffer::TWavelengthbuffer(){
    initialised = false;
    memset(&SpectrCoefficients,0,sizeof(SpectrCoefficients));
};

void TWavelengthbuffer::setCoefficients(TSPectrWLCoefficients* coeff){
    if ((coeff->Offset != SpectrCoefficients.Offset)||(coeff->Coeff1 != SpectrCoefficients.Coeff1)||(coeff->Coeff2 != SpectrCoefficients.Coeff2)||
        (coeff->Coeff3 != SpectrCoefficients.Coeff3)){

        for (uint i = 0;i<MAXWAVELEGNTH_BUFFER_ELEMTENTS;i++){
            buf[i] = coeff->Offset+
                                       (double)i*coeff->Coeff1+
                                       pow((double)i,2.0)*coeff->Coeff2+
                                       pow((double)i,3.0)*coeff->Coeff3;
        }
    }
    memcpy(&SpectrCoefficients,coeff,sizeof(TSPectrWLCoefficients));
    SpectrCoefficients.overWrittenFromFile = true;
    SpectrCoefficients.uninitialized = false;
    initialised = true;
}

TSPectrWLCoefficients TWavelengthbuffer::hwdGetWLCoefficients()
{
    return SpectrCoefficients;
}

void TWavelengthbuffer::setUnintialised(){
    initialised = false;
}

bool TWavelengthbuffer::isInitialised(){
    return initialised;
}
