#include "tfrmspectrconfig.h"
#include "ui_tfrmspectrconfig.h"


TFrmSpectrConfig::TFrmSpectrConfig(THWDriver *hwdriver,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TFrmSpectrConfig)
{

    ui->setupUi(this);
    GotSpecList = false;
    this->hwdriver=hwdriver;
    connect(hwdriver,SIGNAL(hwdSigSpectrometersDiscovered()),this,SLOT(SpectrometersDiscovered()));
    ms = TMaxdoasSettings::instance();

    ui->sbAvg->setValue(ms->getRetrievalAvgCount());
    ui->sbIntegrTime->setValue(ms->getRetrievalIntegTimeuS());
    ui->sbTargetIntensity->setValue(ms->getRetrievalAutoTargetPeak());
    ui->sbTargetCorridor->setValue(ms->getRetrievalAutoTargetPeakCorridor());
    ui->sbMaxIntegTime->setValue(ms->getRetrievalAutoMaxIntegTime());
    ui->rbtnAuto->setChecked(ms->getRetrievalAutoEnabled());
    hwdriver->hwdDiscoverSpectrometers();


}

void TFrmSpectrConfig::SpectrometersDiscovered(){
    QList<QString> specl;
    QString serial;

    specl = hwdriver->hwdGetListSpectrometer();
    serial = ms->getPreferredSpecSerial();
    ui->cbSpectrList->addItems(specl);
    ui->cbSpectrList->setCurrentIndex(specl.indexOf(serial));
    GotSpecList = true;
}

TFrmSpectrConfig::~TFrmSpectrConfig()
{
    delete ui;
}

void TFrmSpectrConfig::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TFrmSpectrConfig::on_buttonBox_accepted(){
    if (GotSpecList)
        ms->setPreferredSpecSerial(ui->cbSpectrList->currentText());
    ms->setRetrievalAvgCount(ui->sbAvg->value());
    ms->setRetrievalIntegTimeuS(ui->sbIntegrTime->value());
    ms->setRetrievalAutoTargetPeak(ui->sbTargetIntensity->value());
    ms->setRetrievalAutoTargetPeakCorridor(ui->sbTargetCorridor->value());
    ms->setRetrievalAutoMaxIntegTime(ui->sbMaxIntegTime->value());
    ms->setRetrievalAutoEnabled(ui->rbtnAuto->isChecked());
}
