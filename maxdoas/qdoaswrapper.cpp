#include "qdoaswrapper.h"
#include <QProcess>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QScriptEngine>
#include <QScriptContext>
#include <QScriptValue>
#include <QDebug>

QString getTempPath(){
    QDir dir = QDir::temp();
    if (!dir.cd("qdoas"))
        dir.mkdir("qdoas");
    dir.cd("qdoas");
    return dir.absolutePath();
}

QString MakeTempfileName(QString prefix,QString suffix, qint64 time,int random){

    QByteArray num;
    char c;
    qint64 t;
    for(int i = 0; i != sizeof(time); ++i){
        t = time & (0xFF << i*8);
        c = t >> i*8;
        if (i > 3)//we dont use al 64 bits..
            break;
        num.append(c);
    }
    for(int i = 0; i != sizeof(random); ++i){
        t = random & (0xFF << i*8);
        c = t >> i*8;
        num.append(c);
    }
    return prefix+num.toHex()+"."+suffix;
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

QDoasConfigFile::QDoasConfigFile(QDoasConfigFile * other){
    qdoasfile = new QDomDocument("QDoasConfig");
    qdoasfile->setContent(other->qdoasfile->toString());
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

QString getFileName(QString fn){
    QFileInfo fi(fn);
    return fi.baseName()+"."+fi.completeSuffix();
}

QString joinDirectoryFN(QString Directory, QString fn){
    if (!Directory.endsWith("/"))
        Directory = Directory+"/";
    return Directory+fn;
}

bool QDoasConfigFile::saveWorkingCopy(QString Directory,QString Filename, QString inDirectory){
    QDir dir;
    dir.mkpath(Directory);
    dir.cd(Directory);
    QDoasConfigFile *other = new QDoasConfigFile(this);
    QString OffsFN = getOffset();
    QString OffsFNn = joinDirectoryFN(Directory,getFileName(OffsFN));
    QString XSRefFN = getXSRef();
    QString XSRefFNn = joinDirectoryFN(Directory,getFileName(XSRefFN));
    QString CalRefFN = getCalRef();
    QString CalRefFNn = joinDirectoryFN(Directory,getFileName(CalRefFN));
    QString USAMPRefFN = getUSAMPRef();
    QString USAMPRefFNn = joinDirectoryFN(Directory,getFileName(USAMPRefFN));
    QFile::copy(OffsFN,OffsFNn);
    QFile::copy(XSRefFN,XSRefFNn);
    QFile::copy(CalRefFN,CalRefFNn);
    QFile::copy(USAMPRefFN,USAMPRefFNn);
    other->setOffset(OffsFNn);
    other->setXSRef(XSRefFNn);
    other->setCalRef(CalRefFNn);
    other->setUSAMPRef(USAMPRefFNn);
    if (!inDirectory.isEmpty())
        other->setInputDirectory(inDirectory,"*s.spe");
    other->save(joinDirectoryFN(Directory,getFileName(Filename)));
    delete other;
    return true;
}

bool QDoasConfigFile::setInputDirectory(QString fn,QString filter){
    QDir Dir(fn);
    fn = Dir.absolutePath()+"/";
    QDomElement raw_spectraElement;
    QDomElement directoryElement;
    QDomElement qdoas = qdoasfile->firstChildElement("qdoas");
    QDomElement proj = qdoas.firstChildElement("project");
    raw_spectraElement = proj.firstChildElement("raw_spectra");
    directoryElement = raw_spectraElement.firstChildElement("directory");
    directoryElement.setAttribute("name",fn);
    directoryElement.setAttribute("filters",filter);
    /*<raw_spectra>
      <!-- Disable file, folder and directory items with the disable set equal to "true". -->
      <!--  The default is enabled.                                                       -->
      <directory name="/home/arne/.../2011.02.20" filters="" recursive="false" />
    </raw_spectra>*/

    return !directoryElement.tagName().isEmpty();
}

QString QDoasConfigFile::getOffset(){
    QDomElement qdoas = qdoasfile->firstChildElement("qdoas");
    QDomElement proj = qdoas.firstChildElement("project");
    QDomElement instrumentalElement = proj.firstChildElement("instrumental");
    QDomElement mfcstdElement = instrumentalElement.firstChildElement("mfcstd");

    /*<instrumental format="mfcstd" site="CCA_UNAM">
      <mfcstd size="2048" revert="false" straylight="false" date="DD.MM.YYYY" lambda_min="0" lambda_max="0" calib="/home/arne/.../ADUD6545/ADUD6545(c).clb" instr="" dark="" offset="/home/arne/.../offsetd01042011h074025.std" />
      */
    return mfcstdElement.attributeNode("offset").value();
}

bool QDoasConfigFile::setOffset(QString SpecFn){
    QDir Dir(SpecFn);
    SpecFn = Dir.absolutePath();
    QDomElement qdoas = qdoasfile->firstChildElement("qdoas");
    QDomElement proj = qdoas.firstChildElement("project");
    QDomElement instrumentalElement = proj.firstChildElement("instrumental");
    QDomElement mfcstdElement = instrumentalElement.firstChildElement("mfcstd");
    mfcstdElement.setAttribute("offset",SpecFn);

    /*<instrumental format="mfcstd" site="CCA_UNAM">
      <mfcstd size="2048" revert="false" straylight="false" date="DD.MM.YYYY" lambda_min="0" lambda_max="0" calib="/home/arne/.../ADUD6545/ADUD6545(c).clb" instr="" dark="" offset="/home/arne/.../offsetd01042011h074025.std" />
      */
    return !mfcstdElement.tagName().isEmpty();
}

bool QDoasConfigFile::setOffset(TSpectrum *Offset){
    QString SpecFn = getTempfileName("offset","std");
    Offset->SaveSpectrumSTD(SpecFn);
    return setOffset(SpecFn);
}

QString QDoasConfigFile::getXSRef(){
    QDomElement qdoas = qdoasfile->firstChildElement("qdoas");
    QDomElement proj = qdoas.firstChildElement("project");
    QDomNodeList nodeList = proj.elementsByTagName("analysis_window");
    QString result;
    for(int i=0; i < nodeList.count();i++){
        QDomElement filesElement;
        filesElement = nodeList.item(i).firstChildElement("files");
        result = filesElement.attributeNode("refone").value();
    }
    return result;
}

bool QDoasConfigFile::setXSRef(QString SpecFn){
    QDir Dir(SpecFn);
    SpecFn = Dir.absolutePath();
    QDomElement qdoas = qdoasfile->firstChildElement("qdoas");
    QDomElement proj = qdoas.firstChildElement("project");
    QDomNodeList nodeList = proj.elementsByTagName("analysis_window");
    bool result = true;
    for(int i=0; i < nodeList.count();i++){
        QDomElement filesElement;
        filesElement = nodeList.item(i).firstChildElement("files");
        filesElement.setAttribute("refone",SpecFn);
        result = !filesElement.tagName().isNull();
    }
    return result;
}

bool QDoasConfigFile::setXSRef(QString calibfn, TSpectrum *Spectrum){
    QString SpecFn = getTempfileName("xsref","ref");
    Spectrum->SaveSpectrumRef(calibfn,SpecFn);
    return setXSRef(SpecFn);
}

QString QDoasConfigFile::getCalRef(){

    QDomElement qdoas = qdoasfile->firstChildElement("qdoas");
    QDomElement proj = qdoas.firstChildElement("project");
    QDomElement node = proj.firstChildElement("calibration");

    /*    <calibration ref="/home/arne/.../ADUD6545/WD_XS/original_XS/SOLARFL.ktz" method="ODF">*/

    return node.attributeNode("ref").value();
}

bool QDoasConfigFile::setCalRef(QString SpecFn){
    QDir Dir(SpecFn);
    SpecFn = Dir.absolutePath();
    QDomElement qdoas = qdoasfile->firstChildElement("qdoas");
    QDomElement proj = qdoas.firstChildElement("project");
    QDomElement node = proj.firstChildElement("calibration");
    node.setAttribute("ref",SpecFn);

    /*    <calibration ref="/home/arne/.../ADUD6545/WD_XS/original_XS/SOLARFL.ktz" method="ODF">*/

    return !node.tagName().isNull();;
}

bool QDoasConfigFile::setCalRef(QString calibfn, TSpectrum *Spectrum){
    QString SpecFn = getTempfileName("calref","ref");
    Spectrum->SaveSpectrumRef(calibfn,SpecFn);
    return setCalRef(SpecFn);
}

QString QDoasConfigFile::getUSAMPRef(){

    QDomElement qdoas = qdoasfile->firstChildElement("qdoas");
    QDomElement proj = qdoas.firstChildElement("project");
    QDomElement node = proj.firstChildElement("undersampling");
    /*<undersampling ref="/home/arne/../ADUD6545/WD_XS/original_XS/SOLARFL.ktz" method="file" shift="0.000000" />*/
    return node.attributeNode("ref").value();
}


bool QDoasConfigFile::setUSAMPRef(QString SpecFn){
    QDir Dir(SpecFn);
    SpecFn = Dir.absolutePath();
    QDomElement qdoas = qdoasfile->firstChildElement("qdoas");
    QDomElement proj = qdoas.firstChildElement("project");
    QDomElement node = proj.firstChildElement("undersampling");
    node.setAttribute("ref",SpecFn);
    /*<undersampling ref="/home/arne/../ADUD6545/WD_XS/original_XS/SOLARFL.ktz" method="file" shift="0.000000" />*/
    return !node.tagName().isNull();
}


bool QDoasConfigFile::setUSAMPRef(QString calibfn, TSpectrum *Spectrum){
    QString SpecFn = getTempfileName("usampref","ref");
    Spectrum->SaveSpectrumRef(calibfn,SpecFn);
    return setUSAMPRef(SpecFn);
}

QString QDoasConfigFile::getName(){
    QDomElement qdoas = qdoasfile->firstChildElement("qdoas");
    QDomElement node = qdoas.firstChildElement("project");
    QString result = node.attributeNode("name").value();
    /*<project name="CCA" disable="false">*/
    return result;
}

bool QDoasConfigFile::enableSpecNo(){
    QDomElement qdoas = qdoasfile->firstChildElement("qdoas");
    QDomElement node = qdoas.firstChildElement("project");
    QDomElement display = node.firstChildElement("display");
    QDomNodeList nodeList = display.elementsByTagName("field");
    bool found = false;
    for(int i=0; i < nodeList.count();i++){
        QDomNamedNodeMap dm = nodeList.item(i).attributes();
        QDomNode dn  = dm.namedItem("name");
        QString val = dn.nodeValue();
        if(val == QString("specno")){
            found = true;
            break;
        }
    }
    if(!found){
        QDomElement specno = qdoasfile->createElement("field");
        specno.setAttribute("name","specno");
        display.appendChild(specno);
    }
//    QString result = display.;
    /*<project name="CCA" disable="false">
        <display fits="true" spectra="true" data="true">
           <field name="specno"/>*/
    return true;
}

bool QDoasConfigFile::enable4ASCIIFields(){
    QDomElement qdoas = qdoasfile->firstChildElement("qdoas");
    QDomElement node = qdoas.firstChildElement("project");
    QDomElement instrumental = node.firstChildElement("instrumental");
    QDomElement ascii = instrumental.firstChildElement("ascii");
    ascii.setAttribute("lambda","false");
    ascii.setAttribute("zen","false");
    ascii.setAttribute("azi","true");
    ascii.setAttribute("time","true");
    ascii.setAttribute("date","true");
    ascii.setAttribute("ele","true");
    /*<instrumental format="ascii" site="CCA_UNAM">
    <ascii calib="/home/arne/diplom/software/application/qdoas/projects/scanner/USB2G10713/USB2G10713.clb" size="2048"
    format="line" instr="" ele="true" lambda="false" zen="false" azi="true" time="true" date="true"/>*/
    return true;
}





QDoasWrapper* QDoasWrapper::m_Instance = 0;


QDoasWrapper::QDoasWrapper()
{
    ms = TMaxdoasSettings::instance();
    outPath = getTempPath()+"/";
    lastposImage = NULL;
}

QDoasWrapper::~QDoasWrapper(){
    for(int i=0;i<tempFiles.count();i++){
        QFile::remove(tempFiles[i]);
    }
    delete lastposImage;
}

void QDoasWrapper::setOutPath(QString path){
    if (!path.endsWith("/"))
        path += "/";
    outPath = path;
}

QString QDoasWrapper::retrieve(QString in,QString defname, QDoasConfigFile *cf){
    QProcess proc;
    QStringList args;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();;
    QString xmlfile = getTempfileName("qdoasproj","xml");
    QString result;
    QDir dir(in);
    bool ok;
    cf->setInputDirectory(dir.currentPath(),"*s.spe");
    cf->enableSpecNo();
    cf->enable4ASCIIFields();
    cf->save(xmlfile);
    //-v -c claudia/Evaluation.xml -a "CCA" -o claudia/out/
    args << "-c" << xmlfile;
    args << "-a" << cf->getName();
    args << "-o" << outPath;
    args << "-f" << in;
    //args.append("-v");
    env.insert("LANG","en_US.UTF8");
    proc.setProcessEnvironment(env);
    proc.setStandardOutputFile("QDOAS.out");
    proc.setStandardErrorFile("QDOAS.err");
    qDebug() << ms->getQDoasPath()+" -c "+xmlfile+" -a "+cf->getName()+" -o "+outPath+" -f "+in;
    //proc.start(ms->getQDoasPath()+" -c "+xmlfile+" -a "+cf->getName()+" -o "+outPath+" -f "+in,args);
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
                if (QFile::rename(outPath+dl[0],defname)){
                    result = defname;
                }else{
                    result = outPath+dl[0];
                }
            }
        }
    }

    QFile::remove(xmlfile);
    lastoutfile = result;
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
    if (lastposImage != NULL)
        delete lastposImage;
    lastposImage = specImage->getIntensityImage();
