#include "tspectrum.h"
#include "tspectrumplotter.h"
#include <QSettings>
#include <QDir>
#include <QObject>
#include <QDateTime>
#include <iostream>
#include <locale>

QString DefaultFileNameFromSeqNumber(QString Directory, QString BaseName,int seqnumber,QDateTime datetime){
    QString seq = QString::number(seqnumber).rightJustified(5, '0');
    return Directory+'/'+BaseName+'_'+datetime.toString("yyyy_MM_dd__hh_mm_ss")+"_seq"+seq+"s";
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
    filters << "*_seq"+seq+"s.spe";
    QStringList list = dir.entryList(filters,QDir::Files,QDir::Name);
    QDateTime dtprev,dtnext;
    QString strprev,strnext;
    //darkoffset_2011_10_05__09_19_34_seq00001s.spe
    if (list.count() == 0)
        return "";
    QString firstBasname=list[0];
    int currgroupindex = 0;
    firstBasname = firstBasname.left(firstBasname.indexOf("_"));
    strnext = "";
    for (int i = 0;i < list.count();i++){
        QString s = list[i];
        s = s.left(s.indexOf("_"));
        if (s == firstBasname){
            currgroupindex++;
            if (currgroupindex == groupindex){
                strprev = strprev.mid(strprev.indexOf("_")+1,20);
                dtprev = QDateTime::fromString(strprev,"yyyy_MM_dd__hh_mm_ss");
            }
            if (currgroupindex == groupindex+1){
                strnext = strnext.mid(strnext.indexOf("_")+1,20);
                dtnext = QDateTime::fromString(strnext,"yyyy_MM_dd__hh_mm_ss");
            }
        }
    }
    if (!dtprev.isValid())
        return "";

    filters.clear();
    seq = QString::number(Sequence).rightJustified(5, '0');
    filters << BaseName+"_*_seq"+seq+"s.spe";
    list = dir.entryList(filters,QDir::Files,QDir::Name);

    for (int i = 0; i < list.size(); ++i) {
        QString t;
        QDateTime dt;
        t = list[i];
        t = t.mid(t.indexOf("_")+1,20);
        dt = QDateTime::fromString(t,"yyyy_MM_dd__hh_mm_ss");
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

QString TSpectrum::getFileName(){
    return FileName;
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

QString extractFileName(QString fn){
    QFileInfo fi(fn);
    return fi.baseName()+"."+fi.completeSuffix();
}

void TSpectrum::SaveSpectrumSTD(QString fn){
    QFile data(fn);
    QString fnn;
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream datastream(&data);

        double wl1;
        datastream << "GDBGMNUP\n";
        datastream << "1\n";
        datastream << NumOfSpectrPixels<<"\n";
        for (int i=0;i<NumOfSpectrPixels;i++){
           datastream << spectrum[i] << '\n';
        }
        fnn = extractFileName(fn);
        datastream << fnn << "\n";
        datastream << SpectrometerSerialNumber << "\n";
        datastream << SpectrometerSerialNumber << "\n";
        datastream << datetime.toString("dd.MM.yyyy") << "\n";
        datastream << datetime.toString("hh.mm.ss") << "\n";
        datastream << datetime.toString("hh.mm.ss") << "\n";
        wl1 = WLCoefficients.Offset+
              WLCoefficients.Coeff1*NumOfSpectrPixels+
              pow(NumOfSpectrPixels,2)*WLCoefficients.Coeff2+
              pow(NumOfSpectrPixels,3)*WLCoefficients.Coeff3;
        datastream << wl1 << "\n";
        datastream << 0 << "\n";
        datastream << "INT_TIME "<< IntegTime << "\n";
        datastream << "SCANS "<< AvgCount << "\n";
    }
    data.close();
}

void TSpectrum::SaveSpectrumRef(QString calib,QString fn){
    QFile data(fn);
    QFile cal(calib);
    if (data.open(QFile::WriteOnly | QFile::Truncate)&&(cal.open(QFile::ReadOnly))) {
        QTextStream datastream(&data);
        QTextStream calibstream(&cal);
        for (int i=0;i<NumOfSpectrPixels;i++){
           QString wl=calibstream.readLine();
           datastream << wl << '\t' << spectrum[i] << '\n';
        }

    }
    data.close();
}

void TSpectrum::SaveSpectrum(QString fn){
    bool Dark = type==stDarkOrRef;
    SaveSpectrum_(fn,Dark,false);
}

void TSpectrum::SaveSpectrumTmp(QString fn){
    bool Dark = type==stDarkOrRef;
    SaveSpectrum_(fn,Dark,true);
}

void TSpectrum::SaveSpectrumDark(QString fn){

    SaveSpectrum_(fn,true,false);
}

void TSpectrum::SaveSpectrum_(QString fn,bool Dark,bool istmp){
    if (!istmp)
        FileName = fn;
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
    SaveSpectrum_(getDefaultFileName( Directory, BaseName, seqnumber)+".spe",true,false);
}

void TSpectrum::SaveSpectrumDefName(QString Directory, QString BaseName,int seqnumber){
    bool Dark = type==stDarkOrRef;
    SaveSpectrum_(getDefaultFileName( Directory, BaseName, seqnumber)+".spe",Dark,false);
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

void TSpectrum::setXPixelIndex(int index){
    if(MirrorCoordinate != NULL){
        MirrorCoordinate->pixelIndexX = index;
    }
}

void TSpectrum::setYPixelIndex(int index){
    if(MirrorCoordinate != NULL){
        MirrorCoordinate->pixelIndexY = index;
    }
}


bool getnextword(char *linebuffer,int linebufferlength, char *wordbuffer, int wordbufferlength, int *bufferindex, int *wordbufferindex, bool *newline,bool replacedecimalpoint){
    bool wasspace = true;
    bool isspace = false;
    bool result = false;
    char decimalpoint = *(localeconv()->decimal_point);
    while(*bufferindex<linebufferlength){
        isspace = (linebuffer[*bufferindex] == ' ') || (linebuffer[*bufferindex] == '\t');
        if(linebuffer[*bufferindex] == '\n'){
            *newline = true;
          //  break;
        }
   //     qDebug() << buffer[bufferindex];
        if (!isspace){
            if (*wordbufferindex > wordbufferlength){
//                index = -1;//error
                break;
            }
            if((linebuffer[*bufferindex] == '.')&&(replacedecimalpoint)){
                wordbuffer[*wordbufferindex] = decimalpoint;
            }else{
                wordbuffer[*wordbufferindex] = linebuffer[*bufferindex];
            }
            (*wordbufferindex)++;
        }else{
            if ((!wasspace && isspace) || *newline){
                wordbuffer[*wordbufferindex] = '\0';
                *wordbufferindex = 0;
                result = true;
                break;
            }
        }
        (*bufferindex)++;
        wasspace = isspace;
    }
    return result;
}

QString getNextString(QFile *data,char *linebuffer,int *linebufferlength, int linebuffersize, char *wordbuffer, int wordbufferlength, int *bufferindex, int *wordbufferindex, bool *newline){

    bool ok;

    QString result;
    if (*bufferindex == 0)
        *linebufferlength = data->readLine(linebuffer,linebuffersize);
    do{
        if (*linebufferlength < 0)
            *newline = true;
        ok = getnextword(linebuffer,*linebufferlength, wordbuffer, wordbufferlength, bufferindex, wordbufferindex, newline,false);
        if(!ok && !*newline){
            *linebufferlength = data->readLine(linebuffer,linebuffersize);
        }
        if(ok){
            result = QString(wordbuffer);
        }
    }while(!ok && !newline);



    return result;
}

double getNextDouble(QFile *data,char *linebuffer,int *linebufferlength,int linebuffersize, char *wordbuffer, int wordbufferlength, int *bufferindex, int *wordbufferindex, bool *newline){

    double result;
    QString t;
    t = getNextString(data,linebuffer,linebufferlength,linebuffersize, wordbuffer, wordbufferlength, bufferindex, wordbufferindex, newline);
    result = t.toDouble();

    return result;
}

bool TSpectrum::LoadMeta(QFile &meta,bool versionWithDateInLineWithoutSZA){
    char wordbuffer[50];
    int wordbufferindex=0;
    char buffer[15000];
    int lineLength;
    int bufferindex=0;
    bool newline=false;

    float x,y;
    TMirrorCoordinate mc;
    QPointF ac;
    QString tm1;
    QString tm2;

    //QString line_str = meta.readLine();
    //QTextStream line(&line_str);
    tm1 = getNextString(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
    tm2 = getNextString(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
    tm1 = tm1+" "+tm2;
    datetime = QDateTime::fromString(tm1,"dd.MM.yyyy hh:mm:ss");

    x = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
    y = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
    if (!versionWithDateInLineWithoutSZA) // There was a bug in TMirrorCoordinate.setMotorCoordinate(x,y) sign of Y was wrong
        y = -y;                          //fixed 13.nov.2011 (the same date when date introduced in spectrumfile)
    ac.setX(x);
    ac.setY(y);
    mc.setAngleCoordinate(ac);
    setMirrorCoordinate(&mc);
    QString integstyle;
    integstyle = getNextString(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);

    if(integstyle == "auto"){
        IntegConf.autoenabled = true;
        IntegConf.targetPeak = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
        IntegConf.targetCorridor = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
        IntegConf.maxIntegTime = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
        IntegTime = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
    }else{
        bool ok;
        IntegConf.autoenabled = false;
        IntegConf.targetPeak = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
        IntegConf.targetCorridor = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
        integstyle = getNextString(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
        if (integstyle == "nan")
            IntegConf.maxIntegTime = 0;
        else
            IntegConf.maxIntegTime = integstyle.toDouble(&ok);
        IntegConf.fixedIntegtime = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
        IntegTime = IntegConf.fixedIntegtime;
    }
    AvgCount = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
    Temperature = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
    MaxPossibleValue = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
    WLCoefficients.Offset = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
    WLCoefficients.Coeff1 = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
    WLCoefficients.Coeff2 = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
    WLCoefficients.Coeff3 = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
    WLCoefficients.uninitialized = false;
    SpectrometerSerialNumber = getNextString(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
    x = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
    y = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
    Tilt.setY(y);
    Tilt.setX(x);
    mc.pixelIndexX = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);
    mc.pixelIndexY = getNextDouble(&meta,buffer,&lineLength,sizeof(buffer), wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline);

    hash = -1;
}

int TSpectrum::LoadSpectraldata(QFile &data,bool &versionWithDateInLineWithoutSZA,bool isdark){
    int start = 2;
    int index = 0;
    char wordbuffer[50];
    int wordbufferindex=0;
    char buffer[15000];
    int lineLength;
    int bufferindex=0;
    bool newline=false;
    bool ok;
    bool headeranalyzing = !isdark;
    if (isdark)
        start = 0;
    //char decimalpoint = std::use_facet<std::numpunct<char> >(std::cout.getloc()).decimal_point();
    //decimalpoint = *(localeconv()->decimal_point);
    while(!newline){
        lineLength = data.readLine(&buffer[0],sizeof(buffer));
        if (lineLength < 0)
            newline = true;
        do{
            ok = getnextword(buffer,lineLength, wordbuffer, sizeof(wordbuffer), &bufferindex, &wordbufferindex, &newline,true);
            if(ok){

                if (headeranalyzing){
                    if (index == 2){//day
                        QString w = QString(wordbuffer);
                        if (w.contains("/")){
                            versionWithDateInLineWithoutSZA = true;
                            start = 4;
                        }else{
                            start = 2;
                        }
                        headeranalyzing = false;
                    }
                }
                if (index >= start){
                    double val;
                    val  = atof(wordbuffer);
                   //qDebug() << "["<<index-start<<"]" << val;
                    spectrum[index-start] = val;
                    if(!ok){
                        index = -1;
                        break;
                    }
                }
                index++;
                wordbufferindex = 0;
                if(index >= MAXWAVELEGNTH_BUFFER_ELEMTENTS)
                    break;
            }
        }while(ok&&!newline);
    }
    index = index - start;
    NumOfSpectrPixels = index;

    return NumOfSpectrPixels;
}

bool TSpectrum::LoadSpectrum(QFile &file, QFile &meta){

    int index=0;
    bool fileatend = false;
    bool result = true;
    bool versionWithDateInLineWithoutSZA=false;
    bool DarkSpectrum = false;
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
        ok = true;
        index = LoadSpectraldata(file,versionWithDateInLineWithoutSZA,DarkSpectrum);
        hash = -1;
        if(index == 0)
            result = false;
    }else{
        result = false;
        fileatend = true;
    }
    if (!meta.atEnd()&&result){
       LoadMeta(meta,versionWithDateInLineWithoutSZA);

       Wavelength->setCoefficients(&WLCoefficients);
    }else{
        result = false;
    }
    return result;
}

bool TSpectrum::LoadSpectrum_(QString fn,bool istmp){
    bool result=false;
    if (fn == "")
        return false;
    if (!istmp)
        FileName = fn;
    QFile dataf(fn);
    QFile metaf(fnToMetafn(fn));
    if ((dataf.open(QIODevice::ReadOnly | QIODevice::Text))&&(metaf.open(QIODevice::ReadOnly | QIODevice::Text))){
     //   QTextStream meta(&metaf);
        metaf.readLine();
        result = LoadSpectrum(dataf,metaf);
        dataf.close();
        metaf.close();

    }

    return result;
}

bool TSpectrum::Load(QString fn){
    return LoadSpectrum_(fn,false);
}

#if 0
bool TSpectrum::LoadSpectrDefaultName(QString Directory, QString SearchBaseName,QString FileBaseName,int seqnumber,uint startindex, uint groupindex){
    SequenceNumber = seqnumber;
    this->BaseName = FileBaseName;
    QString fn = GetSequenceFileName(Directory,SearchBaseName,seqnumber,startindex,groupindex);
    fn.replace(SearchBaseName,FileBaseName);
    return LoadSpectrum(fn);
}
#endif

bool TSpectrum::LoadSpectrEMT(QString fn){
    (void) fn;
    return false;
}

void TSpectrum::plot(int index){
    TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(0);
    SpectrumPlotter->plotSpectrum(this,index);
}

QString TSpectrum::getSpectSerialNo(){
    return SpectrometerSerialNumber;
}

void TSpectrum::interpolatePixel(int Pixelindex){
    if ((Pixelindex > 0) && (Pixelindex < NumOfSpectrPixels-1)){
        spectrum[Pixelindex] = (spectrum[Pixelindex+1]+spectrum[Pixelindex-1])/2;
    }
    hash = -1;
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
