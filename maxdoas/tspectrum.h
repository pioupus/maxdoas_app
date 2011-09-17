#ifndef TSPECTRUM_H
#define TSPECTRUM_H
#include <QPoint>
#include <QObject>
#include <QDateTime>
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
    void SaveSpectrSTD(QString fn);
    void SaveSpectrMetaAscii(QString fn);

    void SaveSpectrSTD_DefaultName(QString Directory, QString BaseName, uint SquenceNr);
    void SaveSpectrMetaAscii_DefaultName(QString Directory, QString BaseName, uint SquenceNr);


    bool LoadSpectrSTD(QString fn);
    bool LoadSpectrMetaAscii(QString fn);

    bool LoadSpectrSTD_Sequ(QString Directory, QString BaseName, uint SequenceNr);

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

private:
    QString GetSequenceFileName(QString Directory, QString BaseName, uint Sequence);
};

#endif // TSPECTRUM_H
