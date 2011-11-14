#include "tspectralimage.h"
#include "tscanpath.h"
#include <QHashIterator>
#include <QFile>
#include <QFileInfo>
#include <QScriptEngine>
#include <QScriptContext>
#include <QScriptValue>
#include <tspectrumplotter.h>
#include <math.h>


TSpectralImage::TSpectralImage(TScanPath *parent) :
    QObject(parent)
{
  //  FirstDate = 0;
    meanSpectrum = NULL;
    rmsSpectrum = NULL;
    stdDevSpectrum = NULL;
    maxRMSPos = NULL;
    fn = "";
    if (parent != NULL){
        QList<TPatternType*> ps = parent->getPatternSources();
        TPatternType *pt;
        for(int i=0;i<ps.count();i++){
           pt = new TPatternType(ps[i]);
           Patternsources.append(pt);
        }
    }
}

TSpectralImage::~TSpectralImage(){
    QHashIterator<QPair<int,int>, QPair<TSpectrum*,double> > i(spectrumtable);
    QPair<int,int> key;
    TSpectrum* spektrum;
    int c = spectrumtable.count();
    if(c==0)
        key.first = 1;
    while (i.hasNext()) {
        QPair<TSpectrum*,double> val;
        val = i.next().value();
        spektrum = val.first;
        delete spektrum;
    }
    spectrumtable.clear();
    spectrumlist.clear();
    TPatternType *pt;
    for(int i = 0;i<Patternsources.count();i++){
        pt = Patternsources.at(i);
        delete pt;
    }
    Patternsources.clear();
}

int TSpectralImage::getxCount(){
    TPatternType *pt;
    if (Patternsources.count() == 1){
        pt = Patternsources[0];
        return pt->divx;
    }else{
        return -1;
    }
}

int TSpectralImage::getyCount(){
    TPatternType *pt;
    if (Patternsources.count() == 1){
        pt = Patternsources[0];
        return pt->divy;
    }else{
        return -1;
    }
}

bool TSpectralImage::getPositionLine(QPointF P1, QPointF P2, QList<TMirrorCoordinate*> &Points, int div, QMap<float, TMirrorCoordinate*> &line, bool PermitRemovePoints){
    int i;
    bool result = false;
    TParamLine test = TParamLine(P1,P2);
    i=0;
    while (i < Points.count()){
        float key;
        bool ok;

        key = test.containsPoint(Points[i]->getAngleCoordinate(),ok);
        if (ok){
            result = true;
            line.insert(key, Points[i]);
            if (PermitRemovePoints)
                Points.removeAt(i);
            else
                i++;
            if (line.count() == div)
                break;
        }else{
            i++;
        }

    }
    return result;
}
QPointF makeRoundError(QPointF p,QList<TMirrorCoordinate*> PointList){
    TMirrorCoordinate mc;
    QPoint mocoord;
    QPointF result;
    float x_,y_;
    mc.setAngleCoordinate(p);
    mocoord = mc.getMotorCoordinate();
    mc.setMotorCoordinate(mocoord);
    x_ = mc.getAngleCoordinate().x();
    y_ = mc.getAngleCoordinate().y();
    for (int i = 0;i<PointList.count();i++){
        double x = PointList[i]->getAngleCoordinate().x();
        if ((x_-0.09 < x) && (x < x_+0.09)) {
            x_ = x;
            break;
        }
    }
    for (int i = 0;i<PointList.count();i++){

        double y = PointList[i]->getAngleCoordinate().y();
        if ((y_-0.09 < y) && (y < y_+0.09)) {
            y_ = y;
            break;
        }
    }
    result.setX(x_);
    result.setY(y_);
    return result;
}

