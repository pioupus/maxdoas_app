#ifndef VECTORSOLVERSERVICE_H
#define VECTORSOLVERSERVICE_H

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include "tscanpath.h"
#include "tretrievalimage.h"
using namespace Eigen;

MatrixXd                        fromRetrImage(TRetrievalImage& RetImage);
MatrixXd                        weightsFromRetrImage(TRetrievalImage& RetImage);


VectorXd                        MatrixToVec(const MatrixXd& M);

void                            scaleCorrmatrix(MatrixXd& M, double Threshold);

void                            thresholdmatrix(MatrixXd& M, MatrixXd& Corr, float Threshold);

SparseMatrix<double,RowMajor>   matrixToDiag(const MatrixXd& M);

MatrixXd                        constrTest();

//template <typename Derived, typename Derived2 >
//Derived conv2d(const MatrixBase<Derived>& I, const MatrixBase<Derived2> &kernel );

MatrixXd                        conv2d(  MatrixXd& I,  MatrixXd &kernel );

//Thikonov:
SparseMatrix<double,RowMajor>   matrixW2H(int rows, int cols, bool doubleblock = true);
SparseMatrix<double,RowMajor>   constraintBlockTikhonov(int dim,int blockrows);



SparseMatrix<double,RowMajor>   L1Block(int dim,int blockrows);
SparseMatrix<double,RowMajor>   L1BlockTranspose(int dim,int blockrows);

QMap<int, QPoint>               getSrcPoints(int Rows,int Cols,TRetrievalImage *ThermalImage,float ThermalThreshold);
MatrixXd                        getAprioriSRC(int Rows,int Cols,int srcRow, int srcCol, double srcVal,int srcSmoothSize);
VectorXd                        getAprioriX(int Rows,int Cols,QPointF &APrioriVec,MatrixXd &AprioriSRC);
VectorXd                        getDiffVector(const MatrixXd& ImgFirst, const MatrixXd ImgSecond, float DiffSeconds);
SparseMatrix<double,RowMajor>   getK(const MatrixXd& values, TRetrievalImage& RetImage,double dt,float PlumeDistance,bool UseDirectPixelsize, MatrixXd CorrelationMatrix);
SparseMatrix<double,RowMajor>   getSAInv(double constraintVec,double constraintSrcOET,double constraintSrcTikhonov, double saDiagonal, int Rows,int Cols, SparseMatrix<double,RowMajor> &Rv ,QMap<int, QPoint>SrcPoints);
SparseMatrix<double,RowMajor>   getSEinv(MatrixXd &CorrelationMatrix);
VectorXd                        getDeltaY(VectorXd &DiffVector,VectorXd &AprioriX, SparseMatrix<double,RowMajor> &K);
SparseMatrix<double,RowMajor>   xy_tikhonov(int Rows,int Cols);

VectorXd                        nextstepOET(const VectorXd& xapriori,const SparseMatrix<double,RowMajor> SAinv,const  VectorXd& deltay,const SparseMatrix<double,RowMajor>SEinv, const SparseMatrix<double,RowMajor>K);

TRetrievalImage*                mapDirectionVector(const  VectorXd& XVec, int Rows, int Cols,TRetrievalImage *pixelpositions);
TRetrievalImage*                mapSrcMatrix(const  VectorXd& XVec, int Rows, int Cols,TRetrievalImage *pixelpositions);
TRetrievalImage*                mapDiffVector(const  VectorXd& Diff,int Rows, int Cols,TRetrievalImage *pixelpositions);
TRetrievalImage*                mapVector(const  VectorXd& vector, int offset , int Rows, int Cols, TRetrievalImage *pixelpositions);
VectorXd                        getAKDiag(SparseMatrix<double,RowMajor>  sainv,SparseMatrix<double,RowMajor> Seinv,SparseMatrix<double,RowMajor> K);

bool                            issymetrical(SparseMatrix<double,RowMajor>  testM);
float                           calcDOF_x(VectorXd AKDiag);
float                           calcDOF_y(VectorXd AKDiag);
float                           calcDOF_src(VectorXd AKDiag);

void                            mapMatrixValues(const  MatrixXd& Values,  TRetrievalImage* RetImage);

float getVecAngle(QPointF Vector);
float get2Norm(QPointF p);
QPointF getMedianPoint(QList<QPointF> Apriori);
float selectAndIntegrateCorridor(TRetrievalImage* retimg, TParamLine &corridor, QPointF MeanFluxDirection, float CorridorWidth,int index,float distance);

#endif // VECTORSOLVERSERVICE_H
