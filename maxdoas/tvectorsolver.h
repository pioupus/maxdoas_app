#ifndef TVECTORSOLVER_H
#define TVECTORSOLVER_H
#include <QPointF>
#include "tretrievalimage.h"

class TVectorSolver:public QObject
{
    Q_OBJECT

public slots:
    void setConstraintVec(double constraint);
    void setConstraintSrcOET(double constraint);
    void setConstraintSrcTikhonov(double constraint);
    void setImageSmoothValue(int smoothImg);
    void setSrcSmoothValue(int smoothSrc);
    void setSrcVal(float SrcVal);
    void setSrcPosition(QPoint SrcPosition);
    void setSADiagonalvalue(float SaDiag);
    void setCorrThreshold(float CorrThreshold);
    void setAPrioriVec(QPointF APrioriVec);
    void setMeanDistance(float Distance);//in meter


    void solve(TRetrievalImage* imgOldCd,TRetrievalImage* imgOldCorr,TRetrievalImage* imgNewCd,TRetrievalImage* imgNewCorr);

    void loadWeightedColoumDensitiesToRetrieval(void);
    TRetrievalImage* getRetrieval(void);
    QPointF getMeanVec(void);
    float getMeanVelocity();


public:

    static TVectorSolver* instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();

            if (!m_Instance){
                m_Instance = new TVectorSolver();
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
    TVectorSolver();
    ~TVectorSolver();
    TVectorSolver(const TVectorSolver &); // hide copy constructor
    TVectorSolver& operator=(const TVectorSolver &); // hide assign op
                                 // we leave just the declarations, so the compiler will warn us
                                 // if we try to use those two functions by accident

    static TVectorSolver* m_Instance;




    double  ConstraintSrcOET;
    double  ConstraintSrcTikhonov;
    double  ConstraintVec;
    int     smoothImg;
    int     smoothSrc;
    QPoint  SrcPosition;
    float   SrcVal;
    float   SaDiag;
    float   MeanDistance;
    float   CorrThreshold;
    QPointF APrioriVec;

    TRetrievalImage* LastRetrieval;
    QPointF MeanVector;

};

#endif // TVECTORSOLVER_H
