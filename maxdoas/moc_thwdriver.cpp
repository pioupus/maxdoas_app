/****************************************************************************
** Meta object code from reading C++ file 'thwdriver.h'
**
** Created: Fri May 27 00:27:16 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "thwdriver.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'thwdriver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_THWDriverThread[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      34,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      13,       // signalCount

 // signals: signature, parameters, type, tag, flags
      46,   17,   16,   16, 0x05,
     118,   89,   16,   16, 0x05,
     180,  168,   16,   16, 0x05,
     216,  208,   16,   16, 0x05,
     248,   16,   16,   16, 0x05,
     283,   16,   16,   16, 0x05,
     329,  318,   16,   16, 0x05,
     363,   16,   16,   16, 0x05,
     399,  384,   16,   16, 0x05,
     441,   16,   16,   16, 0x05,
     468,   16,   16,   16, 0x05,
     495,   16,   16,   16, 0x05,
     516,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
     549,  544,   16,   16, 0x0a,
     581,  576,   16,   16, 0x0a,
     627,  622,   16,   16, 0x0a,
     670,  653,   16,   16, 0x0a,
     726,  714,   16,   16, 0x0a,
     761,   16,   16,   16, 0x0a,
     794,  778,   16,   16, 0x0a,
     844,  168,   16,   16, 0x0a,
     875,   16,   16,   16, 0x0a,
     895,   16,   16,   16, 0x0a,
     920,   16,   16,   16, 0x0a,
     970,  944,   16,   16, 0x0a,
    1035,   16,   16,   16, 0x0a,
    1059,   16,   16,   16, 0x0a,
    1096, 1085, 1080,   16, 0x0a,
    1122, 1085,   16,   16, 0x0a,
    1180, 1155,   16,   16, 0x0a,
    1249, 1223,   16,   16, 0x0a,
    1297,   16,   16,   16, 0x0a,
    1330, 1324,   16,   16, 0x0a,
    1360,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_THWDriverThread[] = {
    "THWDriverThread\0\0TransferState,ErrorParameter\0"
    "hwdtSigTransferDone(THWTransferState,uint)\0"
    "sensorID,Temperature,byTimer\0"
    "hwdtSigGotTemperature(THWTempSensorID,float,bool)\0"
    "TiltX,TiltY\0hwdtSigGotTilt(float,float)\0"
    "Heading\0hwdtSigGotCompassHeading(float)\0"
    "hwdtSigCompassStartedCalibrating()\0"
    "hwdtSigCompassStoppedCalibrating()\0"
    "lightValue\0hwdtSigGotLightSensorValue(float)\0"
    "hwdtSigMotorIsHome()\0LastShutterCMD\0"
    "hwdtSigShutterStateChanged(THWShutterCMD)\0"
    "hwdtSigScanPixelMeasured()\0"
    "hwdtSigGotWLCoefficients()\0"
    "hwdtSigGotSpectrum()\0hwdtSigSpectrumeterOpened()\0"
    "name\0hwdtSloSetComPort(QString)\0baud\0"
    "hwdtSloSetBaud(AbstractSerial::BaudRate)\0"
    "open\0hwdtSloSerOpenClose(bool)\0"
    "sensorID,byTimer\0"
    "hwdtSloAskTemperature(THWTempSensorID,bool)\0"
    "temperature\0hwdtSloSetTargetTemperature(float)\0"
    "hwdtSloAskTilt()\0Resolution,Gain\0"
    "hwdtSloConfigTilt(TTiltConfigRes,TTiltConfigGain)\0"
    "hwdtSloOffsetTilt(float,float)\0"
    "hwdtSloAskCompass()\0hwdtSloStartCompassCal()\0"
    "hwdtSloStopCompassCal()\0"
    "Gain,LightSensorIntegTime\0"
    "hwdtSloConfigLightSensor(TLightSensorGain,TLightSensorIntegTime)\0"
    "hwdtSloAskLightSensor()\0hwdtSloGoMotorHome()\0"
    "bool\0ShutterCMD\0SetShutter(THWShutterCMD)\0"
    "hwdtSloSetShutter(THWShutterCMD)\0"
    "PosX,PosY,avg,integrTime\0"
    "hwdtSloMeasureScanPixel(int,int,uint,uint)\0"
    "avg,integrTime,shutterCMD\0"
    "hwdtSloMeasureSpectrum(uint,uint,THWShutterCMD)\0"
    "hwdtSloAskWLCoefficients()\0index\0"
    "hwdtSloOpenSpectrometer(uint)\0"
    "hwdtSloCloseSpectrometer()\0"
};

const QMetaObject THWDriverThread::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_THWDriverThread,
      qt_meta_data_THWDriverThread, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &THWDriverThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *THWDriverThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *THWDriverThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_THWDriverThread))
        return static_cast<void*>(const_cast< THWDriverThread*>(this));
    return QObject::qt_metacast(_clname);
}

int THWDriverThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: hwdtSigTransferDone((*reinterpret_cast< THWTransferState(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 1: hwdtSigGotTemperature((*reinterpret_cast< THWTempSensorID(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 2: hwdtSigGotTilt((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 3: hwdtSigGotCompassHeading((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 4: hwdtSigCompassStartedCalibrating(); break;
        case 5: hwdtSigCompassStoppedCalibrating(); break;
        case 6: hwdtSigGotLightSensorValue((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 7: hwdtSigMotorIsHome(); break;
        case 8: hwdtSigShutterStateChanged((*reinterpret_cast< THWShutterCMD(*)>(_a[1]))); break;
        case 9: hwdtSigScanPixelMeasured(); break;
        case 10: hwdtSigGotWLCoefficients(); break;
        case 11: hwdtSigGotSpectrum(); break;
        case 12: hwdtSigSpectrumeterOpened(); break;
        case 13: hwdtSloSetComPort((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 14: hwdtSloSetBaud((*reinterpret_cast< AbstractSerial::BaudRate(*)>(_a[1]))); break;
        case 15: hwdtSloSerOpenClose((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: hwdtSloAskTemperature((*reinterpret_cast< THWTempSensorID(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 17: hwdtSloSetTargetTemperature((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 18: hwdtSloAskTilt(); break;
        case 19: hwdtSloConfigTilt((*reinterpret_cast< TTiltConfigRes(*)>(_a[1])),(*reinterpret_cast< TTiltConfigGain(*)>(_a[2]))); break;
        case 20: hwdtSloOffsetTilt((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 21: hwdtSloAskCompass(); break;
        case 22: hwdtSloStartCompassCal(); break;
        case 23: hwdtSloStopCompassCal(); break;
        case 24: hwdtSloConfigLightSensor((*reinterpret_cast< TLightSensorGain(*)>(_a[1])),(*reinterpret_cast< TLightSensorIntegTime(*)>(_a[2]))); break;
        case 25: hwdtSloAskLightSensor(); break;
        case 26: hwdtSloGoMotorHome(); break;
        case 27: { bool _r = SetShutter((*reinterpret_cast< THWShutterCMD(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 28: hwdtSloSetShutter((*reinterpret_cast< THWShutterCMD(*)>(_a[1]))); break;
        case 29: hwdtSloMeasureScanPixel((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3])),(*reinterpret_cast< uint(*)>(_a[4]))); break;
        case 30: hwdtSloMeasureSpectrum((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< THWShutterCMD(*)>(_a[3]))); break;
        case 31: hwdtSloAskWLCoefficients(); break;
        case 32: hwdtSloOpenSpectrometer((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 33: hwdtSloCloseSpectrometer(); break;
        default: ;
        }
        _id -= 34;
    }
    return _id;
}

// SIGNAL 0
void THWDriverThread::hwdtSigTransferDone(THWTransferState _t1, uint _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void THWDriverThread::hwdtSigGotTemperature(THWTempSensorID _t1, float _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void THWDriverThread::hwdtSigGotTilt(float _t1, float _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void THWDriverThread::hwdtSigGotCompassHeading(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void THWDriverThread::hwdtSigCompassStartedCalibrating()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void THWDriverThread::hwdtSigCompassStoppedCalibrating()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void THWDriverThread::hwdtSigGotLightSensorValue(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void THWDriverThread::hwdtSigMotorIsHome()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void THWDriverThread::hwdtSigShutterStateChanged(THWShutterCMD _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void THWDriverThread::hwdtSigScanPixelMeasured()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void THWDriverThread::hwdtSigGotWLCoefficients()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void THWDriverThread::hwdtSigGotSpectrum()
{
    QMetaObject::activate(this, &staticMetaObject, 11, 0);
}

// SIGNAL 12
void THWDriverThread::hwdtSigSpectrumeterOpened()
{
    QMetaObject::activate(this, &staticMetaObject, 12, 0);
}
static const uint qt_meta_data_THWDriver[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      43,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      30,       // signalCount

 // signals: signature, parameters, type, tag, flags
      32,   11,   10,   10, 0x05,
      88,   76,   10,   10, 0x05,
     123,  115,   10,   10, 0x05,
     154,   10,   10,   10, 0x05,
     188,   10,   10,   10, 0x05,
     233,  222,   10,   10, 0x05,
     266,   10,   10,   10, 0x05,
     301,  286,   10,   10, 0x05,
     342,   10,   10,   10, 0x05,
     368,   10,   10,   10, 0x05,
     388,   10,   10,   10, 0x05,
     444,  415,   10,   10, 0x05,
     491,  486,   10,   10, 0x05,
     523,  518,   10,   10, 0x05,
     581,  564,   10,   10, 0x05,
     637,  625,   10,   10, 0x05,
     688,  672,   10,   10, 0x05,
     738,   10,   10,   10, 0x05,
     755,   76,   10,   10, 0x05,
     786,   10,   10,   10, 0x05,
     806,   10,   10,   10, 0x05,
     831,   10,   10,   10, 0x05,
     855,   10,   10,   10, 0x05,
     879,   10,   10,   10, 0x05,
     911,  900,   10,   10, 0x05,
     969,  944,   10,   10, 0x05,
    1038, 1012,   10,   10, 0x05,
    1086,   10,   10,   10, 0x05,
    1119, 1113,   10,   10, 0x05,
    1149,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
    1205, 1176,   10,   10, 0x08,
    1254,   76,   10,   10, 0x08,
    1281,  115,   10,   10, 0x08,
    1312,   10,   10,   10, 0x08,
    1346,   10,   10,   10, 0x08,
    1380,  222,   10,   10, 0x08,
    1413,   10,   10,   10, 0x08,
    1433,  286,   10,   10, 0x08,
    1474,   10,   10,   10, 0x08,
    1500,   10,   10,   10, 0x08,
    1520,   10,   10,   10, 0x08,
    1547,   10,   10,   10, 0x08,
    1573,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_THWDriver[] = {
    "THWDriver\0\0sensorID,Temperature\0"
    "hwdSigGotTemperature(THWTempSensorID,float)\0"
    "TiltX,TiltY\0hwdSigGotTilt(float,float)\0"
    "Heading\0hwdSigGotCompassHeading(float)\0"
    "hwdSigCompassStartedCalibrating()\0"
    "hwdSigCompassStoppedCalibrating()\0"
    "lightValue\0hwdSigGotLightSensorValue(float)\0"
    "hwdSigMotorIsHome()\0LastShutterCMD\0"
    "hwdSigShutterStateChanged(THWShutterCMD)\0"
    "hwdSigScanPixelMeasured()\0hwdSigGotSpectrum()\0"
    "hwdSigSpectrumeterOpened()\0"
    "TransferState,ErrorParameter\0"
    "hwdSigTransferDone(THWTransferState,uint)\0"
    "name\0hwdtSigSetComPort(QString)\0baud\0"
    "hwdtSigSetBaud(AbstractSerial::BaudRate)\0"
    "sensorID,byTimer\0"
    "hwdtSigAskTemperature(THWTempSensorID,bool)\0"
    "temperature\0hwdtSigSetTargetTemperature(float)\0"
    "Resolution,Gain\0"
    "hwdtSigConfigTilt(TTiltConfigRes,TTiltConfigGain)\0"
    "hwdtSigAskTilt()\0hwdtSigOffsetTilt(float,float)\0"
    "hwdtSigAskCompass()\0hwdtSigStartCompassCal()\0"
    "hwdtSigStopCompassCal()\0hwdtSigAskLightSensor()\0"
    "hwdtSigGoMotorHome()\0ShutterCMD\0"
    "hwdtSigSetShutter(THWShutterCMD)\0"
    "PosX,PosY,avg,integrTime\0"
    "hwdtSigMeasureScanPixel(int,int,uint,uint)\0"
    "avg,integrTime,shutterCMD\0"
    "hwdtSigMeasureSpectrum(uint,uint,THWShutterCMD)\0"
    "hwdtSigAskWLCoefficients()\0index\0"
    "hwdtSigOpenSpectrometer(uint)\0"
    "hwdtSigCloseSpectrometer()\0"
    "sensorID,Temperature,byTimer\0"
    "hwdSloGotTemperature(THWTempSensorID,float,bool)\0"
    "hwdSloGotTilt(float,float)\0"
    "hwdSloGotCompassHeading(float)\0"
    "hwdSloCompassStartedCalibrating()\0"
    "hwdSloCompassStoppedCalibrating()\0"
    "hwdSloGotLightSensorValue(float)\0"
    "hwdSloMotorIsHome()\0"
    "hwdSloShutterStateChanged(THWShutterCMD)\0"
    "hwdSloScanPixelMeasured()\0hwdSloGotSpectrum()\0"
    "hwdSloSpectrumeterOpened()\0"
    "hwdSloGotWLCoefficients()\0"
    "hwdSlotTemperatureTimer()\0"
};

const QMetaObject THWDriver::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_THWDriver,
      qt_meta_data_THWDriver, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &THWDriver::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *THWDriver::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *THWDriver::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_THWDriver))
        return static_cast<void*>(const_cast< THWDriver*>(this));
    return QObject::qt_metacast(_clname);
}

int THWDriver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: hwdSigGotTemperature((*reinterpret_cast< THWTempSensorID(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 1: hwdSigGotTilt((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 2: hwdSigGotCompassHeading((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 3: hwdSigCompassStartedCalibrating(); break;
        case 4: hwdSigCompassStoppedCalibrating(); break;
        case 5: hwdSigGotLightSensorValue((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 6: hwdSigMotorIsHome(); break;
        case 7: hwdSigShutterStateChanged((*reinterpret_cast< THWShutterCMD(*)>(_a[1]))); break;
        case 8: hwdSigScanPixelMeasured(); break;
        case 9: hwdSigGotSpectrum(); break;
        case 10: hwdSigSpectrumeterOpened(); break;
        case 11: hwdSigTransferDone((*reinterpret_cast< THWTransferState(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 12: hwdtSigSetComPort((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 13: hwdtSigSetBaud((*reinterpret_cast< AbstractSerial::BaudRate(*)>(_a[1]))); break;
        case 14: hwdtSigAskTemperature((*reinterpret_cast< THWTempSensorID(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 15: hwdtSigSetTargetTemperature((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 16: hwdtSigConfigTilt((*reinterpret_cast< TTiltConfigRes(*)>(_a[1])),(*reinterpret_cast< TTiltConfigGain(*)>(_a[2]))); break;
        case 17: hwdtSigAskTilt(); break;
        case 18: hwdtSigOffsetTilt((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 19: hwdtSigAskCompass(); break;
        case 20: hwdtSigStartCompassCal(); break;
        case 21: hwdtSigStopCompassCal(); break;
        case 22: hwdtSigAskLightSensor(); break;
        case 23: hwdtSigGoMotorHome(); break;
        case 24: hwdtSigSetShutter((*reinterpret_cast< THWShutterCMD(*)>(_a[1]))); break;
        case 25: hwdtSigMeasureScanPixel((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3])),(*reinterpret_cast< uint(*)>(_a[4]))); break;
        case 26: hwdtSigMeasureSpectrum((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< THWShutterCMD(*)>(_a[3]))); break;
        case 27: hwdtSigAskWLCoefficients(); break;
        case 28: hwdtSigOpenSpectrometer((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 29: hwdtSigCloseSpectrometer(); break;
        case 30: hwdSloGotTemperature((*reinterpret_cast< THWTempSensorID(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 31: hwdSloGotTilt((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 32: hwdSloGotCompassHeading((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 33: hwdSloCompassStartedCalibrating(); break;
        case 34: hwdSloCompassStoppedCalibrating(); break;
        case 35: hwdSloGotLightSensorValue((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 36: hwdSloMotorIsHome(); break;
        case 37: hwdSloShutterStateChanged((*reinterpret_cast< THWShutterCMD(*)>(_a[1]))); break;
        case 38: hwdSloScanPixelMeasured(); break;
        case 39: hwdSloGotSpectrum(); break;
        case 40: hwdSloSpectrumeterOpened(); break;
        case 41: hwdSloGotWLCoefficients(); break;
        case 42: hwdSlotTemperatureTimer(); break;
        default: ;
        }
        _id -= 43;
    }
    return _id;
}

// SIGNAL 0
void THWDriver::hwdSigGotTemperature(THWTempSensorID _t1, float _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void THWDriver::hwdSigGotTilt(float _t1, float _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void THWDriver::hwdSigGotCompassHeading(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void THWDriver::hwdSigCompassStartedCalibrating()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void THWDriver::hwdSigCompassStoppedCalibrating()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void THWDriver::hwdSigGotLightSensorValue(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void THWDriver::hwdSigMotorIsHome()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void THWDriver::hwdSigShutterStateChanged(THWShutterCMD _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void THWDriver::hwdSigScanPixelMeasured()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void THWDriver::hwdSigGotSpectrum()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void THWDriver::hwdSigSpectrumeterOpened()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void THWDriver::hwdSigTransferDone(THWTransferState _t1, uint _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void THWDriver::hwdtSigSetComPort(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void THWDriver::hwdtSigSetBaud(AbstractSerial::BaudRate _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void THWDriver::hwdtSigAskTemperature(THWTempSensorID _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void THWDriver::hwdtSigSetTargetTemperature(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void THWDriver::hwdtSigConfigTilt(TTiltConfigRes _t1, TTiltConfigGain _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void THWDriver::hwdtSigAskTilt()
{
    QMetaObject::activate(this, &staticMetaObject, 17, 0);
}

// SIGNAL 18
void THWDriver::hwdtSigOffsetTilt(float _t1, float _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 19
void THWDriver::hwdtSigAskCompass()
{
    QMetaObject::activate(this, &staticMetaObject, 19, 0);
}

// SIGNAL 20
void THWDriver::hwdtSigStartCompassCal()
{
    QMetaObject::activate(this, &staticMetaObject, 20, 0);
}

// SIGNAL 21
void THWDriver::hwdtSigStopCompassCal()
{
    QMetaObject::activate(this, &staticMetaObject, 21, 0);
}

// SIGNAL 22
void THWDriver::hwdtSigAskLightSensor()
{
    QMetaObject::activate(this, &staticMetaObject, 22, 0);
}

// SIGNAL 23
void THWDriver::hwdtSigGoMotorHome()
{
    QMetaObject::activate(this, &staticMetaObject, 23, 0);
}

// SIGNAL 24
void THWDriver::hwdtSigSetShutter(THWShutterCMD _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 24, _a);
}

// SIGNAL 25
void THWDriver::hwdtSigMeasureScanPixel(int _t1, int _t2, uint _t3, uint _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 25, _a);
}

// SIGNAL 26
void THWDriver::hwdtSigMeasureSpectrum(uint _t1, uint _t2, THWShutterCMD _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 26, _a);
}

// SIGNAL 27
void THWDriver::hwdtSigAskWLCoefficients()
{
    QMetaObject::activate(this, &staticMetaObject, 27, 0);
}

// SIGNAL 28
void THWDriver::hwdtSigOpenSpectrometer(uint _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 28, _a);
}

// SIGNAL 29
void THWDriver::hwdtSigCloseSpectrometer()
{
    QMetaObject::activate(this, &staticMetaObject, 29, 0);
}
QT_END_MOC_NAMESPACE
