#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "log4qt/consoleappender.h"
#include "log4qt/logger.h"
#include "log4qt/ttcclayout.h"
#include "mainwindowgui.h"
#include "thwdriver.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit MainWindow(QWidget *parent = 0);
    void setupLog4Qt();
    ~MainWindow();

private slots:
   // void startprogram();

private slots:
    void HWThreadFinished();
    void on_actionTempctrler_triggered();
    void on_actionConfigSpectrometer_triggered();
    void on_actionClose_triggered();
    void on_GotSpectrum();
    void StartMeasure();

private:
    Ui::MainWindow *ui;
    THWDriver *HWDriver;
    QwtPlot *ImagePlot;
    QwtPlot *SpectrPlot;
    QwtPlotCurve *SpectrPlotCurve;
    QwtPlotSpectrogram *d_spectrogram;
    TSpectrum spectrum;
    TMaxdoasSettings *ms;
    void closeEvent(QCloseEvent *event);
    bool closenow;

};

#endif // MAINWINDOW_H
