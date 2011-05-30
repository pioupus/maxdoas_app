#include "tspectrum.h"

TSpectrum::TSpectrum()
{
    Wavelength = &globalWaveLengthBuffer[0];
    type = stNone;
    position.setX(0);
    position.setY(0);
    IntegTime = 0;
    AvgCount = 0;
    //TSPectrWLCoefficients WLCoefficients;
}
