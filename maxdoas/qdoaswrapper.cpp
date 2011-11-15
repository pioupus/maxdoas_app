#include "qdoaswrapper.h"
#include <QProcess>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QScriptEngine>
#include <QScriptContext>
#include <QScriptValue>

QString getTempPath(){
    QDir dir = QDir::temp();
    if (!dir.cd("qdoas"))
        dir.mkdir("qdoas");
    dir.cd("qdoas");
    return dir.absolutePath();
}

QString MakeTempfileName(QString prefix,QString suffix, qint64 time,int random){

    QByteArray num;
    for(int i = 0; i != sizeof(time); ++i){
        num.append((char)(time&(0xFF << i) >>i));
    }
    for(int i = 0; i != sizeof(random); ++i){
        num.append((char)(random&(0xFF << i) >>i));
    }
    return prefix+num.toBase64()+"."+suffix;
}

QString getTempfileName(QString prefix,QString suffix){
    QDoasWrapper *inst = QDoasWrapper::instance();
    QString result=prefix;
    QDir dir(getTempPath());
    qint64 t = QDateTime::currentDateTime().date().year()*1000*60*60*24*31*12+
               QDateTime::currentDateTime().date().month()*1000*60*60*24*31+
               QDateTime::currentDateTime().date().day()*1000*60*60*24+
               QDateTime::currentDateTime().time().hour()*1000*60*60+
               QDateTime::currentDateTime().time().minute()*1000*60+
               QDateTime::currentDateTime().time().second()*1000+
               QDateTime::currentDateTime().time().msec();
    do{
        result = MakeTempfileName(prefix,suffix,t,qrand());
    }while (dir.exists(result));
    inst->appendTempFileNameToDeleteLater(result);
    return dir.absoluteFilePath(result);
}

QDoasConfigFile::QDoasConfigFile(QString fn){
    qdoasfile = new QDomDocument("QDoasConfig");

    load(fn);
}

QDoasConfigFile::~QDoasConfigFile(){
    delete qdoasfile;
}


bool QDoasConfigFile::load(QString fn){
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if (!qdoasfile->setContent(&file)) {
        file.close();
        return false;
    }
    file.close();

    return true;
}

bool QDoasConfigFile::save(QString fn){
    QFile file(fn);
    bool result = false;
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)){

        QTextStream out(&file);
        out << qdoasfile->toString();
        file.close();
        result = true;
    }
    return result;
}


bool QDoasConfigFile::setInputDirectory(QString fn){
    QDomElement raw_spectraElement;
    QDomElement directoryElement;

    raw_spectraElement = qdoasfile->firstChildElement("raw_spectra");
    directoryElement = raw_spectraElement.firstChildElement("directory");
    directoryElement.setAttribute("name",fn);

    /*<raw_spectra>
      <!-- Disable file, folder and directory items with the disable set equal to "true". -->
      <!--  The default is enabled.                                                       -->
      <directory name="/home/arne/.../2011.02.20" filters="" recursive="false" />
    </raw_spectra>*/

    return true;
}

bool QDoasConfigFile::setOffset(TSpectrum *Offset){
    QDomElement instrumentalElement;
    QDomElement mfcstdElement;
    QString SpecFn = getTempfileName("offset","std");
    Offset->SaveSpectrumSTD(SpecFn);
    instrumentalElement = qdoasfile->firstChildElement("instrumental");
    mfcstdElement = instrumentalElement.firstChildElement("mfcstd");
    mfcstdElement.setAttribute("offset",SpecFn);

    /*<instrumental format="mfcstd" site="CCA_UNAM">
      <mfcstd size="2048" revert="false" straylight="false" date="DD.MM.YYYY" lambda_min="0" lambda_max="0" calib="/home/arne/.../ADUD6545/ADUD6545(c).clb" instr="" dark="" offset="/home/arne/.../offsetd01042011h074025.std" />
      */
    return true;
}

bool QDoasConfigFile::setXSRef(QString calibfn, TSpectrum *Spectrum){
    QString SpecFn = getTempfileName("xsref","ref");
    Spectrum->SaveSpectrumRef(calibfn,SpecFn);
    QDomNodeList nodeList = qdoasfile->elementsByTagName("analysis_window");
    for(int i=0; i < nodeList.count();i++){
        QDomElement filesElement;
        filesElement = nodeList.item(i).firstChildElement("files");
        filesElement.setAttribute("refone",SpecFn);
    }
    return true;
}

bool QDoasConfigFile::setCalRef(QString calibfn, TSpectrum *Spectrum){
    QString SpecFn = getTempfileName("calref","ref");
    Spectrum->SaveSpectrumRef(calibfn,SpecFn);
    QDomElement node = qdoasfile->firstChildElement("calibration");
    node.setAttribute("ref",SpecFn);

    /*    <calibration ref="/home/arne/.../ADUD6545/WD_XS/original_XS/SOLARFL.ktz" method="ODF">*/

    return true;
}

