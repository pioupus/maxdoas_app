#include "tspectrum.h"
#include "tspectrumplotter.h"
#include <QSettings>
#include <QDir>
#include <QObject>
#include <QDateTime>

QString DefaultFileNameFromSeqNumber(QString Directory, QString BaseName,int seqnumber,QDateTime datetime){
    QString seq = QString::number(seqnumber).rightJustified(5, '0');
    return Directory+'/'+BaseName+'_'+datetime.toString("dd_MM_yyyy__hh_mm_ss")+"_seq"+seq+"s";
}

QString fnToMetafn(QString fn){

    QFileInfo fi(fn);
    QString fp = fi.absolutePath()+"/";
    QString name = fi.baseName();
    if (name.endsWith("s"))
        name.remove(name.length()-1,1);
    name = name+"_meta";
    name = fp+name+"."+fi.completeSuffix();
    return name;
}


QString GetSequenceFileName(QString Directory, QString BaseName, uint Sequence, uint firstindex ,uint groupindex){
    QString result="";
    QDir dir(Directory);
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);
    QStringList filters;
    QString seq = QString::number(firstindex).rightJustified(5, '0');
    filters << BaseName+"_*_seq"+seq+"s.spe";
    QStringList list = dir.entryList(filters,QDir::Files,QDir::Name);
    QDateTime dtprev,dtnext;
    QString strprev,strnext;
    //darkoffset_05_10_2011__09_19_34_seq00001s.spe
    if((int)groupindex < list.count()){
        strprev = list[groupindex];
        strprev = strprev.mid(strprev.indexOf("_")+1,20);
        dtprev = QDateTime::fromString(strprev,"dd_MM_yyyy__hh_mm_ss");
    }else{
        return "";
    }
    if ((int)groupindex+1 < list.count()){
        strnext = list[groupindex+1];
        strnext = strnext.mid(strnext.indexOf("_")+1,20);
        dtnext = QDateTime::fromString(strnext,"dd_MM_yyyy__hh_mm_ss");
    }

    filters.clear();
    seq = QString::number(Sequence).rightJustified(5, '0');
    filters << BaseName+"_*_seq"+seq+"s.spe";
    list = dir.entryList(filters,QDir::Files,QDir::Name);

    for (int i = 0; i < list.size(); ++i) {
        QString t;
        QDateTime dt;
        t = list[i];
        t = t.mid(t.indexOf("_")+1,20);
        dt = QDateTime::fromString(t,"dd_MM_yyyy__hh_mm_ss");
        if ((((dt >= dtprev)&&(dt < dtnext))||((dt >= dtprev) && (!dtnext.isValid())))){
            result = Directory+"/"+list[i];
            break;
        }
    }
    return result;
}

TSpectrum::TSpectrum(QObject* parent){
    setParent(parent);
    Wavelength = TWavelengthbuffer::instance();
    type = stNone;
    memset(&IntegConf,0,sizeof(IntegConf));
    memset(&WLCoefficients,0,sizeof(WLCoefficients));
    WLCoefficients.Coeff1 = 1;
    WLCoefficients.uninitialized = true;
    IntegTime = 0;
    AvgCount = 0;
    SequenceNumber = -1;
    datetime = QDateTime::currentDateTime();
    rmsval = -1;
    maxval = -1;
    hash = -1;
    meanval = -1;
    stddevval = -1;
    ScanPixelIndex = -1;
    BaseName = "";
    SpectrometerSerialNumber = "NULL";
    MirrorCoordinate = NULL;

}

TSpectrum::TSpectrum(TSpectrum * other){
    Wavelength = TWavelengthbuffer::instance();
    type = other->type;
    IntegTime = other->IntegTime;
    IntegConf = other->IntegConf;
    AvgCount = other->AvgCount;
    SequenceNumber = other->SequenceNumber;
    datetime = other->datetime;
    rmsval = other->rmsval;
    maxval = other->maxval;
    hash = other->hash;
    meanval = other->meanval;
    stddevval = other->stddevval;
    BaseName = other->BaseName;
    MirrorCoordinate = NULL;
    Temperature = other->Temperature;
    WLCoefficients = other->WLCoefficients;
    NumOfSpectrPixels = other->NumOfSpectrPixels;
    MaxPossibleValue = other->MaxPossibleValue;
    setMirrorCoordinate(other->getMirrorCoordinate());
    SpectrometerSerialNumber = other->SpectrometerSerialNumber;
    memcpy(spectrum,other->spectrum,sizeof(double)*NumOfSpectrPixels);
    ScanPixelIndex = other->ScanPixelIndex;
}

