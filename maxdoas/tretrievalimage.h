#ifndef TRETRIEVALIMAGE_H
#define TRETRIEVALIMAGE_H

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

class TRetrievalImage:public QObject
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
public slots:
    void save(QString fn);
    void plot(int plotIndex, int Pixelsize=10);
    void oplotWindField(int plotIndex, int Average=1, bool normalize=true,bool excludezero=true);
    float getMinVal();
    float getMaxVal();
private:
    void inibuffer(int width, int height, TRetrievalImage *other);
    int width;
    int height;
};

#endif // TRETRIEVALIMAGE_H
