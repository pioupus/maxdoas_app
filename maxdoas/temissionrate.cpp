#include "temissionrate.h"
#include "tspectrumplotter.h"
#include "vectorsolverservice.h"

TEmissionrate::TEmissionrate()
{
    corridorpainted = false;
    emissionFactor = 1;
    PixelCounts = QPoint(0,0);
    maxCorridorLengthEnd = 0.9;
    maxCorridorLengthStart = 0.9;
    corridornumberlimit = 0.9;
    corridornumberStart = 0.25;
}

TEmissionrate::~TEmissionrate(){
    for(int i=0;i<count();i++){
        tEmissionentry* ee=new(tEmissionentry);
        ee = EmissionList[i];
        delete ee;
    }
    EmissionList.clear();
}

void TEmissionrate::setMaxCorridorLength(float start, float end){
    maxCorridorLengthEnd = end;
    maxCorridorLengthStart = start;
}

void TEmissionrate::setCorridorNumberLimit(float start, float end){
    corridornumberlimit = end;
    corridornumberStart = start;
}


void TEmissionrate::AddEmmision(float time, float emission, TParamLine* corridor, QPointF PositionInMeters){
    tEmissionentry* ee=new(tEmissionentry);
    ee->CorridorDirection = corridor->getDiffVec();
    ee->CorridorPoint = corridor->getOffset();
    ee->Emission = emission;
    ee->time = time;
    ee->PrevEmission = 0;
    ee->NextEmission = 0;
    ee->PrevEmissionForCorrelation = -20000;
    ee->NextEmissionForCorrelation = -20000;
    ee->correlation = 0;
    ee->CorridorPointMeters = PositionInMeters;
    QPointF FrameCollision1 = corridor->getRectCollisionPoint(ImageSize,true);
    QPointF FrameCollision2 = corridor->getRectCollisionPoint(ImageSize,false);
    FrameCollision1 = FrameCollision2-FrameCollision1;
    ee->CorridorLength = get2Norm(FrameCollision1);
    EmissionList.append(ee);
}

void TEmissionrate::prependEmmision(float time, float emission, TParamLine* corridor, QPointF PositionInMeters){
    tEmissionentry* ee=new(tEmissionentry);
    ee->CorridorDirection = corridor->getDiffVec();
    ee->CorridorPoint = corridor->getOffset();
    ee->Emission = emission;
    ee->time = time;
    ee->PrevEmission = 0;
    ee->NextEmission = 0;
    ee->PrevEmissionForCorrelation = -20000;
    ee->NextEmissionForCorrelation = -20000;
    ee->correlation = 0;
    ee->CorridorPointMeters = PositionInMeters;
    QPointF FrameCollision1 = corridor->getRectCollisionPoint(ImageSize,true);
    QPointF FrameCollision2 = corridor->getRectCollisionPoint(ImageSize,false);
    FrameCollision1 = FrameCollision2-FrameCollision1;
    ee->CorridorLength = get2Norm(FrameCollision1);
    EmissionList.prepend(ee);
}

void TEmissionrate::save(QString fn){
    QFile data(fn);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream datastream(&data);
        datastream << "distance[m],timediff[sec]:" <<'\n';
        datastream << QString::number(meanDistance)+" "+
                      QString::number(timeDiff) <<'\n';
        datastream << "frame(left,top,right,bottom):" <<'\n';
        datastream << QString::number(ImageSize.left())+" "
                   << QString::number(ImageSize.top())+" "
                   << QString::number(ImageSize.right())+" "
                   << QString::number(ImageSize.bottom()) <<'\n';


        datastream << "Flowdirection(Point1X,Point1Y,Point2X,Point2Y):" <<'\n';
        datastream << QString::number(MainDirectionLineP1.x())+" "
                   << QString::number(MainDirectionLineP1.y())+" "
                   << QString::number(MainDirectionLineP2.x())+" "
                   << QString::number(MainDirectionLineP2.y()) <<'\n';

        datastream << "correlationFitLine(Prev_a0,Prev_a1,Next_a0,Next_a1):" <<'\n';
        datastream << QString::number(correlationLineFitParameterPrev_a0)+" "
                   << QString::number(correlationLineFitParameterPrev_a1)+" "
                   << QString::number(correlationLineFitParameterNext_a0)+" "
                   << QString::number(correlationLineFitParameterNext_a1) <<'\n';

        datastream << "Corridor(Point1X,Point1Y,Point2X,Point2Y,length,time,Emission,PrevEmission,NextEmission,CorrPrevEmission,CorrNextEmission):" <<'\n';
        for(int i=0;i<count();i++){
            tEmissionentry *ee = EmissionList[i];
            TParamLine l;
            l.iniDiff(ee->CorridorPoint,ee->CorridorDirection);
            QPointF p1 = l.getRectCollisionPoint(ImageSize,true);
            QPointF p2 = l.getRectCollisionPoint(ImageSize,false);
            datastream  << QString::number(p1.x())+" "
                        << QString::number(p1.y())+" "
                        << QString::number(p2.x())+" "
                        << QString::number(p2.y())+" "
                        << QString::number(ee->CorridorLength)+" "
                        << QString::number(ee->time)+" "
                        << QString::number(ee->Emission)+" "
                        << QString::number(ee->PrevEmission)+" "
                        << QString::number(ee->NextEmission)+" "
                        << QString::number(ee->PrevEmissionForCorrelation)+" "
                        << QString::number(ee->NextEmissionForCorrelation) <<'\n';
        }
        datastream << "Correlation(lag[sec],corrVal):" <<'\n';
        for(int i=0;i<CorrelationList.count();i++){
            QPointF p = CorrelationList[i];
            datastream  << QString::number(p.x())+" "
                        << QString::number(p.y()) <<'\n';
        }
    }
}

