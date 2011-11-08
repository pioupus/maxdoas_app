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
    ~TRetrievalImage();

    TRetrieval* **valueBuffer;
    int getWidth();
    int getHeight();
public slots:
    void save(QString fn);
private:

    int width;
    int height;
};

#endif // TRETRIEVALIMAGE_H
