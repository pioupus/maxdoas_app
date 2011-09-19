#ifndef TSPECTRUMPLOTTER_H
#define TSPECTRUMPLOTTER_H
#include <QMutex>
#include <QBoxLayout>
#include "tspectrum.h"
#include <qwt_plot_curve.h>

class TPlot
{
public:
    TPlot(QBoxLayout *parent);
    ~TPlot();
    QwtPlotCurve * getLastCurve();
    QwtPlotCurve * getCurve(int index);
    void addCurve();
    int count();
    QwtPlot *getPlot();
private:
    QBoxLayout *parent;
    QwtPlot *plot;
    QList<QwtPlotCurve *> CurveList;
};

class TSpectrumPlotter
{
public:
    void setParentLayout(QBoxLayout *parent);

    void plotSpectrum(TSpectrum *spectrum,int plotIndex);

    static TSpectrumPlotter* instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();

            if (!m_Instance){
                m_Instance = new TSpectrumPlotter();
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
private:
    TSpectrumPlotter();
    ~TSpectrumPlotter();
    TSpectrumPlotter(const TSpectrumPlotter &); // hide copy constructor
    TSpectrumPlotter& operator=(const TSpectrumPlotter &); // hide assign op
                                 // we leave just the declarations, so the compiler will warn us
                                 // if we try to use those two functions by accident
    QBoxLayout *parentLayout;
    static TSpectrumPlotter* m_Instance;
    QList<TPlot*> PlotList;

};



#endif // TSPECTRUMPLOTTER_H
