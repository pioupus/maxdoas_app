/********************************************************************************
** Form generated from reading UI file 'tfrmtempctrl.ui'
**
** Created: Mon May 23 19:06:57 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TFRMTEMPCTRL_H
#define UI_TFRMTEMPCTRL_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TfrmTempctrl
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *hbox;

    void setupUi(QDialog *TfrmTempctrl)
    {
        if (TfrmTempctrl->objectName().isEmpty())
            TfrmTempctrl->setObjectName(QString::fromUtf8("TfrmTempctrl"));
        TfrmTempctrl->resize(739, 505);
        buttonBox = new QDialogButtonBox(TfrmTempctrl);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 470, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        horizontalLayoutWidget = new QWidget(TfrmTempctrl);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(10, 10, 721, 431));
        hbox = new QHBoxLayout(horizontalLayoutWidget);
        hbox->setObjectName(QString::fromUtf8("hbox"));
        hbox->setContentsMargins(0, 0, 0, 0);

        retranslateUi(TfrmTempctrl);
        QObject::connect(buttonBox, SIGNAL(accepted()), TfrmTempctrl, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), TfrmTempctrl, SLOT(reject()));

        QMetaObject::connectSlotsByName(TfrmTempctrl);
    } // setupUi

    void retranslateUi(QDialog *TfrmTempctrl)
    {
        TfrmTempctrl->setWindowTitle(QApplication::translate("TfrmTempctrl", "Dialog", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TfrmTempctrl: public Ui_TfrmTempctrl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TFRMTEMPCTRL_H
