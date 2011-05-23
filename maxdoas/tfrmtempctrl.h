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

namespace Ui {
    class TfrmTempctrl;
}

class TfrmTempctrl : public QDialog
{
    Q_OBJECT

public:
    explicit TfrmTempctrl(QWidget *parent = 0);
    ~TfrmTempctrl();

private:
    Ui::TfrmTempctrl *ui;
    QwtPlotCurve *curvePeltier;
    QwtPlotCurve *curveHeatsink;
    QwtPlot *plot;
    QwtLegend *legend;
    double BufferX1[2000],BufferY1[2000];
    double BufferX2[2000];
    double factor1,factor2;


private slots:
    void showCurve(QwtPlotItem *item, bool on);

protected:
    virtual void timerEvent(QTimerEvent *e);
};

#endif // TFRMTEMPCTRL_H
