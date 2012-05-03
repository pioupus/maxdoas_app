#include "qwtmarkerarrow.h"
#include <qwt_plot_item.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>

#include <qpainter.h>
#include <qwt_scale_map.h>
#include <qwt_plot_curve.h>
#include "qwtmarkerarrow.h"
#include "qwtsymbolarrow.h"
QwtMarkerArrow::QwtMarkerArrow(  ):QwtPlotMarker(  )
{
    setRenderHint(QwtPlotItem::RenderAntialiased, QwtPlotItem::RenderAntialiased);
}

void QwtMarkerArrow::setValue( double x, double y ,double dirx, double diry )
{
    QwtPlotMarker::setValue(x,y);
    if (dirx != directionx || diry != directiony)
    {
        directionx = dirx;
        directiony = diry;

        itemChanged();
    }
}

void QwtMarkerArrow::draw( QPainter *painter,
                           const QwtScaleMap &xMap, const QwtScaleMap &yMap,
                           const QRectF &canvasRect ) const
{

    if (this->symbol()->style()==QwtSymbol::UserStyle){

        if ( symbol() &&
             ( symbol()->style() != QwtSymbol::NoSymbol ) )
        {
            const QwtSymbolArrow *sy = dynamic_cast< const QwtSymbolArrow*> (symbol());
            if(sy){
                const QPointF *dir = new QPointF(xMap.transform( directionx+xValue() ), yMap.transform( directiony+yValue()));
                const QPointF *pos = new QPointF(xMap.transform( xValue() ), yMap.transform( yValue()) );
                sy->drawSymbols( painter, pos,dir,  1 );
            }
        }

    }else{
        QwtPlotMarker::draw(painter,xMap,yMap,canvasRect);
    }

//
//    // draw lines
//
//    drawLines( painter, canvasRect, pos );
//
//    // draw symbol
//
//    drawLabel( painter, canvasRect, pos );
}
