#ifndef QWTSYMBOLARROW_H
#define QWTSYMBOLARROW_H

#include <QPointF>
#include "qwt_symbol.h"

class QwtSymbolArrow : public QwtSymbol
{
public:
    QwtSymbolArrow();
    QwtSymbolArrow( Style, const QBrush &, const QPen &, const QSize & );
    ~QwtSymbolArrow();
    void drawSymbols( QPainter *painter, const QPointF *points, const QPointF *directions,  int numPoints ) const;

    void drawArrow( QPainter *painter, const QPointF *points, /*const QPointF *directions, */ int numPoints ) const;

private:

};

#endif // QWTSYMBOLARROW_H
