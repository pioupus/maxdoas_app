#include "tfrmtempctrl.h"
#include "ui_tfrmtempctrl.h"
#include <QMessageBox>
#include <QInputDialog>



TfrmTempctrl::TfrmTempctrl(THWDriver *hwdriver, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TfrmTempctrl)
{
    int i;
    ui->setupUi(this);
    this->hwdriver=hwdriver;
    setAttribute(Qt::WA_DeleteOnClose, true);
    ScannerConfigModified = false;
    plot = new QwtPlot(this);
    curvePeltier = new QwtPlotCurve("Peltier");
    curveHeatsink = new QwtPlotCurve("Heatsink");
    curveSpectrometer = new QwtPlotCurve("Spectrometer");
    QwtLegend *legend = new QwtLegend;
    bubblewidget = new TBubbleWidget;
    ui->tiltlayout->addWidget(bubblewidget);
    hwdriver->hwdSetTiltInterval(1000);
    legend->setItemMode(QwtLegend::CheckableItem);
    plot->insertLegend(legend, QwtPlot::RightLegend);
  //  plot->aut
    ui->hbox->addWidget(plot);
    ms = TMaxdoasSettings::instance();
    ComPortSettings = ms->getComPortConfiguration();
    ui->sedtTargetTemp->setValue(ms->getTargetTemperature());
    ui->chbComBySysPath->setChecked(!ComPortSettings.ByName);
    ui->lieSiteName->setText(ms->getSiteName());
    ui->spbLatitude->setValue(ms->getSiteLatitude());
    ui->spbLongitude->setValue(ms->getSiteLongitude());
    ui->spbScannerdirection->setValue(ms->getScannerDirection());

    if (ms->isInConfigMode()){
        ui->btnMotHome->setVisible(true);
        ui->btnSetSerialNumber->setVisible(true);
        ui->btnSetShutterClosePos->setVisible(true);
        ui->btnSetZenithPos->setVisible(true);
        ui->btnShutterPosDown->setVisible(true);
        ui->btnShutterPosUp->setVisible(true);
        ui->btnTiltCalibration->setVisible(true);
        ui->btnTiltSetAsZenith->setVisible(true);
        ui->btnZenithMotDown->setVisible(true);
        ui->btnZenithMotUp->setVisible(true);
        ui->label_13->setVisible(true);
        ui->spbIncrementBy->setVisible(true);
    }else{
        ui->btnMotHome->setVisible(false);
        ui->btnSetSerialNumber->setVisible(false);
        ui->btnSetShutterClosePos->setVisible(false);
        ui->btnSetZenithPos->setVisible(false);
        ui->btnShutterPosDown->setVisible(false);
        ui->btnShutterPosUp->setVisible(false);
        ui->btnTiltCalibration->setVisible(false);
        ui->btnTiltSetAsZenith->setVisible(false);
        ui->btnZenithMotDown->setVisible(false);
        ui->btnZenithMotUp->setVisible(false);
        ui->label_13->setVisible(false);
        ui->spbIncrementBy->setVisible(false);
    }
    if (ms->getAttachedScanningDevice()==sdtMAXDOAS){

        ui->loScannerTemperature->setEnabled(true);
        ui->loShutterPosition->setEnabled(true);
        ui->loTiltDirection->setEnabled(true);
        ui->loTiltcalibration->setEnabled(true);
        ui->loShutterState->setEnabled(true);
        ui->loZenithMaxDoasPosition->setEnabled(true);
        ui->loincrementby->setEnabled(true);
        ui->loEndswitch->setEnabled(true);
        ui->loEndswitch_dsfsdf->setEnabled(true);
        ui->tabTilt->setVisible(false);
        ui->tabCompass->setVisible(false);
    }else{
        ui->loScannerTemperature->setEnabled(false);
        ui->loShutterPosition->setEnabled(false);
        ui->loTiltDirection->setEnabled(false);
        ui->loTiltcalibration->setEnabled(false);
        ui->loShutterState->setEnabled(false);
        ui->loZenithMaxDoasPosition->setEnabled(false);
        ui->loincrementby->setEnabled(false);
        ui->loEndswitch->setEnabled(false);
        ui->loEndswitch_dsfsdf->setEnabled(false);
        ui->tabTilt->setVisible(true);
        ui->tabCompass->setVisible(true);
    }

    startTimer(1000);
    curvePeltier->attach(plot);
    curveHeatsink->attach(plot);
    curveSpectrometer->attach(plot);
    curvePeltier->setPen(QPen(Qt::green));
    curveHeatsink->setPen(QPen(Qt::red));
    BufferFilled = 0;



    for (i=0;i<TEMPERAT_BUFFER_SIZE;i++){
        BufferX1[i] = i;
    }
    connect(plot, SIGNAL(legendChecked(QwtPlotItem *, bool)),
        SLOT(showCurve(QwtPlotItem *, bool)));
    connect(hwdriver, SIGNAL(hwdSigGotTemperatures(float , float, float)),
        SLOT(SloGotTemperature(float , float, float )));

    connect(hwdriver, SIGNAL(hwdSigGotTilt(float , float, int,int,float,float)),
        SLOT(SlotGotTilt(float , float, int,int ,float,float)));

    connect(hwdriver, SIGNAL(hwdSigGotDeviceInfo(int , int, int)),
        SLOT(SlotGotDeviceInfo(int , int, int)));

    connect(hwdriver, SIGNAL(hwdSigGotMotorSetup(int , int, int, int, int)),
        SLOT(SlotGotMotorSetup(int ,int , int , int , int )));

    connect(hwdriver,SIGNAL(hwdSigGotScannerStatus(float, bool,bool  )),
                SLOT (SlotGotScannerStatus(float, bool,bool )));

    connect(hwdriver,SIGNAL(hwdSigGotTiltDirection(float)),
                SLOT (SlotGotTiltDirection(float)));

    connect(hwdriver,SIGNAL(hwdSigGotTiltMinVal(int,int,int,int)),
                SLOT (SlotGotTiltMinVal(int,int,int,int)));

    connect(hwdriver,SIGNAL(hwdSigGotTiltMaxVal(int,int,int,int)),
                SLOT (SlotGotTiltMaxVal(int,int,int,int)));



 //   void hwdSigGotTilt(float TiltX,float TiltY,int Gain, int Resolution,float ResolutionBorder,float MaxTilt);

    showCurve(curvePeltier, true);
    showCurve(curveHeatsink, true);
    showCurve(curveSpectrometer, true);

    this->m_sde = new SerialDeviceEnumerator(this);
    connect(this->m_sde, SIGNAL(hasChanged(QStringList)),
            this, SLOT(slotCOMPorts(QStringList)));
    this->m_sde->setEnabled(true);
    tiltcalibration = false;
    show();

    hwdriver->hwdAskDeviceInfo();
    hwdriver->hwdAskTiltMaxValue();
}

