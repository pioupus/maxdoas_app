#include "vectorsolverservicetester.h"
#include <iostream>
#include <QTime>

bool rvstMatrixToVec(const MatrixXd &Matrix){

    std::cout << Matrix << std::endl;
    VectorXd vec = MatrixToVec(Matrix);
    std::cout << vec << std::endl;
    return true;
}


bool rvstMatrixSmooth(){
    MatrixXd Kernel(3,3);
//    Kernel << 0,1,2,3,4,
//              5,6,7,8,9,
//              10,11,12,13,14,
//              15,16,17,18,19,
//              20,21,22,23,24;
    Kernel.setOnes(3,3);
    //std::cout << "Kernel:" << std::endl;
    //std::cout << Kernel << std::endl;
    MatrixXd Matrix = constrTest();
    //std::cout << "Matrix:" << std::endl;
    //std::cout << Matrix << std::endl;

    MatrixXd conved = conv2d(Matrix, Kernel );
    //std::cout << "Conved:" << std::endl;
    //std::cout << conved << std::endl;
    return true;
}
bool rvstMatrixW2H(){
    SparseMatrix<double,RowMajor> result = matrixW2H(40,40);
   // std::cout << result << std::endl;
    return true;
}

bool rvstConstraintBlockTikhonov(){
    SparseMatrix<double,RowMajor> result = constraintBlockTikhonov(10000,10);
   // std::cout << result << std::endl;
    return true;
}

bool rvstL1Block(){
    SparseMatrix<double,RowMajor> result = L1Block(6,3);
    std::cout << result << std::endl;
    return true;
}

bool rvstL1Blocktranspose(){
    SparseMatrix<double,RowMajor> result = L1BlockTranspose(12,2);
    std::cout << result << std::endl;
    return true;
}


bool rvstXYTikhonov(){
    SparseMatrix<double,RowMajor> result = xy_tikhonov(4,2);
    std::cout << result << std::endl;
    return true;
}

bool rvstScaleCorrmatrix(MatrixXd &Matrix){

    scaleCorrmatrix(Matrix,0.5);
   // std::cout << Matrix << std::endl;

    return true;
}

bool rvsdirectionAngle(){

    //scaleCorrmatrix(Matrix,0.5);
    std::cout << getVecAngle(QPointF(0,1)) << " " << getVecAngle(QPointF(1,0)) << " " <<  getVecAngle(QPointF(0,-1))<<  " " << getVecAngle(QPointF(-1,0)) << std::endl;
    std::cout << getVecAngle(QPointF(0.5,0.5)) << " " << getVecAngle(QPointF(0.5,-0.5)) << " " <<  getVecAngle(QPointF(-0.5,-0.5))<<  " " << getVecAngle(QPointF(-0.1,0.5)) << std::endl;
    return true;
}

bool rvsAngleMedian(){
    //scaleCorrmatrix(Matrix,0.5);
    QList<QPointF> Apriori;
    QPointF p;
    Apriori.append(QPointF(0,0.4));
    Apriori.append(QPointF(0,0.5));
    Apriori.append(QPointF(0,0.1));
    p=getMedianPoint(Apriori);
    std::cout << p.x() << " " << p.y() << std::endl;
    return true;
}

void runVecSolverTest(){
    QTime timer;
    timer.start();
    //for (int i=0; i < 50; i++){
        //MatrixXd Matrix = constrTest();
        //rvstMatrixToVec(Matrix);
        //rvstScaleCorrmatrix(Matrix);
        //rvstMatrixSmooth();
        //rvstMatrixW2H();
        //rvstConstraintBlockTikhonov();
        //rvstXYTikhonov();
        rvsdirectionAngle();
        //rvsAngleMedian();
        //rvstL1Blocktranspose();
        //rvstL1Block();
    //}
    qDebug("Time elapsed: %d ms", timer.elapsed());
}
