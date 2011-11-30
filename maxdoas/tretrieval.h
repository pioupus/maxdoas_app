#ifndef TRETRIEVAL_H
#define TRETRIEVAL_H

#include <QPointF>

#include <tmirrorcoordinate.h>

//TRetrieval
//    list of TSignatures
//    correlation
//    TSpectrum
//

class TRetrieval
{
public:
    TRetrieval();
    TRetrieval(TRetrieval * other);
    ~TRetrieval();
    void setMirrorCoordinate(TMirrorCoordinate *mc);

    double val;
    TMirrorCoordinate *mirrorCoordinate;
    QPointF getWindVector();
    void setWindVector(QPointF v);
private:

    QString symbName;
    QPointF WindVector;
};

#endif // TRETRIEVAL_H
