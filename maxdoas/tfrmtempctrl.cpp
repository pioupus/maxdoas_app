#include "tfrmtempctrl.h"
#include "ui_tfrmtempctrl.h"



//void TfrmTempctrl::setHWDriver(THWDriver *hwdriver){
//    this->hwdriver = hwdriver;
//}

TfrmTempctrl::TfrmTempctrl(THWDriver *hwdriver, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TfrmTempctrl)
{
    int i;
    ui->setupUi(this);
    this->hwdriver=hwdriver;
    setAttribute(Qt::WA_DeleteOnClose, true);
    plot = new QwtPlot(this);
    curvePeltier = new QwtPlotCurve("Peltier");
    curveHeatsink = new QwtPlotCurve("Heatsink");
    curveSpectrometer = new QwtPlotCurve("Spectrometer");
    QwtLegend *legend = new QwtLegend;
    legend->setItemMode(QwtLegend::CheckableItem);
    plot->insertLegend(legend, QwtPlot::RightLegend);
  //  plot->aut
    ui->hbox->addWidget(plot);

    startTimer(1000);
    curvePeltier->attach(plot);
    curveHeatsink->attach(plot);
    curveSpectrometer->attach(plot);
    curvePeltier->setPen(QPen(Qt::green));
    curveHeatsink->setPen(QPen(Qt::red));
    BufferFilled = 0;

    for (i=0;i<TEMPERAT_BUFFER_SIZE;i++){
        BufferX1[i] = i;
    }
    connect(plot, SIGNAL(legendChecked(QwtPlotItem *, bool)),
        SLOT(showCurve(QwtPlotItem *, bool)));
    connect(hwdriver, SIGNAL(hwdSigGotTemperatures(float , float, float)),
        SLOT(SloGotTemperature(float , float, float )));

    showCurve(curvePeltier, true);
    showCurve(curveHeatsink, true);
    showCurve(curveSpectrometer, true);
    show();
}

void TfrmTempctrl::showCurve(QwtPlotItem *item, bool on)
{
    item->setVisible(on);
    QWidget *w = plot->legend()->find(item);
    if ( w && w->inherits("QwtLegendItem") )
        ((QwtLegendItem *)w)->setChecked(on);

    plot->replot();
}

void TfrmTempctrl::SloGotTemperature(float TemperaturePeltier, float TemperatureSpectr, float TemperatureHeatsink){
        int i;
        (void)TemperaturePeltier;
        (void)TemperatureSpectr;
        (void)TemperatureHeatsink;
        BufferFilled++;
        if (BufferFilled == TEMPERAT_BUFFER_SIZE){
            for (i=1;i<TEMPERAT_BUFFER_SIZE;i++){
                BufferPeltier[i-1] = BufferPeltier[i];
                BufferHeatSink[i-1] = BufferHeatSink[i];
                BufferSpectrometer[i-1] = BufferSpectrometer[i];
            }
            BufferFilled = TEMPERAT_BUFFER_SIZE-1;
        }
        BufferPeltier[BufferFilled] = hwdriver->hwdGetTemperature(tsPeltier);
        BufferHeatSink[BufferFilled] = hwdriver->hwdGetTemperature(tsHeatSink);
        BufferSpectrometer[BufferFilled] = hwdriver->hwdGetTemperature(tsSpectrometer);

        curvePeltier->setRawSamples(&BufferX1[0],&BufferPeltier[0],BufferFilled);
        curveHeatsink->setRawSamples(&BufferX1[0],&BufferHeatSink[0],BufferFilled);
        curveSpectrometer->setRawSamples(&BufferX1[0],&BufferSpectrometer[0],BufferFilled);
        plot->replot();
}

void TfrmTempctrl::timerEvent(QTimerEvent *){




}


TfrmTempctrl::~TfrmTempctrl()
{
    delete ui;
}
