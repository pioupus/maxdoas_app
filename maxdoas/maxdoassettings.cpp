
#include <QSettings>
#include <QString>
#include "maxdoassettings.h"
#include <QPointF>
#include <QMessageBox>
#include <stdint.h>

TMaxdoasSettings* TMaxdoasSettings::m_Instance = 0;

QString rawDataToSerialNumber(int guid, int devicetype){
    uint16_t year = guid >> 12; //select the 8 msbs
    year += 2000;
    uint16_t serial = guid & 0xFFF; //select the 12 lsbs
    QString ret=QString::number(year)+'.'+QString::number(devicetype)+'.'+QString::number(serial);
    if(devicetype < 0)
        ret=QString::number(year)+".x."+QString::number(serial);
    else
        ret=QString::number(year)+'.'+QString::number(devicetype)+'.'+QString::number(serial);
    return ret;
}

void serialNumberToRawData(QString serial, int* guid, int* devicetype){
    QStringList items = serial.split(".");

    uint16_t year = items[0].toInt();
    if (items[1].compare("x",Qt::CaseInsensitive) == 0)
        *devicetype = -1;
    else
        *devicetype = items[1].toInt();
    uint8_t ser = items[2].toInt();
    if (year > 2000)
        year -= 2000;
    uint32_t guid_ = year << 12;
    guid_ |= (ser & 0x0FFF);
    *guid = guid_;
}

TMaxdoasSettings::TMaxdoasSettings(){
    settings = new QSettings();
//               a.setApplicationName("MaxDoas");
//               a.setOrganizationName("CentroDeCienciasDeLaAtmosfera_UNAM");
//               a.setOrganizationDomain("www.atmosfera.unam.mx");
    getLastScannerSerial();

}

TMaxdoasSettings::~TMaxdoasSettings(){
    delete settings;
}



QString TMaxdoasSettings::getPreferredSpecSerial(){
    return settings->value("Spectrometer/PreferredSerial","").toString();
}

void TMaxdoasSettings::setPreferredSpecSerial(QString s){
    settings->setValue("Spectrometer/PreferredSerial",s);
}

int TMaxdoasSettings::getRetrievalAvgCount(){
    return settings->value("RetrievalParameters/AvgCount",1).toInt();
}
void TMaxdoasSettings::setRetrievalAvgCount(int s){
     settings->setValue("RetrievalParameters/AvgCount",s);
}

void TMaxdoasSettings::workwiththisserial(QString serial){
    serialnum = serial;
    settings->setValue("Scanner/lastSerial",serial);
}

QString TMaxdoasSettings::getLastScannerSerial(){
    serialnum = settings->value("Scanner/lastSerial","default").toString();
    return serialnum;
}
TScannerDeviceType TMaxdoasSettings::askAttachedScanningDevice(void){
    TScannerDeviceType ret = sdtUnknown;
    QMessageBox mb;
    mb.setText("The attached device type is unknown. Please select: WindfieldDOAS or MAXDOAS?");
    QPushButton* a = mb.addButton("Windfield DOAS", QMessageBox::ActionRole);
    QPushButton* b = mb.addButton("MAXDOAS", QMessageBox::ActionRole);
    mb.exec();
    if (mb.clickedButton() == (QAbstractButton*)a) {
        ret = sdtWindField;
    } else if (mb.clickedButton() == (QAbstractButton*)b) {
        ret = sdtMAXDOAS;
    }
    setAttachedScanningDevice(ret);
    return ret;
}

TScannerDeviceType TMaxdoasSettings::getAttachedScanningDevice(void){
    QString t = settings->value(serialnum+"/ScanningDevice/Type","unknown").toString();
    TScannerDeviceType dt = sdtUnknown;
    if(t.compare("maxdoas",Qt::CaseInsensitive)==0) dt = sdtMAXDOAS;
    if(t.compare("windfielddoas",Qt::CaseInsensitive)==0) dt = sdtWindField;
    return dt;
}

 void TMaxdoasSettings::setAttachedScanningDevice(TScannerDeviceType sdt){
    QString t="unknown";
    if (sdt == sdtMAXDOAS)
        t = "maxdoas";
    if (sdt == sdtWindField)
        t = "windfielddoas";
    settings->setValue(serialnum+"/ScanningDevice/Type",t);
}

bool TMaxdoasSettings::isInConfigMode(){
    int t = 0;
    t=settings->value(serialnum+"/ScanningDevice/mode",0).toInt();
    if (t==0x58F6){
        return true;
    }else{
        return false;
    }
}

