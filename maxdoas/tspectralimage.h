#ifndef TSPECTRALIMAGE_H
#define TSPECTRALIMAGE_H

#include <QObject>
#include <QHash>
#include <QPair>
#include <QDateTime>
#include "tspectrum.h"
#include "tmirrorcoordinate.h"

class TSpectralImage : public QObject
{
Q_OBJECT
public:
    explicit TSpectralImage(QObject *parent = 0);
    ~TSpectralImage();
private:
    QHash<QPair<int,int>, TSpectrum*> spectrumtable;
    QDateTime FirstDate;
signals:

public slots:
     void add(TMirrorCoordinate* coord, TSpectrum* spektrum);
     void save(QString Directory,QString BaseName);

};

#endif // TSPECTRALIMAGE_H
