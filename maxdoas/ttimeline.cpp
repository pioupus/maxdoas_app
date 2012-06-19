#include "ttimeline.h"
#include "tspectrumplotter.h"
ttimeline::ttimeline()
{
    timeAxisScalems =-1;
}

void ttimeline::plot(int plotIndex)
{
    TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(0);
    SpectrumPlotter->plotTimeLine(this,plotIndex);
}

void ttimeline::scaleTimeAxis(int hour, int minutes)
{
    qint64 hourms = hour*60*60*1000;;
    qint64 min = minutes*60*1000;
    qint64 scale = hourms*min - QDateTime::currentMSecsSinceEpoch();
    timeAxisScalems = scale;
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

double ttimeline::getmsTimeRange()
{
    if (timeAxisScalems != -1){
        return timeAxisScalems;
    }
    else
        return -1;
}