//    lastposImage = new TRetrievalImage(specImage->getxCount(),specImage->getyCount());
//    specImage->getPositionArray(lastposImage->valueBuffer,lastposImage->getWidth(),lastposImage->getHeight());
    lastposImage->datetime = specImage->getDateTime();
    QFile::remove(infile);
    return !result.isEmpty();
}

QScriptValue QDoasWrapper::getRetrievalImage(QString symbol){
    TRetrievalImage* img = getRetrievalImage_(symbol);
    return engine()->newQObject(img);
}

TRetrievalImage* QDoasWrapper::getRetrievalImage_(QString symbol){
    QFile asc(lastoutfile);
    bool ok = true;
    if (asc.open(QIODevice::ReadOnly)){
        QTextStream in(&asc);
        bool end;
        QString l;
        QString ll;
        int lastpos=0;
        while (!end){
            ll = l;
            lastpos = in.pos();
            l = in.readLine();
            if (!l.startsWith("#"))
                end = true;
        }
        in.seek(lastpos);
        QStringList symbols = ll.split('\t');
        QMap<int, double> values;
        int index = symbols.indexOf(symbol);
        int specnoindex = symbols.indexOf("Spec No");
        do{
           QStringList fields = in.readLine().split('\t');
           double val = fields[index].toDouble();
           int specno = fields[specnoindex].toDouble();
           values.insert(specno,val);
        }while(!in.atEnd());
        if (lastposImage != NULL){
            for(int y = 0;y<lastposImage->getHeight();y++ ){
                for(int x = 0;x<lastposImage->getWidth();x++ ){
                    TRetrieval *tr = lastposImage->valueBuffer[y][x];
                    int index = tr->mirrorCoordinate->pixelIndex;
                    if (index > values.count())
                        ok = false;
                    else{
                        double val = values[index];
                        lastposImage->valueBuffer[y][x]->val = val;
                        lastposImage->valueBuffer[y][x]->origval = val;
                    }
                }
            }
        }
        asc.close();
    }

    if (ok){
        return new TRetrievalImage(lastposImage);
    }else{
        return NULL;
    }

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
