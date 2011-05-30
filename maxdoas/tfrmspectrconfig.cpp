#include "tfrmspectrconfig.h"
#include "ui_tfrmspectrconfig.h"

TFrmSpectrConfig::TFrmSpectrConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TFrmSpectrConfig)
{
    ui->setupUi(this);
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
