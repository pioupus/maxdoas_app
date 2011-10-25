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
private:
    double val;
    QString symbName;
    TMirrorCoordinate *mirrorCoordinate;
    QPointF WindVector;
};

#endif // TRETRIEVAL_H
