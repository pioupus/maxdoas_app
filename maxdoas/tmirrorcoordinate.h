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
    TMirrorCoordinate();
    QPoint getMotorCoordinate();
    QPointF getAngleCoordinate();
//    TMirrorCoordinate & operator= (const TMirrorCoordinate & other)
//    {
//        if (this != &other) // protect against invalid self-assignment
//        {
//            AngleCoordinate = other.AngleCoordinate;
//        }
//        // by convention, always return *this
//        return *this;
//    }

private:
    QPointF AngleCoordinate;
};

#endif // TMIRRORCOORDINATE_H
