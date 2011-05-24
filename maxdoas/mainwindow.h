#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QwtPlot *plot;
    QwtPlotSpectrogram *d_spectrogram;

private slots:

    void on_actionTempctrler_triggered();


};

#endif // MAINWINDOW_H
