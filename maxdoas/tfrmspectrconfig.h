#ifndef TFRMSPECTRCONFIG_H
#define TFRMSPECTRCONFIG_H

#include <QDialog>

namespace Ui {
    class TFrmSpectrConfig;
}

class TFrmSpectrConfig : public QDialog {
    Q_OBJECT
public:
    TFrmSpectrConfig(QWidget *parent = 0);
    ~TFrmSpectrConfig();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TFrmSpectrConfig *ui;
};

#endif // TFRMSPECTRCONFIG_H
