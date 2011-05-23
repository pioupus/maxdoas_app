#include "tfrmtempctrl.h"
#include "ui_tfrmtempctrl.h"



TfrmTempctrl::TfrmTempctrl(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TfrmTempctrl)
{
    int i;
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    plot = new QwtPlot(this);
    curvePeltier = new QwtPlotCurve("Peltier");
    curveHeatsink = new QwtPlotCurve("Heatsink");
    //curve2 = new QwtPlotCurve("Spectrometer");
    QwtLegend *legend = new QwtLegend;
    legend->setItemMode(QwtLegend::CheckableItem);
    plot->insertLegend(legend, QwtPlot::RightLegend);
  //  plot->aut
    ui->hbox->addWidget(plot);
    factor1 = 1.5705;
    factor2 = 1.5705;
    startTimer(20);
    curvePeltier->attach(plot);
    curveHeatsink->attach(plot);
    QPen Pen(Qt::green);

    curvePeltier->setPen(Pen);


    for (i=0;i<2000;i++){
        BufferY1[i] = sin((double)i/1);
    }

    connect(plot, SIGNAL(legendChecked(QwtPlotItem *, bool)),
        SLOT(showCurve(QwtPlotItem *, bool)));

//    connect(this, SIGNAL(finished()),
//        SLOT(on_close(void)));

    showCurve(curvePeltier, true);
    showCurve(curveHeatsink, true);
}

void TfrmTempctrl::showCurve(QwtPlotItem *item, bool on)
{
    item->setVisible(on);
    QWidget *w = plot->legend()->find(item);
    if ( w && w->inherits("QwtLegendItem") )
        ((QwtLegendItem *)w)->setChecked(on);

    plot->replot();
}

void TfrmTempctrl::timerEvent(QTimerEvent *){
    int i;
    for (i=0;i<2000;i++){
        BufferX1[i] = cos((double)(i+factor1)/2);
        BufferX2[i] = cos((double)(i+factor2)/3);
    }
    factor1+=0.03;
    factor2-=0.07;
    curvePeltier->setRawSamples(&BufferX1[0],&BufferY1[0],2000);
    curveHeatsink->setRawSamples(&BufferX2[0],&BufferY1[0],2000);
    plot->replot();

}


TfrmTempctrl::~TfrmTempctrl()
{
    delete ui;
}