TSpectrum::~TSpectrum(){
    if (MirrorCoordinate != NULL){
        delete MirrorCoordinate;
    }
}

void TSpectrum::setZero(){
    if (NumOfSpectrPixels==0){
        NumOfSpectrPixels = MAXWAVELEGNTH_BUFFER_ELEMTENTS;
    }
    memset(spectrum,0,sizeof(double)*NumOfSpectrPixels);
}

QDateTime TSpectrum::GetDateTime(){
    return datetime;
}



void TSpectrum::SaveSpectrum(QTextStream &file, QTextStream &meta, bool DarkSpectrum){
    if (DarkSpectrum)
        type = stDarkOrRef;
    file.setRealNumberPrecision(6);
    if (!DarkSpectrum){
        if (MirrorCoordinate == NULL){
            file << -1 << ' ';
            file << -1 << ' ';
        }else{
            file << MirrorCoordinate->getZenithCoordinate().x() << ' '; //Azimuth
            file << 90.0-MirrorCoordinate->getZenithCoordinate().y() << ' '; //Elevation
        }
        file << datetime.toString("dd/MM/yyyy")<< " ";
        file << (double)datetime.time().hour()+(double)(datetime.time().minute()*60+datetime.time().second())/3600 << " ";
    }
    for(int i = 0;i<NumOfSpectrPixels;i++){
        file << spectrum[i] << ' ';
    }
    file << '\n';

    file.setRealNumberPrecision(6);     //metadata

    meta << datetime.toString("dd.MM.yyyy hh:mm:ss") << '\t';

    if (MirrorCoordinate == NULL){
        meta << -1 << '\t';
        meta << -1 << '\t';
    }else{
        meta << MirrorCoordinate->getAngleCoordinate().x() << '\t';
        meta << MirrorCoordinate->getAngleCoordinate().y() << '\t';
    }
    if (IntegConf.autoenabled){
        meta << "auto" << '\t';
        meta << IntegConf.targetPeak << '\t';
        meta << IntegConf.targetCorridor << '\t';
        meta << IntegConf.maxIntegTime << '\t';
    }else{
       meta << "fixed" << '\t';
       meta << "-1" << '\t';
       meta << "-1" << '\t';
       meta << "-1" << '\t';
    }
    meta << IntegTime << '\t';
    meta << AvgCount << '\t';
    meta << Temperature << '\t';
    meta << MaxPossibleValue << '\t';
    meta << WLCoefficients.Offset << '\t';
    meta << WLCoefficients.Coeff1 << '\t';
    meta << WLCoefficients.Coeff2 << '\t';
    meta << WLCoefficients.Coeff3 << '\t';
    meta << SpectrometerSerialNumber << '\t';
    meta << Tilt.x() << '\t';
    meta << Tilt.y() << '\t';
    if (MirrorCoordinate == NULL){
        meta << -1 << '\t';
        meta << -1 << '\t';
    }else{
        meta << MirrorCoordinate->pixelIndexX << '\t';
        meta << MirrorCoordinate->pixelIndexY << '\t';
    }
    if (DarkSpectrum)
        meta << "1";
    else
        meta << "0";
    meta << '\n';
}


void TSpectrum::SaveSpectrum(QString fn){
    bool Dark = type==stDarkOrRef;
    SaveSpectrum_(fn,Dark);
}

void TSpectrum::SaveSpectrumDark(QString fn){

    SaveSpectrum_(fn,true);
}

void TSpectrum::SaveSpectrum_(QString fn,bool Dark){
    QFile data(fn);

    QFile meta(fnToMetafn(fn));
    if (data.open(QFile::WriteOnly | QFile::Truncate) && meta.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream datastream(&data);
        QTextStream metastream(&meta);
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
        SaveSpectrum(datastream,metastream,Dark);
        data.close();
        meta.close();
    }
}

