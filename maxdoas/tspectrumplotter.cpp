#include "tspectrumplotter.h"
#include <qwt_legend.h>
#include <qwt_legend_item.h>
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
#include <qwt_color_map.h>
#include <qwt_interval.h>

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

TSpectrumPlotter* TSpectrumPlotter::m_Instance = 0;

QColor intToColor(int i){
    switch (i){
        case 0: return Qt::black;
        case 1: return Qt::white;
        case 2: return Qt::darkGray;
        case 3: return Qt::gray;
        case 4: return Qt::lightGray;
        case 5: return Qt::red;
        case 6: return Qt::green;
        case 7: return Qt::blue;
        case 8: return Qt::cyan;
        case 9: return Qt::magenta;
        case 10: return Qt::yellow;
        case 11: return Qt::darkRed;
        case 12: return Qt::darkGreen;
        case 13: return Qt::darkBlue;
        case 14: return Qt::darkCyan;
        case 15: return Qt::darkMagenta;
        case 16: return Qt::darkYellow;
        default: return Qt::black;
    }
}

QwtPlot::LegendPosition intToLegendPos(int i){
    switch (i){
        case 0: return QwtPlot::LeftLegend;
        case 1: return QwtPlot::RightLegend;
        case 2: return QwtPlot::BottomLegend;
        case 3: return QwtPlot::TopLegend;
        default: return QwtPlot::LeftLegend;
    }
}

TPlot::TPlot(QBoxLayout *parent){
    plot = new QwtPlot();
    parent->addWidget(plot);
    legend = NULL;
    AutoIntegMarkerTop=NULL;
    AutoIntegMarkerCenter=NULL;
    AutoIntegMarkerBot=NULL;
    imgPlot = NULL;
}

TPlot::~TPlot(){
    clearMarkers();
    clearCurves();
    delete plot;
    delete legend;
}

