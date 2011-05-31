#ifndef TSPECTRUM_H
#define TSPECTRUM_H
#include <QPoint>
#include "wavelengthbuffer.h"

struct TSPectrWLCoefficients{
    double Offset;
    double Coeff0;
    double Coeff1;
    double Coeff2;
};

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
    private:
};

#endif // TSPECTRUM_H
