#include "ttimeline.h"
#include "tspectrumplotter.h"
ttimeline::ttimeline()
{
}

void ttimeline::plot(int plotIndex)
{
    TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(0);
    SpectrumPlotter->plotRetrievalImage(this,plotIndex,Pixelsize);
}

void ttimeline::setXAxisName(QString name)
{
    xAxisName = name;
}

void ttimeline::setYAxisName(QString name)
{
    xAxisName = name;
}

void ttimeline::addPoint(QDateTime time, double val)
{
    timeLine.append(QPair<QDateTime,float>(time,val));
}

