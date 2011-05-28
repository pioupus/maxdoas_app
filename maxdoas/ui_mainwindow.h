/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Fri May 27 16:42:36 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionTempctrler;
    QAction *actionSensors;
    QAction *actionClose;
    QAction *actionHjkjhkj;
    QWidget *centralWidget;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *hbox;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuConfiguration;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(936, 508);
        actionTempctrler = new QAction(MainWindow);
        actionTempctrler->setObjectName(QString::fromUtf8("actionTempctrler"));
        actionSensors = new QAction(MainWindow);
        actionSensors->setObjectName(QString::fromUtf8("actionSensors"));
        actionClose = new QAction(MainWindow);
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        actionHjkjhkj = new QAction(MainWindow);
        actionHjkjhkj->setObjectName(QString::fromUtf8("actionHjkjhkj"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalLayoutWidget = new QWidget(centralWidget);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(10, 30, 911, 421));
        hbox = new QHBoxLayout(horizontalLayoutWidget);
        hbox->setSpacing(6);
        hbox->setContentsMargins(11, 11, 11, 11);
        hbox->setObjectName(QString::fromUtf8("hbox"));
        hbox->setSizeConstraint(QLayout::SetDefaultConstraint);
        hbox->setContentsMargins(0, 0, 0, 0);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 936, 25));
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
        menuFile->addAction(actionClose);
        menuConfiguration->addAction(actionTempctrler);
        menuConfiguration->addAction(actionSensors);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionTempctrler->setText(QApplication::translate("MainWindow", "Temperaturecontroller", 0, QApplication::UnicodeUTF8));
        actionSensors->setText(QApplication::translate("MainWindow", "Motion controll sensors", 0, QApplication::UnicodeUTF8));
        actionClose->setText(QApplication::translate("MainWindow", "Close", 0, QApplication::UnicodeUTF8));
        actionHjkjhkj->setText(QApplication::translate("MainWindow", "hjkjhkj", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuConfiguration->setTitle(QApplication::translate("MainWindow", "Configuration", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
