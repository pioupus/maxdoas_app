#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include "thwdriver.h"
#include <qwt_plot_curve.h>

#include "log4qt/consoleappender.h"
#include "log4qt/logger.h"
#include "log4qt/ttcclayout.h"

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

private:
    Ui::MainWindow *ui;

    QwtPlot *ImagePlot;
    QwtPlot *SpectrPlot;
    QwtPlotCurve *SpectrPlotCurve;
    QwtPlotSpectrogram *d_spectrogram;
    THWDriver  *HWDriver;
    void timerEvent(QTimerEvent *);
private slots:

    void on_actionTempctrler_triggered();
    void on_actionConfigSpectrometer_triggered();


};

#endif // MAINWINDOW_H