QPoint TMaxdoasSettings::getTiltMaxValue(){
     return settings->value(serialnum+"/TiltSensor/TiltMaxValue",QPoint(0,0)).toPoint();
}

void TMaxdoasSettings::setTiltMaxValue(QPoint MaxVal){
    settings->setValue(serialnum+"/TiltSensor/TiltMaxValue",MaxVal);
}

QPoint TMaxdoasSettings::getTiltMinValue(){
     return settings->value(serialnum+"/TiltSensor/TiltMinValue",QPoint(0,0)).toPoint();
}

void TMaxdoasSettings::setTiltMinValue(QPoint Minval){
    settings->setValue(serialnum+"/TiltSensor/TiltMinValue",Minval);
}

QPoint TMaxdoasSettings::getTiltOffset(){
     return settings->value("TiltSensor/TiltOffset",QPoint(0,0)).toPoint();
}

void TMaxdoasSettings::setTiltOffset(QPoint TiltOffset){
    settings->setValue("TiltSensor/TiltOffset",TiltOffset);
}

//float TMaxdoasSettings::getTiltZenithDirection(){
//     return settings->value(serialnum+"/TiltSensor/TiltZenithDirection",0).toFloat();
//}

//void TMaxdoasSettings::setTiltZenithDirection(float TiltZenithDirection){
//    settings->setValue(serialnum+"/TiltSensor/TiltZenithDirection",TiltZenithDirection);
//}

QPoint TMaxdoasSettings::getTiltZenith(){
     return settings->value(serialnum+"/TiltSensor/TiltZenith",QPoint(0,0)).toPoint();
}

void TMaxdoasSettings::setTiltZenith(QPoint TiltZenith){
    settings->setValue(serialnum+"/TiltSensor/TiltZenith",TiltZenith);
}
QPoint TMaxdoasSettings::getZenithSteps(){
     return settings->value(serialnum+"/Axis/ZenithSteps",QPoint(1270,640)).toPoint();
}

void TMaxdoasSettings::setZenithSteps(QPoint ZenithPosition){
    settings->setValue(serialnum+"/Axis/ZenithSteps",ZenithPosition);
}

QPoint TMaxdoasSettings::getMicrostepping(){
     return settings->value(serialnum+"/Axis/Microsteps",QPoint(32,32)).toPoint();
}

void TMaxdoasSettings::setMicrostepping(QPoint Microsteps){
    settings->setValue(serialnum+"/Axis/Microsteps",Microsteps);
}

QPointF TMaxdoasSettings::getMotorStepAngle(){
     return settings->value(serialnum+"/Axis/MotorStepAngle",QPointF(1.8,1.8)).toPointF();
}

void TMaxdoasSettings::setMotorStepAngle(QPointF MotorStepAngle){
    settings->setValue(serialnum+"/Axis/MotorStepAngle",MotorStepAngle);
}

int TMaxdoasSettings::getShutterClosePos(){
     return settings->value(serialnum+"/Shutter/ClosePosition",0).toInt();
}

void TMaxdoasSettings::setShutterClosePos(int ClosePosition){
    settings->setValue(serialnum+"/Shutter/ClosePosition",ClosePosition);
}


QString TMaxdoasSettings::getQDoasPath(){
    return settings->value("QDOAS/Path","/home/arne/diplom/software/application/qdoas/Qdoas/qdoas/release/doas_cl").toString();
}

void TMaxdoasSettings::setQDoasPath(QString path){
    settings->setValue("QDOAS/Path",path);
}

TSPectrWLCoefficients TMaxdoasSettings::getWaveLengthCoefficients(QString serial){
    TSPectrWLCoefficients ret;
    ret.Offset = settings->value("WavelengthCoefficients/"+serial+"/Intercept",0).toDouble();
    ret.Coeff1 = settings->value("WavelengthCoefficients/"+serial+"/Coef1",1).toDouble();
    ret.Coeff2 = settings->value("WavelengthCoefficients/"+serial+"/Coef2",0).toDouble();
    ret.Coeff3 = settings->value("WavelengthCoefficients/"+serial+"/Coef3",0).toDouble();
    ret.overWrittenFromFile = true;
    ret.uninitialized = !settings->value("WavelengthCoefficients/"+serial+"/UseFileInsteadOfSpectrometer",false).toBool();
    return ret;
}

