#include "tspectrum.h"


TSpectrum::TSpectrum()
{
    Wavelength = TWavelengthbuffer::instance();
    type = stNone;
    position.setX(0);
    position.setY(0);
    IntegTime = 0;
    AvgCount = 0;
    //TSPectrWLCoefficients WLCoefficients;
}
