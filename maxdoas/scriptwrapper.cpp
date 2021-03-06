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
#include "temissionrate.h"

TScanner* TScanner::m_Instance = 0;

bool ScriptAborting;

TScanner::TScanner(){
}

void TScanner::setHWDriver(THWDriver* hwdriver){
    this->hwdriver = hwdriver;
    connect(hwdriver,SIGNAL(hwdSigGotSpectrum()),this,SLOT(on_GotSpectrum()));
    connect(hwdriver,SIGNAL(hwdSigMotMoved(int, int)),this,SLOT(on_MotMoved(int, int)));
    connect(hwdriver,SIGNAL(hwdSigMotorIsHome(int, int)),this,SLOT(on_MotMoved(int, int)));
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

void TScanner::on_MotMoved(int, int){
    MotMoved = true;
}

void TScanner::on_MotTimeOut(THWTransferState TransferState, uint ErrorParameter){
    (void)ErrorParameter;
    if(TransferState == tsTimeOut)
        MotMoved = true;
}

bool DoSleep_(int ms){
    bool result=true;
    for(int i = 0;i<ms;i+=2){
        if (ScriptAborting){
            result = false;
            break;
        }
        QTest::qWait(2);
    }
    return result;
}

QScriptValue DoSleep(QScriptContext *context, QScriptEngine *engine)
{
    QScriptValue ms = context->argument(0);
    if (!DoSleep_(ms.toNumber())){
        engine->abortEvaluation();
        return 0;
    }
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

QScriptValue TScriptStringListConstructor(QScriptContext *context, QScriptEngine *engine)
{
    (void)context;
    QObject *object = new TScriptStringList();
    return engine->newQObject(object, QScriptEngine::ScriptOwnership);
}

QScriptValue TTimeLineConstructor(QScriptContext *context, QScriptEngine *engine)
{
    (void)context;
    QObject *object = new ttimeline();
    return engine->newQObject(object, QScriptEngine::ScriptOwnership);
}

QScriptValue TDirListConstructor(QScriptContext *context, QScriptEngine *engine)
{
    QObject *slo = context->argument(0).toQObject();
    TScriptStringList *sl = dynamic_cast<TScriptStringList*>(slo);
    int startindex = context->argument(1).toInteger();
    QString sp;
    QObject *object;
    if (sl == NULL){
        sp = context->argument(0).toString();
        object = new tdirlist(sp,startindex,"CCA");
    }else{
        object = new tdirlist(sl,startindex,"CCA");
    }

    return engine->newQObject(object, QScriptEngine::ScriptOwnership);
}

QScriptValue TDirListConstructorCCARetImage(QScriptContext *context, QScriptEngine *engine)
{
    QObject *slo = context->argument(0).toQObject();
    TScriptStringList *sl = dynamic_cast<TScriptStringList*>(slo);
    int startindex = context->argument(1).toInteger();
    QString sp;
    QObject *object;
    if (sl == NULL){
        sp = context->argument(0).toString();
        object = new tdirlist(sp,startindex,"CCARetImage");
    }else{
        object = new tdirlist(sl,startindex,"CCARetImage");
    }

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
    float PixSizeWidth = context->argument(1).toNumber();
    float PixSizeHeigtht = context->argument(2).toNumber();
    QObject *object = new TRetrievalImage(FileName,"SIGIS",PixSizeWidth,PixSizeHeigtht);
    return engine->newQObject(object, QScriptEngine::ScriptOwnership);
}

QScriptValue TRetrievalImageConstructorCCA(QScriptContext *context, QScriptEngine *engine)
{
    QString FileName = context->argument(0).toString();
    QObject *object = new TRetrievalImage(FileName,"CCA",0,0);
    return engine->newQObject(object, QScriptEngine::ScriptOwnership);
}


QScriptValue TRetrievalImageConstructorCopy(QScriptContext *context, QScriptEngine *engine)
{
    QObject *from = context->argument(0).toQObject();
    TRetrievalImage *rtimg = dynamic_cast<TRetrievalImage*>(from);
    if (rtimg != NULL){
        QObject *object = new TRetrievalImage(rtimg);
        return engine->newQObject(object, QScriptEngine::ScriptOwnership);
    }else{
        return 0;
    }
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

QScriptValue CalcAvgByScanTime(QScriptContext *context, QScriptEngine *engine)
{
    (void)engine;
    int avg = 1;
    TScanner* scanner = TScanner::instance(NULL);
    THWDriver* hwDriver = scanner->getHWDriver();

    uint time = context->argument(0).toNumber()*1000; //in ms
    uint minavg = context->argument(1).toNumber();

    int lastintegtime=hwDriver->getLastSpectrIntegrationTime();

    if (time == 0)
        time = TILT_AFTER_MOTMOVE_TIME_OUT*1000;
    avg = time / lastintegtime;
    avg++;
    if (minavg > 0)
        if (avg < minavg)
            avg = minavg;
    return avg;
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

QScriptValue leadingZero(QScriptContext *context, QScriptEngine *engine)
{
    (void)context;
    (void)engine;
    int count = context->argument(0).toInteger();
    int num   = context->argument(1).toInteger();
    QString result = QString::number(num).rightJustified(count, '0');

    return result;
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
               TVectorSolver* vs = TVectorSolver::instance();
               vs->retrievalImageDestructed(rtimg);
               delete rtimg;
           }else{
               QDoasConfigFile *qdconf = dynamic_cast<QDoasConfigFile*>(obj);
               if(qdconf != NULL){
                   delete qdconf;
               }else{
                   tdirlist *dl = dynamic_cast<tdirlist*>(obj);
                   if(dl != NULL){
                       delete dl;
                   }else{
                       TEmissionrate *er = dynamic_cast<TEmissionrate*>(obj);
                       if(er != NULL){
                           delete er;
                       }else{
                           TScriptStringList *sl = dynamic_cast<TScriptStringList*>(obj);
                           if(sl != NULL){
                                delete sl;
                           }else{
                               ttimeline *tl = dynamic_cast<ttimeline*>(obj);
                               if(tl != NULL){
                                   delete tl;
                               }
                           }
                       }
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

QScriptValue isMaxDOAS(QScriptContext *context, QScriptEngine *engine)
{
    (void)engine;
    (void)context;
    bool result = false;
    TMaxdoasSettings *ms = TMaxdoasSettings::instance();
    if (ms->askAttachedScanningDevice()==sdtWindField){
        result = true;
    }
    return result;
}

QScriptValue isWindfield(QScriptContext *context, QScriptEngine *engine)
{
    (void)engine;
    (void)context;
    bool result = false;
    TMaxdoasSettings *ms = TMaxdoasSettings::instance();
    if (ms->askAttachedScanningDevice()==sdtMAXDOAS){
        result = true;
    }
    return result;
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
    DoSleep_(90*1000);
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

QScriptValue msSinceEpoch(QScriptContext *context, QScriptEngine *engine){
    (void)context;
    (void)engine;
    double result = QDateTime::currentMSecsSinceEpoch();
    return result;
}

QScriptValue getSiteName(QScriptContext *context, QScriptEngine *engine){
    (void)context;
    (void)engine;
    TMaxdoasSettings *ms = TMaxdoasSettings::instance();
    return ms->getSiteName();
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

    QScriptValue msSinceEpochFun = ScriptEngine->newFunction(msSinceEpoch);
    ScriptEngine->globalObject().setProperty("msSinceEpoch", msSinceEpochFun);

    QScriptValue getSiteNameFun = ScriptEngine->newFunction(getSiteName);
    ScriptEngine->globalObject().setProperty("getSiteName", getSiteNameFun);

    QScriptValue leadingZeroFun = ScriptEngine->newFunction(leadingZero);
    ScriptEngine->globalObject().setProperty("leadingZero", leadingZeroFun);

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

    QScriptValue CalcAvgByScanTimeFun = ScriptEngine->newFunction(CalcAvgByScanTime);
    ScriptEngine->globalObject().setProperty("CalcAvgByScanTime", CalcAvgByScanTimeFun);


    QScriptValue isMaxdoasFun = ScriptEngine->newFunction(isMaxDOAS);
    ScriptEngine->globalObject().setProperty("isMaxDOAS", isMaxdoasFun);

    QScriptValue isWindfieldFun = ScriptEngine->newFunction(isWindfield);
    ScriptEngine->globalObject().setProperty("isWindfield", isWindfieldFun);

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

    QScriptValue ctordlcri = ScriptEngine->newFunction(TDirListConstructorCCARetImage);
    QScriptValue metactordcri = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, ctordlcri);
    ScriptEngine->globalObject().setProperty("TDirlistCCARetImg", metactordcri);



    QScriptValue ctordretimg = ScriptEngine->newFunction(TRetrievalImageConstructorSIGIS);
    QScriptValue metactordretimg = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, ctordretimg);
    ScriptEngine->globalObject().setProperty("TRetrievalImageSIGIS", metactordretimg);

    QScriptValue ctordretimgcca = ScriptEngine->newFunction(TRetrievalImageConstructorCCA);
    QScriptValue metactordretimgcca = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, ctordretimgcca);
    ScriptEngine->globalObject().setProperty("TRetrievalImageCCA", metactordretimgcca);



    QScriptValue ctordretimgc = ScriptEngine->newFunction(TRetrievalImageConstructorCopy);
    QScriptValue metactordretimgc = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, ctordretimgc);
    ScriptEngine->globalObject().setProperty("TRetrievalImage", metactordretimgc);


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

    QScriptValue qdsl = ScriptEngine->newFunction(TScriptStringListConstructor);
    QScriptValue metasl = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, qdsl);
    ScriptEngine->globalObject().setProperty("TStringList", metasl);

    QScriptValue qdtl = ScriptEngine->newFunction(TTimeLineConstructor);
    QScriptValue metatl = ScriptEngine->newQMetaObject(&QObject::staticMetaObject, qdtl);
    ScriptEngine->globalObject().setProperty("TTimeLine", metatl);

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
