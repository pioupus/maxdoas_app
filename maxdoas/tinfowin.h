#ifndef TINFOWIN_H
#define TINFOWIN_H

#include <QDialog>

namespace Ui {
class TInfoWin;
}

class TInfoWin : public QDialog
{
    Q_OBJECT
    
public:
    explicit TInfoWin(QWidget *parent = 0);
    ~TInfoWin();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::TInfoWin *ui;
};

#endif // TINFOWIN_H