void TfrmTempctrl::showCurve(QwtPlotItem *item, bool on)
{
    item->setVisible(on);
    QWidget *w = plot->legend()->find(item);
    if ( w && w->inherits("QwtLegendItem") )
        ((QwtLegendItem *)w)->setChecked(on);

    plot->replot();
}

void TfrmTempctrl::SloGotTemperature(float TemperaturePeltier, float TemperatureSpectr, float TemperatureHeatsink){
        (void)TemperaturePeltier;
        (void)TemperatureSpectr;
        (void)TemperatureHeatsink;
//        BufferFilled++;
//        if (BufferFilled == TEMPERAT_BUFFER_SIZE){
//            for (i=1;i<TEMPERAT_BUFFER_SIZE;i++){
//                BufferPeltier[i-1] = BufferPeltier[i];
//                BufferHeatSink[i-1] = BufferHeatSink[i];
//                BufferSpectrometer[i-1] = BufferSpectrometer[i];
//            }
//            BufferFilled = TEMPERAT_BUFFER_SIZE-1;
//        }
//        BufferPeltier[BufferFilled] = hwdriver->hwdGetTemperature(tsPeltier);
//        BufferHeatSink[BufferFilled] = hwdriver->hwdGetTemperature(tsHeatSink);
//        BufferSpectrometer[BufferFilled] = hwdriver->hwdGetTemperature(tsSpectrometer);

        curvePeltier->setRawSamples(&BufferX1[0],&hwdriver->TempBufferPeltier[0],hwdriver->TempBufferPointer);
        curveHeatsink->setRawSamples(&BufferX1[0],&hwdriver->TempBufferHeatSink[0],hwdriver->TempBufferPointer);
        curveSpectrometer->setRawSamples(&BufferX1[0],&hwdriver->TempBufferSpectr[0],hwdriver->TempBufferPointer);
        plot->replot();
}