void TEmissionrate::setTimeOffset(float TimeOffset){
    for(int i = 0;i<EmissionList.count();i++){
        EmissionList[i]->time += TimeOffset;
    }
}

void TEmissionrate::setMainDirectionLine(QPointF startpoint, QPointF direction,QRectF PlotRegion){
    TParamLine WindMean;
    WindMean.iniDiff(startpoint,direction);
    MainDirectionLineP1 = startpoint;
    //MainDirectionLineP1 = WindMean.getRectCollisionPoint(PlotRegion,true);
    MainDirectionLineP2 = WindMean.getRectCollisionPoint(PlotRegion,false);
}

int TEmissionrate::count(){
    return EmissionList.count();
}

int TEmissionrate::countCorrelation(){
    return CorrelationList.count();
}

tEmissionentry* TEmissionrate::EmissionAt(int index){
    return EmissionList.at(index);
}

QPointF TEmissionrate::CorrelationAt(int index){
    return CorrelationList[index];
}

void TEmissionrate::setEmissionFactor(float ef){
    emissionFactor = ef;
}

void TEmissionrate::oplotEmissionRaster(int plotindex){

    TSpectrumPlotter* sp = TSpectrumPlotter::instance(0);
    sp->oplotEmissionRaster(this,plotindex);
}

void TEmissionrate::plot(int plotindex){
    TSpectrumPlotter* sp = TSpectrumPlotter::instance(0);
    sp->plotEmission(this,plotindex);
}

void TEmissionrate::plotCorrelation(int plotindex){
    TSpectrumPlotter* sp = TSpectrumPlotter::instance(0);
    sp->plotCorrelation(this,plotindex);
}

void TEmissionrate::oplot(int plotindex){
    TSpectrumPlotter* sp = TSpectrumPlotter::instance(0);
    sp->oplotEmission(this,plotindex);
}

void TEmissionrate::setImageSize(QRectF rect){
    ImageSize = rect;
}

QRectF TEmissionrate::getImageSize(){
    return ImageSize;
}

void TEmissionrate::setPixelCounts(QPoint pc){
    PixelCounts = pc;
}

void TEmissionrate::calcEmission_(TRetrievalImage* retimage, bool Prev,bool Next){
    QPointF MeanWindDirection = MainDirectionLineP2-MainDirectionLineP1;
    float maxlength;
    for(int i=0;i<count();i++){
        tEmissionentry *ee = EmissionList[i];
        if(maxlength < ee->CorridorLength || i==0)
            maxlength = ee->CorridorLength;
    }
    bool start=false;
    bool end=false;
    QPointF PixelSize = ImageSize.bottomLeft()-ImageSize.topRight();
    PixelSize.setX(PixelSize.x()/(PixelCounts.x()-1));
    PixelSize.setY(PixelSize.y()/(PixelCounts.y()-1));
    float meanPixellength=get2Norm(PixelSize);
    for(int i=0;i<count();i++){
        float val;
        tEmissionentry *ee = EmissionList[i];


//        if(!start && ee->CorridorLength > maxlength*0.9)
//
//
//            //if (i > count()*0.25)
//            if (i > count()*0.40)
//                start = true;
//            //start = true;
////        if(start && ((ee->CorridorLength < maxlength*0.9) || (i > count()*0.9))){
//        if(start && ((ee->CorridorLength < maxlength*0.75) || (i > count()))){
//            end = true;
//        }



        if(!start && ee->CorridorLength > maxlength*maxCorridorLengthStart)
            if (i > count()*corridornumberStart)
                start = true;
        if(start && ((ee->CorridorLength < maxlength*maxCorridorLengthEnd) || (i > count()*corridornumberlimit))){
            end = true;
        }
        TParamLine *corridor = new TParamLine();
        corridor->iniDiff(ee->CorridorPoint,ee->CorridorDirection);
        int n=i;
        if (corridorpainted)
            n=2;
        if(n==1)
            corridorpainted = true;
        val = selectAndIntegrateCorridor(retimage, *corridor, MeanWindDirection, meanPixellength,n,meanDistance)*emissionFactor;
        if(start && !end){
        }else{
            val = -1;
        }
        ee->Emission = val;
        if (Prev)
            ee->PrevEmission = val;
        if (Next)
            ee->NextEmission = val;
        delete corridor;
    }
}

