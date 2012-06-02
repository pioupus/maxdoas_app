#ifndef TTIMELINE_H
#define TTIMELINE_H
#include <QObject>
#include <QDateTime>
class ttimeline: public  QObject
{
public:
    ttimeline();

    void addPoint(QDateTime time,double val);
public slots:
    void plot();
};

#endif // TTIMELINE_H
