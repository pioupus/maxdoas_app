#include "tretrievalimage.h"
#include <QFile>
#include <QTextStream>
#include <tspectrumplotter.h>
#include "vectorsolverservice.h"
#include "temissionrate.h"
#include <Eigen/Dense>
#include <Eigen/Sparse>

TRetrievalImage::TRetrievalImage(int width, int height)
{
    inibuffer(width,height,(TRetrievalImage*)NULL);
    emissionFactor = 1;
    dof_x = -1;
    dof_y = -1;
    dof_src = -1;
}

TRetrievalImage::TRetrievalImage(TRetrievalImage *other){
    inibuffer(other->width,other->height,other);
    datetime = other->datetime;
    timeDiff = other->timeDiff;
    meanDistance = other->meanDistance;
    emissionFactor = other->emissionFactor;
    dof_x = other->dof_x;
    dof_y = other->dof_y;
    dof_src = other->dof_src;
}


TRetrievalImage::TRetrievalImage(QString fn,QString fmt,float PixelWidthAngle,float PixelHeightAngle){
    emissionFactor = 1;
    dof_x = -1;
    dof_y = -1;
    dof_src = -1;
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

                QPointF ac(xPos,((float)(rows-1)*PixelHeightAngle)- yPos);
                if(row == rows-1)
                    ac.setY(0);
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
                valueBuffer[rows-row-1][col-StartCol]->origval = val;
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

void TRetrievalImage::setMeanDistance(float Distance){//in meter
    meanDistance = Distance;
}

void TRetrievalImage::setTimeDiff(float sec){
    timeDiff = sec;
}

void TRetrievalImage::setEmissionFactor(float ef){
    emissionFactor = ef;
}


QPointF TRetrievalImage::coordinateInMeters(QPointF angle){
    angle = M_PI*angle/180.0;
    angle.setX(sin(angle.x())*meanDistance);
    angle.setY(sin(angle.y())*meanDistance);
    return angle;
}

QPointF TRetrievalImage::getCoordinateInMeters(int col,int row){
    QPointF coordinate = valueBuffer[row][col]->mirrorCoordinate->getAngleCoordinate();
    return coordinateInMeters(coordinate);
}


void TRetrievalImage::loadWeightsMatrix(Eigen::MatrixXd& weights){
    for (int i = 0; i < height; i++){
        for(int n = 0; n<width;n++){
            valueBuffer[i][n]->weight = weights(i,n);
        }
    }
}

void TRetrievalImage::loadWeights(TRetrievalImage* weights){
    for (int i = 0; i < height; i++){
        for(int n = 0; n<width;n++){
            valueBuffer[i][n]->weight = weights->valueBuffer[i][n]->val;
        }
    }
}

void TRetrievalImage::loadWeightSIGIS(QString fn){
    TRetrievalImage* weights = new TRetrievalImage(fn, "SIGIS",1,1);
    loadWeights(weights);
    delete weights;
}

void TRetrievalImage::subMatrix(MatrixXd& sub,bool sign){
    for (int i = 0; i < height; i++){
        for(int n = 0; n<width;n++){
            if (sign){
                valueBuffer[i][n]->val = valueBuffer[i][n]->val- sub(i,n);
                valueBuffer[i][n]->origval = valueBuffer[i][n]->origval - sub(i,n);
            }else{
                valueBuffer[i][n]->val = sub(i,n) - valueBuffer[i][n]->val;
                valueBuffer[i][n]->origval = sub(i,n) - valueBuffer[i][n]->origval;
            }
        }
    }
}

int TRetrievalImage::getWidth(){
    return width;
}

int TRetrievalImage::getHeight(){
    return height;
}

void TRetrievalImage::save(QString fn,bool PosInfo, bool windvector){
    QFile data(fn);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream datastream(&data);
        QString s;
        if (PosInfo)
            s = "PosDegX;PosDegY;";
        if (windvector)
            s += "WindX[m/s];WindX[m/s];";
        s +=  "value\n";
        datastream << s;
        datastream << "distance:\n"+ QString::number(meanDistance)+'\n';
        datastream << "dof x y src:\n"+
                    QString::number(dof_x) + ";"+
                    QString::number(dof_y) + ";"+
                    QString::number(dof_src)+'\n';
        datastream << "date:\n"+ datetime.toString("yyyy.MM.dd hh:mm:ss:zzz")+'\n';
        datastream << "fields(width,height):\n"+ QString::number(width)+'\n'+ QString::number(height)+'\n';
        for (int i=0; i < height; i++){
            for(int n = 0; n< width; n++){
                //datastream << valueBuffer[i][n]->val << " "<<valueBuffer[i][n]->mirrorCoordinate->getAngleCoordinate().x()<<","<<valueBuffer[i][n]->mirrorCoordinate->getAngleCoordinate().y()<<"\t\t";
                QString s="";
                if (PosInfo)
                    s = QString::number(valueBuffer[i][n]->mirrorCoordinate->getAngleCoordinate().x()) + ";" +
                        QString::number(valueBuffer[i][n]->mirrorCoordinate->getAngleCoordinate().y())+";";
                if (windvector)
                    s += QString::number(valueBuffer[i][n]->getWindVector().x()) + ";" +
                         QString::number(valueBuffer[i][n]->getWindVector().y())+";";
                s +=  QString::number(valueBuffer[i][n]->val);
                datastream << s << "\t";
            }
            datastream << '\n';
        }
    }
}

