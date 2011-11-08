#include "tretrievalimage.h"
#include <QFile>
#include <QTextStream>

TRetrievalImage::TRetrievalImage(int width, int height)
{
    this->width = width;
    this->height = height;
    valueBuffer = new TRetrieval** [height];
    for (int i = 0; i < height; i++){
        valueBuffer[i] = new TRetrieval* [width];
        for(int n = 0; n<width;n++){
            valueBuffer[i][n] = new TRetrieval();
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
