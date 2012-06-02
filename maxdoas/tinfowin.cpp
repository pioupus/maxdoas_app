#include "tinfowin.h"
#include "ui_tinfowin.h"
#include "vc.h"

TInfoWin::TInfoWin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TInfoWin)
{
    ui->setupUi(this);
    ui->lblDate->setText("Date: "+QString(GITDATE));
    ui->lblHash->setText("Version: "+QString(GITHASH));
    QPalette palette( ui->lneURL->palette() );
    QBrush br = palette.brush(palette.Window);

    palette.setBrush( QPalette::Base, br );
    ui->lneURL->setPalette(palette);
}

TInfoWin::~TInfoWin()
{
    delete ui;
}

void TInfoWin::on_pushButton_clicked()
{
    close();
}