bool TSpectralImage::getPositionArrayRect(TPatternType *pt, TRetrieval* **buffer, int cntX, int cntY){
    QList<TMirrorCoordinate*> PointList;
    QMap<float, TMirrorCoordinate*>  LineRight;
    QMap<float, TMirrorCoordinate*>  LineLeft;
    QMap<float, TMirrorCoordinate*>  LineHoriz;

    bool result = true;
    for (int i = 0;i<spectrumlist.count();i++){
        PointList.append(spectrumlist[i]->getMirrorCoordinate());
    }
    //        p[1]------p[3]
    //        |         |
    //        |         |
    //        p[2]------p[4]
    getPositionLine(pt->edge1, pt->edge2, PointList, pt->divy, LineRight,false);
    getPositionLine(pt->edge3, pt->edge4, PointList, pt->divy, LineLeft,false);

    if (LineRight.count() != pt->divy){
        //seems we have a bug that Points were in motorcoord and by hwdriver transfered into anglecoordinates again -> rounding errors
        logger()->debug("realigning Rect Pattern edges in file "+fn);
        pt->edge1 = makeRoundError(pt->edge1,PointList);
        pt->edge2 = makeRoundError(pt->edge2,PointList);
        pt->edge3 = makeRoundError(pt->edge3,PointList);
        pt->edge4 = makeRoundError(pt->edge4,PointList);
        getPositionLine(pt->edge1, pt->edge2, PointList, pt->divy, LineRight,false);
        getPositionLine(pt->edge3, pt->edge4, PointList, pt->divy, LineLeft,false);
        if (LineRight.count() != pt->divy){
            logger()->error("Could not realign Rect Pattern edges in file "+fn);
        }
    }
    if (LineLeft.count() != pt->divy){
        logger()->error("Could not realign Rect Pattern edges in file "+fn);
    }
    result = LineLeft.count()==LineRight.count();
    int i=0;

    QMapIterator<float, TMirrorCoordinate*> i_r(LineRight);
    QMapIterator<float, TMirrorCoordinate*> i_l(LineLeft);
    while (i_r.hasNext() && i_l.hasNext()) {
        TMirrorCoordinate* mc_l;
        TMirrorCoordinate* mc_r;
        i_l.next();
        i_r.next();
        mc_l = i_l.value();
        mc_r = i_r.value();
        LineHoriz.clear();
        getPositionLine(mc_l->getAngleCoordinate(), mc_r->getAngleCoordinate(), PointList, pt->divx, LineHoriz,true);
        if (LineHoriz.count() != pt->divx){
            logger()->error("Could not realign Rect Pattern edges in file "+fn);
        }
        QMapIterator<float, TMirrorCoordinate*> i_h(LineHoriz);
        i_h.toBack();
        int n=0;
        if (cntY > i){
            while (i_h.hasPrevious()){
                i_h.previous();
                if (cntX > n)
                    buffer[i][n]->setMirrorCoordinate(i_h.value());
                else
                    result = false;

                n++;
            }
            i++;
        }else
            result = false;

    }

    return result;

}

bool TSpectralImage::getPositionArrayLine(TPatternType *pt,  TRetrieval* **buffer, int cntX, int cntY){
    QList<TMirrorCoordinate*> PointList;
    QMap<float, TMirrorCoordinate*>  Line;

    bool result = true;
    for (int i = 0;i<spectrumlist.count();i++){
        PointList.append(spectrumlist[i]->getMirrorCoordinate());
    }

    getPositionLine(pt->edge1, pt->edge2, PointList, pt->divx, Line,true);
    if (Line.count() == 0 && pt->divx != 0){
        //seems we have a bug that Points were in motorcoord and by hwdriver transfered into anglecoordinates again -> rounding errors
        logger()->debug("realigning Line Pattern edges in file "+fn);
        pt->edge1 = makeRoundError(pt->edge1,PointList);
        pt->edge2 = makeRoundError(pt->edge2,PointList);

        getPositionLine(pt->edge1, pt->edge2, PointList, pt->divx, Line,true);
        if (Line.count() == 0 && pt->divx != 0){
            logger()->error("Could not realign Line Pattern edges in file "+fn);
        }
    }
    QMapIterator<float, TMirrorCoordinate*> i_l(Line);
    int n = 0;
    if(cntY > 0){
        while (i_l.hasNext()) {
            TMirrorCoordinate* mc_l;
            i_l.next();
            mc_l = i_l.value();
            if (cntX > n)
                buffer[0][n]->setMirrorCoordinate(mc_l);
            else
                result = false;

            n++;
        }
    }
    else
        result = false;
    return result;
}

bool TSpectralImage::getPositionArray( TRetrieval* **buffer, int cntX, int cntY){
    TPatternType *pt;
    bool result = false;
    if (Patternsources.count() == 1){
        pt = Patternsources[0];
        switch (pt->Patternstyle){
            case psRect:
                result = getPositionArrayRect(pt,buffer,cntX,cntY);
                break;
            case psLine:
                result = getPositionArrayLine(pt,buffer,cntX,cntY);
                break;
            case psEllipse:
            case psNone:
                break;

        }

    }
    return result;
}

