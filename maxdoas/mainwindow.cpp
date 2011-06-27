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
#include <QCloseEvent>
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
    if (!s.contains("Debug")){
        s.setValue("Debug", "ERROR");

        // Configure logging to log to the file C:/myapp.log using the level TRACE
        s.beginGroup("Properties");
        s.setValue("log4j.appender.A1", "org.apache.log4j.FileAppender");
        s.setValue("log4j.appender.A1.file", "log4qt.log");
        s.setValue("log4j.appender.A1.layout", "org.apache.log4j.TTCCLayout");
        s.setValue("log4j.appender.A1.layout.DateFormat", "ISO8601");
        s.setValue("log4j.rootLogger", "ERROR, A1");
    }
    // Settings will become active on next application startup
}



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    closenow = false;
    ui->setupUi(this);    
    lblComPortStatus = new QLabel("");
    statusBar()->addWidget(lblComPortStatus);
    HWDriver = new THWDriver();
    setupLog4Qt();    // Log first message, which initialises Log4Qt
    logger()->warn("test");
    thread()->setObjectName("Main");
    //Logger("MaxDoas")->name("Main");
    ms = TMaxdoasSettings::instance();

  //  connect(this,SIGNAL(finished()),this,SLOT(on_Finished()));
    //  HWDriver->hwdGetListSpectrometer();
    //ui->cbSpectrList->addItems(HWDriver->hwdGetListSpectrometer());

    ImagePlot = new QwtPlot(this);
    ui->hbox->addWidget(ImagePlot);
    SpectrPlot = new QwtPlot(this);
    ui->hbox->addWidget(SpectrPlot);
    SpectrPlot->setAxisScale(0,-2,7000);
    SpectrPlotCurve = new QwtPlotCurve("Spectrum");
    SpectrPlotCurve->attach(SpectrPlot);
    d_spectrogram = new QwtPlotSpectrogram();
    d_spectrogram->setRenderThreadCount(0); // use system specific thread count

    d_spectrogram->setColorMap( new ColorMap() );

    d_spectrogram->setData(new RasterData());
    d_spectrogram->attach(ImagePlot);
    marker_corr_top = new QwtPlotMarker();
    marker_corr_bot = new QwtPlotMarker();
    marker_target  = new QwtPlotMarker();
    marker_corr_top->setSymbol( new QwtSymbol(QwtSymbol::NoSymbol  ,
                          QColor(Qt::red), QPen(Qt::red,1), QSize(20,20)));
    marker_corr_bot->setSymbol( new QwtSymbol(QwtSymbol::  NoSymbol,
                          QColor(Qt::red), QPen(Qt::red,1), QSize(20,20)));
    marker_target->setSymbol( new QwtSymbol(QwtSymbol::  NoSymbol,
                          QColor(Qt::green), QPen(Qt::green,1), QSize(20,20)));

    marker_corr_top->setLineStyle( QwtPlotMarker::HLine);
    marker_corr_bot->setLineStyle( QwtPlotMarker::HLine);
    marker_target->setLineStyle( QwtPlotMarker::HLine);

    marker_corr_top->attach(SpectrPlot);
    marker_corr_bot->attach(SpectrPlot);
    marker_target->attach(SpectrPlot);

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

    connect(HWDriver,SIGNAL(hwdSigGotSpectrum()),this,SLOT(on_GotSpectrum()));
    connect(HWDriver,SIGNAL(hwdSigHWThreadFinished()),this,SLOT(HWThreadFinished()));
    connect(HWDriver,SIGNAL(hwdSigCOMPortChanged(QString,bool,bool)),this,SLOT(COMPortChanged(QString,bool,bool)));

    QMetaObject::invokeMethod(this,
                              "StartMeasure",
                              Qt::QueuedConnection);
}

void MainWindow::StartMeasure(){
    TSPectrWLCoefficients wlcoef;
    TAutoIntegConf ac;
    HWDriver->hwdSetTiltOffset(ms->getTiltOffset());
    HWDriver->hwdSetComPort(ms->getComPortConfiguration());
    HWDriver->hwdSetTargetTemperature(ms->getTargetTemperature());
    HWDriver->hwdOpenSpectrometer(ms->getPreferredSpecSerial());
    wlcoef = ms->getWaveLengthCoefficients(ms->getPreferredSpecSerial());
    HWDriver->hwdOverwriteWLCoefficients(&wlcoef);
    ac = ms->getAutoIntegrationRetrievalConf();
    HWDriver->setIntegrationConfiguration(&ac);
    HWDriver->hwdMeasureSpectrum(1,0,scNone);
}

void MainWindow::on_GotSpectrum(){
    TAutoIntegConf ac = ms->getAutoIntegrationRetrievalConf();
    HWDriver->hwdGetSpectrum(&spectrum);
    marker_corr_top->setValue(0, ac.targetPeak*spectrum.MaxPossibleValue/100 + ac.targetCorridor*spectrum.MaxPossibleValue/100);
    marker_corr_bot->setValue(0,ac.targetPeak*spectrum.MaxPossibleValue/100 - ac.targetCorridor*spectrum.MaxPossibleValue/100);
    marker_target->setValue(0,ac.targetPeak*spectrum.MaxPossibleValue/100);
    SpectrPlot->setAxisScale(0,0,spectrum.MaxPossibleValue);
    SpectrPlot->setAxisScale(1,spectrum.Wavelength->buf[0],spectrum.Wavelength->buf[spectrum.NumOfSpectrPixels-1]);
    SpectrPlotCurve->setRawSamples(&spectrum.Wavelength->buf[0],&spectrum.spectrum[0],spectrum.NumOfSpectrPixels);
    SpectrPlot->replot();
    HWDriver->hwdMeasureSpectrum(2,0,scNone);
}

void MainWindow::COMPortChanged(QString name, bool opened, bool error){
    QString s;
    if (opened)
        s = " opened";
    else
        s = " closed";
    if (error)
        s = s + " (error)";
    lblComPortStatus->setText("COM "+name+s);
}

void MainWindow::on_actionConfigSpectrometer_triggered(){
    TFrmSpectrConfig *tfrmspectrconfig = new TFrmSpectrConfig(HWDriver);
    tfrmspectrconfig->show();
}

void MainWindow::on_actionTempctrler_triggered(){
    TfrmTempctrl *frmtempctrler = new TfrmTempctrl(HWDriver);
    frmtempctrler->show();

}

void MainWindow::on_actionClose_triggered(){
    HWDriver->stop();
}


void MainWindow::HWThreadFinished(){
    close();
}

void MainWindow::closeEvent(QCloseEvent *event){


    if (!closenow){
        HWDriver->stop();


        event->ignore();
        closenow = true;
       // QMetaObject::invokeMethod(this,
       //                                      "close",
       //                                      Qt::QueuedConnection);
                //QTimer::singleShot(1000, this, SLOT(close()));
    }else{
        event->accept();
    }


}


MainWindow::~MainWindow()
{
    delete marker_corr_top;
    delete marker_corr_bot;
    delete marker_target;
    delete ImagePlot;
    delete SpectrPlot;
    delete HWDriver;
    delete lblComPortStatus;
    delete ui;
}
