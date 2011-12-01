#include "tretrievalimage.h"
#include <QFile>
#include <QTextStream>
#include <tspectrumplotter.h>


TRetrievalImage::TRetrievalImage(int width, int height)
{
    inibuffer(width,height,(TRetrievalImage*)NULL);
}

TRetrievalImage::TRetrievalImage(TRetrievalImage *other){
    inibuffer(other->width,other->height,other);
    datetime = other->datetime;
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

TRetrievalImage::TRetrievalImage(QString fn,QString fmt,float PixelWidthAngle,float PixelHeightAngle){
    bool result = true;
    QFile data(fn);
    QString s = fn.left(fn.indexOf(")"));
    int StartCol=0;
    if (fmt == "SIGIS")
        StartCol = 1;
    s = s.section("(",1,1);
    datetime = QDateTime::fromString(s,"yyyy_MM_dd_hh_mm_ss_zzz");
    if (data.open(QFile::ReadOnly)) {
        QTextStream datastream(&data);
        int rows = 0;
        int cols = 0;
        int oldcols = 0;
        while(!datastream.atEnd()){
            QString line = datastream.readLine();
            QStringList colstrings = line.split(QRegExp("\\s+"),QString::SkipEmptyParts);//match with spaces and tabs
            rows++;
            cols = colstrings.count();
            if ((oldcols !=  cols)&&(rows != 1)){
                result = false;
                logger()->warn(QString("SIGIS Image %1 has different colsizes").arg(fn));
                break;
            }
            oldcols = cols;
        }
        inibuffer(cols-StartCol,rows,(TRetrievalImage*)NULL);
        datastream.seek(0);
        float yPos=0;
        for (int row=0; row < rows; row++){
            float xPos=0;
            if(!result)
                break;
            QString line = datastream.readLine();
            QStringList colstrings = line.split(QRegExp("\\s+"),QString::SkipEmptyParts);//match with spaces and tabs
            for(int col = StartCol; col< cols; col++){

                QPointF ac(xPos,((float)rows*PixelHeightAngle)- yPos);
                TMirrorCoordinate *mc = new TMirrorCoordinate(ac);
                xPos += PixelWidthAngle;
                QString word = colstrings[col];
                double val;
                bool ok;
                val = word.toDouble(&ok);
                if (!ok){
                    logger()->warn(QString("SIGIS Image %1 cant convert string to double(%2)").arg(fn).arg(word));
                    result = false;
                    break;
                }
                valueBuffer[rows-row-1][col-StartCol]->val = val;
                valueBuffer[rows-row-1][col-StartCol]->setMirrorCoordinate(mc);
                delete mc;
            }
            yPos += PixelHeightAngle;
        }
        data.close();
    }else{
        logger()->warn(QString("SIGIS Image %1 doesnt exist").arg(fn));
        result = false;
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



void TRetrievalImage::plot(int plotIndex, int Pixelsize){
    TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(0);
    SpectrumPlotter->plotRetrievalImage(this,plotIndex,Pixelsize);
}

void TRetrievalImage::oplotWindField(int plotIndex, int Average, bool normalize,bool excludezero){
    TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(0);
    SpectrumPlotter->plotVectorField(this,plotIndex,Average,normalize,excludezero);
}
