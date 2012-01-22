#ifndef TVECTORSOLVER_H
#define TVECTORSOLVER_H
#include <QPointF>
#include <QScriptEngine>
#include <QScriptContext>
#include <QScriptValue>
#include <QScriptable>
#include "vectorsolverservice.h"
#include "tretrievalimage.h"
#include "temissionrate.h"


class TVectorSolver: public QObject, protected QScriptable
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
    void setAPrioriVec(float APrioriVecX,float APrioriVecY);
    void setMeanDistance(float Distance);//in meter
    void setUseDirectPixelsize(bool    UseDirectPixelsize);
    void setUseMedianAprioriFilter(bool MedianAprioriFilter);
    void plotSrcMatrix(int plotindex, int pixelsize=10);
    void plotdcoldt_observed(int plotindex, int pixelsize=10);
    void plotdcoldt_retrieved(int plotindex, int pixelsize=10);
    void plotresiduum(int plotindex, int pixelsize=10);
    void setEmissionFactor(float emissionfactor);
    float getDOF_x();
    float getDOF_y();
    float getDOF_SRC();
    void dontUseThisResultForApriori();
    void setAprioriVelocity(float velocity);
    void setThermalTheshold(float thres);
    void solve(TRetrievalImage* imgOldCd,TRetrievalImage* imgNewCd);

    void loadWeightedColoumDensitiesToRetrieval(TRetrievalImage* RetImg);
    QScriptValue getRetrieval(void);

    QScriptValue getRetrievalPrevAvg(void);
    QScriptValue getRetrievalNextAvg(void);

    QScriptValue getResiduum(void);
    QScriptValue getdcoldt_retrieved(void);
    QScriptValue getdcoldt_observed(void);



    QScriptValue getSrcMatrix(void);
    void plotAKDiagX(int plotindex, int pixelsize);
    void plotAKDiagY(int plotindex, int pixelsize);
    void plotAKDiagSRC(int plotindex, int pixelsize);
    void plotSAinvDiagSRC(int plotindex, int pixelsize);

    QScriptValue getAKDiagX(void);
    QScriptValue getAKDiagY(void);
    QScriptValue getAKDiagSRC(void);
    QScriptValue getSAinvDiagSRC(void);
    void loadThermalImage(TRetrievalImage* thermImage);
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
    void retrievalImageDestructed(TRetrievalImage* img);
private:
    TVectorSolver();
    ~TVectorSolver();
    TVectorSolver(const TVectorSolver &); // hide copy constructor
    TVectorSolver& operator=(const TVectorSolver &); // hide assign op
                                 // we leave just the declarations, so the compiler will warn us
                                 // if we try to use those two functions by accident


    static TVectorSolver* m_Instance;

    //float selectAndIntegrateCorridor(TParamLine &corridor, QPointF MeanFluxDirection, float CorridorWidth);
    TEmissionrate* emissionrate_(float TimeStep, float ScanPixelsize);
    TRetrievalImage* getdcoldt_observed_();
    TRetrievalImage* getdcoldt_retrieved_();
    TRetrievalImage* getResiduum_();

    TRetrievalImage* getSrcMatrix_();
    TRetrievalImage* getAKDiagX_();
    TRetrievalImage* getAKDiagY_();
    TRetrievalImage* getAKDiagSRC_();
    TRetrievalImage* getSAinvDiagSRC_();

    double  ConstraintSrcOET;
    double  ConstraintSrcTikhonov;
    double  ConstraintVec;
    int     smoothImg;
    int     smoothSrc;
    QPoint  SrcPosition;
    float   SrcVal;
    float   SaDiag;
    float   MeanDistance;
    bool    UseDirectPixelsize;
    bool    UseMedianApririFilter;
    float   CorrThreshold;
    float   EmissionFactor;
    QPointF APrioriVec;
    float   AprioriVelocity;
    float   DOF_X;
    float   DOF_Y;
    float   DOF_SRC;
    float   ThermalThreshold;
    bool useNextResultForApriori;

    TRetrievalImage* LastRetrieval;
    TRetrievalImage* PrevImageSubavg;
    TRetrievalImage* NextImageSubavg;
    TRetrievalImage* ThermalImage;

    QPointF MeanVector;


    MatrixXd                        CorrelationMatrix;

    MatrixXd                        CdsForGrad;
    VectorXd                        AprioriX;
    MatrixXd                        AprioriSRC;
    SparseMatrix<double,RowMajor>   SAinv;
    VectorXd                        DiffVector;
    VectorXd                        deltay;
    SparseMatrix<double,RowMajor>   SEinv;
    SparseMatrix<double,RowMajor>   K;
    SparseMatrix<double,RowMajor>   Rv;
    VectorXd                        xVec;


    VectorXd                        AKDiagVec;

    int Rows;
    int Cols;

    QList<QPointF> APrioriList;
};







#endif // TVECTORSOLVER_H