float TRetrievalImage::getMinVal(){
    float result = 0;
    for (int i=0; i < height; i++){
        for(int n = 0; n< width; n++){

            if ((valueBuffer[i][n]->val <  result) || (n+i==0))
                result = valueBuffer[i][n]->val;
        }
    }
    return result;
}

float TRetrievalImage::getMaxVal(){
    float result = 0;
    for (int i=0; i < height; i++){
        for(int n = 0; n< width; n++){

            if ((valueBuffer[i][n]->val >  result) || (n+i==0))
                result = valueBuffer[i][n]->val;
        }
    }
    return result;
}



void TRetrievalImage::plot(int plotIndex, int Pixelsize){
    TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(0);
    SpectrumPlotter->plotRetrievalImage(this,plotIndex,Pixelsize);
}

void TRetrievalImage::oplotWindField(int plotIndex, int Average, bool normalize,bool excludezero){
    TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(0);
    SpectrumPlotter->plotVectorField(this,plotIndex,Average,normalize,excludezero);
}

void TRetrievalImage::scaleThresholdImageValues(float threshold){
    for (int i=0; i < height; i++){
        for(int n = 0; n< width; n++){
            double val = valueBuffer[i][n]->origval;
            double weight = valueBuffer[i][n]->weight;
            weight -= threshold;
            weight /= (1-threshold);
            if (weight < 0)
                weight = 0;
            valueBuffer[i][n]->val = val*weight;

        }
    }
}

void TRetrievalImage::thresholdImageValues(float threshold){
    for (int i=0; i < height; i++){
        for(int n = 0; n< width; n++){
            if (valueBuffer[i][n]->weight < threshold)
                valueBuffer[i][n]->val = 0;
            else
                valueBuffer[i][n]->val = valueBuffer[i][n]->origval;
        }
    }
}


QPointF TRetrievalImage::getMeanVec(void){
    QPointF result = QPointF(0,0);
    float sum=0;
    for(int row=0;row<getHeight();row++){
        for (int col = 0; col<getWidth();col++){
            result += valueBuffer[row][col]->getWindVector()*valueBuffer[row][col]->val;
            sum += valueBuffer[row][col]->val;
        }
    }
    result /= sum;
    //result /= getHeight()*getWidth();
    return result;
}

QPointF TRetrievalImage::getMaxVec(){
    QPointF result;
    float maxVal;
    int maxIndexcol;
    int maxIndexrow;

        for(int row=0;row<getHeight();row++){
            for (int col = 0; col<getWidth();col++){
                float val;
                result = valueBuffer[row][col]->getWindVector();
                val = get2Norm(result);
                if ((val > maxVal)||(col+row==0)){
                    maxVal = val;
                    maxIndexcol = col;
                    maxIndexrow = row;
                }
            }

    }
    result = valueBuffer[maxIndexrow][maxIndexcol]->getWindVector();
    return result;
}