TRetrievalImage* TSpectralImage::getIntensityImage(){
    TRetrievalImage* result=new TRetrievalImage(getxCount(),getyCount());
    if  (getPositionArray( result->valueBuffer, result->getWidth(), result->getHeight())){
        for(int i = 0;i<result->getHeight();i++){
            for(int n = 0;n<result->getWidth();n++){
                TRetrieval *tr = result->valueBuffer[i][n];
                int index = tr->mirrorCoordinate->pixelIndex;
                //double val = spectrumlist[index]->ScanPixelIndex;//for testing porpuses
                double val = spectrumlist[index]->rms();
                result->valueBuffer[i][n]->val = val;
            }
        }
    }

    //buffer
    return result;
}

QScriptValue TSpectralImage::getIntensityArray(){
    TRetrievalImage * ri = getIntensityImage();
    return engine()->newQObject(ri);
}

int TSpectralImage::getPixelIndex(TMirrorCoordinate* mc){
    QPair<int,int> key;

    QPair<TSpectrum*,double> val;

    key.first = mc->getMotorCoordinate().x();
    key.second = mc->getMotorCoordinate().y();
    if (spectrumtable.contains(key)){
        val = spectrumtable.value(key);
        return val.first->ScanPixelIndex;
    }
    return -1;
}

void TSpectralImage::add(TMirrorCoordinate* coord, TSpectrum* spektrum){
    QPair<int,int> key;

    QPair<TSpectrum*,double> val;

    key.first = coord->getMotorCoordinate().x();
    key.second = coord->getMotorCoordinate().y();

    val.first = spektrum;
    val.second = -1;
    spectrumtable.insert(key,val);

    if (spectrumtable.count() == 1){
        FirstDate = spektrum->GetDateTime();
    }else{
        if (spektrum->GetDateTime() < FirstDate){
            FirstDate = spektrum->GetDateTime() ;
        }
    }
    spektrum->setPixelIndex(spectrumlist.count());
    spectrumlist.append(spektrum);

    delete meanSpectrum;
    delete rmsSpectrum;
    delete stdDevSpectrum;
    delete maxRMSPos;
    stdDevSpectrum = NULL;
    rmsSpectrum = NULL;
    meanSpectrum = NULL;
    maxRMSPos = NULL;
}

void TSpectralImage::save(QString FileName){
    fn = FileName;
    QFile data(FileName);
    QFile meta(fnToMetafn(FileName));
    if (data.open(QFile::WriteOnly | QFile::Truncate) && meta.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream datastream(&data);
        QTextStream metastream(&meta);
        QHashIterator<QPair<int,int>, QPair<TSpectrum*,double> > i(spectrumtable);
        TSpectrum* spektrum;
        metastream << "time" << '\t';

        metastream << "Fixed Axis" << '\t';
        metastream << "Mirror Axis" << '\t';

        metastream << "IntegStyle" << '\t';
        metastream << "targetPeak" << '\t';
        metastream << "targetCorridor" << '\t';
        metastream << "maxIntegTime" << '\t';

        metastream << "IntegTime" << '\t';
        metastream << "AvgCount" << '\t';
        metastream << "Temperature" << '\t';
        metastream << "Max possible value" << '\t';
        metastream << "WLOffs" << '\t';
        metastream << "WLCoeff1" << '\t';
        metastream << "WLCoeff2" << '\t';
        metastream << "WLCoeff3" << '\t';
        metastream << "SpectmerSerial\t";
        metastream << "TiltX\t";
        metastream << "TiltY\t";
        metastream << "PixelIndexX\t";
        metastream << "PixelIndexY\t";
        metastream << "dark\n";

        for (int n = 0; n < spectrumlist.count();n++){
            spektrum = spectrumlist[n];
            spektrum->SaveSpectrum(datastream,metastream,false);
        }
        //while (i.hasNext()) {
        //    QPair<TSpectrum*,double> val;
        //   val = i.next().value();
        //   spektrum = val.first;
        //   spektrum->SaveSpectrum(datastream,metastream);
        //}
        TPatternType *pt;
        metastream << "pattern:\n";
        for(int i = 0;i<Patternsources.count();i++){
            pt = Patternsources.at(i);
            pt->save(metastream);
        }

        data.close();
        meta.close();
    }
}

