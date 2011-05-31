#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tfrmtempctrl.h"
#include "thwdriver.h"

#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_layout.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_canvas.h>
#include <qwtsymbolarrow.h>
#include <qwtmarkerarrow.h>
#include <qfiledialog.h>
#include <qimagewriter.h>
#include <qwt_plot_marker.h>
#include <QSettings>

#include "log4qt/consoleappender.h"
#include "log4qt/logger.h"
#include "log4qt/ttcclayout.h"
#include "tspectrum.h"
#include "tfrmspectrconfig.h"


class RasterData: public QwtMatrixRasterData
{
public:
    RasterData()
    {
        const uint size=32;

        QVector<double> values;
        for ( uint j = 0; j < size ; j++ )
            for ( uint i = 0; i < size ; i++ )
            values += sin((double)i/2)*cos((double)j/3);

        const int numColumns = size;
        setValueMatrix(values, numColumns);

        setInterval( Qt::XAxis,
            QwtInterval( 0, size, QwtInterval::ExcludeMaximum ) );
        setInterval( Qt::YAxis,
            QwtInterval( 0, size, QwtInterval::ExcludeMaximum ) );
        setInterval( Qt::ZAxis, QwtInterval(0, 1) );
    }
};

class ColorMap: public QwtLinearColorMap
{
public:
    ColorMap():
    QwtLinearColorMap(Qt::black, Qt::red)
    {
//        addColorStop(0.2, Qt::blue);
//        addColorStop(0.4, Qt::cyan);
//        addColorStop(0.6, Qt::yellow);
//        addColorStop(0.8, Qt::red);
    }
};

void MainWindow::setupLog4Qt()
{
    QSettings s;

    // Set logging level for Log4Qt to TRACE
    s.beginGroup("Log4Qt");
    s.setValue("Debug", "TRACE");

    // Configure logging to log to the file C:/myapp.log using the level TRACE
    s.beginGroup("Properties");
    s.setValue("log4j.appender.A1", "org.apache.log4j.FileAppender");
    s.setValue("log4j.appender.A1.file", "log4qt.log");
    s.setValue("log4j.appender.A1.layout", "org.apache.log4j.TTCCLayout");
    s.setValue("log4j.appender.A1.layout.DateFormat", "ISO8601");
    s.setValue("log4j.rootLogger", "TRACE, A1");

    // Settings will become active on next application startup
}

void MainWindow::timerEvent(QTimerEvent *){
//    uint i;
//    TSpectrum Spectrum;
//    i = HWDriver->hwdGetSpectrum(&Spectrum);
//
//    SpectrPlotCurve->setRawSamples(&Spectrum.spectrum[0],&Spectrum.Wavelength[0],i);
//    SpectrPlot->replot();

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Log first message, which initialises Log4Qt
    setupLog4Qt();
    thread()->setObjectName("Main");
    logger()->warn("test");
    //Logger("MaxDoas")->name("Main");
    ms = TMaxdoasSettings::instance();
    HWDriver = new THWDriver();
    connect(HWDriver,SIGNAL(hwdSigGotSpectrum()),this,SLOT(on_GotSpectrum()));
    //  HWDriver->hwdGetListSpectrometer();
    //ui->cbSpectrList->addItems(HWDriver->hwdGetListSpectrometer());
    HWDriver->hwdOpenSpectrometer(ms->getPreferredSpecSerial());
    HWDriver->hwdMeasureSpectrum(1,100,scNone);
    ImagePlot = new QwtPlot(this);
    ui->hbox->addWidget(ImagePlot);
    SpectrPlot = new QwtPlot(this);
    ui->hbox->addWidget(SpectrPlot);
    SpectrPlot->setAxisScale(0,-2,70000);
    SpectrPlotCurve = new QwtPlotCurve("Spectrum");
    SpectrPlotCurve->attach(SpectrPlot);
    d_spectrogram = new QwtPlotSpectrogram();
    d_spectrogram->setRenderThreadCount(0); // use system specific thread count

    d_spectrogram->setColorMap( new ColorMap() );

    d_spectrogram->setData(new RasterData());
    d_spectrogram->attach(ImagePlot);
    QwtMarkerArrow *d_marker2;
    for (int i=0;i<32;i++){
        for (int j=0;j<32;j++){

            d_marker2 = new QwtMarkerArrow();

            d_marker2->setSymbol( new QwtSymbolArrow(QwtSymbolArrow::  UserStyle,
                                  QColor(Qt::white), QPen(Qt::white,1), QSize(20,20)));
            d_marker2->setValue(j,i,cos(i/2)/2,sin(j/3)/2);
            d_marker2->attach(ImagePlot);
        }
    }

}

void MainWindow::on_GotSpectrum(){
    double wlb[100];
    memcpy(&wlb[0],&spectrum.Wavelength->buf,100*sizeof(double));
    HWDriver->hwdGetSpectrum(&spectrum);
    SpectrPlotCurve->setRawSamples(&spectrum.Wavelength->buf[0],&spectrum.spectrum[0],spectrum.NumOfSpectrPixels);
    SpectrPlot->replot();
    HWDriver->hwdMeasureSpectrum(20,1000,scNone);
}

void MainWindow::on_actionConfigSpectrometer_triggered(){
    TFrmSpectrConfig *tfrmspectrconfig = new TFrmSpectrConfig(HWDriver);
    tfrmspectrconfig->show();
}

void MainWindow::on_actionTempctrler_triggered(){
    TfrmTempctrl *frmtempctrler = new TfrmTempctrl(HWDriver);
    frmtempctrler->show();

}


MainWindow::~MainWindow()
{
    delete ImagePlot;
    delete SpectrPlot;
    delete ui;
}