void TfrmTempctrl::timerEvent(QTimerEvent *){

    hwdriver->hwdAskScannerStatus();
    if (tiltcalibration)
        hwdriver->hwdAskTiltMaxValue();


}

void TfrmTempctrl::SlotGotDeviceInfo(int GitHash,int guid, int deviceType){
#define DT_2DSCANNER	0x01
#define DT_1DSCANNER	0x02
#define DT_SOLTRACKER	0x03

    ui->lblFirmwarehash->setText("0x"+QString::number(GitHash,16));
    ui->lblDeviceSerialNumber->setText(rawDataToSerialNumber(guid,deviceType));
    if (deviceType == DT_2DSCANNER)
        ui->lblDeviceType->setText("Windfield DOAS");
    else if (deviceType == DT_1DSCANNER)
        ui->lblDeviceType->setText("MAXDOAS");
    else if (deviceType == DT_SOLTRACKER)
        ui->lblDeviceType->setText("Solartracker");
    else
        ui->lblDeviceType->setText("unkown");

    hwdriver->hwdAskMotorSetup();
}

void TfrmTempctrl::SlotGotTiltDirection(float TiltDirection){
    ui->lblTiltDirection->setText(QString::number(TiltDirection)+" Deg");
}


void TfrmTempctrl::SlotGotTiltMinVal(int TiltX, int TiltY, int Gain, int Resolution){
    (void) Gain;
    (void) Resolution;
    QPointF minval = QPoint(TiltX,TiltY);
    QPointF maxval = hwdriver->getRawTiltMax();
    ui->lblTiltCalibration->setText("x["+QString::number(minval.x())+","+QString::number(maxval.x())+"],y["+QString::number(minval.y())+","+QString::number(maxval.y())+"]");
    if (ui->btnTiltCalibration->text() == "stop cal"){
        //hwdriver->hwdAskTiltMaxValue();
    }

}

void TfrmTempctrl::SlotGotTiltMaxVal(int TiltX, int TiltY, int Gain, int Resolution){
    (void) Gain;
    (void) Resolution;
    QPointF maxval = QPoint(TiltX,TiltY);
    QPointF minval = hwdriver->getRawTiltMin();
    ui->lblTiltCalibration->setText("x["+QString::number(minval.x())+","+QString::number(maxval.x())+"],y["+QString::number(minval.y())+","+QString::number(maxval.y())+"]");
    hwdriver->hwdAskTiltMinValue();
}

