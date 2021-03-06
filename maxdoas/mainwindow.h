#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_layout.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_canvas.h>
#include <qwtsymbolarrow.h>
#include <qwtmarkerarrow.h>
#include <qwtsymbolarrow.h>
#include <qwtmarkerarrow.h>

#include <qfiledialog.h>
#include <qimagewriter.h>
#include <qwt_plot_marker.h>
#include "log4qt/consoleappender.h"
#include "log4qt/logger.h"
#include "log4qt/ttcclayout.h"
#include "thwdriver.h"
#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include "scriptwrapper.h"
#include "jsedit/jsedit.h"

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
    void on_BtnStart_clicked();
    void on_BtnStop_clicked();
    void HWThreadFinished();
    void on_actionTempctrler_triggered();
    void on_actionInfo_triggered();
    void on_actionConfigSpectrometer_triggered();
    void on_actionClose_triggered();
    void on_actionOpen_triggered();
    void on_GotSpectrum();
    void StartMeasure();
    void COMPortChanged(QString name, bool opened, bool error);
    void onScriptTerminated();

private:
    QScriptEngine *ScriptEngine;
    QScriptEngineDebugger *ScriptDebugger;
    QWidget *DebugOutputWidget;
    QWidget *DebugLocalsWidget;
    QWidget *DebugCodeWidget;
    QWidget *DebugBPWidget;
    QWidget *DebugStackWidget;
    QWidget *DebugConsoleWidget;
    QToolBar *DebugToolbar;
    JSEdit *ScriptEditor;
    THWDriver *HWDriver;
//    QwtPlot *ImagePlot;
//    QwtPlot *SpectrPlot;
//    QwtPlotCurve *SpectrPlotCurve;
//    QwtPlotSpectrogram *d_spectrogram;
//    QwtPlotMarker *marker_corr_top;
//    QwtPlotMarker *marker_corr_bot;
//    QwtPlotMarker *marker_target;
    QLabel *lblComPortStatus;
    TSpectrum spectrum;
    TMaxdoasSettings *ms;
    void closeEvent(QCloseEvent *event);
    bool closenow;
    TScriptWrapper *scriptWrapper;
    Ui::MainWindow *ui;
    bool HWThreadIsFinished;
    void startScript(QString filename);

};

#endif // MAINWINDOW_H
