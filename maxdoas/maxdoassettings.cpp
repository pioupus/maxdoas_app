
#include <QSettings>
#include <QString>
#include "maxdoassettings.h"
#include <QPointF>

TMaxdoasSettings* TMaxdoasSettings::m_Instance = 0;

TMaxdoasSettings::TMaxdoasSettings(){
    settings = new QSettings();
//               a.setApplicationName("MaxDoas");
//               a.setOrganizationName("CentroDeCienciasDeLaAtmosfera_UNAM");
//               a.setOrganizationDomain("www.atmosfera.unam.mx");
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

QPoint TMaxdoasSettings::getTiltOffset(){
     return settings->value("TiltSensor/TiltOffset",QPoint(0,0)).toPoint();
}

void TMaxdoasSettings::setTiltOffset(QPoint TiltOffset){
    settings->setValue("TiltSensor/TiltOffset",TiltOffset);
}

QString TMaxdoasSettings::getQDoasPath(){
    return settings->value("QDOAS/Path","../qdoas").toString();
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