bool QDoasConfigFile::setUSAMPRef(QString calibfn, TSpectrum *Spectrum){
    QString SpecFn = getTempfileName("usampref","ref");
    Spectrum->SaveSpectrumRef(calibfn,SpecFn);
    QDomElement node = qdoasfile->firstChildElement("undersampling");
    node.setAttribute("ref",SpecFn);
    /*<undersampling ref="/home/arne/../ADUD6545/WD_XS/original_XS/SOLARFL.ktz" method="file" shift="0.000000" />*/
    return true;
}

QString QDoasConfigFile::getName(){
    QDomElement node = qdoasfile->firstChildElement("project");
    QString result = node.attributeNode("name").value();
    /*<project name="CCA" disable="false">*/
    return result;
}



QDoasWrapper* QDoasWrapper::m_Instance = 0;


QDoasWrapper::QDoasWrapper()
{
    ms = TMaxdoasSettings::instance();
    outPath = getTempPath();
}

QDoasWrapper::~QDoasWrapper(){
    for(int i=0;i<tempFiles.count();i++){
        QFile::remove(tempFiles[i]);
    }
    delete lastposImage;
}

void QDoasWrapper::setOutPath(QString path){
    outPath = path;
}

QString QDoasWrapper::retrieve(QString in,QString defname, QDoasConfigFile *cf){
    QProcess proc;
    QStringList args;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();;
    QString xmlfile = getTempfileName("qdoasproj","xml");
    QString result;
    bool ok;
    cf->save(xmlfile);
    //-v -c claudia/Evaluation.xml -a "CCA" -o claudia/out/
    args.append("-a "+cf->getName());
    args.append("-c "+xmlfile);
    args.append("-o "+outPath);
    args.append("-f "+in);
    args.append("-v");
    env.insert("LANG","en_US.utf8");
    proc.setProcessEnvironment(env);
    proc.setStandardOutputFile("QDOAS.out");
    proc.setStandardErrorFile("QDOAS.err");
    proc.start(ms->getQDoasPath(),args);
    ok = proc.waitForFinished();
    if (!ok)
        proc.terminate();
    else{
        if (!defname.isEmpty()){
            QDir dir(outPath);
            QStringList filters;
            filters.append("*.asc");
            QStringList dl = dir.entryList(filters,QDir::Files,QDir::Time);
            if (dl.count()==0)
                ok = false;
            else{
                defname += ".asc";
                if (QFile::rename(dl[0],defname)){
                    result = defname;
                }else{
                    result = dl[0];
                }
            }
        }
    }

    QFile::remove(xmlfile);
    return result;
}

bool QDoasWrapper::retrieve(TSpectrum *spectrum, QDoasConfigFile *cf){
    QString infile = getTempfileName("qdoasspec","spe");
    spectrum->SaveSpectrumTmp(infile);
    QString result = retrieve(infile,spectrum->getFileName(),cf);
    if (lastposImage == NULL)
        delete lastposImage;
    lastposImage = NULL;
    QFile::remove(infile);
    return !result.isEmpty();
}

bool QDoasWrapper::retrieve(TSpectralImage *specImage, QDoasConfigFile *cf){
    QString infile = getTempfileName("qdoasspec","spe");
    specImage->saveTmp(infile);
    QString result = retrieve(infile,specImage->getFileName(),cf);
    if (lastposImage == NULL)
        delete lastposImage;
    lastposImage = new TRetrievalImage(specImage->getxCount(),specImage->getyCount());
    specImage->getPositionArray(lastposImage->valueBuffer,lastposImage->getWidth(),lastposImage->getHeight());
    QFile::remove(infile);
    return !result.isEmpty();
}

QScriptValue QDoasWrapper::getRetrievalImage(QString symbol){
    TRetrievalImage* img = getRetrievalImage_(symbol);
    return engine()->newQObject(img);
}

TRetrievalImage* QDoasWrapper::getRetrievalImage_(QString symbol){
    QFile asc(lastoutfile);

    if (asc.open(QIODevice::ReadOnly)){
        QTextStream in(&asc);
        QStringList symbols = in.readLine().split('\t');
        QList<double> values;
        int index = symbols.indexOf(symbol);
        do{
           QStringList fields = in.readLine().split('\t');
           double val = fields[index].toDouble();
           values.append(val);
        }while(!in.atEnd());
        if (lastposImage != NULL){
            for(int y = 0;y<lastposImage->getHeight();y++ ){
                for(int x = 0;x<lastposImage->getWidth();x++ ){
                    TRetrieval *tr = lastposImage->valueBuffer[y][x];
                    int index = tr->mirrorCoordinate->pixelIndex;
                    double val = values[index];
                    lastposImage->valueBuffer[y][x]->val = val;
                }
            }
        }
        asc.close();
    }
    return new TRetrievalImage(lastposImage);

}

double QDoasWrapper::getRetrieval(QString symbol){
    QFile asc(lastoutfile);
    double result;
    if (asc.open(QIODevice::ReadOnly)){
        QTextStream in(&asc);
        QStringList symbols = in.readLine().split('\t');
        int index = symbols.indexOf(symbol);

        QStringList fields = in.readLine().split('\t');
        result = fields[index].toDouble();
        asc.close();
    }
    return result;
}

void QDoasWrapper::appendTempFileNameToDeleteLater(QString s){
    tempFiles.append(s);
}