void TSpectralImage::save(QString Directory,QString BaseName,int SequenceNumber){
    QString fn;
    fn = DefaultFileNameFromSeqNumber(Directory,BaseName,SequenceNumber,FirstDate)+".spe";
    save(fn);
}

bool TSpectralImage::Load(QString Directory, QString BaseName,int seqnumber,uint startindex,uint groupindex){
    QString filename = GetSequenceFileName(Directory,BaseName,seqnumber,startindex,groupindex);
    return Load(filename);
}

bool TSpectralImage::Load(QString fn){
    this->fn=fn;
    bool result=false;
    if (fn == "")
        return false;
    QFile dataf(fn);
    QFile metaf(fnToMetafn(fn));
    if ((dataf.open(QIODevice::ReadOnly | QIODevice::Text))&&(metaf.open(QIODevice::ReadOnly | QIODevice::Text))){
        bool ok;
        TSpectrum *spec;
        QTextStream data(&dataf);
        QTextStream meta(&metaf);
        meta.readLine();
        ok = true;
        while (ok){
            spec = new TSpectrum();
            ok = spec->LoadSpectrum(data,meta);
            if(ok){
                add(spec->getMirrorCoordinate(), spec);
                result = true;
            }else{
                delete spec;
            }
        }
        ok = true;
        while(!meta.atEnd()){

            QString line_str = meta.readLine();
            QTextStream line(&line_str);
            QString t;
            line >> t;
            if (t.startsWith("pattern")){
                while (ok){
                    TPatternType *pt = new(TPatternType);
                    ok = pt->load(meta);
                    if(ok)
                        Patternsources.append(pt);
                    else
                        delete pt;
                }

            }
        }
        dataf.close();
        metaf.close();

    }

    return result;
}

bool TSpectralImage::isChanged(){
    bool result = false;
    TSpectrum* spektrum;
    QHashIterator<QPair<int,int>,  QPair<TSpectrum*,double> > i(spectrumtable);

    while (i.hasNext()) {
        QPair<TSpectrum*,double> val;
        val = i.next().value();
        spektrum = val.first;
        if (spektrum->getHash() != val.second){
            i.toFront();    //for not needing to iterate whole hash next time
            val.second = -1;
            spectrumtable[i.next().key()] = val;
            result = true;
            delete meanSpectrum;
            delete rmsSpectrum;
            delete stdDevSpectrum;
            delete maxRMSPos;
            stdDevSpectrum = NULL;
            rmsSpectrum = NULL;
            meanSpectrum = NULL;
            maxRMSPos = NULL;
            break;
        }
    }
    return result;
}

TSpectrum* TSpectralImage::getMean_(){
    TSpectrum* spektrum;
    int cnt=0;
    QHashIterator<QPair<int,int>,  QPair<TSpectrum*,double> > i(spectrumtable);
    bool changed=isChanged();
    if (changed || meanSpectrum == NULL){
        meanSpectrum = new TSpectrum();
        while (i.hasNext()) {
            QPair<TSpectrum*,double> val;
            val = i.next().value();
            spektrum = val.first;
            if (cnt == 0){
                meanSpectrum->NumOfSpectrPixels = spektrum->NumOfSpectrPixels;
                meanSpectrum->MaxPossibleValue = spektrum->MaxPossibleValue;
                meanSpectrum->setZero();
            }
            meanSpectrum->add(spektrum);
            if (changed){
                val.second = spektrum->getHash();
                spectrumtable[i.key()] = val;
            }
            cnt++;
        }
        meanSpectrum->div(cnt);
    }
    return new TSpectrum(meanSpectrum);
}

QScriptValue TSpectralImage::getMean(){
    TSpectrum * spektrum = getMean_();
    return engine()->newQObject(spektrum);
}

