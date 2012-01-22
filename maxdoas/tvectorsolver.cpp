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
    PrevImageSubavg = NULL;
    NextImageSubavg = NULL;
    UseMedianApririFilter = false;
    EmissionFactor = 1;
    AprioriVelocity = 0;
    useNextResultForApriori = true;
    DOF_X = -1;
    DOF_Y = -1;
    DOF_SRC = -1;
    ThermalThreshold = +10000;
    ThermalImage = NULL;
    if (AKDiagVec.rows()>0)
        AKDiagVec(0) = -1;
}

TVectorSolver::~TVectorSolver(){
    delete LastRetrieval;
    delete ThermalImage;
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

void TVectorSolver::setAprioriVelocity(float velocity){
    AprioriVelocity = velocity;
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

void TVectorSolver::setUseMedianAprioriFilter(bool MedianAprioriFilter){
    UseMedianApririFilter = MedianAprioriFilter;
}

void TVectorSolver::setEmissionFactor(float emissionfactor){
    EmissionFactor = emissionfactor;
}
void TVectorSolver::setThermalTheshold(float thres){
    ThermalThreshold = thres;
}

void TVectorSolver::loadThermalImage(TRetrievalImage* thermImage){
    delete ThermalImage;
    ThermalImage = new TRetrievalImage(thermImage);
}

void TVectorSolver::solve(TRetrievalImage* imgOldCd,TRetrievalImage* imgNewCd){
    if (AKDiagVec.rows()>0)
        AKDiagVec(0) = -1;

    Rows = imgOldCd->getHeight();
    Cols = imgOldCd->getWidth();

    DOF_X = -1;
    DOF_Y = -1;
    DOF_SRC = -1;
    QPointF AprioriVecloc = APrioriVec;

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

    SrcPoints   = getSrcPoints(Rows,Cols,ThermalImage,ThermalThreshold);

    Rv          = xy_tikhonov(Rows, Cols);
    SAinv       = getSAInv( ConstraintVec, ConstraintSrcOET, ConstraintSrcTikhonov, SaDiag, Rows,Cols, Rv ,SrcPoints);

    CorrelationMatrix = weightsFromRetrImage(*imgNewCd);
    scaleCorrmatrix(CorrelationMatrix,CorrThreshold);
    SEinv             = getSEinv(CorrelationMatrix);


    K           = getK(CdsForGrad, *imgOldCd, dt, MeanDistance,UseDirectPixelsize,CorrelationMatrix);
    if (UseMedianApririFilter)
        AprioriVecloc  = getMedianPoint(APrioriList);

    if (AprioriVelocity != 0){
        AprioriVecloc = AprioriVecloc/get2Norm(AprioriVecloc);
        AprioriVecloc = AprioriVecloc*fabs(AprioriVelocity);
    }

    AprioriSRC  = getAprioriSRC(Rows,Cols,SrcPosition.y(), SrcPosition.x(), SrcVal,smoothSrc);
    AprioriX    = getAprioriX(Rows, Cols,AprioriVecloc, AprioriSRC);
    DiffVector  = getDiffVector(OldCds,NewCds,1); // Diff = 1 second since grad and divergence already got multiplicated by dt -> bigger numbers, better precision
    deltay      = getDeltaY(DiffVector,AprioriX, K);





    xVec = nextstepOET(AprioriX,SAinv, deltay,SEinv, K);

    delete LastRetrieval;
    LastRetrieval = new TRetrievalImage(imgOldCd);
    LastRetrieval = mapDirectionVector(xVec,Rows,Cols,imgOldCd);
    LastRetrieval->setMeanDistance(MeanDistance);
    LastRetrieval->setTimeDiff(dt);
    LastRetrieval->setEmissionFactor(EmissionFactor);
    mapMatrixValues(CdsForGrad,LastRetrieval);

    //plotSAinvDiagSRC(0, 20);
    delete  PrevImageSubavg;
    delete  NextImageSubavg;
    PrevImageSubavg = new TRetrievalImage(imgOldCd);
    NextImageSubavg = new TRetrievalImage(imgNewCd);
    PrevImageSubavg->setMeanDistance(MeanDistance);
    NextImageSubavg->setMeanDistance(MeanDistance);
    PrevImageSubavg->setTimeDiff(dt);
    NextImageSubavg->setTimeDiff(dt);
    PrevImageSubavg->setEmissionFactor(EmissionFactor);
    NextImageSubavg->setEmissionFactor(EmissionFactor);
    PrevImageSubavg->mapWindVektors(LastRetrieval);
    NextImageSubavg->mapWindVektors(LastRetrieval);
    //thresholdmatrix(CdsForGrad, CorrelationMatrix, CorrThreshold);

//    PrevImageSubavg->subMatrix(CdsForGrad,true);
//    NextImageSubavg->subMatrix(CdsForGrad,true);

    if(useNextResultForApriori){
        if (APrioriList.count()>2)
            APrioriList.removeAt(0);
        APrioriList.append(LastRetrieval->getMeanVec());
    }
    useNextResultForApriori = true;



    //TSpectrumPlotter* plot = TSpectrumPlotter::instance(0);
    //plot->plotDenseMatrix(CorrelationMatrix,4,10);

    qDebug("Time elapsed: %d ms", timer.elapsed());
}

void TVectorSolver::dontUseThisResultForApriori(){
    useNextResultForApriori = false;
}

float TVectorSolver::getDOF_x(){
    if ((AKDiagVec.rows()==0)||(AKDiagVec(0) == -1))
        AKDiagVec = getAKDiag(SAinv,SEinv,K);

    DOF_X = calcDOF_x(AKDiagVec);
    DOF_Y = calcDOF_y(AKDiagVec);
    DOF_SRC = calcDOF_src(AKDiagVec);

    LastRetrieval->setDOFs(DOF_X,DOF_Y,DOF_SRC);

    PrevImageSubavg->setDOFs(DOF_X,DOF_Y,DOF_SRC);
    NextImageSubavg->setDOFs(DOF_X,DOF_Y,DOF_SRC);

    return DOF_X;
}

float TVectorSolver::getDOF_y(){
    if ((AKDiagVec.rows()==0)||(AKDiagVec(0) == -1))
        AKDiagVec = getAKDiag(SAinv,SEinv,K);

    DOF_X = calcDOF_x(AKDiagVec);
    DOF_Y = calcDOF_y(AKDiagVec);
    DOF_SRC = calcDOF_src(AKDiagVec);

    LastRetrieval->setDOFs(DOF_X,DOF_Y,DOF_SRC);

    PrevImageSubavg->setDOFs(DOF_X,DOF_Y,DOF_SRC);
    NextImageSubavg->setDOFs(DOF_X,DOF_Y,DOF_SRC);

    return DOF_Y;
}

float TVectorSolver::getDOF_SRC(){
    if ((AKDiagVec.rows()==0)||(AKDiagVec(0) == -1))
        AKDiagVec = getAKDiag(SAinv,SEinv,K);

    DOF_X = calcDOF_x(AKDiagVec);
    DOF_Y = calcDOF_y(AKDiagVec);
    DOF_SRC = calcDOF_src(AKDiagVec);

    LastRetrieval->setDOFs(DOF_X,DOF_Y,DOF_SRC);

    PrevImageSubavg->setDOFs(DOF_X,DOF_Y,DOF_SRC);
    NextImageSubavg->setDOFs(DOF_X,DOF_Y,DOF_SRC);

    return DOF_SRC;
}
void TVectorSolver::loadWeightedColoumDensitiesToRetrieval(TRetrievalImage* RetImg){
    for(int row = 0;row < CorrelationMatrix.rows();row++){
        for (int col = 0; col < CorrelationMatrix.cols();col++){
            RetImg->valueBuffer[row][col]->val = RetImg->valueBuffer[row][col]->val*CorrelationMatrix(row,col);
        }
    }
}

QScriptValue TVectorSolver::getRetrieval(void){
    TRetrievalImage* retimg = new TRetrievalImage(LastRetrieval);
    return engine()->newQObject(retimg);
}

QScriptValue TVectorSolver::getRetrievalPrevAvg(void){
    TRetrievalImage* retimg = new TRetrievalImage(PrevImageSubavg);
    return engine()->newQObject(retimg);
}

QScriptValue TVectorSolver::getRetrievalNextAvg(void){
    TRetrievalImage* retimg = new TRetrievalImage(NextImageSubavg);
    return engine()->newQObject(retimg);
}

void TVectorSolver::retrievalImageDestructed(TRetrievalImage* img){
    if (LastRetrieval == img)
        LastRetrieval = NULL;
}

void TVectorSolver::plotSrcMatrix(int plotindex, int pixelsize){
    //TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(0);
    TRetrievalImage* srcmatrix = getSrcMatrix_();
    srcmatrix->plot(plotindex,pixelsize);
    delete srcmatrix;
}

TRetrievalImage* TVectorSolver::getSrcMatrix_(){
    //TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(0);
    TRetrievalImage* srcmatrix = mapSrcMatrix(xVec,Rows,Cols,LastRetrieval);
    return srcmatrix;
}

QScriptValue TVectorSolver::getSrcMatrix(void){
    TRetrievalImage* img = getSrcMatrix_();
    return engine()->newQObject(img);
}

void TVectorSolver::plotdcoldt_observed(int plotindex, int pixelsize){
    TRetrievalImage* srcmatrix = getdcoldt_observed_();
    srcmatrix->plot(plotindex,pixelsize);
    delete srcmatrix;
}

TRetrievalImage* TVectorSolver::getdcoldt_observed_(){
    TRetrievalImage* srcmatrix = mapDiffVector(DiffVector,Rows,Cols,LastRetrieval);
    return srcmatrix;
}

QScriptValue TVectorSolver::getdcoldt_observed(void){
    TRetrievalImage* img = getdcoldt_observed_();
    return engine()->newQObject(img);
}

void TVectorSolver::plotdcoldt_retrieved(int plotindex, int pixelsize){
    TRetrievalImage* img = getdcoldt_retrieved_();
    img->plot(plotindex,pixelsize);
    delete img;
}

TRetrievalImage* TVectorSolver::getdcoldt_retrieved_(){
    VectorXd DiffRetrieved = K*xVec;
    TRetrievalImage* srcmatrix = mapDiffVector(DiffRetrieved,Rows,Cols,LastRetrieval);
    return srcmatrix;
}

QScriptValue TVectorSolver::getdcoldt_retrieved(void){
    TRetrievalImage* img = getdcoldt_retrieved_();
    return engine()->newQObject(img);
}

TRetrievalImage* TVectorSolver::getResiduum_(){
    VectorXd DiffRetrieved = K*xVec;
    VectorXd Residuum = DiffRetrieved-DiffVector;
    TRetrievalImage* srcmatrix = mapDiffVector(Residuum,Rows,Cols,LastRetrieval);
    return srcmatrix;
}

void TVectorSolver::plotresiduum(int plotindex, int pixelsize){
    TRetrievalImage* srcmatrix = getResiduum_();
    srcmatrix->plot(plotindex,pixelsize);
    delete srcmatrix;
}

QScriptValue TVectorSolver::getResiduum(void){
    TRetrievalImage* residuum = getResiduum_();
    return engine()->newQObject(residuum);
}


TRetrievalImage* TVectorSolver::getAKDiagX_(){

    if ((AKDiagVec.rows()==0)||(AKDiagVec(0) == -1))
        AKDiagVec = getAKDiag(SAinv,SEinv,K);

    TRetrievalImage* img = mapVector(AKDiagVec,0,Rows,Cols,LastRetrieval);

    LastRetrieval->setDOFs(calcDOF_x(AKDiagVec),calcDOF_y(AKDiagVec),calcDOF_src(AKDiagVec));
    PrevImageSubavg->setDOFs(calcDOF_x(AKDiagVec),calcDOF_y(AKDiagVec),calcDOF_src(AKDiagVec));
    NextImageSubavg->setDOFs(calcDOF_x(AKDiagVec),calcDOF_y(AKDiagVec),calcDOF_src(AKDiagVec));
    img->setDOFs(calcDOF_x(AKDiagVec),calcDOF_y(AKDiagVec),calcDOF_src(AKDiagVec));

    return img;
}

void TVectorSolver::plotAKDiagX(int plotindex, int pixelsize){
    TRetrievalImage* srcmatrix = getAKDiagX_();
    srcmatrix->plot(plotindex,pixelsize);
    delete srcmatrix;
}

QScriptValue TVectorSolver::getAKDiagX(void){
    TRetrievalImage* residuum = getAKDiagX_();
    return engine()->newQObject(residuum);
}

TRetrievalImage* TVectorSolver::getAKDiagY_(){

    if ((AKDiagVec.rows()==0)||(AKDiagVec(0) == -1))
        AKDiagVec = getAKDiag(SAinv,SEinv,K);

    TRetrievalImage* img = mapVector(AKDiagVec,Rows*Cols,Rows,Cols,LastRetrieval);
    LastRetrieval->setDOFs(calcDOF_x(AKDiagVec),calcDOF_y(AKDiagVec),calcDOF_src(AKDiagVec));
    PrevImageSubavg->setDOFs(calcDOF_x(AKDiagVec),calcDOF_y(AKDiagVec),calcDOF_src(AKDiagVec));
    NextImageSubavg->setDOFs(calcDOF_x(AKDiagVec),calcDOF_y(AKDiagVec),calcDOF_src(AKDiagVec));
    img->setDOFs(calcDOF_x(AKDiagVec),calcDOF_y(AKDiagVec),calcDOF_src(AKDiagVec));

    return img;
}

void TVectorSolver::plotAKDiagY(int plotindex, int pixelsize){
    TRetrievalImage* srcmatrix = getAKDiagY_();
    srcmatrix->plot(plotindex,pixelsize);
    delete srcmatrix;
}

QScriptValue TVectorSolver::getAKDiagY(void){
    TRetrievalImage* residuum = getAKDiagY_();
    return engine()->newQObject(residuum);
}

TRetrievalImage* TVectorSolver::getAKDiagSRC_(){

    if ((AKDiagVec.rows()==0)||(AKDiagVec(0) == -1))
        AKDiagVec = getAKDiag(SAinv,SEinv,K);

    TRetrievalImage* img = mapVector(AKDiagVec,2*Rows*Cols,Rows,Cols,LastRetrieval);

    LastRetrieval->setDOFs(calcDOF_x(AKDiagVec),calcDOF_y(AKDiagVec),calcDOF_src(AKDiagVec));
    PrevImageSubavg->setDOFs(calcDOF_x(AKDiagVec),calcDOF_y(AKDiagVec),calcDOF_src(AKDiagVec));
    NextImageSubavg->setDOFs(calcDOF_x(AKDiagVec),calcDOF_y(AKDiagVec),calcDOF_src(AKDiagVec));
    img->setDOFs(calcDOF_x(AKDiagVec),calcDOF_y(AKDiagVec),calcDOF_src(AKDiagVec));

    return img;
}

void TVectorSolver::plotAKDiagSRC(int plotindex, int pixelsize){
    TRetrievalImage* srcmatrix = getAKDiagSRC_();
    srcmatrix->plot(plotindex,pixelsize);
    delete srcmatrix;
}

QScriptValue TVectorSolver::getAKDiagSRC(void){
    TRetrievalImage* residuum = getAKDiagSRC_();
    return engine()->newQObject(residuum);
}

QScriptValue TVectorSolver::getSAinvDiagSRC(void){
    TRetrievalImage* img = getSAinvDiagSRC_();
    return engine()->newQObject(img);
}

void TVectorSolver::plotSAinvDiagSRC(int plotindex, int pixelsize){
    TRetrievalImage* srcmatrix = getSAinvDiagSRC_();
    srcmatrix->plot(plotindex,pixelsize);
    delete srcmatrix;
}

TRetrievalImage* TVectorSolver::getSAinvDiagSRC_(){
    VectorXd SAInvDiag(SAinv.cols() / 3);
    VectorXd SAInvDiag_(SAinv.cols() / 3);
    MatrixXd SAInvSrc(Rows,Cols);
    MatrixXd SAInvSrcT;
    SparseMatrix<double,RowMajor> T = matrixW2H(Rows,Cols,false);
    for(int i=2*Rows*Cols;i<SAinv.cols();i++){
        SAInvDiag(i-2*Rows*Cols) = SAinv.coeff(i,i);
    }

    //qDebug("SAInvDiagCols, SAInvDiagRows: %d , %d ", SAInvDiag.cols(),SAInvDiag.rows());
    //qDebug("TCols, TcRows: %d , %d ", T.cols(),T.rows());

    //SAInvDiag_ = T*SAInvDiag;

    SAInvDiag_ = SAInvDiag;

    TRetrievalImage* img = mapVector(SAInvDiag_,0,Rows,Cols,LastRetrieval);



    return img;
}