QPoint TRetrievalImage::getClosesPoint(QPointF coor){
    QPoint result;
    float minval;

    for(int row=0;row<getHeight();row++){
        for (int col = 0; col<getWidth();col++){
            QPointF p;
            float val;
            p = valueBuffer[row][col]->mirrorCoordinate->getAngleCoordinate();
            p -= coor;
            val = get2Norm(p);
            if ((val < minval)||(col+row==0)){
                minval = val;
                result.setX(col);
                result.setY(row);
            }
        }
    }
    return result;
}

float TRetrievalImage::getMaxVelocity(){
    QPointF vec = getMaxVec();
    return sqrt(vec.x()*vec.x()+vec.y()*vec.y());
}

float TRetrievalImage::getMeanVelocity(){
    QPointF vec = getMeanVec();
    return sqrt(vec.x()*vec.x()+vec.y()*vec.y());
}

int TRetrievalImage::getTime(){
    return datetime.toTime_t();
}

void TRetrievalImage::setSpeedCorrection(float correctionfactor){
    for(int row=0;row<getHeight();row++){
        for (int col = 0; col<getWidth();col++){
            QPointF p;
            p = valueBuffer[row][col]->getWindVector()*correctionfactor;
            valueBuffer[row][col]->setWindVector(p);
        }
    }
}