QString TSpectrum::getDefaultFileName(QString Directory, QString BaseName,int seqnumber){


    if (SequenceNumber==-1)
        SequenceNumber = seqnumber;

    if (seqnumber==-1)
        seqnumber = SequenceNumber;

    if (BaseName=="")
        BaseName = this->BaseName;

    if (this->BaseName=="")
        this->BaseName = BaseName;

    return DefaultFileNameFromSeqNumber(Directory,BaseName, seqnumber,datetime);

}



void TSpectrum::SaveSpectrumDefNameDark(QString Directory, QString BaseName,int seqnumber){
    SaveSpectrum_(getDefaultFileName( Directory, BaseName, seqnumber)+".spe",true);
}

void TSpectrum::SaveSpectrumDefName(QString Directory, QString BaseName,int seqnumber){
    bool Dark = type==stDarkOrRef;
    SaveSpectrum_(getDefaultFileName( Directory, BaseName, seqnumber)+".spe",Dark);
}

void TSpectrum::setMirrorCoordinate(TMirrorCoordinate* mc){
    if (MirrorCoordinate != NULL){
        delete MirrorCoordinate;
    }
    if (mc == NULL)
        MirrorCoordinate = NULL;
    else
        MirrorCoordinate = new TMirrorCoordinate(mc);
}

TMirrorCoordinate * TSpectrum::getMirrorCoordinate(){
    return MirrorCoordinate;
}

void TSpectrum::setPixelIndex(int index){
    ScanPixelIndex = index;
    if(MirrorCoordinate != NULL){
        MirrorCoordinate->pixelIndex = index;
    }
}

bool TSpectrum::LoadSpectrum(QTextStream &file, QTextStream &meta){
    float x;
    float y;
    int index=0;
    bool fileatend = false;
    bool result = true;
    bool versionWithDateInLineWithoutSZA=false;
    bool DarkSpectrum = false;
    QString nl;
    if (!file.atEnd()) {
        if (!meta.atEnd()&&!fileatend){
            int pos = meta.pos();
            QString line_str = meta.readLine();
            QTextStream line(&line_str);
            QString s;
            for(int i = 0;i < 22;i++){
                line >> s;
            }
            if (s==QString("1")){
                DarkSpectrum = true;
                type = stDarkOrRef;
                versionWithDateInLineWithoutSZA = true;
            }
            if (s==QString("0")){
                type = stMeasurement;
                versionWithDateInLineWithoutSZA = true;
            }
            meta.seek(pos);
        }
        bool ok;
        QString w;
        if (!DarkSpectrum){
            int oldPos = file.pos();
            file >> w;
            file >> w;
            file >> w;
            if (w.contains("/")){
                versionWithDateInLineWithoutSZA = true;
            }
            file.seek(oldPos);

            if (versionWithDateInLineWithoutSZA){
                file >> w;//Azimuth viewing angle
                file >> w;//Elevation viewing angle
                file >> w;//day
                file >> w;//decimaltime
            }else{
                file >> x;
                file >> y;
            }
        }
        ok = true;

        while(!file.atEnd() && ok){
            double val;
            bool end=false;
            nl = file.read(1);
            int chaindex=0;
            if (file.atEnd()){
                end = true;
                break;
            }
            while (nl[chaindex].isSpace() && (!file.atEnd())){
                if (nl[chaindex] == QChar('\n')){
                    end = true;
                    break;
                }
                nl = nl + file.read(1);
                if(file.atEnd()){
                    end = true;
                    break;
                }

                chaindex++;
            }
            if (end){
               // index++;
                break;
            }
            end = false;
            while ((!nl[chaindex].isSpace()) && (!file.atEnd())){

                nl = nl + file.read(1);
                if(file.atEnd()){
                    end = true;
                    break;
                }
                chaindex++;
                if (nl[chaindex] == QChar('\n')){
                    end = true;
                    break;
                }
            }

            w = nl;
            val  = w.toDouble(&ok);
            spectrum[index] = val;

            index++;
            if(end)
                break;
            if(index >= MAXWAVELEGNTH_BUFFER_ELEMTENTS)
                break;
            if(index >= 2047)
                w = "";
        }
        NumOfSpectrPixels = index;
        hash = -1;
        if(index == 0)
            result = false;
    }else{
        result = false;
        fileatend = true;
    }
    if (!meta.atEnd()&&!fileatend){
        TMirrorCoordinate mc;
        QPointF ac;
        QString tm1;
        QString tm2;
        QString line_str = meta.readLine();
        QTextStream line(&line_str);
        line >> tm1;
        line >> tm2;
        tm1 = tm1+" "+tm2;
        datetime = QDateTime::fromString(tm1,"dd.MM.yyyy hh:mm:ss");
        line >> x;


        line >> y;
        if (!versionWithDateInLineWithoutSZA) // There was a bug in TMirrorCoordinate.setMotorCoordinate(x,y) sign of Y was wrong
            y = -y;                          //fixed 13.nov.2011 (the same date when date introduced in spectrumfile)
        ac.setX(x);
        ac.setY(y);
        mc.setAngleCoordinate(ac);
        setMirrorCoordinate(&mc);
        QString integstyle;
        line >> integstyle;

        if(integstyle == "auto"){
            IntegConf.autoenabled = true;
            line >> IntegConf.targetPeak;
            line >> IntegConf.targetCorridor;
            line >> IntegConf.maxIntegTime;
            line >> IntegTime;
        }else{
            bool ok;
            IntegConf.autoenabled = false;
            line >> IntegConf.targetPeak;
            line >> IntegConf.targetCorridor;
            line >> integstyle;
            if (integstyle == "nan")
                IntegConf.maxIntegTime = 0;
            else
                IntegConf.maxIntegTime = integstyle.toDouble(&ok);
            line >> IntegConf.fixedIntegtime;
            IntegTime = IntegConf.fixedIntegtime;
        }
        line >> AvgCount;
        line >> Temperature;
        line >> MaxPossibleValue;
        line >> WLCoefficients.Offset;
        line >> WLCoefficients.Coeff1;
        line >> WLCoefficients.Coeff2;
        line >> WLCoefficients.Coeff3;
        WLCoefficients.uninitialized = false;
        line >> SpectrometerSerialNumber;
        line >> x; Tilt.setX(x);
        line >> y; Tilt.setY(y);
        line >> mc.pixelIndexX;
        line >> mc.pixelIndexY;

        hash = -1;
      //  HWDriver->hwdOverwriteWLCoefficients(&WLCoefficients);
    }else{
        result = false;
    }
    return result;
}

