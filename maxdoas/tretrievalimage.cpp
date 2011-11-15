#include "tretrievalimage.h"
#include <QFile>
#include <QTextStream>

TRetrievalImage::TRetrievalImage(int width, int height)
{
    inibuffer(width,height,(TRetrievalImage*)NULL);
}

TRetrievalImage::TRetrievalImage(TRetrievalImage *other){
    inibuffer(other->width,other->height,other);
}

void TRetrievalImage::inibuffer(int width, int height, TRetrievalImage *other){
    this->width = width;
    this->height = height;
    valueBuffer = new TRetrieval** [height];
    for (int i = 0; i < height; i++){
        valueBuffer[i] = new TRetrieval* [width];
        for(int n = 0; n<width;n++){
            if (other == NULL)
                valueBuffer[i][n] = new TRetrieval();
            else
               valueBuffer[i][n] = new TRetrieval(other->valueBuffer[i][n]);

        }
        if (valueBuffer[i] == NULL){
            this->height = i;
            break;
        }
    }
}

TRetrievalImage::~TRetrievalImage()
{
    for (int i = 0; i < height; i++){
        for(int n = 0; n<width;n++){
            delete valueBuffer[i][n];
        }
        delete valueBuffer[i];
    }
    delete valueBuffer;
}


int TRetrievalImage::getWidth(){
    return width;
}

int TRetrievalImage::getHeight(){
    return height;
}

void TRetrievalImage::save(QString fn){
    QFile data(fn);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream datastream(&data);
        for (int i=0; i < height; i++){
            for(int n = 0; n< width; n++){
                //datastream << valueBuffer[i][n]->val << " "<<valueBuffer[i][n]->mirrorCoordinate->getAngleCoordinate().x()<<","<<valueBuffer[i][n]->mirrorCoordinate->getAngleCoordinate().y()<<"\t\t";
                datastream << valueBuffer[i][n]->val << "\t";
            }
            datastream << '\n';
        }
    }
}
