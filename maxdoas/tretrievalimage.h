#ifndef TRETRIEVALIMAGE_H
#define TRETRIEVALIMAGE_H

#include <QObject>
#include "tretrieval.h"
#include <QScriptEngine>
#include <QScriptContext>
#include <QScriptValue>
#include <QScriptable>

class TRetrievalImage:public QObject
{
    Q_OBJECT
public:
    TRetrievalImage(int width, int height);
    TRetrievalImage(TRetrievalImage *other);
    ~TRetrievalImage();

    TRetrieval* **valueBuffer;
    int getWidth();
    int getHeight();
public slots:
    void save(QString fn);
    void plot(int plotIndex,int Pixelsize);
private:
    void inibuffer(int width, int height, TRetrievalImage *other);
    int width;
    int height;
};

#endif // TRETRIEVALIMAGE_H