void TMaxdoasSettings::setWaveLengthCoefficients(QString serial, TSPectrWLCoefficients coef,bool lock, bool alwaysUseTheseCoef){
    bool locked;
    locked = settings->value("WavelengthCoefficients/"+serial+"/LockTheseCoefficients",false).toBool();
    if ((!locked) && (!coef.uninitialized)){
        settings->setValue("WavelengthCoefficients/"+serial+"/Intercept",coef.Offset);
        settings->setValue("WavelengthCoefficients/"+serial+"/Coef1",coef.Coeff1);
        settings->setValue("WavelengthCoefficients/"+serial+"/Coef2",coef.Coeff2);
        settings->setValue("WavelengthCoefficients/"+serial+"/Coef3",coef.Coeff3);
        settings->setValue("WavelengthCoefficients/"+serial+"/UseFileInsteadOfSpectrometer",alwaysUseTheseCoef);
        settings->setValue("WavelengthCoefficients/"+serial+"/LockTheseCoefficients",lock);
    }
}

float TMaxdoasSettings::getTargetTemperature(){
    return settings->value("TempController/Temperature",20.0).toFloat();
}

void TMaxdoasSettings::setTargetTemperature(float Temperature){
    settings->setValue("TempController/Temperature",Temperature);
}

TCOMPortConf TMaxdoasSettings::getComPortConfiguration(){
    TCOMPortConf ret;
    ret.valid = true;
    ret.Name = settings->value("COMPort/Name","/dev/ttyUSB0").toString();
    ret.SysPath = settings->value("COMPort/SysPath","").toString();
    ret.ByName = settings->value("COMPort/SelectByName","").toBool();
    return ret;
}

void TMaxdoasSettings::setComPortConfiguration(TCOMPortConf ComPortConfiguration){
    settings->setValue("COMPort/Name",ComPortConfiguration.Name);
    settings->setValue("COMPort/SysPath",ComPortConfiguration.SysPath);
    settings->setValue("COMPort/SelectByName",ComPortConfiguration.ByName);
}

TAutoIntegConf TMaxdoasSettings::getAutoIntegrationRetrievalConf(){
    TAutoIntegConf ret;
    ret.autoenabled = settings->value("RetrievalParameters/AutoIntegration/AutoEnabled",false).toBool();
    ret.fixedIntegtime = settings->value("RetrievalParameters/AutoIntegration/fixedTime",10000).toInt();
    ret.maxIntegTime = settings->value("RetrievalParameters/AutoIntegration/maxTime",1000000).toInt();
    ret.targetCorridor = settings->value("RetrievalParameters/AutoIntegration/targetCorridor",10).toFloat();
    ret.targetPeak = settings->value("RetrievalParameters/AutoIntegration/targetPeak",50).toFloat();
    return ret;
}

void TMaxdoasSettings::setAutoIntegrationRetrievalConf(TAutoIntegConf AutoIntegrationConf){
    settings->setValue("RetrievalParameters/AutoIntegration/AutoEnabled",AutoIntegrationConf.autoenabled);
    settings->setValue("RetrievalParameters/AutoIntegration/fixedTime",AutoIntegrationConf.fixedIntegtime);
    settings->setValue("RetrievalParameters/AutoIntegration/maxTime",AutoIntegrationConf.maxIntegTime);
    settings->setValue("RetrievalParameters/AutoIntegration/targetCorridor",AutoIntegrationConf.targetCorridor);
    settings->setValue("RetrievalParameters/AutoIntegration/targetPeak",AutoIntegrationConf.targetPeak);
}

QString TMaxdoasSettings::getSiteName(){
    return settings->value("SiteInfo/SiteName","").toString();
}

void TMaxdoasSettings::setSiteName(QString SiteName){
    settings->setValue("SiteInfo/SiteName",SiteName);
}

float TMaxdoasSettings::getScannerDirection(){
    return settings->value("SiteInfo/ScannerOrientation","").toFloat();
}

void TMaxdoasSettings::setScannerDirection(float ScannerDirection){
    settings->setValue("SiteInfo/ScannerOrientation",ScannerDirection);
}

float TMaxdoasSettings::getSiteLatitude(){
    return settings->value("SiteInfo/Latitude",0.0).toFloat();
}

void TMaxdoasSettings::setSiteLatitude(float SiteLatitude){
    settings->setValue("SiteInfo/Latitude",SiteLatitude);
}

float TMaxdoasSettings::getSiteLongitude(){
    return settings->value("SiteInfo/Longitude",0.0).toFloat();
}

void TMaxdoasSettings::setSiteLongitude(float SiteLongitude){
    settings->setValue("SiteInfo/Longitude",SiteLongitude);
}