TSpectrum* TSpectralImage::getRms_(){
    TSpectrum* spektrum;
    int cnt=0;
    QHashIterator<QPair<int,int>,  QPair<TSpectrum*,double> > i(spectrumtable);
    bool changed=isChanged();
    if (changed || rmsSpectrum == NULL){
        rmsSpectrum = new TSpectrum();
        while (i.hasNext()) {
            QPair<TSpectrum*,double> val;
            val = i.next().value();
            spektrum = val.first;
            if (cnt == 0){
                rmsSpectrum->NumOfSpectrPixels = spektrum->NumOfSpectrPixels;
                rmsSpectrum->MaxPossibleValue = spektrum->MaxPossibleValue;
                rmsSpectrum->setZero();
            }
            for (int n = 0;n<spektrum->NumOfSpectrPixels;n++){
                double v = spektrum->spectrum[n];
                v = pow(v,2);
                rmsSpectrum->spectrum[n] += v;
            }
            rmsSpectrum->MaxPossibleValue += pow(spektrum->MaxPossibleValue,2);
            if (changed){
                val.second = spektrum->getHash();
                spectrumtable[i.key()] = val;
            }
            cnt++;
        }
        rmsSpectrum->div(cnt);
        for (int n = 0;n<spektrum->NumOfSpectrPixels;n++){
            rmsSpectrum->spectrum[n] = sqrt(rmsSpectrum->spectrum[n]);
        }
        rmsSpectrum->MaxPossibleValue = sqrt(rmsSpectrum->MaxPossibleValue);
    }
    return new TSpectrum(rmsSpectrum);
}


QScriptValue TSpectralImage::getRms(){
    TSpectrum * spektrum = getRms_();
    return engine()->newQObject(spektrum);
}

TSpectrum* TSpectralImage::getStdDev_(){
    TSpectrum* spektrum;
    int cnt=0;
    QHashIterator<QPair<int,int>,  QPair<TSpectrum*,double> > i(spectrumtable);
    bool changed=isChanged();
    getMean();
    if (changed || stdDevSpectrum == NULL){
        stdDevSpectrum = new TSpectrum();
        while (i.hasNext()) {
            QPair<TSpectrum*,double> val;
            val = i.next().value();
            spektrum = val.first;
            if (cnt == 0){
                stdDevSpectrum->NumOfSpectrPixels = spektrum->NumOfSpectrPixels;
                stdDevSpectrum->MaxPossibleValue = spektrum->MaxPossibleValue;
                stdDevSpectrum->setZero();
            }
            for (int n = 0;n<spektrum->NumOfSpectrPixels;n++){
                stdDevSpectrum->spectrum[n] += pow((spektrum->spectrum[n] - meanSpectrum->spectrum[n]),2);
            }
            stdDevSpectrum->MaxPossibleValue += pow(spektrum->MaxPossibleValue - spektrum->mean(),2);
            if (changed){
                val.second = spektrum->getHash();
                spectrumtable[i.key()] = val;
            }
            cnt++;
        }
        stdDevSpectrum->div(cnt);
        for (int n = 0;n<spektrum->NumOfSpectrPixels;n++){
            stdDevSpectrum->spectrum[n] = sqrt(rmsSpectrum->spectrum[n]);
        }
        stdDevSpectrum->MaxPossibleValue = sqrt(stdDevSpectrum->MaxPossibleValue);

    }
    return new TSpectrum(stdDevSpectrum);
}


QScriptValue TSpectralImage::getStdDev(){
    TSpectrum * spektrum = getStdDev_();
    return engine()->newQObject(spektrum);
}

TMirrorCoordinate* TSpectralImage::getMaxRMSPos_(){
    TSpectrum* spektrum;
    QHashIterator<QPair<int,int>,  QPair<TSpectrum*,double> > i(spectrumtable);
    bool changed=isChanged();
    if (changed || maxRMSPos == NULL){
        maxRMSVal = 0;
        while (i.hasNext()) {
            QPair<TSpectrum*,double> val;
            val = i.next().value();
            spektrum = val.first;
            if (spektrum->rms() > maxRMSVal){
                maxRMSVal = spektrum->rms();
                maxRMSPos = spektrum->getMirrorCoordinate();
            }
        }
    }
    return maxRMSPos;
}

QScriptValue TSpectralImage::getMaxRMSPos(){
    TMirrorCoordinate * MirrorCoordinate = getMaxRMSPos_();
    return engine()->newQObject(MirrorCoordinate);
}

double TSpectralImage::getMaxRMSVal(){
    getMaxRMSPos();
    return maxRMSVal;
}

int TSpectralImage::count(){
    return spectrumtable.count();
}

TMirrorCoordinate * TSpectralImage::getMirrorCoordinate(int index){
    return spectrumlist[index]->getMirrorCoordinate();
}

TSpectrum * TSpectralImage::getSpectrum(int index){
    return spectrumlist[index];
}

void TSpectralImage::plot(int plotIndex,int Pixelsize){
    TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(0);
    SpectrumPlotter->plotSpectralImage(this,plotIndex,Pixelsize);
}
