#ifndef TTIMELINE_H
#define TTIMELINE_H
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QPair>
#include <QScriptable>

class ttimeline:public QObject, QScriptable
{
    Q_OBJECT
public:
    ttimeline();


public slots:
    void plot(int plotIndex);
    void scaleTimeAxis(int minutes);
    //void addPoint(QDateTime time,double val);
    void addPoint(double ms,double val);
    void setXAxisName(QString name);
    void setYAxisName(QString name);

public:
    int getEntryCount();
    double getEntryX(int i);
    double getEntryY(int i);
    double getmsTimeRange();

private:
    QString xAxisName;
    QString yAxisName;
    QList<QPair<QDateTime,float> > timeLine;
    int timeAxisScaleMinutes;
};

#endif // TTIMELINE_H
