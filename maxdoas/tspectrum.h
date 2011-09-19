#ifndef TSPECTRUM_H
#define TSPECTRUM_H
#include <QPoint>
#include <QObject>
#include <QDateTime>
#include <QTextStream>

#include "wavelengthbuffer.h"
#include "maxdoassettings.h"
#include "tmirrorcoordinate.h"


enum TSprectumType{stNone,stDarkOffset,stDarkNoise,stSunSpec,stMeasurement};

class TSpectrum: public QObject
{
        Q_OBJECT
public:


    TSpectrum(QObject* parent = 0);
    ~TSpectrum();
    void SaveSpectrum(QTextStream &file);

    bool LoadSpectrSTD(QString fn);

    bool LoadSpectrDefaultName(QString Directory, QString BaseName,int seqnumber);

    bool LoadSpectrEMT(QString fn);

    QDateTime GetDateTime();
    double spectrum[ MAXWAVELEGNTH_BUFFER_ELEMTENTS];
    TWavelengthbuffer *Wavelength;//points to global buffer
    int NumOfSpectrPixels;

    TSprectumType type;
    QPoint position;
    uint IntegTime;
    uint AvgCount;
    TSPectrWLCoefficients WLCoefficients;
    double MaxPossibleValue;
    int SequenceNumber;
    float Temperature;
    TMirrorCoordinate *MirrorCoordinate;
    QDateTime datetime;

public slots:
    void plot(int index);

    void SaveSpectrum(QString fn);
    void SaveSpectrumDefName(QString Directory, QString BaseName,int seqnumber);

    void add(QObject *spect);
    void add(double val);

    void sub(QObject *spect);
    void sub(double val);

    void mul(QObject *spect);
    void mul(double val);

    void div(QObject *spect);
    void div(double val);

    double rms();
    double mean();
    double stddev();

private:
    QString GetSequenceFileName(QString Directory, QString BaseName, uint Sequence);
    bool isSpectrumChanged();
    double rmsval;
    double meanval;
    double stddevval;
    double changeindicator;
};

#endif // TSPECTRUM_H
