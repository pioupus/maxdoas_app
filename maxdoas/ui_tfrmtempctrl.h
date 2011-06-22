/********************************************************************************
** Form generated from reading UI file 'tfrmtempctrl.ui'
**
** Created: Tue Jun 21 23:24:24 2011
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TFRMTEMPCTRL_H
#define UI_TFRMTEMPCTRL_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TfrmTempctrl
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QComboBox *cbCOMPort;
    QLabel *lblFixedName;
    QSpacerItem *horizontalSpacer_2;
    QCheckBox *chbComBySysPath;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_3;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *hbox;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QDoubleSpinBox *sedtTargetTemp;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer;
    QWidget *tab_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *TfrmTempctrl)
    {
        if (TfrmTempctrl->objectName().isEmpty())
            TfrmTempctrl->setObjectName(QString::fromUtf8("TfrmTempctrl"));
        TfrmTempctrl->resize(739, 581);
        gridLayout = new QGridLayout(TfrmTempctrl);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_3 = new QLabel(TfrmTempctrl);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_2->addWidget(label_3);

        cbCOMPort = new QComboBox(TfrmTempctrl);
        cbCOMPort->setObjectName(QString::fromUtf8("cbCOMPort"));
        cbCOMPort->setMinimumSize(QSize(150, 0));
        cbCOMPort->setEditable(false);

        horizontalLayout_2->addWidget(cbCOMPort);

        lblFixedName = new QLabel(TfrmTempctrl);
        lblFixedName->setObjectName(QString::fromUtf8("lblFixedName"));

        horizontalLayout_2->addWidget(lblFixedName);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_3->addLayout(horizontalLayout_2);

        chbComBySysPath = new QCheckBox(TfrmTempctrl);
        chbComBySysPath->setObjectName(QString::fromUtf8("chbComBySysPath"));

        verticalLayout_3->addWidget(chbComBySysPath);


        verticalLayout_2->addLayout(verticalLayout_3);

        tabWidget = new QTabWidget(TfrmTempctrl);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout_3 = new QGridLayout(tab);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        hbox = new QHBoxLayout();
        hbox->setObjectName(QString::fromUtf8("hbox"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        hbox->addItem(verticalSpacer);


        verticalLayout->addLayout(hbox);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(tab);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        sedtTargetTemp = new QDoubleSpinBox(tab);
        sedtTargetTemp->setObjectName(QString::fromUtf8("sedtTargetTemp"));
        sedtTargetTemp->setMinimumSize(QSize(100, 0));
        sedtTargetTemp->setDecimals(1);
        sedtTargetTemp->setMinimum(-50);
        sedtTargetTemp->setSingleStep(0.5);
        sedtTargetTemp->setValue(20);

        horizontalLayout->addWidget(sedtTargetTemp);

        label_2 = new QLabel(tab);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);


        gridLayout_3->addLayout(verticalLayout, 0, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        tabWidget->addTab(tab_2, QString());

        verticalLayout_2->addWidget(tabWidget);

        buttonBox = new QDialogButtonBox(TfrmTempctrl);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(buttonBox);


        gridLayout->addLayout(verticalLayout_2, 0, 0, 1, 1);


        retranslateUi(TfrmTempctrl);
        QObject::connect(buttonBox, SIGNAL(accepted()), TfrmTempctrl, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), TfrmTempctrl, SLOT(reject()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(TfrmTempctrl);
    } // setupUi

    void retranslateUi(QDialog *TfrmTempctrl)
    {
        TfrmTempctrl->setWindowTitle(QApplication::translate("TfrmTempctrl", "Dialog", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("TfrmTempctrl", "Com Port:", 0, QApplication::UnicodeUTF8));
        lblFixedName->setText(QApplication::translate("TfrmTempctrl", "()", 0, QApplication::UnicodeUTF8));
        chbComBySysPath->setText(QApplication::translate("TfrmTempctrl", "Choose by Systempath", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("TfrmTempctrl", "Target temp.", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("TfrmTempctrl", "\302\260C", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("TfrmTempctrl", "Temperature", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("TfrmTempctrl", "Tilt", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TfrmTempctrl: public Ui_TfrmTempctrl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TFRMTEMPCTRL_H
