#ifndef TRETRIEVALIMAGE_H
#define TRETRIEVALIMAGE_H

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <QObject>
#include "tretrieval.h"
#include <QScriptEngine>
#include <QScriptContext>
#include <QScriptValue>
#include <QScriptable>
#include <QDateTime>


#include "log4qt/consoleappender.h"
#include "log4qt/logger.h"
#include "log4qt/ttcclayout.h"

class TRetrievalImage:public QObject, QScriptable
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    TRetrievalImage(int width, int height);
    TRetrievalImage(TRetrievalImage *other);
    TRetrievalImage(QString fn,QString fmt,float PixelWidthAngle,float PixelHeightAngle);
    ~TRetrievalImage();

    TRetrieval* **valueBuffer;
    int getWidth();
    int getHeight();
    QDateTime datetime;
    QPointF getMeanVec(void);
    QPointF getMaxVec();
    QPoint getClosesPoint(QPointF coor);
    void subMatrix( Eigen::MatrixXd& sub,bool sign);
    QPointF getCoordinateInMeters(int col,int row);
    void setTimeDiff(float sec);
    void mapWindVektors(TRetrievalImage* windvektor);
    void setEmissionFactor(float ef);
    void setDOFs(float dof_x,float dof_y,float dof_src);
public slots:
    void save(QString fn,bool PosInfo, bool windvector);
    void plot(int plotIndex, int Pixelsize=10);
    void oplotWindField(int plotIndex, int Average=1, bool normalize=true,bool excludezero=true);
    float getMinVal();
    float getMaxVal();
    void scaleThresholdImageValues(float threshold);
    void thresholdImageValues(float threshold);
    void loadWeights(TRetrievalImage* weights);
    void loadWeightSIGIS(QString fn);
    QScriptValue emissionrate(float TimeStep);
    float getMaxVelocity();
    float getMeanVelocity();
    void setMeanDistance(float Distance);
    int getTime();
    void setSpeedCorrection(float correctionfactor);
private:
    void inibuffer(int width, int height, TRetrievalImage *other);
    //TEmissionrate* emissionrate_(float TimeStep, float ScanPixelsize);
    int width;
    int height;
    int meanDistance;
    QPointF coordinateInMeters(QPointF angle);
    float timeDiff;//insec
    float emissionFactor;
    float dof_x;
    float dof_y;
    float dof_src;
};

#endif // TRETRIEVALIMAGE_H
