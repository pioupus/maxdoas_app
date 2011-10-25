#ifndef TFRMSPECTRCONFIG_H
#define TFRMSPECTRCONFIG_H
#include "maxdoassettings.h"

#include <QDialog>
#include "thwdriver.h"

namespace Ui {
    class TFrmSpectrConfig;
}

class TFrmSpectrConfig : public QDialog {
    Q_OBJECT
public:
    TFrmSpectrConfig(THWDriver *hwdriver,QWidget *parent = 0);
    ~TFrmSpectrConfig();

protected:
    void changeEvent(QEvent *e);

private:

    Ui::TFrmSpectrConfig *ui;
    THWDriver *hwdriver;
    TMaxdoasSettings *ms;
    bool GotSpecList;
private slots:
    void SpectrometersDiscovered();
    void on_buttonBox_accepted();
};

#endif // TFRMSPECTRCONFIG_H
