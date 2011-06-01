
#include <QSettings>
#include <QString>
#include "maxdoassettings.h"

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

int TMaxdoasSettings::getRetrievalIntegTimeuS(){
    return settings->value("RetrievalParameters/IntegrTime_us",1000).toInt();
}

void TMaxdoasSettings::setRetrievalIntegTimeuS(int uS){
    settings->setValue("RetrievalParameters/IntegrTime_us",uS);
}

bool TMaxdoasSettings::getRetrievalAutoEnabled(){
    return settings->value("RetrievalParameters/AutoIntegrationTime",false).toBool();
}

void TMaxdoasSettings::setRetrievalAutoEnabled(bool i){
     settings->setValue("RetrievalParameters/AutoIntegrationTime",i);
}

int TMaxdoasSettings::getRetrievalAutoTargetPeak(){
    return settings->value("RetrievalParameters/AutoTargetPeak",3000).toInt();
}

void TMaxdoasSettings::setRetrievalAutoTargetPeak(int i){
    settings->setValue("RetrievalParameters/AutoTargetPeak",i);
}

int TMaxdoasSettings::getRetrievalAutoTargetPeakCorridor(){
    return settings->value("RetrievalParameters/AutoTargetPeakCorridor",1000).toInt();
}

void TMaxdoasSettings::setRetrievalAutoTargetPeakCorridor(int i){
    settings->setValue("RetrievalParameters/AutoTargetPeakCorridor",i);
}

int TMaxdoasSettings::getRetrievalAutoMaxIntegTime(){
    return settings->value("RetrievalParameters/AutoMaxIntegTime",10000).toInt();
}

void TMaxdoasSettings::setRetrievalAutoMaxIntegTime(int i){
    settings->setValue("RetrievalParameters/AutoMaxIntegTime",i);
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