QScriptValue TRetrievalImage::emissionrate(float TimeStep){
    TEmissionrate* result = new TEmissionrate();
    result->setMeanDistance(meanDistance);
    result->setEmissionFactor(emissionFactor);
    result->setPixelCounts(QPoint(width,height));
    float ScanPixelsize;
    TSpectrumPlotter* sp = TSpectrumPlotter::instance(0);
    float maxVal;
    QPointF maxPoint;
    QRectF Plotrect;
    QPointF nextPoint;
    QPointF meanPixelWidth;
    QPointF meanVec = getMeanVec();//in m/s;
    meanVec.setX(180*asin(meanVec.x()/meanDistance)/M_PI);//now in deg/sec
    meanVec.setY(180*asin(meanVec.y()/meanDistance)/M_PI);
    for(int row=0;row<getHeight();row++){
        for (int col = 0; col<getWidth();col++){
            float val;
            val = valueBuffer[row][col]->val;
            QPointF ac = valueBuffer[row][col]->mirrorCoordinate->getAngleCoordinate();
            if ((val > maxVal)||(col+row==0)){
                maxVal = val;
                //maxPoint.setX(ac);
                //maxPoint.setY(row);
                maxPoint = ac;
            }
            if ((ac.x() < Plotrect.left()) ||(col+row==0)){
                Plotrect.setLeft(ac.x());
            }
            if ((ac.x() > Plotrect.right()) ||(col+row==0)){
                Plotrect.setRight(ac.x());
            }
            if ((ac.y() < Plotrect.top()) ||(col+row==0)){
                Plotrect.setTop(ac.y());
            }
            if ((ac.y() > Plotrect.bottom()) ||(col+row==0)){
                Plotrect.setBottom(ac.y());
            }
        }
    }

    meanPixelWidth = Plotrect.topRight()-Plotrect.bottomLeft();
    meanPixelWidth.setX(fabs(meanPixelWidth.x()/(getWidth()-1)));
    meanPixelWidth.setY(fabs(meanPixelWidth.y()/(getHeight()-1)));
    float absmeanPixelWidth = get2Norm(meanPixelWidth);
    result->setImageSize(Plotrect);
    float emissionrate=0;
    TParamLine Windvector;
    Windvector.iniDiff(maxPoint,meanVec);

    float EmissionTime = 0;
    float LineParam = 0;
    sp->clearMarker(0);
    sp->plotXYMarker(maxPoint.x(),maxPoint.y(),"",0);
    QPointF PlotFrameCollision = Windvector.getRectCollisionPoint(Plotrect,true);
    //sp->plotXYMarker(PlotFrameCollision.x(),PlotFrameCollision.y(),"",0);
    float maxlength = get2Norm(maxPoint-PlotFrameCollision);
    float WindMeanlength = get2Norm(meanVec);
    float maxsteps = maxlength/WindMeanlength;
    int index=0;

//    if (fabs(meanVec.x()) > fabs(meanVec.y())){//FIXME! does this really work?
//        float meanrad = getVecAngle(meanVec);
//        meanrad = M_PI*meanrad/180.0;
//        ScanPixelsize = coordinateInMeters(meanPixelWidth).y();
//        ScanPixelsize = ScanPixelsize/sin(meanrad);
//    }else{
//        float meanrad = getVecAngle(meanVec);
//        meanrad = M_PI*meanrad/180.0;
//        ScanPixelsize = coordinateInMeters(meanPixelWidth).x();
//        ScanPixelsize = ScanPixelsize/cos(meanrad);
//    }
//
//    ScanPixelsize = fabs(ScanPixelsize);
//    if(ScanPixelsize > 83){
//        ScanPixelsize++;
//    }
    do{
        nextPoint = Windvector.getPointbyParam(LineParam);

        LineParam -= TimeStep;///ScanPixelsize;//Here we should take the real Pixeldistances into account. aswell with its projections
        EmissionTime -= TimeStep;
        TParamLine* corridor = Windvector.getOrthoLine(nextPoint);
        emissionrate = selectAndIntegrateCorridor(this,*corridor,meanVec,absmeanPixelWidth,0,meanDistance)*emissionFactor;

        result->prependEmmision(EmissionTime,emissionrate,corridor,coordinateInMeters(nextPoint));
        QPoint linep = getClosesPoint(nextPoint);
        if (valueBuffer[linep.y()][linep.x()]->val < maxVal/50.0)
            break;
        delete corridor;
        index++;
    }while(emissionrate>0&&(-LineParam<maxsteps)&&(result->count() < 500));
    //sp->plotXYMarker(nextPoint.x(),nextPoint.y(),"",0);


    //result->setMainDirectionLine(maxPoint,meanVec,Plotrect);
    result->setMainDirectionLine(nextPoint,meanVec,Plotrect);
    result->setTimeOffset(-EmissionTime);
    EmissionTime = -EmissionTime;
    LineParam = 0;
    PlotFrameCollision = Windvector.getRectCollisionPoint(Plotrect,false);
    maxlength = get2Norm(maxPoint-PlotFrameCollision);
    WindMeanlength = get2Norm(meanVec);
    maxsteps = maxlength/WindMeanlength;
    do{
        nextPoint = Windvector.getPointbyParam(LineParam);
        //sp->plotXYMarker(nextPoint.x(),nextPoint.y(),"",0);
        TParamLine* corridor = Windvector.getOrthoLine(nextPoint);
        //emissionrate = selectAndIntegrateCorridor(LastRetrieval,*corridor,meanVec,0.5);
        result->AddEmmision(EmissionTime,0,corridor,coordinateInMeters(nextPoint));
        EmissionTime += TimeStep;

        LineParam += TimeStep;///ScanPixelsize;//Here we should take the real Pixeldistances into account. aswell with its projections


        delete corridor;
        index++;
    }while((LineParam<maxsteps)&&(result->count() < 500));

    result->calcEmission(this);
    result->setTimeDiff(timeDiff);

    return engine()->newQObject(result);
}

void TRetrievalImage::mapWindVektors(TRetrievalImage* windvektor){
    for(int c=0;c<width;c++){
        for(int r=0;r<height;r++){
            valueBuffer[r][c]->setWindVector(windvektor->valueBuffer[r][c]->getWindVector());
        }
    }
}

void TRetrievalImage::setDOFs(float dof_x,float dof_y,float dof_src){
    this->dof_x = dof_x;
    this->dof_y = dof_y;
    this->dof_src = dof_src;
}


