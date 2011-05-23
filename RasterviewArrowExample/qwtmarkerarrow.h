#ifndef QWTMARKERARROW_H
#define QWTMARKERARROW_H

#include <qwt_plot_item.h>
#include <qwt_plot_marker.h>

class QwtMarkerArrow: public QwtPlotMarker
{
public:
    QwtMarkerArrow( );

    void draw( QPainter *painter,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect ) const;

    void setValue( double, double,double,double );
private:
    double directionx;
    double directiony;

};


#endif // QWTMARKERARROW_H
