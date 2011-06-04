/********************************************************************************
** Form generated from reading UI file 'tfrmspectrconfig.ui'
**
** Created: Sat Jun 4 15:36:05 2011
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TFRMSPECTRCONFIG_H
#define UI_TFRMSPECTRCONFIG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_TFrmSpectrConfig
{
public:
    QFormLayout *formLayout;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label;
    QComboBox *cbSpectrList;
    QSpacerItem *horizontalSpacer_5;
    QLabel *label_2;
    QSpinBox *sbAvg;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QRadioButton *rbtnPredefined;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label_3;
    QSpinBox *sbIntegrTime;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *rbtnAuto;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label_4;
    QSpinBox *sbTargetIntensity;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_6;
    QLabel *label_6;
    QSpinBox *sbTargetCorridor;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_5;
    QSpinBox *sbMaxIntegTime;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *TFrmSpectrConfig)
    {
        if (TFrmSpectrConfig->objectName().isEmpty())
            TFrmSpectrConfig->setObjectName(QString::fromUtf8("TFrmSpectrConfig"));
        TFrmSpectrConfig->resize(478, 321);
        formLayout = new QFormLayout(TFrmSpectrConfig);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label = new QLabel(TFrmSpectrConfig);
        label->setObjectName(QString::fromUtf8("label"));
        label->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_4->addWidget(label);

        cbSpectrList = new QComboBox(TFrmSpectrConfig);
        cbSpectrList->setObjectName(QString::fromUtf8("cbSpectrList"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(cbSpectrList->sizePolicy().hasHeightForWidth());
        cbSpectrList->setSizePolicy(sizePolicy);
        cbSpectrList->setMinimumSize(QSize(150, 0));

        horizontalLayout_4->addWidget(cbSpectrList);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_5);

        label_2 = new QLabel(TFrmSpectrConfig);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setEnabled(true);
        label_2->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_4->addWidget(label_2);

        sbAvg = new QSpinBox(TFrmSpectrConfig);
        sbAvg->setObjectName(QString::fromUtf8("sbAvg"));
        sbAvg->setMinimumSize(QSize(100, 0));

        horizontalLayout_4->addWidget(sbAvg);


        verticalLayout_4->addLayout(horizontalLayout_4);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer);

        groupBox = new QGroupBox(TFrmSpectrConfig);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setMinimumSize(QSize(0, 200));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        rbtnPredefined = new QRadioButton(groupBox);
        rbtnPredefined->setObjectName(QString::fromUtf8("rbtnPredefined"));
        rbtnPredefined->setChecked(true);

        horizontalLayout->addWidget(rbtnPredefined);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout->addWidget(label_3);

        sbIntegrTime = new QSpinBox(groupBox);
        sbIntegrTime->setObjectName(QString::fromUtf8("sbIntegrTime"));
        sbIntegrTime->setMinimumSize(QSize(100, 0));

        horizontalLayout->addWidget(sbIntegrTime);


        verticalLayout->addLayout(horizontalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        rbtnAuto = new QRadioButton(groupBox);
        rbtnAuto->setObjectName(QString::fromUtf8("rbtnAuto"));

        horizontalLayout_2->addWidget(rbtnAuto);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_2->addWidget(label_4);

        sbTargetIntensity = new QSpinBox(groupBox);
        sbTargetIntensity->setObjectName(QString::fromUtf8("sbTargetIntensity"));
        sbTargetIntensity->setMinimumSize(QSize(100, 0));

        horizontalLayout_2->addWidget(sbTargetIntensity);


        verticalLayout_2->addLayout(horizontalLayout_2);


        verticalLayout->addLayout(verticalLayout_2);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_6);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_5->addWidget(label_6);

        sbTargetCorridor = new QSpinBox(groupBox);
        sbTargetCorridor->setObjectName(QString::fromUtf8("sbTargetCorridor"));
        sbTargetCorridor->setMinimumSize(QSize(100, 0));

        horizontalLayout_5->addWidget(sbTargetCorridor);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_3->addWidget(label_5);

        sbMaxIntegTime = new QSpinBox(groupBox);
        sbMaxIntegTime->setObjectName(QString::fromUtf8("sbMaxIntegTime"));
        sbMaxIntegTime->setMinimumSize(QSize(100, 0));

        horizontalLayout_3->addWidget(sbMaxIntegTime);


        verticalLayout->addLayout(horizontalLayout_3);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        verticalLayout_4->addWidget(groupBox);

        buttonBox = new QDialogButtonBox(TFrmSpectrConfig);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_4->addWidget(buttonBox);


        formLayout->setLayout(0, QFormLayout::SpanningRole, verticalLayout_4);


        retranslateUi(TFrmSpectrConfig);
        QObject::connect(buttonBox, SIGNAL(accepted()), TFrmSpectrConfig, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), TFrmSpectrConfig, SLOT(reject()));

        QMetaObject::connectSlotsByName(TFrmSpectrConfig);
    } // setupUi

    void retranslateUi(QDialog *TFrmSpectrConfig)
    {
        TFrmSpectrConfig->setWindowTitle(QApplication::translate("TFrmSpectrConfig", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("TFrmSpectrConfig", "Spectrometer", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("TFrmSpectrConfig", "Average", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("TFrmSpectrConfig", "Integrationtime", 0, QApplication::UnicodeUTF8));
        rbtnPredefined->setText(QApplication::translate("TFrmSpectrConfig", "Predefined", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("TFrmSpectrConfig", "IntegrationTime us", 0, QApplication::UnicodeUTF8));
        rbtnAuto->setText(QApplication::translate("TFrmSpectrConfig", "Auto", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("TFrmSpectrConfig", "Targetintensity", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("TFrmSpectrConfig", "Target corridor", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("TFrmSpectrConfig", "MaxIntegrationtime", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TFrmSpectrConfig: public Ui_TFrmSpectrConfig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TFRMSPECTRCONFIG_H