QwtPlotCurve * TPlot::getLastCurve(){
    if (CurveList.count()>0){
        return CurveList.last();
    }else{
        return getCurve(0);
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

QwtPlotMarker * TPlot::getMarker(int index){
    QwtPlotMarker * marker = NULL;
    if(MarkerList.count()<= index){
        marker = MarkerList[index];
    }
    return marker;
}

QwtPlotMarker * TPlot::addMarker(){
    QwtPlotMarker * marker = new QwtPlotMarker();
    marker->attach(plot);
    MarkerList.append(marker);
    return marker;
}

QwtPlotMarker * TPlot::getAutoIntegTimeMarker(int index){
    QwtPlotMarker * marker = NULL;

    switch(index){
        case 0: if (AutoIntegMarkerTop == NULL){
                    AutoIntegMarkerTop = new QwtPlotMarker();
                    AutoIntegMarkerTop->attach(plot);
                    AutoIntegMarkerTop->setLineStyle( QwtPlotMarker::HLine);
                }
                marker = AutoIntegMarkerTop;
                break;
        case 1: if (AutoIntegMarkerCenter == NULL){
                    AutoIntegMarkerCenter = new QwtPlotMarker();
                    AutoIntegMarkerCenter->attach(plot);
                    AutoIntegMarkerCenter->setLineStyle( QwtPlotMarker::HLine);
                }
                marker = AutoIntegMarkerCenter;
                break;
        case 2: if (AutoIntegMarkerBot == NULL){
                    AutoIntegMarkerBot = new QwtPlotMarker();
                    AutoIntegMarkerBot->attach(plot);
                    AutoIntegMarkerBot->setLineStyle( QwtPlotMarker::HLine);
                }
                marker = AutoIntegMarkerBot;
                break;
    }

    return marker;
}

int TPlot::MarkerCount(){
    return MarkerList.count();
}

int TPlot::CurveCount(){
    return CurveList.count();
}

QwtPlot *TPlot::getPlot(){
    return plot;
}

QwtPlotSpectroCurve *TPlot::getImgPlot(){
    if (imgPlot == NULL){
        imgPlot = new QwtPlotSpectroCurve();
        QwtInterval interval(-1.0,1.0);
        //imgPlot->
       // imgPlot->setRenderThreadCount(1);
        imgPlot->setColorMap( new ColorMap() );
        imgPlot->setColorRange(interval);
        imgPlot->attach(plot);
    }
    return imgPlot;

    //    d_spectrogram->setData(new RasterData());


}

QwtLegend *TPlot::getLegend(int pos){
    if (legend==NULL){
        legend = new QwtLegend();
        plot->insertLegend(legend, intToLegendPos(pos));
    }
    return legend;
}

void TPlot::clearMarkers(){
    QwtPlotMarker * marker;
    for(int i = 0; i< MarkerList.count();i++){
        marker = MarkerList[i];
        delete marker;
    }
    clearAutoIntegTimeMarkers();
    MarkerList.clear();
}

void TPlot::clearAutoIntegTimeMarkers(){
    delete AutoIntegMarkerTop;
    delete AutoIntegMarkerCenter;
    delete AutoIntegMarkerBot;
    AutoIntegMarkerTop=NULL;
    AutoIntegMarkerCenter=NULL;
    AutoIntegMarkerBot=NULL;
}

void TPlot::clearCurves(){
    QwtPlotCurve * curve;
    for(int i = 0; i< CurveList.count();i++){
        curve = CurveList[i];
        delete curve;
    }
    CurveList.clear();
}

void TPlot::clearLegend(){
    delete legend;
    legend = NULL;
}

void TPlot::setLastSpecDate(QDateTime  lastSpecDate){
    this->lastSpecDate = lastSpecDate;

}

QDateTime TPlot::getLastSpecDate(){
    return lastSpecDate;
}



TSpectrumPlotter::TSpectrumPlotter(QObject *parent)
{
    setParent(parent);
    nextTitle = "";
    nextXTitle = "";
    nextYTitle = "";
    nextXRange.first =  0.0;
    nextXRange.second = 0.0;
    nextYRange.first =  0.0;
    nextYRange.second = 0.0;
    nextColor = NULL;
    plotAutoIntegrationtimeParametersEnabled = false;
}


TSpectrumPlotter::~TSpectrumPlotter()
{
    TPlot *plot = NULL;
    for (int i = 0;i<PlotList.count();i++){
        plot = PlotList[i];
        delete plot;
    }
    PlotList.clear();
}

void TSpectrumPlotter::setParentLayout(QBoxLayout *parent)
{
    parentLayout = parent;


}

TPlot* TSpectrumPlotter::getPlot(int index){
    TPlot *plot = NULL;
    if (PlotList.count() <= index){
        int c = PlotList.count();
        for(int i = c;i<=index;i++){
            if (i == index){
                plot = new TPlot(parentLayout);
                PlotList.append(plot);
            }else{
                PlotList.append(NULL);
            }
        }
    }else{
        plot = PlotList[index];
        if (plot == NULL){
            plot = new TPlot(parentLayout);
            PlotList[index] = plot;
        }
    }
    return plot;
}

void TSpectrumPlotter::plotSpectrum(TSpectrum *spectrum, int plotIndex){
    TPlot *plot = getPlot(plotIndex);
    QwtPlot * p = plot->getPlot();
    QwtPlotCurve * c = plot->getCurve(0);
    QPen pe;
   // pe.setWidthF(1.5);
    p->setAxisScale(0,0,spectrum->MaxPossibleValue);
    p->setAxisScale(1,spectrum->Wavelength->buf[0],spectrum->Wavelength->buf[spectrum->NumOfSpectrPixels-1]);
    c->setSamples(&spectrum->Wavelength->buf[0],&spectrum->spectrum[0],spectrum->NumOfSpectrPixels);
    c->setPen(pe);
    if(plotAutoIntegrationtimeParametersEnabled && spectrum->IntegConf.autoenabled){
        float tp = spectrum->IntegConf.targetPeak;
        float tc = spectrum->IntegConf.targetCorridor;
        float mv = spectrum->MaxPossibleValue;
        plot->getAutoIntegTimeMarker(0)->setYValue(tp*mv/100 + tc*mv/100);
        plot->getAutoIntegTimeMarker(1)->setYValue(tp*mv/100 - tc*mv/100);
        plot->getAutoIntegTimeMarker(2)->setYValue(tp*mv/100);
    }else{
        plot->clearAutoIntegTimeMarkers();
    }
    if(!nextTitle.isEmpty())
        p->setTitle(nextTitle);
    nextTitle = "";
    if(!nextXTitle.isEmpty())
        p->setAxisTitle(QwtPlot::xBottom,nextXTitle);
    nextXTitle = "";
    if(!nextYTitle.isEmpty())
        p->setAxisTitle(QwtPlot::yLeft,nextYTitle);
    nextYTitle = "";
    if(nextXRange.first != nextXRange.second)
        p->setAxisScale(QwtPlot::xBottom,nextXRange.first,nextXRange.second);
    nextXRange.first = 0;
    nextXRange.second = 0;
    if(nextYRange.first != nextYRange.second)
        p->setAxisScale(QwtPlot::yLeft,nextYRange.first,nextYRange.second);
    nextYRange.first = 0;
    nextYRange.second = 0;
    if(nextColor != NULL)
        c->setPen(*nextColor);
    delete nextColor;
    nextColor = NULL;

    if (spectrum->datetime.isValid()){
        plot->setLastSpecDate(spectrum->datetime);
    }
    p->replot();

}

void TSpectrumPlotter::plotRetrievalImage(TRetrievalImage *img,int plotIndex){
    TPlot *plot = getPlot(plotIndex);
    QwtPlotSpectroCurve * s = plot->getImgPlot();
    QVector< QwtPoint3D > * vec = new  QVector< QwtPoint3D >(img->getHeight()*img->getWidth());
    double minval=0,maxval=0,val;
    for(int y =0; y<img->getHeight();y++){
        for(int x =0; x<img->getWidth();x++){
            QwtPoint3D  p3d;// = new QwtPoint3D();
            TMirrorCoordinate * mc;
            mc = img->valueBuffer[y][x]->mirrorCoordinate;

            p3d.setX(mc->getAngleCoordinate().x());
            p3d.setY(mc->getAngleCoordinate().y());
            val = img->valueBuffer[y][x]->val;
            p3d.setZ(val);
            if ((minval > val)||((y+x)==0)){
                minval = val;
            }
            if ((maxval < val)||((y+x)==0)){
                maxval = val;
            }
            vec->replace(y*img->getWidth()+x,p3d);
        }
    }
    s->setPenWidth(10);
    //s->setInterval( Qt::ZAxis, QwtInterval(0, 1) );
    s->setSamples(*vec);
}

void TSpectrumPlotter::plotSpectralImage(TSpectralImage *img,int plotIndex, int Pixelsize){
    TPlot *plot = getPlot(plotIndex);
    QwtPlotSpectroCurve * s = plot->getImgPlot();
    QVector< QwtPoint3D > * vec = new  QVector< QwtPoint3D >(img->count());
    double minval=0,maxval=0,val;
    for(int i =0; i<img->count();i++){
        QwtPoint3D  p3d;// = new QwtPoint3D();
        TMirrorCoordinate * mc;
        mc = img->getMirrorCoordinate(i);
        p3d.setX(mc->getAngleCoordinate().x());
        p3d.setY(mc->getAngleCoordinate().y());
        val = img->getSpectrum(i)->rms();
        p3d.setZ(val);
        if ((minval > val)||i==0){
            minval = val;
        }
        if ((maxval < val)||i==0){
            maxval = val;
        }
        vec->replace(i,p3d);
    }
    s->setPenWidth(Pixelsize);
    QwtInterval interval(minval,maxval);
    s->setColorRange(interval);
    s->setSamples(*vec);
    plot->getPlot()->replot();
}

void TSpectrumPlotter::setTitle(QString text,int plotindex){
    if (plotindex != -1){
        TPlot *plot = getPlot(plotindex);
        QwtPlot * p = plot->getPlot();
        p->setTitle(text);
    }else{
        nextTitle = text;
    }
}

void TSpectrumPlotter::setXAxisTitle(QString text,int plotindex){
    if (plotindex != -1){
        TPlot *plot = getPlot(plotindex);
        QwtPlot * p = plot->getPlot();
        p->setAxisTitle(QwtPlot::xBottom,text);
    }else{
        nextXTitle = text;
    }
}

void TSpectrumPlotter::setYAxisTitle(QString text,int plotindex){
    if (plotindex != -1){
        TPlot *plot = getPlot(plotindex);
        QwtPlot * p = plot->getPlot();
        p->setAxisTitle(QwtPlot::yLeft,text);
    }else{
        nextYTitle = text;
    }
}


void TSpectrumPlotter::setXAxisRange(double min,double max,int plotindex){
    if (plotindex != -1){
        TPlot *plot = getPlot(plotindex);
        QwtPlot * p = plot->getPlot();
        p->setAxisScale(QwtPlot::xBottom,min,max);
    }else{
        nextXRange.first = min;
        nextXRange.second = max;
    }
}

void TSpectrumPlotter::setYAxisRange(double min,double max,int plotindex){
    if (plotindex != -1){
        TPlot *plot = getPlot(plotindex);
        QwtPlot * p = plot->getPlot();
        p->setAxisScale(QwtPlot::yLeft,min,max);
    }else{
        nextYRange.first = min;
        nextYRange.second = max;
    }
}

void TSpectrumPlotter::setLegend(QString title,int pos, int plotindex,int curveindex){
    if (plotindex != -1){
        TPlot *plot = getPlot(plotindex);
        //wtPlot * p = plot->getPlot();
        plot->getLegend(pos);
        QwtPlotCurve * c;
        if (curveindex == -1)
            c = plot->getLastCurve();
        else
            c = plot->getCurve(curveindex);

        c->setTitle(title);
    }else{

    }

}

void TSpectrumPlotter::setCurveColor(int color,int plotindex){
    if (plotindex != -1){
        QPen p;
       // p.setWidthF(1.5);
        p.setColor(intToColor(color));
        TPlot *plot = getPlot(plotindex);
        QwtPlotCurve * c = plot->getCurve(0);
        c->setPen(p);
    }else{
        delete nextColor;
        nextColor = new QColor(intToColor(color));
    }
}

void TSpectrumPlotter::plotVMarker(double x,QString title,int plotindex){
    TPlot *plot = getPlot(plotindex);
    if (plot != NULL){
        QPen p;
   //     p.setWidthF(1.5);
        QwtPlotMarker * marker = plot->addMarker();
        marker->setLineStyle( QwtPlotMarker::VLine);
        marker->setLinePen(p);
        marker->setXValue(x);
        marker->setTitle(title);
    }
}

void TSpectrumPlotter::plotHMarker(double y,QString title,int plotindex){
    TPlot *plot = getPlot(plotindex);
    if (plot != NULL){
        QPen p;
      //  p.setWidthF(1.5);
        QwtPlotMarker * marker = plot->addMarker();
        marker->setLineStyle( QwtPlotMarker::HLine);
        marker->setLinePen(p);
        marker->setYValue(y);
        marker->setTitle(title);
    }
}

void TSpectrumPlotter::plotXYMarker(double x,double y,QString title,int plotindex){
    TPlot *plot = getPlot(plotindex);
    if (plot != NULL){
        QPen p;
       // p.setWidthF(1.5);
        QwtPlotMarker * marker = plot->addMarker();
        marker->setLineStyle( QwtPlotMarker::Cross);
        marker->setXValue(x);
        marker->setYValue(y);
        marker->setLinePen(p);
//      marker->setSymbol( new QwtSymbol(QwtSymbol::Diamond  ,
//                          Qt::NoBrush, QPen(Qt::red,1), QSize(10,10)));
//      marker->setSymbol( new QwtSymbol(QwtSymbol::Diamond));
        marker->setTitle(title);
        marker->setLabel(title);
        marker->setLabelAlignment(Qt::AlignLeft);
    }
}

void TSpectrumPlotter::reset(int plotindex){
    TPlot *plot = getPlot(plotindex);
    if (plot != NULL){
        plot->clearMarkers();
        plot->clearCurves();
        plot->clearLegend();
    }
}

void TSpectrumPlotter::clearMarker(int plotindex){
    TPlot *plot = getPlot(plotindex);
    if (plot != NULL){
        plot->clearMarkers();
    }
}

void TSpectrumPlotter::plotAutoIntegrationtimeParameters(bool enabled){
    plotAutoIntegrationtimeParametersEnabled = enabled;
}

void TSpectrumPlotter::plotToFile(QString format, QString Directory , QString BaseName, int SequenceNo , int plotindex,int width,int height, int resolution ){
    TPlot *plot = getPlot(plotindex);
    if (plot != NULL){
        QDateTime lastSpecDate = plot->getLastSpecDate();
        QString filename;
        filename = DefaultFileNameFromSeqNumber(Directory,BaseName,SequenceNo,lastSpecDate)+".png";
        plotToFile(format,filename,plotindex,width,height,resolution);
    }
}

void TSpectrumPlotter::plotToFile(QString format, QString filename ,int plotindex,int width,int height, int resolution){
    TPlot *plot = getPlot(plotindex);
    if (plot != NULL){
        QwtPlot * p = plot->getPlot();
        if ( (!filename.isEmpty()) && p != NULL )
        {
            QwtPlotRenderer renderer;

            QList<QPen>PenList;
            for (int i = 0;i<plot->CurveCount();i++){
                QPen pe = plot->getCurve(i)->pen();
                PenList.append(pe);

                pe.setWidthF(pe.widthF()+1);
                plot->getCurve(i)->setPen(pe);
            }
            QwtScaleDraw * axisX;
            QwtScaleDraw * axisY;
            int axisXWidth;
            int axisYWidth;
            axisX = p->axisScaleDraw(QwtPlot::xBottom);
            axisY = p->axisScaleDraw(QwtPlot::yLeft);
            axisXWidth = axisX->penWidth();
           // qDebug()<<axisXWidth;
            axisYWidth = axisY->penWidth();
            axisX->setPenWidth(axisYWidth+1);
            axisY->setPenWidth(axisXWidth+1);
            renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasBackground,true);
            renderer.renderDocument(p, filename,format, QSizeF(width, height), resolution);

            for (int i = 0;i<plot->CurveCount();i++){
                plot->getCurve(i)->setPen(PenList[i]);
            }
            axisX->setPenWidth(axisYWidth);
            axisY->setPenWidth(axisXWidth);
        }
    }



}
//marker_corr_top = new QwtPlotMarker();
//marker_corr_top->setSymbol( new QwtSymbol(QwtSymbol::NoSymbol  ,
//                      QColor(Qt::red), QPen(Qt::red,1), QSize(20,20)));
//
//marker_corr_top->setLineStyle( QwtPlotMarker::HLine);
//
//    marker_corr_top->setValue(0, ac.targetPeak*spectrum.MaxPossibleValue/100 + ac.targetCorridor*spectrum.MaxPossibleValue/100);
//    marker_corr_bot->setValue(0,ac.targetPeak*spectrum.MaxPossibleValue/100 - ac.targetCorridor*spectrum.MaxPossibleValue/100);
//    marker_target->setValue(0,ac.targetPeak*spectrum.MaxPossibleValue/100);

//marker_corr_top->attach(SpectrPlot);
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


//    ImagePlot = new QwtPlot(this);
//    parentLayout->addWidget(ImagePlot);
//    SpectrPlot = new QwtPlot(this);
//    parentLayout->addWidget(SpectrPlot);
//    SpectrPlot->setAxisScale(0,-2,7000);
//    SpectrPlotCurve = new QwtPlotCurve("Spectrum");
//    SpectrPlotCurve->attach(SpectrPlot);
//    d_spectrogram = new QwtPlotSpectrogram();
//    d_spectrogram->setRenderThreadCount(0); // use system specific thread count
//
//    d_spectrogram->setColorMap( new ColorMap() );
//
//    d_spectrogram->setData(new RasterData());
//    d_spectrogram->attach(ImagePlot);
