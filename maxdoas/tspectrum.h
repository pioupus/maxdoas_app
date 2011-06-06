#ifndef TSPECTRUM_H
#define TSPECTRUM_H
#include <QPoint>
#include "wavelengthbuffer.h"
#include "maxdoassettings.h"


enum TSprectumType{stNone,stDarkOffset,stDarkNoise,stSunSpec,stMeasurement};

class TSpectrum
{
public:
    TSpectrum();
    double spectrum[ MAXWAVELEGNTH_BUFFER_ELEMTENTS];
    TWavelengthbuffer *Wavelength;//points to global buffer
    int NumOfSpectrPixels;

    TSprectumType type;
    QPoint position;
    uint IntegTime;
    uint AvgCount;
    TSPectrWLCoefficients WLCoefficients;
    double MaxPossibleValue;
    private:
};

#endif // TSPECTRUM_H
