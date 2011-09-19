#include <typeinfo>
#include <QTextStream>
#include <QFile>
#include <QTest>
#include <QObject>
#include <QScriptValue>
#include "tspectrum.h"
#include "scriptwrapper.h"
#include "tmirrorcoordinate.h"
#include "tspectralimage.h"
#include "tscanpath.h"

TScanner* TScanner::m_Instance = 0;

bool ScriptAborting;

TScanner::TScanner(){
}

void TScanner::setHWDriver(THWDriver* hwdriver){
    this->hwdriver = hwdriver;
    connect(hwdriver,SIGNAL(hwdSigGotSpectrum()),this,SLOT(on_GotSpectrum()));
}

THWDriver *TScanner::getHWDriver(){
    return hwdriver;
}

TScanner::~TScanner(){

}

void TScanner::startWaiting(){
    GotSpectrum = false;
}

void TScanner::WaitForSpectrum(){
    while(!GotSpectrum){
        QTest::qWait(1);
    }
}

void TScanner::on_GotSpectrum(){
    GotSpectrum = true;
}

QScriptValue DoSleep(QScriptContext *context, QScriptEngine *engine)
{
    if (ScriptAborting){
        engine->abortEvaluation();
        return 0;
    }
    QScriptValue ms = context->argument(0);
    QTest::qSleep(ms.toNumber());
    return 0;
}

QScriptValue isAborting(QScriptContext *context, QScriptEngine *engine)
{
    (void)context;
    (void)engine;
    return ScriptAborting;

}



QScriptValue TSpectrumConstructor(QScriptContext *context, QScriptEngine *engine)
{
    QObject *parent = context->argument(0).toQObject();
    QObject *object = new TSpectrum(parent);
    return engine->newQObject(object, QScriptEngine::ScriptOwnership);
}

QScriptValue TSpectralImageConstructor(QScriptContext *context, QScriptEngine *engine)
{
    QObject *parent = context->argument(0).toQObject();
    QObject *object = new TSpectralImage(parent);
    return engine->newQObject(object, QScriptEngine::ScriptOwnership);
}

QScriptValue TScanPathConstructor(QScriptContext *context, QScriptEngine *engine)
{
    QObject *parent = context->argument(0).toQObject();
    QObject *object = new TScanPath(parent);
    return engine->newQObject(object, QScriptEngine::ScriptOwnership);
}

QScriptValue TMirrorCoordinateConstructor(QScriptContext *context, QScriptEngine *engine)
{
    QObject *parent = context->argument(0).toQObject();
    QObject *object = new TMirrorCoordinate(parent);
    return engine->newQObject(object, QScriptEngine::ScriptOwnership);
}

QScriptValue SetAutoIntegrationTime(QScriptContext *context, QScriptEngine *engine)
{
    (void)engine;
    TAutoIntegConf AutoConf;
    TScanner* scanner = TScanner::instance(NULL);
    THWDriver* hwDriver = scanner->getHWDriver();

    float targetPeak = context->argument(0).toNumber();
    float targetCorridor = context->argument(1).toNumber();
    uint maxIntegTime = context->argument(2).toInteger();
    AutoConf.autoenabled = true;
    AutoConf.targetPeak = targetPeak;
    AutoConf.maxIntegTime = maxIntegTime;
    AutoConf.targetCorridor = targetCorridor;
    hwDriver->setIntegrationConfiguration(&AutoConf);
    return 0;
}

QScriptValue SetFixedIntegrationTime(QScriptContext *context, QScriptEngine *engine)
{
    (void)engine;
    TAutoIntegConf AutoConf;
    TScanner* scanner = TScanner::instance(NULL);
    THWDriver* hwDriver = scanner->getHWDriver();

    uint fixedIntegTime = context->argument(0).toNumber();
    AutoConf.autoenabled = false;
    AutoConf.fixedIntegtime = fixedIntegTime;
    hwDriver->setIntegrationConfiguration(&AutoConf);
    return 0;
}

QScriptValue FreeObject(QScriptContext *context, QScriptEngine *engine)
{
    (void)context;
    (void)engine;
    QObject *obj = context->argument(0).toQObject();
    TSpectrum *Spektrum = dynamic_cast<TSpectrum*>(obj);
    if (Spektrum != NULL)
        delete Spektrum;
    return 0;
}

