/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Tue Sep 6 00:16:32 2011
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionTempctrler;
    QAction *actionSensors;
    QAction *actionClose;
    QAction *actionHjkjhkj;
    QAction *actionConfigSpectrometer;
    QAction *actionOpen;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_2;
    QTabWidget *tabWidget;
    QWidget *tabPlot;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *hbox;
    QSpacerItem *verticalSpacer_2;
    QWidget *tabScript;
    QHBoxLayout *debugoutLayout;
    QSpacerItem *debugoutspcer;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuConfiguration;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(919, 481);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        actionTempctrler = new QAction(MainWindow);
        actionTempctrler->setObjectName(QString::fromUtf8("actionTempctrler"));
        actionSensors = new QAction(MainWindow);
        actionSensors->setObjectName(QString::fromUtf8("actionSensors"));
        actionClose = new QAction(MainWindow);
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        actionHjkjhkj = new QAction(MainWindow);
        actionHjkjhkj->setObjectName(QString::fromUtf8("actionHjkjhkj"));
        actionConfigSpectrometer = new QAction(MainWindow);
        actionConfigSpectrometer->setObjectName(QString::fromUtf8("actionConfigSpectrometer"));
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy1);
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(-1, 0, -1, -1);
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabPlot = new QWidget();
        tabPlot->setObjectName(QString::fromUtf8("tabPlot"));
        gridLayout_2 = new QGridLayout(tabPlot);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetNoConstraint);
        hbox = new QHBoxLayout();
        hbox->setSpacing(6);
        hbox->setObjectName(QString::fromUtf8("hbox"));
        hbox->setSizeConstraint(QLayout::SetDefaultConstraint);
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        hbox->addItem(verticalSpacer_2);


        verticalLayout->addLayout(hbox);


        gridLayout_2->addLayout(verticalLayout, 0, 0, 1, 1);

        tabWidget->addTab(tabPlot, QString());
        tabScript = new QWidget();
        tabScript->setObjectName(QString::fromUtf8("tabScript"));
        tabWidget->addTab(tabScript, QString());

        verticalLayout_2->addWidget(tabWidget);

        debugoutLayout = new QHBoxLayout();
        debugoutLayout->setSpacing(6);
        debugoutLayout->setObjectName(QString::fromUtf8("debugoutLayout"));
        debugoutLayout->setContentsMargins(-1, 0, 0, -1);
        debugoutspcer = new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Maximum);

        debugoutLayout->addItem(debugoutspcer);


        verticalLayout_2->addLayout(debugoutLayout);


        gridLayout->addLayout(verticalLayout_2, 2, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 919, 23));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuConfiguration = new QMenu(menuBar);
        menuConfiguration->setObjectName(QString::fromUtf8("menuConfiguration"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuConfiguration->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionClose);
        menuConfiguration->addAction(actionTempctrler);
        menuConfiguration->addAction(actionSensors);
        menuConfiguration->addAction(actionConfigSpectrometer);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionTempctrler->setText(QApplication::translate("MainWindow", "Temperaturecontroller", 0, QApplication::UnicodeUTF8));
        actionSensors->setText(QApplication::translate("MainWindow", "Motion controll sensors", 0, QApplication::UnicodeUTF8));
        actionClose->setText(QApplication::translate("MainWindow", "Close", 0, QApplication::UnicodeUTF8));
        actionHjkjhkj->setText(QApplication::translate("MainWindow", "hjkjhkj", 0, QApplication::UnicodeUTF8));
        actionConfigSpectrometer->setText(QApplication::translate("MainWindow", "Spectrometer", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("MainWindow", "Open Script", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabPlot), QApplication::translate("MainWindow", "Plots", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabScript), QApplication::translate("MainWindow", "Script", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuConfiguration->setTitle(QApplication::translate("MainWindow", "Configuration", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
