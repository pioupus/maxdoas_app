#include "tspectrumplotter.h"
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

#include <qwt_plot_marker.h>


TSpectrumPlotter* TSpectrumPlotter::m_Instance = 0;

TPlot::TPlot(QBoxLayout *parent){
    plot = new QwtPlot();
    parent->addWidget(plot);
}

TPlot::~TPlot(){
    QwtPlotCurve * curve;
    for(int i = 0; i< CurveList.count();i++){
        curve = CurveList[i];
        delete curve;
    }
    delete plot;
}

QwtPlotCurve * TPlot::getLastCurve(){
    if (CurveList.count()>0){
        return CurveList[CurveList.count()-1];
    }else{
        return NULL;
    }
}

QwtPlotCurve * TPlot::getCurve(int index){
    if(CurveList.count()<= index){
        int s = CurveList.count();
        for(int i=s;i<=index;i++){
            addCurve();
        }
    }
    return CurveList[index];
}

void TPlot::addCurve(){
    QwtPlotCurve * curve = new QwtPlotCurve();
    curve->attach(plot);
    CurveList.append(curve);
}

int TPlot::count(){
    return CurveList.count();
}

QwtPlot *TPlot::getPlot(){
    return plot;
}

//QBoxLayout *parent;
//QwtPlot *plot;
//QList<QwtPlotCurve *> CurveList;

TSpectrumPlotter::TSpectrumPlotter()
{
}

void TSpectrumPlotter::setParentLayout(QBoxLayout *parent)
{
    parentLayout = parent;

//    ImagePlot = new QwtPlot(this);
//    parentLayout->addWidget(ImagePlot);
//    SpectrPlot = new QwtPlot(this);
//    parentLayout->addWidget(SpectrPlot);
//    SpectrPlot->setAxisScale(0,-2,7000);
    //SpectrPlotCurve = new QwtPlotCurve("Spectrum");
  //  SpectrPlotCurve->attach(SpectrPlot);
//    d_spectrogram = new QwtPlotSpectrogram();
//    d_spectrogram->setRenderThreadCount(0); // use system specific thread count
//
//    d_spectrogram->setColorMap( new ColorMap() );
//
//    d_spectrogram->setData(new RasterData());
//    d_spectrogram->attach(ImagePlot);
}

void TSpectrumPlotter::plotSpectrum(TSpectrum *spectrum, int plotIndex){
    TPlot *plot = NULL;
    if (PlotList.count() <= plotIndex){
        int c = PlotList.count();
        for(int i = c;i<=plotIndex;i++){
            if (i == plotIndex){
                plot = new TPlot(parentLayout);
                PlotList.append(plot);
            }else{
                PlotList.append(NULL);
            }
        }
    }else{
        plot = PlotList[plotIndex];
    }
    QwtPlot * p;
    QwtPlotCurve * c;
//    marker_corr_top->setValue(0, ac.targetPeak*spectrum.MaxPossibleValue/100 + ac.targetCorridor*spectrum.MaxPossibleValue/100);
//    marker_corr_bot->setValue(0,ac.targetPeak*spectrum.MaxPossibleValue/100 - ac.targetCorridor*spectrum.MaxPossibleValue/100);
//    marker_target->setValue(0,ac.targetPeak*spectrum.MaxPossibleValue/100);
    p = plot->getPlot();
    c = plot->getCurve(0);
    p->setAxisScale(0,0,spectrum->MaxPossibleValue);
    p->setAxisScale(1,spectrum->Wavelength->buf[0],spectrum->Wavelength->buf[spectrum->NumOfSpectrPixels-1]);
    c->setSamples(&spectrum->Wavelength->buf[0],&spectrum->spectrum[0],spectrum->NumOfSpectrPixels);
    p->replot();

}

//marker_corr_top = new QwtPlotMarker();
//marker_corr_bot = new QwtPlotMarker();
//marker_target  = new QwtPlotMarker();
//marker_corr_top->setSymbol( new QwtSymbol(QwtSymbol::NoSymbol  ,
//                      QColor(Qt::red), QPen(Qt::red,1), QSize(20,20)));
//marker_corr_bot->setSymbol( new QwtSymbol(QwtSymbol::  NoSymbol,
//                      QColor(Qt::red), QPen(Qt::red,1), QSize(20,20)));
//marker_target->setSymbol( new QwtSymbol(QwtSymbol::  NoSymbol,
//                      QColor(Qt::green), QPen(Qt::green,1), QSize(20,20)));
//
//marker_corr_top->setLineStyle( QwtPlotMarker::HLine);
//marker_corr_bot->setLineStyle( QwtPlotMarker::HLine);
//marker_target->setLineStyle( QwtPlotMarker::HLine);
//
//marker_corr_top->attach(SpectrPlot);
//marker_corr_bot->attach(SpectrPlot);
//marker_target->attach(SpectrPlot);
//
//QwtMarkerArrow *d_marker2;
//for (int i=0;i<32;i++){
//    for (int j=0;j<32;j++){
//
//        d_marker2 = new QwtMarkerArrow();
//
//        d_marker2->setSymbol( new QwtSymbolArrow(QwtSymbolArrow::  UserStyle,
//                              QColor(Qt::white), QPen(Qt::white,1), QSize(20,20)));
//        d_marker2->setValue(j,i,cos(i/2)/2,sin(j/3)/2);
//        d_marker2->attach(ImagePlot);
//    }
//}
