#ifndef TFRMTEMPCTRL_H
#define TFRMTEMPCTRL_H

#include <QDialog>

#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <qwt_plot_canvas.h>
#include "thwdriver.h"
#include "serialdeviceenumerator.h"
#include "maxdoassettings.h"
#include "bubblewidget.h"

#define TEMPERAT_BUFFER_SIZE 100

namespace Ui {
    class TfrmTempctrl;
}

class TfrmTempctrl : public QDialog
{
    Q_OBJECT

public:
    explicit TfrmTempctrl(THWDriver *hwdriver,QWidget *parent = 0);
    ~TfrmTempctrl();
   // void setHWDriver(THWDriver *hwdriver);
private:
    Ui::TfrmTempctrl *ui;
    QwtPlotCurve *curvePeltier;
    QwtPlotCurve *curveHeatsink;
    QwtPlotCurve *curveSpectrometer;
    QwtPlot *plot;
    QwtLegend *legend;
    double BufferX1[TEMPERAT_BUFFER_SIZE];
    double BufferPeltier[TEMPERAT_BUFFER_SIZE];
    double BufferHeatSink[TEMPERAT_BUFFER_SIZE];
    double BufferSpectrometer[TEMPERAT_BUFFER_SIZE];
    int BufferFilled;
    double factor1,factor2;
    THWDriver *hwdriver;
    SerialDeviceEnumerator *m_sde;
    TMaxdoasSettings *ms;
    TCOMPortConf ComPortSettings;
    TBubbleWidget *bubblewidget;
    bool tiltcalibration;
    bool ScannerConfigModified;
private slots:
    void on_btnSetZenithPos_clicked();
    void on_btnSetShutterClosePos_clicked();
    void on_btnTiltSetAsZenith_clicked();
    void on_btnTiltCalibration_clicked();
    void on_btnZenithMotDown_clicked();
    void on_btnZenithMotUp_clicked();
    void on_btnShutterPosDown_clicked();
    void on_btnShutterPosUp_clicked();
    void on_btnSetSerialNumber_clicked();
    void on_btnSetTiltToZero_clicked();
    void on_chbComBySysPath_stateChanged(int );
    void on_buttonBox_accepted();
    void on_cbCOMPort_activated(QString s);
    void showCurve(QwtPlotItem *item, bool on);
    void SloGotTemperature(float TemperaturePeltier, float TemperatureSpectr, float TemperatureHeatsink);
    void slotCOMPorts(const QStringList &list);
    void SlotGotTilt(float x,float y,int Gain,int Resolution,float ResolutionBorder,float MaxTilt);
    void SlotGotDeviceInfo(int GitHash,int guid, int deviceType);
    void SlotGotMotorSetup(int MaxDoasZenithPosition,int ShutterClosePosition, int MaxDoasMicrosteps, int ShutterMicrosteps, int ShutterType);
    void SlotGotScannerStatus(float ScannerTemperature,bool ShutterOpenedBySwitch, bool EndSwitchError);
    void SlotGotTiltDirection(float TiltDirection);

    void SlotGotTiltMinVal(int TiltX, int TiltY, int Gain, int Resolution);
    void SlotGotTiltMaxVal(int TiltX, int TiltY, int Gain, int Resolution);
    void on_btnMotHome_clicked();

protected:
    virtual void timerEvent(QTimerEvent *e);
};

#endif // TFRMTEMPCTRL_H
