#include "qwtsymbolarrow.h"
#include "qwt_symbol.h"
#include "qwt_painter.h"
#include <qapplication.h>
#include <qpainter.h>
#include <qmath.h>

QwtSymbolArrow::QwtSymbolArrow()
{

}

QwtSymbolArrow::QwtSymbolArrow( QwtSymbol::Style style, const QBrush &brush,
                                const QPen &pen, const QSize &size ): QwtSymbol( style, brush, pen, size )
{

    //d_data = new PrivateData( style, brush, pen, size );
}

static inline float sign(float const x){
    if (x < 0)
        return -1;
    else
        return 1;
}

static inline void qwtDrawArrowSymbols( QPainter *painter,
    const QPointF *points, const QPointF *directions, int numPoints, const QwtSymbolArrow &symbol )
{
    painter->setBrush( symbol.brush() );
    painter->setPen( symbol.pen() );

  //  const QSize size = symbol.size();

    if ( QwtPainter::roundingAlignment( painter ) )
    {
        for ( int i = 0; i < numPoints; i++ )
        {
            const double x = points[i].x();
            const double y = points[i].y();
            const QPointF diff = points[i]-directions[i];
            const double len = sqrt(pow(diff.y(),2)+pow(diff.x(),2));
            const double as = qAsin(diff.y()/len);
            const float signx = sign(diff.x());
            static QPointF Arrow[5];
            Arrow[0] = QPointF(points[i]);
            Arrow[1] = QPointF(directions[i]);
            Arrow[2] = QPointF(points[i].x()-signx*qCos(as+M_PI/16)*(len-len/4),points[i].y()-qSin(as+M_PI/16)*(len-len/4));
            Arrow[3] = QPointF(directions[i]);
            Arrow[4] = QPointF(points[i].x()-signx*qCos(as-M_PI/16)*(len-len/4),points[i].y()-qSin(as-M_PI/16)*(len-len/4));
            QwtPainter::drawPolyline(painter, Arrow,5);
        }
    }
    else
    {
        for ( int i = 0; i < numPoints; i++ )
        {
            const double x = points[i].x();
            const double y = points[i].y();
            const QPointF diff = points[i]-directions[i];
            const double len = sqrt(pow(diff.y(),2)+pow(diff.x(),2));
            const double as = qAsin(diff.y()/len);
            const float signx = sign(diff.x());
            static QPointF Arrow[5];
            Arrow[0] = QPointF(points[i]);
            Arrow[1] = QPointF(directions[i]);
            Arrow[2] = QPointF(points[i].x()-signx*qCos(as+M_PI/16)*(len-len/4),points[i].y()-qSin(as+M_PI/16)*(len-len/4));
            Arrow[3] = QPointF(directions[i]);
            Arrow[4] = QPointF(points[i].x()-signx*qCos(as-M_PI/16)*(len-len/4),points[i].y()-qSin(as-M_PI/16)*(len-len/4));
            QwtPainter::drawPolyline(painter, Arrow,5);
        }
    }
}


/*!
  Draw an array of symbols

  Painting several symbols is more effective than drawing symbols
  one by one, as a couple of layout calculations and setting of pen/brush
  can be done once for the complete array.

  \param painter Painter
  \param points Array of points
  \param numPoints Number of points
*/
void QwtSymbolArrow::drawSymbols( QPainter *painter,
    const QPointF *points, const QPointF *directions,  int numPoints ) const
{
    if ( numPoints <= 0 )
        return;

    painter->save();
    if (style() == UserStyle){
        qwtDrawArrowSymbols(painter,points,directions,numPoints,*this);
        painter->restore();
    }else{
        QwtSymbol::drawSymbols( painter,points,numPoints );
    }

}

//void QwtSymbolArrow::drawArrow( QPainter *painter,
//    const QPointF *points,/* const QPointF *directions,*/  int numPoints ) const
//{
//    if ( numPoints <= 0 )
//        return;

//    painter->save();
//    if (style() == UserStyle){
//        qwtDrawArrowSymbols(painter,points,numPoints,*this);
//        painter->restore();
//    }else{
//        QwtSymbol::drawSymbols( painter,points,numPoints );
//    }

//}

