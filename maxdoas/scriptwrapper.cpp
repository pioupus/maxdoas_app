#include <typeinfo>
#include <QTextStream>
#include <QFile>
#include <QTest>
#include <QObject>
#include <QScriptValue>
#include <QDir>
#include "tspectrum.h"
#include "scriptwrapper.h"
#include "tmirrorcoordinate.h"
#include "tspectralimage.h"
#include "tscanpath.h"
#include "tspectrumplotter.h"
#include "qdoaswrapper.h"
#include "tdirlist.h"
#include "tvectorsolver.h"

TScanner* TScanner::m_Instance = 0;

bool ScriptAborting;

TScanner::TScanner(){
}

void TScanner::setHWDriver(THWDriver* hwdriver){
    this->hwdriver = hwdriver;
    connect(hwdriver,SIGNAL(hwdSigGotSpectrum()),this,SLOT(on_GotSpectrum()));
    connect(hwdriver,SIGNAL(hwdSigMotMoved()),this,SLOT(on_MotMoved()));
    connect(hwdriver,SIGNAL(hwdSigMotorIsHome()),this,SLOT(on_MotMoved()));
    connect(hwdriver,SIGNAL(hwdSigMotFailed()),this,SLOT(on_MotFailed()));

    connect(hwdriver,SIGNAL(hwdSigTransferDone(THWTransferState,uint)),this,SLOT(on_MotTimeOut(THWTransferState,uint)));


}

THWDriver *TScanner::getHWDriver(){
    return hwdriver;
}

TScanner::~TScanner(){

}

QString TScanner::getSpectSerialNo(){
    return hwdriver->getSpectrSerial();
}

void TScanner::startWaiting(){
    GotSpectrum = false;
    MotMoved = false;
}

void TScanner::WaitForSpectrum(){
    while(!GotSpectrum){
        if (ScriptAborting)
            break;
        QTest::qWait(1);
    }
}

void TScanner::WaitForMotMoved(){
    while(!MotMoved){
        if (ScriptAborting)
            break;
        QTest::qWait(1);
    }
}


void TScanner::on_GotSpectrum(){
    GotSpectrum = true;
}

void TScanner::on_MotFailed(){
    MotMoved = true;
}

void TScanner::on_MotMoved(){
    MotMoved = true;
}

void TScanner::on_MotTimeOut(THWTransferState TransferState, uint ErrorParameter){
    (void)ErrorParameter;
    if(TransferState == tsTimeOut)
        MotMoved = true;
}

