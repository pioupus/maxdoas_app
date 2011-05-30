#ifndef TFRMTEMPCTRL_H
#define TFRMTEMPCTRL_H

#include <QDialog>

#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <qwt_plot_canvas.h>
#include "thwdriver.h"

#define TEMPERAT_BUFFER_SIZE 100

namespace Ui {
    class TfrmTempctrl;
}

class TfrmTempctrl : public QDialog
{
    Q_OBJECT

public:
    explicit TfrmTempctrl(THWDriver *hwdriver,QWidget *parent = 0);
    ~TfrmTempctrl();
   // void setHWDriver(THWDriver *hwdriver);
private:
    Ui::TfrmTempctrl *ui;
    QwtPlotCurve *curvePeltier;
    QwtPlotCurve *curveHeatsink;
    QwtPlotCurve *curveSpectrometer;
    QwtPlot *plot;
    QwtLegend *legend;
    double BufferX1[TEMPERAT_BUFFER_SIZE];
    double BufferPeltier[TEMPERAT_BUFFER_SIZE];
    double BufferHeatSink[TEMPERAT_BUFFER_SIZE];
    double BufferSpectrometer[TEMPERAT_BUFFER_SIZE];
    int BufferFilled;
    double factor1,factor2;
    THWDriver *hwdriver;

private slots:
    void showCurve(QwtPlotItem *item, bool on);

protected:
    virtual void timerEvent(QTimerEvent *e);
};

#endif // TFRMTEMPCTRL_H
