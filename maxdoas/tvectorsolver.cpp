#include "tvectorsolver.h"
#include "vectorsolverservice.h"
#include <QDateTime>
#include "tspectrumplotter.h"
#include "tscanpath.h"
#include "temissionrate.h"

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
    UseDirectPixelsize = false;
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


void TVectorSolver::setAPrioriVec(float APrioriVecX,float APrioriVecY){
    this->APrioriVec = QPointF(APrioriVecX,APrioriVecY);
}


void TVectorSolver::setMeanDistance(float Distance){//in meter
    this->MeanDistance = Distance;
}

void TVectorSolver::setUseDirectPixelsize(bool    UseDirectPixelsize){
    this->UseDirectPixelsize=UseDirectPixelsize;
}

void TVectorSolver::solve(TRetrievalImage* imgOldCd,TRetrievalImage* imgOldCorr,TRetrievalImage* imgNewCd,TRetrievalImage* imgNewCorr){
    Rows = imgOldCd->getHeight();
    Cols = imgOldCd->getWidth();



    MatrixXd                        OldCds;
    MatrixXd                        NewCds;

    MatrixXd                        Smoothkernel(smoothImg,smoothImg);

    QTime timer;
    timer.start();

    Smoothkernel.setOnes(smoothImg,smoothImg);


    int tnew = imgNewCd->datetime.toTime_t();
    int told = imgOldCd->datetime.toTime_t();
    float dt = tnew-told; // in seconds..

    QMap<int, QPoint>               SrcPoints;

    OldCds            = fromRetrImage(*imgOldCd);
    NewCds            = fromRetrImage(*imgNewCd);

    OldCds = conv2d(OldCds,Smoothkernel);
    NewCds = conv2d(NewCds,Smoothkernel);

    CdsForGrad        = 0.5*(OldCds+NewCds);

    SrcPoints   = getSrcPoints(Rows,Cols,*imgOldCd);

    Rv          = xy_tikhonov(Rows, Cols);
    SAinv       = getSAInv( ConstraintVec, ConstraintSrcOET, ConstraintSrcTikhonov, SaDiag, Rows,Cols, Rv ,SrcPoints);

    CorrelationMatrix = fromRetrImage(*imgNewCorr);
    scaleCorrmatrix(CorrelationMatrix,CorrThreshold);
    SEinv             = getSEinv(CorrelationMatrix);


    K           = getK(CdsForGrad, *imgOldCd, dt, MeanDistance,UseDirectPixelsize,CorrelationMatrix);

    AprioriSRC  = getAprioriSRC(Rows,Cols,SrcPosition.y(), SrcPosition.x(), SrcVal,smoothSrc);
    AprioriX    = getAprioriX(Rows, Cols,APrioriVec, AprioriSRC);
    DiffVector  = getDiffVector(OldCds,NewCds,1); // Diff = 1 second since grad and divergence already got multiplicated by dt -> bigger numbers, better precision
    deltay      = getDeltaY(DiffVector,AprioriX, K);





    xVec = nextstepOET(AprioriX,SAinv, deltay,SEinv, K);

    delete LastRetrieval;

    LastRetrieval = mapDirectionVector(xVec,Rows,Cols,imgOldCd);
    mapMatrixValues(CdsForGrad,LastRetrieval);

    //TSpectrumPlotter* plot = TSpectrumPlotter::instance(0);
    //plot->plotDenseMatrix(CorrelationMatrix,4,10);

    qDebug("Time elapsed: %d ms", timer.elapsed());
}

void TVectorSolver::loadWeightedColoumDensitiesToRetrieval(TRetrievalImage* RetImg){
    for(int row = 0;row < CorrelationMatrix.rows();row++){
        for (int col = 0; col < CorrelationMatrix.cols();col++){
            RetImg->valueBuffer[row][col]->val = RetImg->valueBuffer[row][col]->val*CorrelationMatrix(row,col);
        }
    }
}

