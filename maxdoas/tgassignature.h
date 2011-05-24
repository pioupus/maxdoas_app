#ifndef TGASSIGNATURE_H
#define TGASSIGNATURE_H
#include <QString>
//
//TGasSignature
//    wlstart
//    wlend
//    SignatureWl
//    SignatureSpec
//
//    Measuredppmm
//    MeasuredCorrelation
//
//
class TGasSignatureSpectr
{
public:
    TGasSignatureSpectr();
    TGasSignatureSpectr(QString filename);

    void LoadFromFile(QString filename);

    double* GetSpectr();
    double* GetSpectrWl();
    uint GetSpectrCount();
    double GetWlStart();
    double GetWlEnd();

    void InterpolateGrid(double* targetgrid, double* interpolatedgrid, uint targetgridsize);
private:
    double wlstart;
    double wlend;
    double *SignatureWL;
    double *SignatureSpec;

};

class TGasSignature
{
public:
    TGasSignature();

    void SetGasSignatureSpectr(TGasSignatureSpectr *gassignaturespectr);


private:
    TGasSignatureSpectr *GasSignatureSpectr;

    double Measuredppm;
    double MeasuredCorrelation;

};

#endif // TGASSIGNATURE_H
