#include "tfrmspectrconfig.h"
#include "ui_tfrmspectrconfig.h"


TFrmSpectrConfig::TFrmSpectrConfig(THWDriver *hwdriver,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TFrmSpectrConfig)
{
    TAutoIntegConf integtimeconf;
    ui->setupUi(this);
    GotSpecList = false;
    this->hwdriver=hwdriver;
    setAttribute(Qt::WA_DeleteOnClose, true);
    connect(hwdriver,SIGNAL(hwdSigSpectrometersDiscovered()),this,SLOT(SpectrometersDiscovered()));
    ms = TMaxdoasSettings::instance();
    integtimeconf = ms->getAutoIntegrationRetrievalConf();
    ui->sbAvg->setValue(ms->getRetrievalAvgCount());
    ui->sbIntegrTime->setValue(integtimeconf.fixedIntegtime);
    ui->sbTargetIntensity->setValue(integtimeconf.targetPeak);
    ui->sbTargetCorridor->setValue(integtimeconf.targetCorridor);
    ui->sbMaxIntegTime->setValue(integtimeconf.maxIntegTime);
    ui->rbtnAuto->setChecked(integtimeconf.autoenabled);
    hwdriver->hwdDiscoverSpectrometers();


}

void TFrmSpectrConfig::SpectrometersDiscovered(){
    QList<QString> specl;
    int i;
    bool changed=false;
    QString actserial = ui->cbSpectrList->currentText();

    specl = hwdriver->hwdGetListSpectrometer();
    if (actserial == "")
        actserial = ms->getPreferredSpecSerial();
    for (i = 0; i<specl.count();i++ ){
        if (ui->cbSpectrList->findText(specl[i])==-1){
            ui->cbSpectrList->addItem(specl[i]);
            changed = true;
        }
    }
    if (changed)
        ui->cbSpectrList->setCurrentIndex(specl.indexOf(actserial));
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
    TAutoIntegConf integtimeconf;
    if (GotSpecList)
        ms->setPreferredSpecSerial(ui->cbSpectrList->currentText());
    integtimeconf.autoenabled       = ui->rbtnAuto->isChecked();
    integtimeconf.fixedIntegtime    = ui->sbIntegrTime->value();
    integtimeconf.maxIntegTime      = ui->sbMaxIntegTime->value();
    integtimeconf.targetCorridor    = ui->sbTargetCorridor->value();
    integtimeconf.targetPeak        = ui->sbTargetIntensity->value();
    ms->setAutoIntegrationRetrievalConf(integtimeconf);
    ms->setRetrievalAvgCount(ui->sbAvg->value());

}
