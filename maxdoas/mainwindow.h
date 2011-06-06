#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
#include <qwt_plot_curve.h>
#include <qfiledialog.h>
#include <qimagewriter.h>
#include <qwt_plot_marker.h>
#include "log4qt/consoleappender.h"
#include "log4qt/logger.h"
#include "log4qt/ttcclayout.h"
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
    QwtPlotMarker *marker_corr_top;
    QwtPlotMarker *marker_corr_bot;
    QwtPlotMarker *marker_target;
    TSpectrum spectrum;
    TMaxdoasSettings *ms;
    void closeEvent(QCloseEvent *event);
    bool closenow;

};

#endif // MAINWINDOW_H
