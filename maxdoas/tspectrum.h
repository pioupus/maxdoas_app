#ifndef TSPECTRUM_H
#define TSPECTRUM_H
#include <QPoint>
#include <QObject>
#include <QDateTime>
#include <QTextStream>

#include "wavelengthbuffer.h"
#include "maxdoassettings.h"
#include "tmirrorcoordinate.h"


enum TSprectumType{stNone,stDarkOrRef,stMeasurement};

class TSpectrum: public QObject
{
        Q_OBJECT
public:


    TSpectrum(QObject* parent = 0);
    TSpectrum(TSpectrum * other);
    ~TSpectrum();
    void SaveSpectrum(QTextStream &file, QTextStream &meta, bool DarkSpectrum);

    bool LoadSpectrum(QTextStream &file, QTextStream &meta);

    bool LoadSpectrEMT(QString fn);

    void SaveSpectrumSTD(QString fn);
    void SaveSpectrumRef(QString calib,QString fn);

    QString getDefaultFileName(QString Directory, QString BaseName,int seqnumber);

    QDateTime GetDateTime();
    double spectrum[ MAXWAVELEGNTH_BUFFER_ELEMTENTS];
    double getHash();
    void setMirrorCoordinate(TMirrorCoordinate * mc);
    void setPixelIndex(int index);
    void setXPixelIndex(int index);
    void setYPixelIndex(int index);
    TMirrorCoordinate * getMirrorCoordinate();

    TWavelengthbuffer *Wavelength;//points to global buffer
    int NumOfSpectrPixels;
    QString getFileName();
    TSprectumType type;
    uint IntegTime;

    TAutoIntegConf IntegConf;

    uint AvgCount;
    TSPectrWLCoefficients WLCoefficients;
    double MaxPossibleValue;
    int SequenceNumber;
    QString BaseName;
    QString FileName;
    float Temperature;
    QPointF Tilt;
    QString SpectrometerSerialNumber;

    QDateTime datetime;
    void setZero();
    uint ScanPixelIndex;
public slots:
    void plot(int index);

    void SaveSpectrum(QString fn);
    void SaveSpectrumTmp(QString fn);
    void SaveSpectrumDark(QString fn);
    void SaveSpectrumDefName(QString Directory, QString BaseName,int seqnumber);
    void SaveSpectrumDefNameDark(QString Directory, QString BaseName,int seqnumber);

    bool LoadSpectrum(QString fn);
    bool LoadSpectrDefaultName(QString Directory, QString SearchBaseName,QString FileBaseName,int seqnumber,uint startindex, uint groupindex );

    QString getSpectSerialNo();
    void interpolatePixel(int Pixelindex);
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
    double max();

private:
    void SaveSpectrum_(QString fn,bool Dark,bool istmp);
    bool LoadSpectrum_(QString fn,bool istmp);
    bool isSpectrumChanged();
    double rmsval;
    double maxval;
    double meanval;
    double stddevval;
    double hash;
    TMirrorCoordinate *MirrorCoordinate;
};

QString DefaultFileNameFromSeqNumber(QString Directory, QString BaseName,int seqnumber,QDateTime datetime);
QString GetSequenceFileName(QString Directory, QString BaseName, uint Sequence, uint firstindex ,uint groupindex);
QString fnToMetafn(QString fn);

#endif // TSPECTRUM_H
