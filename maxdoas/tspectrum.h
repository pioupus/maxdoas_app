#ifndef TSPECTRUM_H
#define TSPECTRUM_H
#include <QPoint>


struct TSPectrWLCoefficients{
    double Offset;
    double Coeff0;
    double Coeff1;
    double Coeff2;
};

enum TSprectumType{stNone,stDarkOffset,stDarkNoise,stSunSpec,stMeasurement};
//TSpectrum
//    ARRAY of double
//    position
//    type(darkoffset,darknoise,sunspec,measurement)
//    const integtime
//    const avg

class TSpectrum
{
public:
    TSpectrum();
    double* spectrum;
    double* Wavelength;//points to global buffer
private:
    TSprectumType type;
    uint SpectrCount;
    QPoint position;
    uint IntegTime;
    uint AvgCount;
    TSPectrWLCoefficients WLCoefficients;
};

#endif // TSPECTRUM_H
