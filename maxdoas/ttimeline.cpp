#include "ttimeline.h"
#include "tspectrumplotter.h"
ttimeline::ttimeline()
{
}

void ttimeline::plot(int plotIndex)
{
    TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(0);
    SpectrumPlotter->plotTimeLine(this,plotIndex);
}

void ttimeline::setXAxisName(QString name)
{
    xAxisName = name;
}

void ttimeline::setYAxisName(QString name)
{
    xAxisName = name;
}

int ttimeline::getEntryCount()
{

    return timeLine.count();
}

void ttimeline::addPoint(double ms, double val)
{
    timeLine.append(QPair<QDateTime,float>(QDateTime::fromMSecsSinceEpoch(ms),val));
}

double ttimeline::getEntryX(int i)
{
    return timeLine[i].first.toMSecsSinceEpoch();
}

double ttimeline::getEntryY(int i)
{
    return timeLine[i].second;
}