QScriptValue MeasureSpektrum(QScriptContext *context, QScriptEngine *engine)
{
    THWShutterCMD shuttercmd;
    TScanner* scanner = TScanner::instance(NULL);
    THWDriver* hwDriver = scanner->getHWDriver();

    //pos: TMirrorCoordinate
    //avg: int
    //shutter: bool
    //plot: bool
    QObject *MirrorCoordQObj = context->argument(0).toQObject();
    TMirrorCoordinate *MirrorCoord = dynamic_cast<TMirrorCoordinate*>(MirrorCoordQObj);
    uint avg = context->argument(1).toInteger();
    if (avg < 1)
        avg = 1;
    bool shutter = context->argument(2).toBool();
    bool plot = context->argument(3).toBool();
    (void)plot;
    if (shutter){
        shuttercmd = scOpen;
    }else{
        shuttercmd = scClose;
    }
    shuttercmd = scNone;
    if( MirrorCoord != NULL){
        scanner->startWaiting();
        hwDriver->hwdMeasureSpectrum(avg,0,shuttercmd);
//        hwDriver->hwdMeasureScanPixel(MirrorCoord->getMotorCoordinate(),avg,0);
        scanner->WaitForSpectrum();
    }
    TSpectrum *spektrum = new TSpectrum(NULL);
    hwDriver->hwdGetSpectrum(spektrum);
    return engine->newQObject(spektrum, QScriptEngine::ScriptOwnership);
}





TScriptWrapper::TScriptWrapper(THWDriver* hwdriver)
{
    scanner = TScanner::instance(hwdriver);
    ScriptEngine = new QScriptEngine();
    ScriptAborting = false;
    ScriptEngine->setProcessEventsInterval(1);

    QScriptValue SleepFun = ScriptEngine->newFunction(DoSleep);
    ScriptEngine->globalObject().setProperty("sleep", SleepFun);

    QScriptValue isAbortingFun = ScriptEngine->newFunction(isAborting);
    ScriptEngine->globalObject().setProperty("isAborting", isAbortingFun);

    QScriptValue isAbortingFree = ScriptEngine->newFunction(FreeObject);
    ScriptEngine->globalObject().setProperty("free", isAbortingFree);


    QScriptValue SetAutoIntegrationTimeFun = ScriptEngine->newFunction(SetAutoIntegrationTime);
    ScriptEngine->globalObject().setProperty("SetAutoIntegrationTime", SetAutoIntegrationTimeFun);

    QScriptValue SetFixedIntegrationTimeFun = ScriptEngine->newFunction(SetFixedIntegrationTime);
    ScriptEngine->globalObject().setProperty("SetFixedIntegrationTime", SetFixedIntegrationTimeFun);

    QScriptValue MeasureSpektrumFun = ScriptEngine->newFunction(MeasureSpektrum);
    ScriptEngine->globalObject().setProperty("MeasureSpektrum", MeasureSpektrumFun);

    QScriptValue ctorSpec = ScriptEngine->newFunction(TSpectrumConstructor);
    QScriptValue metaObjectSpec = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, ctorSpec);
    ScriptEngine->globalObject().setProperty("Spektrum", metaObjectSpec);

    QScriptValue ctorSpImg = ScriptEngine->newFunction(TSpectralImageConstructor);
    QScriptValue metaObjectSpImg = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, ctorSpImg);
    ScriptEngine->globalObject().setProperty("SpektralImage", metaObjectSpImg);

    QScriptValue ctorscpt = ScriptEngine->newFunction(TScanPathConstructor);
    QScriptValue metaObjectscpt = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, ctorscpt);
    ScriptEngine->globalObject().setProperty("Scanpath", metaObjectscpt);

    QScriptValue ctormc = ScriptEngine->newFunction(TMirrorCoordinateConstructor);
    QScriptValue metaObjectmc = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, ctormc);
    ScriptEngine->globalObject().setProperty("Mirrorcoordinate", metaObjectmc);


}





TScriptWrapper::~TScriptWrapper()
{
    TScanner::drop();
    delete ScriptEngine;
}

QScriptEngine * TScriptWrapper::getScriptEngine()
{
    return ScriptEngine;
}

void TScriptWrapper::startScriptFile(QString fn){

    QFile data(fn);
    if (data.open(QFile::ReadOnly | QFile::Truncate)) {
        QTextStream file(&data);
        ScriptAborting = false;
        ScriptEngine->evaluate(file.readAll(),"test.qs");
    }
}

void TScriptWrapper::startScript(QString test){
    ScriptAborting = false;
    ScriptEngine->evaluate(test);
}

void TScriptWrapper::abort(){
    ScriptAborting = true;
    ScriptEngine->setProcessEventsInterval(1);
    ScriptEngine->abortEvaluation();
}
