#ifndef TSPECTRUMPLOTTER_H
#define TSPECTRUMPLOTTER_H
#include <QMutex>
#include <QBoxLayout>
#include <QObject>
#include <QScriptable>
#include <QPair>

#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>

#include "tspectrum.h"

class TPlot
{
public:
    TPlot(QBoxLayout *parent);
    ~TPlot();
    QwtPlotCurve * getLastCurve();
    QwtPlotCurve * getCurve(int index);
    void addCurve();
    int CurveCount();
    int MarkerCount();
    QwtPlot *getPlot();
    QwtLegend * getLegend(int pos =-1);
    QwtPlotMarker * getMarker(int index);
    QwtPlotMarker * addMarker();
    QwtPlotMarker * getAutoIntegTimeMarker(int index);
    void clearMarkers();
    void clearAutoIntegTimeMarkers();
    void clearLegend();
    void clearCurves();
    void setLastSpecDate(QDateTime lastSpecDate);
    QDateTime  getLastSpecDate();
private:
    QBoxLayout *parent;
    QwtPlot *plot;
    QwtLegend *legend;
    QList<QwtPlotCurve *> CurveList;
    QDateTime lastSpecDate;
    QList<QwtPlotMarker *> MarkerList;

    QwtPlotMarker * AutoIntegMarkerTop;
    QwtPlotMarker * AutoIntegMarkerCenter;
    QwtPlotMarker * AutoIntegMarkerBot;
};

class TSpectrumPlotter:public QObject, protected QScriptable
{
    Q_OBJECT

public:
    void setParentLayout(QBoxLayout *parent);

    void plotSpectrum(TSpectrum *spectrum,int plotIndex);

    static TSpectrumPlotter* instance(QObject *parent)
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();

            if (!m_Instance){
                m_Instance = new TSpectrumPlotter(parent);
            }
            mutex.unlock();
        }

        return m_Instance;
    }

    static void drop()
    {
        static QMutex mutex;
        mutex.lock();
        delete m_Instance;
        m_Instance = 0;
        mutex.unlock();
    }

public slots:

    void setTitle(QString text,int plotindex = -1);
    void setXAxisTitle(QString text,int plotindex = -1);
    void setYAxisTitle(QString text,int plotindex = -1);

    void setXAxisRange(double min,double max,int plotindex = -1);
    void setYAxisRange(double min,double max,int plotindex = -1);

    void setLegend(QString title,int pos, int plotindex,int curveindex = -1);
    void setCurveColor(int color,int plotindex = -1);

    void plotVMarker(double x,QString title,int plotindex);
    void plotHMarker(double y,QString title,int plotindex);
    void plotXYMarker(double x,double y,QString title,int plotindex);

    void plotAutoIntegrationtimeParameters(bool enabled);

    void plotToFile(QString format, QString Directory , QString BaseName, int SequenceNo , int plotindex,int width,int height, int resolution );
    void plotToFile(QString format, QString filename, int plotindex,int width ,int height, int resolution );

    void reset(int plotindex);
    void clearMarker(int plotindex);
private:
    TSpectrumPlotter(QObject *parent = 0);
    ~TSpectrumPlotter();
    TSpectrumPlotter(const TSpectrumPlotter &); // hide copy constructor
    TSpectrumPlotter& operator=(const TSpectrumPlotter &); // hide assign op
                                 // we leave just the declarations, so the compiler will warn us
                                 // if we try to use those two functions by accident
    QBoxLayout *parentLayout;
    static TSpectrumPlotter* m_Instance;
    QList<TPlot*> PlotList;
    TPlot* getPlot(int index);
    QString nextTitle;
    QString nextXTitle;
    QString nextYTitle;

    QPair<double,double> nextXRange;
    QPair<double,double> nextYRange;
    QColor *nextColor;
    bool plotAutoIntegrationtimeParametersEnabled;
};



#endif // TSPECTRUMPLOTTER_H
