#include "tfrmtempctrl.h"
#include "ui_tfrmtempctrl.h"




TfrmTempctrl::TfrmTempctrl(THWDriver *hwdriver, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TfrmTempctrl)
{
    int i;
    ui->setupUi(this);
    this->hwdriver=hwdriver;
    setAttribute(Qt::WA_DeleteOnClose, true);
    plot = new QwtPlot(this);
    curvePeltier = new QwtPlotCurve("Peltier");
    curveHeatsink = new QwtPlotCurve("Heatsink");
    curveSpectrometer = new QwtPlotCurve("Spectrometer");
    QwtLegend *legend = new QwtLegend;
    bubblewidget = new TBubbleWidget;
    ui->tiltlayout->addWidget(bubblewidget);
    hwdriver->hwdSetTiltInterval(500);
    legend->setItemMode(QwtLegend::CheckableItem);
    plot->insertLegend(legend, QwtPlot::RightLegend);
  //  plot->aut
    ui->hbox->addWidget(plot);
    ms = TMaxdoasSettings::instance();
    ComPortSettings = ms->getComPortConfiguration();
    ui->sedtTargetTemp->setValue(ms->getTargetTemperature());
    ui->chbComBySysPath->setChecked(!ComPortSettings.ByName);
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

 //   void hwdSigGotTilt(float TiltX,float TiltY,int Gain, int Resolution,float ResolutionBorder,float MaxTilt);

    showCurve(curvePeltier, true);
    showCurve(curveHeatsink, true);
    showCurve(curveSpectrometer, true);

    this->m_sde = new SerialDeviceEnumerator(this);
    connect(this->m_sde, SIGNAL(hasChanged(QStringList)),
            this, SLOT(slotCOMPorts(QStringList)));
    this->m_sde->setEnabled(true);

    show();
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

void TfrmTempctrl::on_chbComBySysPath_stateChanged(int )
{
    ComPortSettings.ByName = !ui->chbComBySysPath->isChecked();
}

void TfrmTempctrl::on_btnSetTiltToZero_clicked()
{

    ms->setTiltOffset(QPointF(hwdriver->hwdGetTilt()));
    hwdriver->hwdSetTiltOffset(hwdriver->hwdGetTilt());
}