void TfrmTempctrl::slotCOMPorts(const QStringList &list)
{

    //qDebug() << "\n ===> All devices: " << list;
    int SelectedIndex = -1;
    int i=0;
    ui->cbCOMPort->clear();

    foreach (QString s, list) {
        this->m_sde->setDeviceName(s);
        QString syspath = m_sde->systemPath();
        syspath = syspath.left(syspath.indexOf("tty"));
        if (!ComPortSettings.ByName){
            if (ComPortSettings.SysPath == syspath){
                SelectedIndex = i;
                ui->lblFixedName->setText(syspath);
            }
        }else{
            if (ComPortSettings.Name == s){
                SelectedIndex = i;
            }
        }
        if ((ComPortSettings.ByName)&&(ComPortSettings.Name == s)){
            ComPortSettings.SysPath = syspath;
            ui->lblFixedName->setText(syspath);
        }
        if(s.indexOf("USB") > -1){
            ui->cbCOMPort->addItem(s+" | "+m_sde->description());
            i++;
        }
//        qDebug() << "\n <<< info about: " << this->m_sde->name() << " >>>";
//        qDebug() << "-> description  : " << this->m_sde->description();
//        qDebug() << "-> driver       : " << this->m_sde->driver();
//        qDebug() << "-> friendlyName : " << this->m_sde->friendlyName();
//        qDebug() << "-> hardwareID   : " << this->m_sde->hardwareID();
//        qDebug() << "-> locationInfo : " << this->m_sde->locationInfo();
//        qDebug() << "-> manufacturer : " << this->m_sde->manufacturer();
//        qDebug() << "-> productID    : " << this->m_sde->productID();
//        qDebug() << "-> service      : " << this->m_sde->service();
//        qDebug() << "-> shortName    : " << this->m_sde->shortName();
//        qDebug() << "-> subSystem    : " << this->m_sde->subSystem();
//        qDebug() << "-> systemPath   : " << this->m_sde->systemPath();
//        qDebug() << "-> vendorID     : " << this->m_sde->vendorID();

//        qDebug() << "-> revision     : " << this->m_sde->revision();
//        qDebug() << "-> bus          : " << this->m_sde->bus();
//        //
//        qDebug() << "-> is exists    : " << this->m_sde->isExists();
//        qDebug() << "-> is busy      : " << this->m_sde->isBusy();
    }
    ui->cbCOMPort->setCurrentIndex(SelectedIndex);
}

TfrmTempctrl::~TfrmTempctrl()
{
    hwdriver->hwdSetTiltInterval(5000);
    delete m_sde;
    delete bubblewidget;
    delete ui;
}

void TfrmTempctrl::on_cbCOMPort_activated(QString s)
{
    int i = s.indexOf(" | ");
    m_sde->setDeviceName(s.left(i));


    QString syspath = m_sde->systemPath();
    syspath = syspath.left(syspath.indexOf("tty"));

    ui->lblFixedName->setText(syspath);

    ComPortSettings.Name = s.left(i);
    ComPortSettings.SysPath = syspath;
    ComPortSettings.ByName = !ui->chbComBySysPath->isChecked();
}

void TfrmTempctrl::on_buttonBox_accepted()
{
    ms->setComPortConfiguration(ComPortSettings);
    ms->setTargetTemperature(ui->sedtTargetTemp->value());
    hwdriver->hwdSetTargetTemperature(ui->sedtTargetTemp->value());

    ms->setSiteName( ui->lieSiteName->text());
    ms->setSiteLongitude( ui->spbLongitude->value());

    ms->setSiteLatitude( ui->spbLatitude->value());
    ms->setScannerDirection( ui->spbScannerdirection->value());

    if (ScannerConfigModified)
        hwdriver->FetchScannerInfos();
}


void TfrmTempctrl::SlotGotTilt(float x,float y,int Gain,int Resolution,float ResolutionBorder,float MaxTilt){
    (void) x;
    (void) y;
    (void) Gain;
    (void) Resolution;
    (void) ResolutionBorder;
    (void) MaxTilt;
    bubblewidget->SetGainRes(Gain,Resolution,ResolutionBorder,MaxTilt);
    bubblewidget->SetTilt(x,y);
    bubblewidget->SetTiltOffset(ms->getTiltOffset());
    ui->lblTiltRoll->setText(QString("Roll: %1").arg(x));
    ui->lblTiltPitch->setText(QString("Pitch: %1").arg(y));
}


void TfrmTempctrl::SlotGotMotorSetup(int MaxDoasZenithPosition,int ShutterClosePosition, int MaxDoasMicrosteps, int ShutterMicrosteps, int ShutterType){
    (void)MaxDoasMicrosteps;
    (void)ShutterMicrosteps;
    (void)ShutterType;
    ui->lblZenithposition->setText(QString::number(MaxDoasZenithPosition));
    ui->lblShutterClosePos->setText(QString::number(ShutterClosePosition));
}

