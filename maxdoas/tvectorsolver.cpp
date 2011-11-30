#include "tvectorsolver.h"
#include "vectorsolverservice.h"
#include <QDateTime>

TVectorSolver* TVectorSolver::m_Instance = 0;

TVectorSolver::TVectorSolver()
{
    ConstraintSrcOET = 1e-15;
    ConstraintSrcTikhonov = 1e+07;
    ConstraintVec = 1e7;
    smoothImg = 3 ;
    smoothSrc = 1;
    SrcPosition = QPoint(-1,-1);
    SrcVal = 10;
    SaDiag = 0;
    MeanDistance = 12*1000;
    CorrThreshold = 0.93;
    APrioriVec = QPoint(0,0);
    LastRetrieval = NULL;
}

TVectorSolver::~TVectorSolver(){
    delete LastRetrieval;
}

void TVectorSolver::setConstraintVec(double constraint){
    ConstraintVec = constraint;
}

void TVectorSolver::setConstraintSrcOET(double constraint){
    ConstraintSrcOET = constraint;
}


void TVectorSolver::setConstraintSrcTikhonov(double constraint){
    ConstraintSrcTikhonov = constraint;
}


void TVectorSolver::setImageSmoothValue(int smoothImg){
    this->smoothImg = smoothImg;
}


void TVectorSolver::setSrcSmoothValue(int smoothSrc){
    this->smoothSrc = smoothSrc;
}


void TVectorSolver::setSrcVal(float SrcVal){
    this->SrcVal = SrcVal;
}


void TVectorSolver::setSrcPosition(QPoint SrcPosition){
    this->SrcPosition = SrcPosition;
}


void TVectorSolver::setSADiagonalvalue(float SaDiag){
    this->SaDiag = SaDiag;
}


void TVectorSolver::setCorrThreshold(float CorrThreshold){
    this->CorrThreshold = CorrThreshold;
}


void TVectorSolver::setAPrioriVec(QPointF APrioriVec){
    this->APrioriVec = APrioriVec;
}


void TVectorSolver::setMeanDistance(float Distance){//in meter
    this->MeanDistance = Distance;
}


void TVectorSolver::solve(TRetrievalImage &imgOldCd,TRetrievalImage &imgOldCorr,TRetrievalImage &imgNewCd,TRetrievalImage &imgNewCorr){
    int Rows = imgOldCd.getHeight();
    int Cols = imgOldCd.getWidth();

    MatrixXd                        CorrelationMatrix;
    MatrixXd                        CdsForGrad;
    MatrixXd                        OldCds;
    MatrixXd                        NewCds;
    VectorXd                        AprioriX;
    MatrixXd                        AprioriSRC;
    SparseMatrix<double,RowMajor>   SAinv;
    VectorXd                        DiffVector;
    VectorXd                        deltay;
    SparseMatrix<double,RowMajor>   SEinv;
    SparseMatrix<double,RowMajor>   K;
    SparseMatrix<double,RowMajor>   Rv;

    VectorXd                        xVec;

    float dt = imgNewCd.datetime.toTime_t()-imgOldCd.datetime.toTime_t(); // in seconds..

    QMap<int, QPoint>               SrcPoints;

    OldCds            = fromRetrImage(imgOldCd);
    NewCds            = fromRetrImage(imgNewCd);
    CdsForGrad        = 0.5*(OldCds+NewCds);

    SrcPoints   = getSrcPoints(Rows,Cols,imgOldCd);

    Rv          = xy_tikhonov(Rows, Cols);
    SAinv       = getSAInv( ConstraintVec, ConstraintSrcOET, ConstraintSrcTikhonov, SaDiag, Rows,Cols, Rv ,SrcPoints);

    K           = getK(CdsForGrad, imgOldCd, dt, MeanDistance);

    AprioriSRC  = getAprioriSRC(Rows,Cols,SrcPosition.y(), SrcPosition.x(), SrcVal,smoothSrc);
    AprioriX    = getAprioriX(Rows, Cols,APrioriVec, AprioriSRC);
    DiffVector  = getDiffVector(OldCds,NewCds,1); // Diff = 1 second since grad and divergence already got multiplicated by dt -> bigger numbers, better precision
    deltay      = getDeltaY(DiffVector,AprioriX, K);

    CorrelationMatrix = fromRetrImage(imgNewCorr);
    scaleCorrmatrix(CorrelationMatrix,CorrThreshold);
    SEinv             = getSEinv(CorrelationMatrix);



    xVec = nextstepOET(AprioriX,SAinv, deltay,SEinv, K);

    delete LastRetrieval;

    LastRetrieval = mapDirectionVector(xVec,Rows,Cols);
    mapMatrixValues(CdsForGrad,LastRetrieval);

}

void TVectorSolver::loadWeightedColoumDensitiesToRetrieval(void){

}

TRetrievalImage* TVectorSolver::getRetrieval(void){
    return LastRetrieval;
}

QPointF TVectorSolver::getMeanVec(void){
    QPointF result = QPointF(0,0);
    if (LastRetrieval != NULL){
        for(int row=0;row<LastRetrieval->getHeight();row++){
            for (int col = 0; col<LastRetrieval->getWidth();col++){
                result += LastRetrieval->valueBuffer[row][col]->getWindVector();
            }
        }
    }
    result /= LastRetrieval->getHeight()*LastRetrieval->getWidth();
    return result;
}

float TVectorSolver::getMeanVelocity(){
    QPointF vec = getMeanVec();
    return sqrt(vec.x()*vec.x()+vec.y()*vec.y());
}