bool TSpectrum::LoadSpectrum(QString fn){
    bool result=false;
    if (fn == "")
        return false;
    QFile dataf(fn);
    QFile metaf(fnToMetafn(fn));
    if ((dataf.open(QIODevice::ReadOnly | QIODevice::Text))&&(metaf.open(QIODevice::ReadOnly | QIODevice::Text))){
        QTextStream data(&dataf);
        QTextStream meta(&metaf);
        meta.readLine();
        result = LoadSpectrum(data,meta);
        dataf.close();
        metaf.close();

    }

    return result;
}

bool TSpectrum::LoadSpectrDefaultName(QString Directory, QString BaseName,int seqnumber,uint startindex, uint groupindex ){
    SequenceNumber = seqnumber;
    this->BaseName = BaseName;
    return LoadSpectrum(GetSequenceFileName(Directory,BaseName,seqnumber,startindex,groupindex));
}


bool TSpectrum::LoadSpectrEMT(QString fn){
    (void) fn;
    return false;
}

void TSpectrum::plot(int index){
    TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(0);
    SpectrumPlotter->plotSpectrum(this,index);
}

void TSpectrum::add(QObject *spect){
    TSpectrum *spec = dynamic_cast<TSpectrum*>(spect);
    if (spec != NULL){
        if (NumOfSpectrPixels == spec->NumOfSpectrPixels){
            for(int i = 0;i<NumOfSpectrPixels;i++){
                this->spectrum[i] = this->spectrum[i] + spec->spectrum[i];
            }
            hash = -1;
            MaxPossibleValue += spec->MaxPossibleValue;
        }

    }
}

void TSpectrum::add(double val){
    for(int i = 0;i<NumOfSpectrPixels;i++){
        this->spectrum[i] = this->spectrum[i] + val;
    }
    MaxPossibleValue += val;
    hash = -1;
}