void TEmissionrate::setMeanDistance(float distance){
    meanDistance = distance;
}

void TEmissionrate::calcEmission(TRetrievalImage* retimage){
    calcEmission_(retimage,false,false);
}

void TEmissionrate::calcEmissionPrev(TRetrievalImage* retimage){
    calcEmission_(retimage,true,false);
}

void TEmissionrate::calcEmissionNext(TRetrievalImage* retimage){
    calcEmission_(retimage,false,true);
}


QVector<float> smooth( QVector<float>& I,  int size )
{
    QVector<float> result(I.count());

    if (size == 1){
        return I;
    }
    int kernelRow = size/2;

    int limitRow = I.count()-kernelRow;
    double norm = size;
    for (int row = 0; row < I.count(); row++ ){
        if ((row >= kernelRow) && (row < limitRow)){
            double b = 0;
            for(int n=row-kernelRow;n<row-kernelRow+size;n++){
                b += I[n];
            }
            result[row] = b/norm;
        }else{
            int rowoffsetu = kernelRow - row;
            int rowoffsetb = row+1 - limitRow;

            if (rowoffsetu < 0) rowoffsetu = 0;
            if (rowoffsetb < 0) rowoffsetb = 0;

            int krows_sub =  size - (rowoffsetu + rowoffsetb);

            int mrow_start =  row - kernelRow;

            if (mrow_start < 0) mrow_start = 0;

      //      MatrixXd kernel_sub = kernel.block(rowoffsetu,coloffsetl,krows_sub,kcols_sub );
            //std::cout << "kernel_sub:" << std::endl;
            //std::cout << kernel_sub << std::endl;
            double norm_sub = krows_sub;
            double b=0;
            for(int n = mrow_start;n<mrow_start+krows_sub;n++){
                b += I[n];
            }
            result[row] = b/norm_sub;
        }
    }
   return result;
}



