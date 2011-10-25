/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Thu Sep 8 02:15:59 2011
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
#include <QtGui/QToolButton>
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
    QVBoxLayout *mainVertLayout;
    QHBoxLayout *horizontalLayout_2;
    QToolButton *BtnStop;
    QToolButton *BtnStart;
    QSpacerItem *horizontalSpacer;
    QTabWidget *tabWidget;
    QWidget *tabPlot;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *hbox;
    QSpacerItem *verticalSpacer_2;
    QWidget *tabScript;
    QGridLayout *gridLayout_4;
    QHBoxLayout *ScriptLayout;
    QWidget *tabDebug;
    QGridLayout *gridLayout_3;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *code_vertLayout;
    QHBoxLayout *code_horizLayout;
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
        mainVertLayout = new QVBoxLayout();
        mainVertLayout->setSpacing(6);
        mainVertLayout->setObjectName(QString::fromUtf8("mainVertLayout"));
        mainVertLayout->setContentsMargins(-1, 0, -1, -1);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 0, -1, -1);
        BtnStop = new QToolButton(centralWidget);
        BtnStop->setObjectName(QString::fromUtf8("BtnStop"));
        BtnStop->setEnabled(false);

        horizontalLayout_2->addWidget(BtnStop);

        BtnStart = new QToolButton(centralWidget);
        BtnStart->setObjectName(QString::fromUtf8("BtnStart"));
        BtnStart->setEnabled(false);

        horizontalLayout_2->addWidget(BtnStart);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        mainVertLayout->addLayout(horizontalLayout_2);

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
        gridLayout_4 = new QGridLayout(tabScript);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        ScriptLayout = new QHBoxLayout();
        ScriptLayout->setSpacing(6);
        ScriptLayout->setObjectName(QString::fromUtf8("ScriptLayout"));

        gridLayout_4->addLayout(ScriptLayout, 0, 0, 1, 1);

        tabWidget->addTab(tabScript, QString());
        tabDebug = new QWidget();
        tabDebug->setObjectName(QString::fromUtf8("tabDebug"));
        gridLayout_3 = new QGridLayout(tabDebug);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        code_vertLayout = new QVBoxLayout();
        code_vertLayout->setSpacing(6);
        code_vertLayout->setObjectName(QString::fromUtf8("code_vertLayout"));
        code_vertLayout->setContentsMargins(10, -1, -1, -1);

        horizontalLayout->addLayout(code_vertLayout);

        code_horizLayout = new QHBoxLayout();
        code_horizLayout->setSpacing(6);
        code_horizLayout->setObjectName(QString::fromUtf8("code_horizLayout"));

        horizontalLayout->addLayout(code_horizLayout);


        gridLayout_3->addLayout(horizontalLayout, 0, 0, 1, 1);

        tabWidget->addTab(tabDebug, QString());

        mainVertLayout->addWidget(tabWidget);

        debugoutLayout = new QHBoxLayout();
        debugoutLayout->setSpacing(6);
        debugoutLayout->setObjectName(QString::fromUtf8("debugoutLayout"));
        debugoutLayout->setContentsMargins(-1, 0, 0, -1);
        debugoutspcer = new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Maximum);

        debugoutLayout->addItem(debugoutspcer);


        mainVertLayout->addLayout(debugoutLayout);


        gridLayout->addLayout(mainVertLayout, 2, 0, 1, 1);

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
        BtnStop->setText(QApplication::translate("MainWindow", "stop", 0, QApplication::UnicodeUTF8));
        BtnStart->setText(QApplication::translate("MainWindow", "start", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabPlot), QApplication::translate("MainWindow", "Plots", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabScript), QApplication::translate("MainWindow", "Script", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabDebug), QApplication::translate("MainWindow", "Script Debugger", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuConfiguration->setTitle(QApplication::translate("MainWindow", "Configuration", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