QScriptValue DoSleep(QScriptContext *context, QScriptEngine *engine)
{
    if (ScriptAborting){
        engine->abortEvaluation();
        return 0;
    }
    QScriptValue ms = context->argument(0);
    QTest::qWait(ms.toNumber());
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
    QObject *obj = context->argument(0).toQObject();
    TScanPath *sp = dynamic_cast<TScanPath*>(obj);
    QObject *object = new TSpectralImage(sp);
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

QScriptValue QDoasConfigFileConstructor(QScriptContext *context, QScriptEngine *engine)
{
    QString sp = context->argument(0).toString();
    QObject *object = new QDoasConfigFile(sp);
    return engine->newQObject(object, QScriptEngine::ScriptOwnership);
}

QScriptValue TDirListConstructor(QScriptContext *context, QScriptEngine *engine)
{
    QString sp = context->argument(0).toString();
    int startindex = context->argument(1).toInteger();
    QObject *object = new tdirlist(sp,startindex,"CCA");
    return engine->newQObject(object, QScriptEngine::ScriptOwnership);
}

QScriptValue TDirListConstructorSIGIS(QScriptContext *context, QScriptEngine *engine)
{
    QString sp = context->argument(0).toString();
    QObject *object = new tdirlist(sp,1,"SIGIS");
    return engine->newQObject(object, QScriptEngine::ScriptOwnership);
}

QScriptValue TRetrievalImageConstructorSIGIS(QScriptContext *context, QScriptEngine *engine)
{
    QString FileName = context->argument(0).toString();
    QObject *object = new TRetrievalImage(FileName,"SIGIS");
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
    if (Spektrum != NULL){
        delete Spektrum;
    }else{
        TSpectralImage *spimg = dynamic_cast<TSpectralImage*>(obj);
        if (spimg != NULL){
            delete spimg;
        }else{
           TRetrievalImage *rtimg = dynamic_cast<TRetrievalImage*>(obj);
           if(rtimg != NULL){
               delete rtimg;
           }else{
               QDoasConfigFile *qdconf = dynamic_cast<QDoasConfigFile*>(obj);
               if(qdconf != NULL){
                   delete qdconf;
               }else{
                   tdirlist *dl = dynamic_cast<tdirlist*>(obj);
                   if(dl != NULL){
                       delete dl;
                   }
               }
           }
        }
    }
    return 0;
}

QScriptValue MeasureSpektrum(QScriptContext *context, QScriptEngine *engine)
{
    if (!ScriptAborting){
        THWShutterCMD shuttercmd;
        TScanner* scanner = TScanner::instance(NULL);
        THWDriver* hwDriver = scanner->getHWDriver();

        //pos: TMirrorCoordinate
        //avg: int
        //shutter: bool

        uint avg = context->argument(0).toInteger();
        if (avg < 1)
            avg = 1;
        bool shutter = context->argument(1).toBool();
        if (!context->argument(1).isBoolean()){
            shutter = true;
        }

        if (shutter){
            shuttercmd = scOpen;
        }else{
            shuttercmd = scClose;
        }
        shuttercmd = scNone;
        //if( MirrorCoord != NULL){
            scanner->startWaiting();
            hwDriver->hwdMeasureSpectrum(avg,0,shuttercmd);
    //        hwDriver->hwdMeasureScanPixel(MirrorCoord->getAngleCoordinate(),avg,0);
            scanner->WaitForSpectrum();
      //  }
        TSpectrum *spektrum = new TSpectrum();
        hwDriver->hwdGetSpectrum(spektrum);
//        if (MirrorCoord != NULL){
//            spektrum->setMirrorCoordinate(MirrorCoord);
//        }
        return engine->newQObject(spektrum, QScriptEngine::QtOwnership);
    }else{
        return 0;
    }
}


QScriptValue GetMinimumIntegrationTime(QScriptContext *context, QScriptEngine *engine)
{
    (void)engine;
    (void)context;
    int mininteg;
    if (!ScriptAborting){
        TScanner* scanner = TScanner::instance(NULL);
        THWDriver* hwDriver = scanner->getHWDriver();
        mininteg = hwDriver->hwdGetMinimumIntegrationTime();
        return mininteg;
    }else{
        return 0;
    }
}

QScriptValue MotMove(QScriptContext *context, QScriptEngine *engine)
{
    (void)engine;
    if (!ScriptAborting){
        TScanner* scanner = TScanner::instance(NULL);
        THWDriver* hwDriver = scanner->getHWDriver();

        //pos: TMirrorCoordinate

        QObject *MirrorCoordQObj = context->argument(0).toQObject();
        TMirrorCoordinate *MirrorCoord;
        if (MirrorCoordQObj == NULL){
            QPointF p;
            p.setX(context->argument(0).toNumber());
            p.setY(context->argument(1).toNumber());
            MirrorCoord = new TMirrorCoordinate(p);
        }else{
            MirrorCoord = dynamic_cast<TMirrorCoordinate*>(MirrorCoordQObj);
        }
        if( MirrorCoord != NULL){
            scanner->startWaiting();
            hwDriver->hwdMotMove(MirrorCoord->getAngleCoordinate());
            scanner->WaitForMotMoved();
             if (MirrorCoordQObj == NULL){
                 delete MirrorCoord;
             }
        }
        return 0;
    }else{
        return 0;
    }
}

QScriptValue MotHome(QScriptContext *context, QScriptEngine *engine)
{
    (void)engine;
    (void)context;
    if (!ScriptAborting){
        TScanner* scanner = TScanner::instance(NULL);
        THWDriver* hwDriver = scanner->getHWDriver();

        //pos: TMirrorCoordinate
        scanner->startWaiting();
        hwDriver->hwdGoMotorHome();
        scanner->WaitForMotMoved();
        return 0;
    }else{
        return 0;
    }
}

QScriptValue MKDir(QScriptContext *context, QScriptEngine *engine)
{
    (void)engine;
    (void)context;
    bool result = false;
    if (!ScriptAborting){
        QString path = context->argument(0).toString();
        QDir dir(".");
        result = dir.mkpath(path);

    }
    return result;
}

QScriptValue GetDateStr(QScriptContext *context, QScriptEngine *engine)
{
    (void)engine;
    (void)context;
    QString result = "";
    if (!ScriptAborting){
        result = QDateTime::currentDateTime().toString("yyyyMMdd");
    }
    return result;
}

QScriptValue GetTimeStr(QScriptContext *context, QScriptEngine *engine)
{
    (void)engine;
    (void)context;
    QString result = "";
    if (!ScriptAborting){
        result = QDateTime::currentDateTime().toString("hhmmss");
    }
    return result;
}

bool TimePassed_raw(int hour,int minute)
{
    bool result = false;
    QDateTime now = QDateTime::currentDateTime();
    if ((now.time().hour() == hour) && (now.time().minute() >= minute))
        result = true;
    if (now.time().hour() > hour)
        result = true;
    return result;
}

bool WaitUntilToday_raw(uint hour,uint minute)
{
    while (!TimePassed_raw(hour,minute) && !ScriptAborting ){
        QTest::qWait(500);
    }
    return TimePassed_raw(hour,minute);
}

QScriptValue TimePassed(QScriptContext *context, QScriptEngine *engine)
{
    (void)engine;
    (void)context;
    bool result = false;
    if (!ScriptAborting){
        uint hour = context->argument(0).toInteger();
        uint minute = context->argument(1).toInteger();
        result =  TimePassed_raw(hour,minute);
    }
    return result;
}

QScriptValue WaitUntilToday(QScriptContext *context, QScriptEngine *engine)
{
    (void)engine;
    (void)context;
    bool result = false;
    if (!ScriptAborting){
        uint hour = context->argument(0).toInteger();
        uint minute = context->argument(1).toInteger();
        result = WaitUntilToday_raw(hour,minute);
    }
    return result;
}

QScriptValue WaitUntilTomorrow(QScriptContext *context, QScriptEngine *engine)
{
    (void)engine;
    (void)context;
    bool result = true;
    uint hour = context->argument(0).toInteger();
    uint minute = context->argument(1).toInteger();
    WaitUntilToday_raw(23,59);
    result = WaitUntilToday_raw(hour,minute);

    return result;
}

QScriptValue SetShutter(QScriptContext *context, QScriptEngine *engine){
    (void)engine;
    if (!ScriptAborting){
        TScanner* scanner = TScanner::instance(NULL);
        THWDriver* hwDriver = scanner->getHWDriver();
        bool shutteropen = context->argument(0).toBool();
        //idle bool
        if (shutteropen){
            hwDriver->hwdSetShutter(scOpen);
        }else{
            hwDriver->hwdSetShutter(scClose);
        }
        return 0;
    }else{
        return 0;
    }
}

QScriptValue MotIdle(QScriptContext *context, QScriptEngine *engine)
{
    (void)engine;
    if (!ScriptAborting){
        TScanner* scanner = TScanner::instance(NULL);
        THWDriver* hwDriver = scanner->getHWDriver();
        bool idle = context->argument(0).toBool();
        //idle bool
        hwDriver->hwdMotIdleState(idle);
        return 0;
    }else{
        return 0;
    }
}

QScriptValue getSpectSerialNo(QScriptContext *context, QScriptEngine *engine){
    (void)context;
    (void)engine;
    TScanner* scanner = TScanner::instance(NULL);
    return scanner->getSpectSerialNo();
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

    QScriptValue MotMoveFun = ScriptEngine->newFunction(MotMove);
    ScriptEngine->globalObject().setProperty("MotMove", MotMoveFun);

    QScriptValue MotHomeFun = ScriptEngine->newFunction(MotHome);
    ScriptEngine->globalObject().setProperty("MotHome", MotHomeFun);


    QScriptValue MotIdleFun = ScriptEngine->newFunction(MotIdle);
    ScriptEngine->globalObject().setProperty("SetMotIdle", MotIdleFun);

    QScriptValue SetShutterFun = ScriptEngine->newFunction(SetShutter);
    ScriptEngine->globalObject().setProperty("SetShutterOpen", SetShutterFun);

    QScriptValue GetMinimumIntegrationTimeFun = ScriptEngine->newFunction(GetMinimumIntegrationTime);
    ScriptEngine->globalObject().setProperty("GetMinimumIntegrationTime", GetMinimumIntegrationTimeFun);

    QScriptValue TimePassedFun = ScriptEngine->newFunction(TimePassed);
    ScriptEngine->globalObject().setProperty("TimePassed", TimePassedFun);

    QScriptValue WaitUntilTodayFun = ScriptEngine->newFunction(WaitUntilToday);
    ScriptEngine->globalObject().setProperty("WaitUntilToday", WaitUntilTodayFun);

    QScriptValue WaitUntilTomorrowFun = ScriptEngine->newFunction(WaitUntilTomorrow);
    ScriptEngine->globalObject().setProperty("WaitUntilTomorrow", WaitUntilTomorrowFun);

    QScriptValue MKDirFun = ScriptEngine->newFunction(MKDir);
    ScriptEngine->globalObject().setProperty("mkdir", MKDirFun);

    QScriptValue GetDateStrFun = ScriptEngine->newFunction(GetDateStr);
    ScriptEngine->globalObject().setProperty("GetDateStr", GetDateStrFun);

    QScriptValue GetTimeStrFun = ScriptEngine->newFunction(GetTimeStr);
    ScriptEngine->globalObject().setProperty("GetTimeStr", GetTimeStrFun);

    QScriptValue getSpectSerialNoFun = ScriptEngine->newFunction(getSpectSerialNo);
    ScriptEngine->globalObject().setProperty("GetSpectSerialNo", getSpectSerialNoFun);


    QScriptValue ctordl = ScriptEngine->newFunction(TDirListConstructor);
    QScriptValue metactordl = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, ctordl);
    ScriptEngine->globalObject().setProperty("TDirlist", metactordl);

    QScriptValue ctordlsi = ScriptEngine->newFunction(TDirListConstructorSIGIS);
    QScriptValue metactordlsi = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, ctordlsi);
    ScriptEngine->globalObject().setProperty("TDirlistSIGIS", metactordlsi);

    QScriptValue ctordretimg = ScriptEngine->newFunction(TRetrievalImageConstructorSIGIS);
    QScriptValue metactordretimg = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, ctordretimg);
    ScriptEngine->globalObject().setProperty("TRetrievalImageSIGIS", metactordretimg);


    QScriptValue ctorSpec = ScriptEngine->newFunction(TSpectrumConstructor);
    QScriptValue metaObjectSpec = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, ctorSpec);
    ScriptEngine->globalObject().setProperty("TSpektrum", metaObjectSpec);

    QScriptValue ctorSpImg = ScriptEngine->newFunction(TSpectralImageConstructor);
    QScriptValue metaObjectSpImg = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, ctorSpImg);
    ScriptEngine->globalObject().setProperty("TSpektralImage", metaObjectSpImg);

    QScriptValue ctorscpt = ScriptEngine->newFunction(TScanPathConstructor);
    QScriptValue metaObjectscpt = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, ctorscpt);
    ScriptEngine->globalObject().setProperty("TScanpath", metaObjectscpt);

    QScriptValue ctormc = ScriptEngine->newFunction(TMirrorCoordinateConstructor);
    QScriptValue metaObjectmc = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, ctormc);
    ScriptEngine->globalObject().setProperty("TMirrorcoordinate", metaObjectmc);

    QScriptValue qdconf = ScriptEngine->newFunction(QDoasConfigFileConstructor);
    QScriptValue metaqdconf = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, qdconf);
    ScriptEngine->globalObject().setProperty("TQDoasConfigFile", metaqdconf);

    QScriptValue plotobj = ScriptEngine->newQObject(TSpectrumPlotter::instance(0),QScriptEngine::ScriptOwnership);
    ScriptEngine->globalObject().setProperty("plot", plotobj);

    QScriptValue plotvectorsolver = ScriptEngine->newQObject(TVectorSolver::instance(),QScriptEngine::ScriptOwnership);
    ScriptEngine->globalObject().setProperty("VectorSolver", plotvectorsolver);


    QScriptValue qdoasobj = ScriptEngine->newQObject(QDoasWrapper::instance(),QScriptEngine::ScriptOwnership);
    ScriptEngine->globalObject().setProperty("qdoas", qdoasobj);
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