void TSpectrum::sub(QObject *spect){
    TSpectrum *spec = dynamic_cast<TSpectrum*>(spect);
    if (spec != NULL){
        if (NumOfSpectrPixels == spec->NumOfSpectrPixels){
            for(int i = 0;i<NumOfSpectrPixels;i++){
                this->spectrum[i] = this->spectrum[i] - spec->spectrum[i];
            }
            hash = -1;
        }
    }
}

void TSpectrum::sub(double val){
    for(int i = 0;i<NumOfSpectrPixels;i++){
        this->spectrum[i] = this->spectrum[i] - val;
    }
    hash = -1;
}

void TSpectrum::mul(QObject *spect){
    TSpectrum *spec = dynamic_cast<TSpectrum*>(spect);
    if (spec != NULL){
        if (NumOfSpectrPixels == spec->NumOfSpectrPixels){
            for(int i = 0;i<NumOfSpectrPixels;i++){
                this->spectrum[i] = this->spectrum[i] * spec->spectrum[i];
            }
            hash = -1;
        }
    }
}

void TSpectrum::mul(double val){
    for(int i = 0;i<NumOfSpectrPixels;i++){
        this->spectrum[i] = this->spectrum[i] * val;
    }
    hash = -1;
}


void TSpectrum::div(QObject *spect){
    TSpectrum *spec = dynamic_cast<TSpectrum*>(spect);
    if (spec != NULL){
        if (NumOfSpectrPixels == spec->NumOfSpectrPixels){
            for(int i = 0;i<NumOfSpectrPixels;i++){
                if (spec->spectrum[i] != 0)
                    this->spectrum[i] = this->spectrum[i] / spec->spectrum[i];
                else
                     this->spectrum[i] = NAN;
            }
            hash = -1;

        }
    }
}
void TSpectrum::div(double val){
    if (val == 0){
        for(int i = 0;i<NumOfSpectrPixels;i++){
            this->spectrum[i] = NAN;
        }
    }else{
        for(int i = 0;i<NumOfSpectrPixels;i++){
            this->spectrum[i] = this->spectrum[i] / val;
        }
        MaxPossibleValue /= val;
    }
    hash = -1;
}

double TSpectrum::getHash(){
    double result=0;
    for(int i = (NumOfSpectrPixels>>2)-20;i<(NumOfSpectrPixels>>2)+20;i++){
        if (i>-1 && i<NumOfSpectrPixels)
            result+=spectrum[i];
    }
    return result;
}

bool TSpectrum::isSpectrumChanged(){
    double hash=getHash();
    bool result;

    result = hash != hash;
    if (result){
       rmsval = -1;
       meanval = -1;
       stddevval = -1;
       maxval = -1;
   }
    this->hash = hash;
    return result;
}

double TSpectrum::rms(){
    isSpectrumChanged();
    if (rmsval == -1){
        rmsval = 0;
        for(int i = 0;i<NumOfSpectrPixels;i++){
            rmsval += pow(this->spectrum[i],2);
        }
        rmsval /= NumOfSpectrPixels;
        rmsval = sqrt(rmsval);
    }
    return rmsval;
}

double TSpectrum::max(){
    isSpectrumChanged();
    if (maxval == -1){
        maxval = 0;
        for(int i = 0;i<NumOfSpectrPixels;i++){
            if (maxval < this->spectrum[i])
                maxval = this->spectrum[i];
        }
    }
    return maxval;
}

double TSpectrum::mean(){
    isSpectrumChanged();
    if (meanval == -1){
        meanval = 0;
        for(int i = 0;i<NumOfSpectrPixels;i++){
            meanval += this->spectrum[i];
        }
        meanval /= NumOfSpectrPixels;
    }
    return meanval;
}

double TSpectrum::stddev(){
    isSpectrumChanged();
    if (stddevval == -1){
        double m=mean();
        stddevval = 0;
        for(int i = 0;i<NumOfSpectrPixels;i++){
            stddevval += pow(this->spectrum[i]-m,2);
        }
        stddevval = sqrt(stddevval);
        stddevval /= (NumOfSpectrPixels-1);

    }
    return stddevval;
}
