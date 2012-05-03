#ifndef TMIRRORCOORDINATE_H
#define TMIRRORCOORDINATE_H
#include <QObject>
#include <QPoint>
#include <QPointF>



class TMirrorCoordinate:public QObject
{
    Q_OBJECT
public:

    TMirrorCoordinate(QObject *parent = 0);
    TMirrorCoordinate(QPointF c);
    TMirrorCoordinate(TMirrorCoordinate * other);

    //TMirrorCoordinate();
    ~TMirrorCoordinate();
    QPoint getMotorCoordinate();
    QPointF getAngleCoordinate();
    QPointF getZenithCoordinate();
    void setAngleCoordinate(QPointF ac);
    void setMotorCoordinate(QPoint mc);
    void setMotorCoordinate(int X, int Y);

    int pixelIndexX;
    int pixelIndexY;
    int pixelIndex;
private:
    QPointF AngleCoordinate;
};

#endif // TMIRRORCOORDINATE_H