void TEmissionrate::calcCorrelation(){
    CorrelationList.clear();
    bool started = false;
    bool ended = false;
    QVector<float> XVec;
    QVector<float> YVec;
    QVector<float> TimeVec;
    const int interpolationfactor = 10;
    float txAvg = 0; float tyAvg = 0;
    float a1XNum = 0;   float a1YNum = 0;
    float a1XDeNum = 0;   float a1YDeNum = 0;
    int index = 0;   int startindex = 0;


    for(int i=0;i<count();i++){ //create Data vectors corresponding to its start and stop conditions(like integration corridor is too short due to hitting the frame border)
        tEmissionentry *ee_inner = EmissionList[index];
        float XVal = ee_inner->PrevEmission;
        float YVal = ee_inner->NextEmission;
        index++;
        if ((XVal != -1) && (YVal != -1) && (started == false)){
            //if (i > EmissionList.count()/4){
                started = true;
                startindex = i;
            //}
        }
        if (((XVal == -1) || (YVal == -1)) && (started)){
            ended = true;
        }
        if (started && !ended){
            XVec.append(XVal);
            YVec.append(YVal);
            TimeVec.append(ee_inner->time);

            txAvg += ee_inner->time;
        }
    }
    int XVecCount = XVec.count();
    XVec = smooth(XVec,7);
    YVec = smooth(YVec,7);
    float XAvg = 0;  float YAvg = 0;
    for(int i=0;i<XVecCount;i++){
        XAvg += XVec[i];
        YAvg += YVec[i];
    }

    XAvg /= (float)XVecCount;
    YAvg /= (float)XVecCount;
    txAvg /= XVecCount;
    tyAvg = txAvg;
    for(int i=0;i<XVecCount-1;i++){     //interpolate Vectors for having finer resolution in correlation
        float v1=XVec[i*interpolationfactor];float v2=XVec[i*interpolationfactor+1];
        float Xdiff = v2-v1;
        float Ydiff = YVec[i*interpolationfactor+1]-YVec[i*interpolationfactor];
        float tDiff = TimeVec[i*interpolationfactor+1]-TimeVec[i*interpolationfactor];
        Xdiff /= interpolationfactor;
        Ydiff /= interpolationfactor;
        tDiff /= tDiff;
        for(int n=1;n<interpolationfactor;n++){
            float v1=Xdiff*(float)n;
            float v2 = XVec[i*interpolationfactor];
            v1 += v2;
            XVec.insert(i*interpolationfactor+n,v1);
            YVec.insert(i*interpolationfactor+n,Ydiff*(float)n+YVec[i*interpolationfactor]);
            TimeVec.insert(i*interpolationfactor+n,tDiff*(float)n+TimeVec[i*interpolationfactor]);
        }
    }
    XVecCount = XVec.count();

    for(int i=0;i<XVecCount;i++){           //get line fit parameters:
        a1XNum += ((XVec[i] - XAvg)*((float)TimeVec[i]-txAvg));
        a1YNum += ((YVec[i] - YAvg)*((float)TimeVec[i]-tyAvg));
        a1XDeNum += ((float)TimeVec[i]-txAvg)*((float)TimeVec[i]-txAvg);
        a1YDeNum += ((float)TimeVec[i]-tyAvg)*((float)TimeVec[i]-tyAvg);
    }
    correlationLineFitParameterPrev_a1 = a1XNum/a1XDeNum;                                   correlationLineFitParameterNext_a1 = a1YNum/a1YDeNum;
    correlationLineFitParameterPrev_a0 = XAvg - correlationLineFitParameterPrev_a1*txAvg;   correlationLineFitParameterNext_a0 = YAvg - correlationLineFitParameterNext_a1*tyAvg;

    XAvg = 0;
    YAvg = 0;

    for(int i=0;i<XVecCount;i++){           //substract fitted line from data and recalculation of avg
        int n = (i/interpolationfactor)+startindex;
        tEmissionentry *ee_inner = EmissionList[n];

        XVec[i] = XVec[i] - (correlationLineFitParameterPrev_a0 + correlationLineFitParameterPrev_a1*TimeVec[i]);
        YVec[i] = YVec[i] - (correlationLineFitParameterNext_a0 + correlationLineFitParameterNext_a1*TimeVec[i]);
        XAvg += XVec[i];
        YAvg += YVec[i];

        ee_inner->PrevEmissionForCorrelation = XVec[i];
        ee_inner->NextEmissionForCorrelation = YVec[i];
    }

    XAvg /= (float)XVecCount;
    YAvg /= (float)XVecCount;

#if 1
    float denumX = 0; float denumY = 0;
    for(int i=0;i<XVecCount;i++){           //get correlation parameters
        denumX += (XVec[i]-XAvg)*(XVec[i]-XAvg);
        denumY += (YVec[i]-YAvg)*(YVec[i]-YAvg);
    }
    float denum = denumX*denumY;
    denum = sqrt(denum);

    for(int offset=-XVecCount/2;offset<XVecCount/2;offset++){ //do correlation with its lag
        int o = fabs(offset);
        float num = 0;
        float lag = 0;
        for(int i=0;i<XVecCount-o;i++){

            if (offset < 0){
                num += (XVec[i+o]-XAvg)*(YVec[i]-YAvg);
                lag += (TimeVec[i+o] - TimeVec[i]);
            }else{
                num += (XVec[i]-XAvg)*(YVec[i+o]-YAvg);
                lag += (TimeVec[i+o] - TimeVec[i]);
            }
        }

        lag = lag/(float)(XVecCount-o);
        if (offset < 0)
            lag = -lag;
        num = num/denum;
        CorrelationList.append(QPointF(lag,num));
    }
    #endif
}

float TEmissionrate::getMaxCorrelationPos(){
    QPointF maxCorr;
    for(int i=0;i<CorrelationList.count();i++){
        QPointF p = CorrelationList[i];
        if((p.y() > maxCorr.y()) || (i == 0)){
            maxCorr = p;
        }
    }
    return maxCorr.x();
}

float TEmissionrate::getMaxCorrelationVal(){
    QPointF maxCorr;
    for(int i=0;i<CorrelationList.count();i++){
        QPointF p = CorrelationList[i];
        if((p.y() > maxCorr.y()) || (i == 0)){
            maxCorr = p;
        }
    }
    return maxCorr.y();
}

void TEmissionrate::setTimeDiff(float dt){
    timeDiff = dt;
}

float TEmissionrate::getCorrelationCorrectionfactor(){
    float x = getMaxCorrelationPos();

    return x/timeDiff;
}
