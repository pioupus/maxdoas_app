#ifndef QWTSYMBOLARROW_H
#define QWTSYMBOLARROW_H

#include <QPointF>
#include "qwt_symbol.h"

class QwtSymbolArrow : public QwtSymbol
{
public:
    QwtSymbolArrow();
    QwtSymbolArrow( Style, const QBrush &, const QPen &, const QSize & );
    void drawSymbols( QPainter *painter, const QPointF *points, const QPointF *directions,  int numPoints ) const;

    void drawArrow( QPainter *painter, const QPointF *points, /*const QPointF *directions, */ int numPoints ) const;
    //setsymboldirection(QPointF direction);
private:
 //   QPointF direction;
};

#endif // QWTSYMBOLARROW_H
