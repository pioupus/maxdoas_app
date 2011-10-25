/********************************************************************************
** Form generated from reading UI file 'tfrmtempctrl.ui'
**
** Created: Fri Aug 26 20:41:59 2011
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
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
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
    QWidget *tabTemp;
    QGridLayout *gridLayout_3;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *hbox;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QDoubleSpinBox *sedtTargetTemp;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer;
    QWidget *tabTilt;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *tiltlayout;
    QSpacerItem *horizontalSpacer_3;
    QFrame *line;
    QVBoxLayout *verticalLayout_4;
    QLabel *lblTiltRoll;
    QLabel *lblTiltPitch;
    QSpacerItem *verticalSpacer_4;
    QPushButton *btnSetTiltToZero;
    QSpacerItem *verticalSpacer_3;
    QLabel *label_5;
    QLabel *label_4;
    QWidget *tabCompass;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_7;
    QSpacerItem *horizontalSpacer_5;
    QVBoxLayout *verticalLayout_6;
    QSpacerItem *verticalSpacer_2;
    QPushButton *btnCompassCalibrate;
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
        tabTemp = new QWidget();
        tabTemp->setObjectName(QString::fromUtf8("tabTemp"));
        gridLayout_3 = new QGridLayout(tabTemp);
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
        label = new QLabel(tabTemp);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        sedtTargetTemp = new QDoubleSpinBox(tabTemp);
        sedtTargetTemp->setObjectName(QString::fromUtf8("sedtTargetTemp"));
        sedtTargetTemp->setMinimumSize(QSize(100, 0));
        sedtTargetTemp->setDecimals(1);
        sedtTargetTemp->setMinimum(-50);
        sedtTargetTemp->setSingleStep(0.5);
        sedtTargetTemp->setValue(20);

        horizontalLayout->addWidget(sedtTargetTemp);

        label_2 = new QLabel(tabTemp);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);


        gridLayout_3->addLayout(verticalLayout, 0, 0, 1, 1);

        tabWidget->addTab(tabTemp, QString());
        tabTilt = new QWidget();
        tabTilt->setObjectName(QString::fromUtf8("tabTilt"));
        gridLayout_2 = new QGridLayout(tabTilt);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        tiltlayout = new QVBoxLayout();
        tiltlayout->setObjectName(QString::fromUtf8("tiltlayout"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        tiltlayout->addItem(horizontalSpacer_3);


        horizontalLayout_3->addLayout(tiltlayout);

        line = new QFrame(tabTilt);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);

        horizontalLayout_3->addWidget(line);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        lblTiltRoll = new QLabel(tabTilt);
        lblTiltRoll->setObjectName(QString::fromUtf8("lblTiltRoll"));

        verticalLayout_4->addWidget(lblTiltRoll);

        lblTiltPitch = new QLabel(tabTilt);
        lblTiltPitch->setObjectName(QString::fromUtf8("lblTiltPitch"));

        verticalLayout_4->addWidget(lblTiltPitch);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_4);

        btnSetTiltToZero = new QPushButton(tabTilt);
        btnSetTiltToZero->setObjectName(QString::fromUtf8("btnSetTiltToZero"));

        verticalLayout_4->addWidget(btnSetTiltToZero);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_3);

        label_5 = new QLabel(tabTilt);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        verticalLayout_4->addWidget(label_5);

        label_4 = new QLabel(tabTilt);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setMinimumSize(QSize(150, 0));
        label_4->setFrameShape(QFrame::NoFrame);
        label_4->setLineWidth(0);

        verticalLayout_4->addWidget(label_4);


        horizontalLayout_3->addLayout(verticalLayout_4);


        gridLayout_2->addLayout(horizontalLayout_3, 0, 0, 1, 1);

        tabWidget->addTab(tabTilt, QString());
        tabCompass = new QWidget();
        tabCompass->setObjectName(QString::fromUtf8("tabCompass"));
        gridLayout_4 = new QGridLayout(tabCompass);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout_7->addItem(horizontalSpacer_5);


        horizontalLayout_4->addLayout(verticalLayout_7);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer_2);

        btnCompassCalibrate = new QPushButton(tabCompass);
        btnCompassCalibrate->setObjectName(QString::fromUtf8("btnCompassCalibrate"));
        btnCompassCalibrate->setCheckable(true);
        btnCompassCalibrate->setAutoDefault(true);
        btnCompassCalibrate->setDefault(false);
        btnCompassCalibrate->setFlat(false);

        verticalLayout_6->addWidget(btnCompassCalibrate);


        horizontalLayout_4->addLayout(verticalLayout_6);


        gridLayout_4->addLayout(horizontalLayout_4, 0, 0, 1, 1);

        tabWidget->addTab(tabCompass, QString());

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

        tabWidget->setCurrentIndex(2);


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
        tabWidget->setTabText(tabWidget->indexOf(tabTemp), QApplication::translate("TfrmTempctrl", "Temperature", 0, QApplication::UnicodeUTF8));
        lblTiltRoll->setText(QApplication::translate("TfrmTempctrl", "Roll:", 0, QApplication::UnicodeUTF8));
        lblTiltPitch->setText(QApplication::translate("TfrmTempctrl", "Pitch:", 0, QApplication::UnicodeUTF8));
        btnSetTiltToZero->setText(QApplication::translate("TfrmTempctrl", "Set to Zero", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("TfrmTempctrl", "Gain:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("TfrmTempctrl", "Resolution:", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabTilt), QApplication::translate("TfrmTempctrl", "Tilt", 0, QApplication::UnicodeUTF8));
        btnCompassCalibrate->setText(QApplication::translate("TfrmTempctrl", "Calibration mode", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabCompass), QApplication::translate("TfrmTempctrl", "Compass", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TfrmTempctrl: public Ui_TfrmTempctrl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TFRMTEMPCTRL_H