void TfrmTempctrl::SlotGotScannerStatus(float ScannerTemperature,bool ShutterOpenedBySwitch, bool EndSwitchError){
    (void)EndSwitchError;
    ui->lblScannerTemperature->setText(QString::number(ScannerTemperature));
    if (ShutterOpenedBySwitch){
        ui->lblShutterState->setText("opened");
    }else{
        ui->lblShutterState->setText("closed");
    }
    if (EndSwitchError){
        ui->lblEndSwitchstate->setText("Error");
    }else{
        ui->lblEndSwitchstate->setText("ok");
    }
}

void TfrmTempctrl::on_chbComBySysPath_stateChanged(int )
{
    ComPortSettings.ByName = !ui->chbComBySysPath->isChecked();
}


void TfrmTempctrl::on_btnSetTiltToZero_clicked()
{
    ms->setTiltMaxValue(QPoint(hwdriver->hwdGetRawTilt()));
    ms->setTiltMinValue(QPoint(hwdriver->hwdGetRawTilt()));
    hwdriver->hwdSetTiltMinMaxCalib(hwdriver->hwdGetRawTilt(),hwdriver->hwdGetRawTilt());
    ms->setTiltOffset(hwdriver->hwdGetRawTilt());
}

void TfrmTempctrl::on_btnSetSerialNumber_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, "program serial number",
                                         "Please define a serial number! format: 2011.x.4", QLineEdit::Normal,
                                         ui->lblDeviceSerialNumber->text(), &ok);
    if (ok){
        int guid;
        int devicetype;
        ui->lblDeviceSerialNumber->setText(text);
        serialNumberToRawData(text,&guid,&devicetype);
        hwdriver->hwdSetGUID(guid);
    }
    ScannerConfigModified = true;
}

void TfrmTempctrl::on_btnShutterPosUp_clicked()
{
    hwdriver->hwdSetShuttPos(hwdriver->hwdGetShuttPos()+ui->spbIncrementBy->value());
}

void TfrmTempctrl::on_btnShutterPosDown_clicked()
{
    hwdriver->hwdSetShuttPos(hwdriver->hwdGetShuttPos()-ui->spbIncrementBy->value());
}

void TfrmTempctrl::on_btnSetShutterClosePos_clicked()
{
    hwdriver->hwdSetStepperShutterClosePos();
    hwdriver->hwdAskMotorSetup();
    ScannerConfigModified = true;
}


void TfrmTempctrl::on_btnZenithMotUp_clicked()
{
    hwdriver->hwdMotMoveBySteps(QPoint(hwdriver->getStepperPos().x()+ui->spbIncrementBy->value(),hwdriver->getStepperPos().y()));

}

void TfrmTempctrl::on_btnZenithMotDown_clicked()
{
    hwdriver->hwdMotMoveBySteps(QPoint(hwdriver->getStepperPos().x()-ui->spbIncrementBy->value(),hwdriver->getStepperPos().y()));
}

void TfrmTempctrl::on_btnSetZenithPos_clicked()
{
    hwdriver->hwdSetMaxdoasZenithPos();
    hwdriver->hwdAskMotorSetup();
    ScannerConfigModified = true;
}

void TfrmTempctrl::on_btnTiltCalibration_clicked()
{
    tiltcalibration = !tiltcalibration;
    if (ui->btnTiltCalibration->text() == "start cal"){
        hwdriver->hwdTiltStartCal();
        ui->btnTiltCalibration->setText("stop cal");
        hwdriver->hwdAskTiltMaxValue();
        ScannerConfigModified = true;
    }else{
        hwdriver->hwdTiltStopCal();
        ScannerConfigModified = true;
        ui->btnTiltCalibration->setText("start cal");
    }
}

void TfrmTempctrl::on_btnTiltSetAsZenith_clicked()
{
    hwdriver->hwdTiltSetZenith();
    hwdriver->hwdAskTiltZenithValue();
    ScannerConfigModified = true;
}




void TfrmTempctrl::on_btnMotHome_clicked()
{
    hwdriver->hwdMotIdleState(false);
    hwdriver->hwdGoMotorHome();
}