QScriptValue TVectorSolver::getRetrieval(void){
    return engine()->newQObject(LastRetrieval);
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


QPointF TVectorSolver::getMaxVec(){
    QPointF result;
    float maxVal;
    int maxIndexcol;
    int maxIndexrow;
    if (LastRetrieval != NULL){
        for(int row=0;row<LastRetrieval->getHeight();row++){
            for (int col = 0; col<LastRetrieval->getWidth();col++){
                float val;
                result = LastRetrieval->valueBuffer[row][col]->getWindVector();
                val = sqrt(result.x()*result.x()+result.y()*result.y());
                if ((val > maxVal)||(col+row==0)){
                    maxVal = val;
                    maxIndexcol = col;
                    maxIndexrow = row;
                }
            }
        }
    }
    result = LastRetrieval->valueBuffer[maxIndexrow][maxIndexcol]->getWindVector();
    return result;
}

float TVectorSolver::getMaxVelocity(){
    QPointF vec = getMaxVec();
    return sqrt(vec.x()*vec.x()+vec.y()*vec.y());
}

float TVectorSolver::getMeanVelocity(){
    QPointF vec = getMeanVec();
    return sqrt(vec.x()*vec.x()+vec.y()*vec.y());
}

void TVectorSolver::retrievalImageDestructed(TRetrievalImage* img){
    if (LastRetrieval == img)
        LastRetrieval = NULL;
}

void TVectorSolver::plotSrcMatrix(int plotindex, int pixelsize){
    //TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(0);
    TRetrievalImage* srcmatrix = mapSrcMatrix(xVec,Rows,Cols,LastRetrieval);
    srcmatrix->plot(plotindex,pixelsize);
    delete srcmatrix;
}

void TVectorSolver::plotdcoldt_observed(int plotindex, int pixelsize){
    TRetrievalImage* srcmatrix = mapDiffVector(DiffVector,Rows,Cols,LastRetrieval);
    srcmatrix->plot(plotindex,pixelsize);
    delete srcmatrix;
}

void TVectorSolver::plotdcoldt_retrieved(int plotindex, int pixelsize){
    VectorXd DiffRetrieved = K*xVec;
    TRetrievalImage* srcmatrix = mapDiffVector(DiffRetrieved,Rows,Cols,LastRetrieval);
    srcmatrix->plot(plotindex,pixelsize);
    delete srcmatrix;
}

void TVectorSolver::plotresiduum(int plotindex, int pixelsize){
    VectorXd DiffRetrieved = K*xVec;
    VectorXd Residuum = DiffRetrieved-DiffVector;
    TRetrievalImage* srcmatrix = mapDiffVector(Residuum,Rows,Cols,LastRetrieval);
    srcmatrix->plot(plotindex,pixelsize);
    delete srcmatrix;
}

TEmissionrate* TVectorSolver::emissionrate(float TimeStep, float ScanPixelsize, bool plotit){
    TEmissionrate* result = new TEmissionrate();
    float maxVal;
    QPointF maxPoint;
    if (LastRetrieval != NULL){
        for(int row=0;row<LastRetrieval->getHeight();row++){
            for (int col = 0; col<LastRetrieval->getWidth();col++){
                float val;
                val += LastRetrieval->valueBuffer[row][col]->val;
                if ((val > maxVal)||(col+row==0)){
                    maxVal = val;
                    maxPoint.setX(col);
                    maxPoint.setY(row);
                }
            }
        }
    }

    float emissionrate=0;
    TParamLine Windvector;
    Windvector.iniDiff(maxPoint,getMeanVec());
    QDateTime EmissionTime = LastRetrieval->datetime;
    float LineParam = 0;
    do{

        QPointF nextPoint = Windvector.getPointbyParam(LineParam);
        LineParam -= TimeStep/ScanPixelsize;//Here we should take the real Pixeldistances into account. aswell with its projections
        EmissionTime.addMSecs(-TimeStep*1000);
        TParamLine* corridor = Windvector.getOrthoLine(nextPoint);
        emissionrate = selectAndIntegrateCorridor(*corridor,0.5);
        result->AddEmmision(EmissionTime,emissionrate,corridor);
        delete corridor;
    }while(emissionrate>0);
    result->setTimeOffset(EmissionTime);
    EmissionTime = LastRetrieval->datetime;
    LineParam = 0;
    do{
        QPointF nextPoint = Windvector.getPointbyParam(LineParam);
        TParamLine* corridor = Windvector.getOrthoLine(nextPoint);
        emissionrate = selectAndIntegrateCorridor(*corridor,0.5);
        result->AddEmmision(EmissionTime,emissionrate,corridor);
        EmissionTime.addMSecs(TimeStep*1000);

        LineParam += TimeStep/ScanPixelsize;//Here we should take the real Pixeldistances into account. aswell with its projections


        delete corridor;
    }while(emissionrate>0);

    return result;
}



float TVectorSolver::selectAndIntegrateCorridor(TParamLine &corridor, float CorridorWidth){
    float result=0;
    QPointF unitydirectionvector = QPointF(corridor.getDiffVec());
    result = sqrt(unitydirectionvector.x()*unitydirectionvector.x()+unitydirectionvector.y()*unitydirectionvector.y());
    unitydirectionvector /= result;
    result = 0;
    for (int row=0;row<LastRetrieval->getHeight();row++){
        for (int col=0;col<LastRetrieval->getWidth();col++){
            QPointF p = LastRetrieval->valueBuffer[row][col]->mirrorCoordinate->getAngleCoordinate();
            if (corridor.GetDistanceToPoint(p) < CorridorWidth){
                float windspeedprojection = unitydirectionvector.x()*LastRetrieval->valueBuffer[row][col]->getWindVector().x()+
                                            unitydirectionvector.y()*LastRetrieval->valueBuffer[row][col]->getWindVector().y();
                result += LastRetrieval->valueBuffer[row][col]->val*windspeedprojection;
            }
        }
    }
}
