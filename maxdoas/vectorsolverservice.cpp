#include "vectorsolverservice.h"
#include <iostream>
#include <QDebug>
#include <QMap>
#include <QMapIterator>
#include <Eigen/SparseCholesky>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SuperLUSupport>
#include "tspectrumplotter.h"

#include <iostream>
#include <fstream>

#include "log4qt/consoleappender.h"
#include "log4qt/logger.h"
#include "log4qt/ttcclayout.h"


//LOG4QT_DECLARE_QCLASS_LOGGER

#define PLOTGRADIENT 0
#define USEDIV 1

bool odd (int a){
  if ((a%2)!=0)
      return true;
  else
      return false;
}

bool even (int a)
{
  return !odd(a);
}

double getKXStart(double KRows){
    (void)KRows;
    return 0;
}

double getKXEnd(double KRows){
    return KRows-1;
}

double getKYStart(double KRows){
    return KRows;
}

double getKYEnd(double KRows){
    return KRows*2-1;
}

double getKSrcStart(double KRows){
    return KRows*2;
}

double getKSrcEnd(double KRows){
    return KRows*2;
}

int indexMatrixToVec(int rows, int cols, int row, int col){
    (void)cols;
    int result;
    result = col * rows;
    result = result + row;
    return result;
}

QPoint indexVecToMatrix(int rows, int cols, int index){
    (void)cols;
    QPoint result;
    result.setX(index / rows);
    result.setY(index % rows);
    return result;
}



MatrixXd constrTest(){
    const int rows = 100;
    const int cols = 100;
    MatrixXd result(rows,cols);
    result.setZero(rows,cols);
    for (int row = 0; row < rows;row++){
        for (int col = 0; col <cols;col++){
            //result(row,col) = (double)(col+1)/rows;
            result(row,col) = (double)(col+1)*(row+1);
        }
    }
    return result;
}

MatrixXd fromRetrImage(TRetrievalImage& RetImage){
    MatrixXd result(RetImage.getHeight(),RetImage.getWidth());
    for (int col = 0; col < RetImage.getWidth();col++){
        for (int row = 0; row < RetImage.getHeight();row++){
            result(row,col) = RetImage.valueBuffer[row][col]->val;
        }
    }
    return result;
}

MatrixXd weightsFromRetrImage(TRetrievalImage& RetImage){
    MatrixXd result(RetImage.getHeight(),RetImage.getWidth());

    for (int col = 0; col < RetImage.getWidth();col++){
        for (int row = 0; row < RetImage.getHeight();row++){
            result(row,col) = RetImage.valueBuffer[row][col]->weight;
        }
    }
    return result;
}

void scaleCorrmatrix(MatrixXd& M, double Threshold){
   // M = ((M.array() - Threshold) * 1/(1-Threshold)).matrix(); //on 1000*1000 this is 68% slower
    for(int i = 0; i< M.size();i++ ){
        *(M.data() + i) -= Threshold;
        *(M.data() + i) /= (1-Threshold);
        if (*(M.data() + i) < 0)
            *(M.data() + i) = 0;
    }
}

void thresholdmatrix(MatrixXd& M, MatrixXd& Corr, float Threshold){
    for(int i = 0; i< M.size();i++ ){
        if (Threshold > *(Corr.data() + i)){
            *(M.data() + i) = 0;
        }
    }
}


MatrixXd conv2d( MatrixXd& I,  MatrixXd &kernel )
{

    MatrixXd result(I.rows(),I.cols());

    if (even(kernel.rows()) || even(kernel.cols())){
        qDebug() << "Smoothing kernel has even dimensions";
        //return Derived::Zero(1,1);
    }

    if ((kernel.rows() > I.rows()) || (kernel.cols() > I.cols())){
        qDebug() << "Smoothing kernel bigger than matrix to be smoothed";
        //return Derived::Zero(1,1);
    }

    if ((kernel.rows() == 1)||(kernel.cols() == 1)){
        return I;
    }
    int kernelRow = kernel.rows()/2;
    int kernelCol = kernel.cols()/2;
    int limitRow = I.rows()-kernelRow;
    int limitCol = I.cols()-kernelCol;
    int krows =  kernel.rows();
    int kcols =  kernel.cols();

//    std::ofstream filein;
//    filein.open ("conv_in.txt");
//    filein << I;
//    filein.close();


    double norm = kernel.sum();
    for (int col = 0; col < I.cols(); col++ ){
        for (int row = 0; row < I.rows(); row++ ){
            if ((row >= kernelRow) && (row < limitRow) && (col >= kernelCol) && (col < limitCol)){

                double b=(static_cast<MatrixXd>( I.block(row-kernelRow,col-kernelCol,krows,kcols ) ).cwiseProduct(kernel)).sum();
                //double b=(static_cast<MatrixXd>( I.block(row-kernelRow,col-kernelCol,krows,kcols ) )).sum();
                //std::cout << "kernel bei " << row <<","<< col << ": start (" << row-kernelRow << "," << col-kernelCol << "," << krows << "," << kcols<<")" << std::endl;
                result.coeffRef(row,col) = b/norm;
            }else{
                int rowoffsetu = kernelRow - row;
                int rowoffsetb = row+1 - limitRow;
                int coloffsetl = kernelCol - col;
                int coloffsetr = col+1 - limitCol;

                if (rowoffsetu < 0) rowoffsetu = 0;
                if (rowoffsetb < 0) rowoffsetb = 0;
                if (coloffsetl < 0) coloffsetl = 0;
                if (coloffsetr < 0) coloffsetr = 0;

                int krows_sub =  kernel.rows() - (rowoffsetu + rowoffsetb);
                int kcols_sub =  kernel.cols() - (coloffsetl + coloffsetr);

                int mrow_start =  row - kernelRow;
                int mcol_start =  col - kernelCol;
                if (mrow_start < 0) mrow_start = 0;
                if (mcol_start < 0) mcol_start = 0;

                MatrixXd kernel_sub = kernel.block(rowoffsetu,coloffsetl,krows_sub,kcols_sub );
                //std::cout << "kernel_sub:" << std::endl;
                //std::cout << kernel_sub << std::endl;
                double norm_sub = kernel_sub.sum();
                double b=(static_cast<MatrixXd>( I.block(mrow_start,mcol_start,krows_sub,kcols_sub ) ).cwiseProduct(kernel_sub)).sum();
                result.coeffRef(row,col) = b/norm_sub;
            }
        }
    }

//    std::ofstream fileout;
//    fileout.open ("conv_out.txt");
//    fileout << result;
//    fileout.close();

    return result;
}

TRetrievalImage* mapDirectionVector(const  VectorXd& XVec, int Rows, int Cols,TRetrievalImage *pixelpositions){
    //rows/cols from image
    TRetrievalImage* result = new TRetrievalImage(pixelpositions);
    for(int row=0;row<Rows;row++){
        for (int col = 0; col<Cols;col++){
            int i = indexMatrixToVec(Rows, Cols, row, col);
           // int i = col*rows+row;
            QPointF WindVector(XVec(i),XVec(Rows*Cols+i));
            result->valueBuffer[row][col]->setWindVector(WindVector);
        }
    }
    return result;
}

TRetrievalImage* mapSrcMatrix(const  VectorXd& XVec, int Rows, int Cols,TRetrievalImage *pixelpositions){
    //rows/cols from image
    TRetrievalImage* result = new TRetrievalImage(pixelpositions);
    for(int row=0;row<Rows;row++){
        for (int col = 0; col<Cols;col++){
            int i = indexMatrixToVec(Rows, Cols,  row,col);
            result->valueBuffer[row][col]->val = XVec(2*Rows*Cols+i);
        }
    }
    return result;
}

TRetrievalImage* mapVector(const  VectorXd& vector, int offset , int Rows, int Cols, TRetrievalImage *pixelpositions){
    //rows/cols from image
    TRetrievalImage* result = new TRetrievalImage(pixelpositions);
    for(int row=0;row<Rows;row++){
        for (int col = 0; col<Cols;col++){
            int i = indexMatrixToVec(Rows, Cols, row, col);
            result->valueBuffer[row][col]->val = vector(i+offset);
            result->valueBuffer[row][col]->origval = vector(i+offset);
        }
    }
    return result;
}

TRetrievalImage* mapDiffVector(const  VectorXd& Diff, int Rows, int Cols, TRetrievalImage *pixelpositions){
    //rows/cols from image
    TRetrievalImage* result = mapVector(   Diff,0,  Rows,  Cols, pixelpositions);

    return result;
}


void mapMatrixValues(const  MatrixXd& Values,  TRetrievalImage* RetImage){

     //rows/cols from image
    for(int row=0;row<Values.rows();row++){
        for (int col = 0; col<Values.cols();col++){
           // int i = col*rows+row;
            RetImage->valueBuffer[row][col]->val = Values(row,col);
            RetImage->valueBuffer[row][col]->origval = Values(row,col);
            //double w = correlation->valueBuffer[row][col]->val;
            //RetImage->valueBuffer[row][col]->weight = w;
        }
    }
}

MatrixXd getSourceMatrix(const  VectorXd& XVec, int rows, int cols){
    //rows/cols from image
    MatrixXd result(rows,cols);
    result.setZero(rows,cols);
    for(int row=0;row<rows;row++){
        for (int col = 0; col<cols;col++){
            int i = indexMatrixToVec(rows, cols, row, col);
            result(row,col) = XVec(rows*cols*2+i);
        }
    }
    return result;
}

VectorXd getDiffVector(const MatrixXd& ImgFirst, const MatrixXd ImgSecond, float DiffSeconds){
    MatrixXd imgdiff = (ImgSecond - ImgFirst)/DiffSeconds;

    return MatrixToVec(imgdiff);
}

VectorXd getDeltaY(VectorXd &DiffVector,VectorXd &AprioriX, SparseMatrix<double,RowMajor> &K){
    VectorXd result;
//    std::cout << "AprioriX rows,cols " << AprioriX.rows() << ","<< AprioriX.cols() << std::endl;
//    std::cout << "K rows,cols " << K.rows() << ","<< K.cols() << std::endl;
    VectorXd yFit = K*AprioriX;
//    std::cout << "yFit rows,cols " << yFit.rows() << ","<< yFit.cols() << std::endl;
//    std::cout << "DiffVector rows,cols " << DiffVector.rows() << ","<< DiffVector.cols() << std::endl;
    result = DiffVector - yFit;
    return result;
}

VectorXd MatrixToVec(const MatrixXd& M){
        //getestet
    #ifdef EIGEN_DEFAULT_TO_ROW_MAJOR
        #error MatrixToVec relies on coloum-major
    #endif
    VectorXd result(M.cols()*M.rows());
    memcpy(result.data(),M.data(),M.size()*sizeof(double));
    return result;
}

SparseMatrix<double,RowMajor> matrixToDiag(const MatrixXd& M){
        //nicht getestet
    SparseMatrix<double,RowMajor> result(M.cols()*M.rows(),M.cols()*M.rows());

    result.reserve(M.cols()*M.rows());
    int rowcol=0;
    for (int col = 0;col<M.cols();col++){
        for (int row = 0;row<M.rows();row++){
            result.startVec(rowcol);
            result.insertBack(rowcol,rowcol) = M(row,col);
            rowcol++;
        }
    }
    result.finalize();
    return result;
}

SparseMatrix<double,RowMajor> getK(const MatrixXd& values, TRetrievalImage& RetImage,double dt,float PlumeDistance,bool UseDirectPixelsize, MatrixXd CorrelationMatrix){
    //nicht getestet
    //Coloum densities are taken von values, while its coordinates are taken from RetImage
    (void)CorrelationMatrix;
    SparseMatrix<double,RowMajor> result(RetImage.getWidth()*RetImage.getHeight(),RetImage.getWidth()*RetImage.getHeight()*3);
    #if PLOTGRADIENT
        TRetrievalImage* grad = new TRetrievalImage(&RetImage);
    #endif
    int col = 0;
    int row = 0;
    double val;
    double dist;

    if (dt == 0){
        std::cout << "ERROR: dt == 0 in getK" << std::endl;
    }
    if (dt < 0){
        std::cout << "ERROR: dt < 0 in getK" << std::endl;
    }
    #if USEDIV
        double meanPixelSizeY =(RetImage.valueBuffer[0][0]->mirrorCoordinate->getAngleCoordinate().y()+RetImage.valueBuffer[RetImage.getHeight()-1][0]->mirrorCoordinate->getAngleCoordinate().y())/(RetImage.getHeight()-1);
        double meanPixelSizeX =(RetImage.valueBuffer[0][0]->mirrorCoordinate->getAngleCoordinate().x()+RetImage.valueBuffer[0][RetImage.getWidth()-1]->mirrorCoordinate->getAngleCoordinate().x())/(RetImage.getWidth()-1);
        if (UseDirectPixelsize){

        }else{
            meanPixelSizeY = meanPixelSizeY*M_PI/180;               meanPixelSizeX = meanPixelSizeX*M_PI/180;
            meanPixelSizeY = PlumeDistance*sin(meanPixelSizeY);     meanPixelSizeX = PlumeDistance*sin(meanPixelSizeX);
        }

        SparseMatrix<double,RowMajor> coloumDensitiesDiag = matrixToDiag(values);// for col multiplication

        SparseMatrix<double,RowMajor> divergenceX         = L1BlockTranspose  (RetImage.getWidth()*RetImage.getHeight(),RetImage.getWidth());// /PIXELSIZE
        SparseMatrix<double,RowMajor> divergenceY         = L1Block           (RetImage.getWidth()*RetImage.getHeight(),RetImage.getHeight());// /PIXELSIZE


        double cotient = dt/meanPixelSizeX;
        divergenceX = divergenceX*cotient;
        cotient = dt/meanPixelSizeY;
        divergenceY = divergenceY*cotient;

//        std::ofstream filein;
//                filein.open ("matrixout1/divergenceX.txt");
//                filein << divergenceX;
//                filein.close();
//
//        filein.open ("matrixout1/divergenceY.txt");
//        filein << divergenceY;
//        filein.close();

        divergenceX = coloumDensitiesDiag*divergenceX.transpose();
        divergenceY = coloumDensitiesDiag*divergenceY.transpose();

    #endif
//    (x00|y00|SRC)         (100)   x = grad_x + divergenceX
//    (0x0|0y0|SRC) SRC =   (010)
//    (00x|00y|SRC)         (001)   y = grad_y + divergenceY

//    result.reserve(RetImage.getWidth()*RetImage.getHeight()*3);

//        std::ofstream filein;
//        filein.open ("matrixout1/divergenceX.txt");
//        filein << divergenceX;
//        filein.close();
//
//        filein.open ("matrixout1/divergenceY.txt");
//        filein << divergenceY;
//        filein.close();
    for (int retcol = 0;retcol<RetImage.getWidth();retcol++){
        for (int retrow = 0;retrow<RetImage.getHeight();retrow++){

            result.startVec(row);

            #if USEDIV
                SparseMatrix<double,RowMajor>::InnerIterator IteratorX(divergenceX,row);
                SparseMatrix<double,RowMajor>::InnerIterator IteratorY(divergenceY,row);

                while(IteratorX){ //lets fill up first port before diagonal
                   //CHECKME: does it really work?
                    int itcol=IteratorX.col();
                    if (row>itcol){
                        double val = IteratorX.value();
                        result.insertBack(row,itcol) = val;
                    }else{
                        break;
                    }
                    ++IteratorX;
                }
            #endif
            if (retcol==0){
                val = values(retrow,retcol+1)-values(retrow,retcol);
                dist =  RetImage.valueBuffer[retrow][retcol+1]->mirrorCoordinate->getAngleCoordinate().x();
                dist -=  RetImage.valueBuffer[retrow][retcol]->mirrorCoordinate->getAngleCoordinate().x();
             }else if (retcol == RetImage.getWidth()-1){
                val = values(retrow,retcol)-values(retrow,retcol-1);
                dist =  RetImage.valueBuffer[retrow][retcol]->mirrorCoordinate->getAngleCoordinate().x();
                dist -=  RetImage.valueBuffer[retrow][retcol-1]->mirrorCoordinate->getAngleCoordinate().x();
             }else{
                val = values(retrow,retcol+1)-values(retrow,retcol-1);
                dist =  RetImage.valueBuffer[retrow][retcol+1]->mirrorCoordinate->getAngleCoordinate().x();
                dist -=  RetImage.valueBuffer[retrow][retcol-1]->mirrorCoordinate->getAngleCoordinate().x();
            }

            dist = -dist;//gradient is negative in continuity equ
            if (UseDirectPixelsize){
                val = val*dt/dist;
            }else{
                dist = dist*M_PI/180.0;
                val = val*dt/(PlumeDistance*sin(dist));
            }
            #if PLOTGRADIENT
                QPointF gradient;
                gradient.setX(val);
            #endif
            #if USEDIV
                if (IteratorX){
                    int itcol=IteratorX.col();
                    if (itcol==row){
                        double itval = IteratorX.value();
                        val += itval;
                        ++IteratorX;
                    }
                }
            #endif
//             if (CorrelationMatrix(retrow,retcol) == 0)
//                 val = 0;
            result.insertBack(row,col) = val; //DiagonalX passed..
            #if USEDIV
                while(IteratorX){//lets fill up first part behind diagonal
                    int itcol=IteratorX.col();
                    if (row < itcol){
                      result.insertBack(row,itcol) = IteratorX.value();
                    }else{
                        break;
                    }
                    ++IteratorX;
                }

                while(IteratorY){ //lets fill up first port before diagonal
                    int itcol=IteratorY.col();
                    if (row>itcol){
                      result.insertBack(row,itcol+RetImage.getWidth()*RetImage.getHeight()) = IteratorY.value();
                    }else{
                        break;
                    }
                    ++IteratorY;
                }

            #endif
            if (retrow==0){
               val = values(retrow+1,retcol)- values(retrow,retcol);
               dist =  RetImage.valueBuffer[retrow+1][retcol]->mirrorCoordinate->getAngleCoordinate().y();
               dist -=  RetImage.valueBuffer[retrow][retcol]->mirrorCoordinate->getAngleCoordinate().y();
            }else if (retrow == RetImage.getHeight()-1){
               val = values(retrow,retcol)-values(retrow-1,retcol);
               dist =  RetImage.valueBuffer[retrow][retcol]->mirrorCoordinate->getAngleCoordinate().y();
               dist -=  RetImage.valueBuffer[retrow-1][retcol]->mirrorCoordinate->getAngleCoordinate().y();
            }else{
               val = values(retrow+1,retcol)-values(retrow-1,retcol);
               dist =  RetImage.valueBuffer[retrow+1][retcol]->mirrorCoordinate->getAngleCoordinate().y();
               dist -=  RetImage.valueBuffer[retrow-1][retcol]->mirrorCoordinate->getAngleCoordinate().y();
            }

            dist = -dist;//gradient is negative in continuity equ
            if (UseDirectPixelsize){
                val = val*dt/dist;
            }else{
                dist = dist*M_PI/180.0;
                val = val*dt/(PlumeDistance*sin(dist));
            }
            #if PLOTGRADIENT
                gradient.setY(val);
                grad->valueBuffer[retrow][retcol]->setWindVector(gradient);
            #endif
            #if USEDIV
                if (IteratorY){
                    int itcol=IteratorY.col();
                    if (itcol==row){
                        double itval = IteratorY.value();
                        val += itval;
                        ++IteratorY;
                    }
                }
            #endif
//            if (CorrelationMatrix(retrow,retcol) == 0)
//                val = 0;
            result.insertBack(row,col+RetImage.getWidth()*RetImage.getHeight()) = val;
            #if USEDIV
                while(IteratorY){//lets fill up first part behind diagonal
                    int itcol=IteratorY.col();
                    if (row < itcol){
                      result.insertBack(row,itcol+RetImage.getWidth()*RetImage.getHeight()) = IteratorY.value();
                    }else{
                        break;
                    }
                    ++IteratorY;
                }
            #endif
            result.insertBack(row,col+2*RetImage.getWidth()*RetImage.getHeight()) = 1;//sourceMatrix
            col++;
            row++;
        }
    }
    result.finalize();
    #if PLOTGRADIENT
        TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(0);
        SpectrumPlotter->plotVectorField(grad,0,1,true,false);
    #endif
    return result;
}

void addSrcPoint(QMap<int, QPoint>&SrcPoints, int rows,int cols,int row,int col){
    QPoint p(col,row);
    SrcPoints.insert(indexMatrixToVec(rows,cols,row,col),p);
}

QMap<int, QPoint> getSrcPoints(int rows,int cols,TRetrievalImage *ThermalImage,float ThermalThreshold){
    QMap<int, QPoint> result;
    #if 1
    for (int col=0;col<cols;col++){
        for (int row=0;row<rows;row++){
            bool added = false;
            if (ThermalImage){
                if (ThermalImage->valueBuffer[row][col]->val > ThermalThreshold){
                    addSrcPoint(result, rows, cols, row, col);
                    added = true;
                }
            }
            if((row == 0) && (!added)){
                addSrcPoint(result, rows, cols, row, col);
            }else if(col == 0){
                addSrcPoint(result, rows, cols, row, col);
            }else if(row == rows-1){
                addSrcPoint(result, rows, cols, row, col);
            }else if(col == cols-1){
                addSrcPoint(result, rows, cols, row, col);
            }
        }
    }
    #endif
    return result;
}

SparseMatrix<double,RowMajor> getSEinv(MatrixXd &CorrelationMatrix){
    //nicht getestet
    SparseMatrix<double,RowMajor>   result = matrixToDiag(CorrelationMatrix); //KCols = 3*KRows; KRows = ImgRows*imgCols
    return result;
}

MatrixXd getAprioriSRC(int Rows,int Cols,int srcRow, int srcCol, double srcVal,int srcSmoothSize){
        //nicht getestet
    //Rows,cols are image rows,cols
    //if srcCol or srcRow == -1 then this point is ignored
    MatrixXd Apriori_SourceMatrix(Rows,Cols);
    Apriori_SourceMatrix.setZero(Rows,Cols);
    if (srcRow >= 0 && srcCol >= 0 )
        Apriori_SourceMatrix(srcRow,srcCol) = srcVal;

    MatrixXd smoothKernel(srcSmoothSize,srcSmoothSize);
    smoothKernel.setOnes(srcSmoothSize,srcSmoothSize);
    Apriori_SourceMatrix = conv2d(Apriori_SourceMatrix,smoothKernel);

    return Apriori_SourceMatrix;
}



VectorXd getAprioriX(int Rows,int Cols,QPointF &APrioriVec,MatrixXd &AprioriSRC){
    //nicht getestet
    VectorXd result(Rows*Cols*3);//Rows,cols are image rows,cols
    result.setZero(Rows*Cols*3);
    for(int blocksel=0;blocksel<2;blocksel++){
        for(int row=0;row<Rows;row++){
            for(int col=0;col<Cols;col++){
                if(blocksel==0){            //X
                    result(indexMatrixToVec(Rows,Cols,row,col))=APrioriVec.x();
                }else if (blocksel == 1){   //Y
                    result(Rows*Cols+indexMatrixToVec(Rows,Cols,row,col))=APrioriVec.y();
                }else{                      //SRC
                    result(2*Rows*Cols+indexMatrixToVec(Rows,Cols,row,col))=AprioriSRC(row,col);
                }
            }
        }
    }

    return result;
}

bool testSAInvSrc(SparseMatrix<double,RowMajor> test,QMap<int, QPoint>SrcPoints){
    bool result = true;
    int srcstart = test.rows()/3;
    srcstart *= 2;
    // test col and row sum of SrcPoints

    QMapIterator<int, QPoint> i(SrcPoints);

    while (i.hasNext()) {
        i.next();
        float rowsum=0;
        float colsum=0;
        int row = i.key()+srcstart;
        for(int n=0;n<test.cols();n++){
            float val1 = test.coeff(row,n);
            float val2= test.coeff(n,row);
            rowsum+=val1;
            colsum+=val2;
            if((val1 != 0) && (val2!=0)){
                result = false;
            }
        }
        if (rowsum > 0.1){
              result = false;
            //break;
        }
        if (colsum > 0.1){
              result = false;
            //break;
        }
    }
    for (int row = srcstart;row < test.rows();row++){
        float rowsum=0;
        float colsum=0;
        for(int n=0;n<test.cols();n++){
            rowsum+=test.coeff(row,n);
            colsum+=test.coeff(n,row);
        }
        if (rowsum > 0.0001){
            result = false;
            //break;
        }
        if (colsum > 0.0001){
            result = false;
            //break;
        }
    }
    return result;
}


SparseMatrix<double,RowMajor> getSAInv(double constraintVec,double constraintSrcOET,double constraintSrcTikhonov, double saDiagonal, int Rows,int Cols, SparseMatrix<double,RowMajor> &Rv ,QMap<int, QPoint>SrcPoints){
    //Rows,Cols are ImgRows,Cols
    int KRows = Rows*Cols;
    int KCols = 3*KRows;
    SparseMatrix<double,RowMajor>   result(KCols,KCols); //KCols = 3*KRows; KRows = ImgRows*imgCols
    //nicht getestet
    QMap<int, double> RowsToManipulate;
    for (int row=0; row<result.rows(); row++){
        result.startVec(row);
        if(row < getKSrcStart(KRows)){//lets copy the rv operator in Vec Part
            for (SparseMatrix<double,RowMajor>::InnerIterator it(Rv,row); it; ++it){
                double val = it.value()*constraintVec;
                int itcol=it.col();
                if (row==itcol)
                    result.insertBack(row,itcol) = val+saDiagonal;
                else
                    result.insertBack(row,itcol) = val;
            }
        }else{
            if (SrcPoints.contains(row-getKSrcStart(KRows))){ //since in this row is a src we want to hide, lets set this row to 0 except the diagonal element
                result.insertBack(row,row) = constraintSrcOET;
            }else{
                #if 1
                QMapIterator<int, QPoint> i(SrcPoints);
                int nextcol = -1;
                for (SparseMatrix<double,RowMajor>::InnerIterator it(Rv,row-getKSrcStart(KRows)); it; ++it){
                    double savinvval = it.value()*constraintSrcTikhonov;
                    int itcol=it.col();
                    while(nextcol<itcol){
                        if (i.hasNext()) {
                            i.next();
                        }else{
                            break;
                        }
                        nextcol = i.key();
                    }
                    if (nextcol==itcol){
                        //since in this col is a src we want to hide, we should set it to zero. -> this offdiagonal (coeff<0) is set to 0
                        //for keeping the rule diagonalelement[row] == sum(offdiagonalelements[row]) we have to manipulate the
                        //diagonalelement later -> we have to save its position for beeing able to do it later.
                        if (it.value()<0){//Offdiagonal always < 0
                            RowsToManipulate.insertMulti(row,savinvval);
                        }
                        //result.insertBack(row,itcol+getKSrcStart(KRows)) = savinvval;
                    }else{

                        result.insertBack(row,itcol+getKSrcStart(KRows)) = savinvval;
                    }
                }
                #endif
            }
        }
    }
    result.finalize();
    QMapIterator<int, double> i(RowsToManipulate);
    while (i.hasNext()) {
        i.next();
        int row = i.key();
        QList<double> vals = RowsToManipulate.values(row);
        for (int n = 0;n<vals.count();n++){
            result.coeffRef(row,row) = result.coeff(row,row) + i.value();
            if (result.coeffRef(row,row) <= 0){//diagonal element always should be > 0
                result.coeffRef(row,row) = constraintSrcOET;
            }
        }
    }
    //issymetrical(result);
    //testSAInvSrc(result,SrcPoints);
    return result;
}

bool issymetrical(SparseMatrix<double,RowMajor>  testM){
    bool result = true;
    if (testM.cols() == testM.rows()){
        for(int col = 0;col < testM.cols();col++){
            for(int row = 0; row < col;row++){
                if(testM.coeff(row,col)!=testM.coeff(col,row)){
                    result = false;
                    break;
                }
            }
            if (!result)
                break;
        }
    }else{
        result = false;
    }

    return result;
}

SparseMatrix<double,RowMajor> matrixW2H(int rows, int cols, bool doubleblock){
    //getestet
    #ifdef EIGEN_DEFAULT_TO_ROW_MAJOR
        #error matrixW2H relies on coloum-major
    #endif
    {//matrixw2h(3,2): 12 x 12
    //1 0 0 0 0 0  0 0 0 0 0 0
    //0 0 1 0 0 0  0 0 0 0 0 0
    //0 0 0 0 1 0  0 0 0 0 0 0
    //0 1 0 0 0 0  0 0 0 0 0 0
    //0 0 0 1 0 0  0 0 0 0 0 0
    //0 0 0 0 0 1  0 0 0 0 0 0

    //0 0 0 0 0 0  1 0 0 0 0 0
    //0 0 0 0 0 0  0 0 1 0 0 0
    //0 0 0 0 0 0  0 0 0 0 1 0
    //0 0 0 0 0 0  0 1 0 0 0 0
    //0 0 0 0 0 0  0 0 0 1 0 0
    //0 0 0 0 0 0  0 0 0 0 0 1
    }

    int blocks;
    if (doubleblock){
        blocks = 2;
    }else{
        blocks = 1;
    }
    SparseMatrix<double,RowMajor> result(cols*rows*blocks,cols*rows*blocks);
    result.reserve(cols*rows*blocks);
    for(int block=0;block<blocks;block++){
        int lastcol;
        int innerblockrow=rows;
        int blockcolstart=0;
        for(int i=0;i<cols*rows;i++){
            int row = cols*rows*block+i;
            int col;

            if (innerblockrow+1 < rows){
                col = lastcol+cols;
                innerblockrow++;
            }else{
                innerblockrow=0;
                col = blockcolstart;
                blockcolstart++;
           }

            lastcol = col;
            result.startVec(row);
            result.insertBack(row,cols*rows*block+col) = 1.0;
        }
    }
    result.finalize();
    return result;
}

SparseMatrix<double,RowMajor> constraintBlockTikhonov(int dim,int blockrows){
    //getestet
    SparseMatrix<double,RowMajor> result(dim,dim);

    #ifdef EIGEN_DEFAULT_TO_ROW_MAJOR
        #error matrixW2H relies on coloum-major
    #endif
    //constraint_block_tikhonov(8,4):
    // 1 -1  0  0   0  0  0  0
    //-1  2 -1  0   0  0  0  0
    // 0 -1  2 -1   0  0  0  0
    // 0  0 -1  1   0  0  0  0
    //
    // 0  0  0  0   1 -1  0  0
    // 0  0  0  0  -1  2 -1  0
    // 0  0  0  0   0 -1  2 -1
    // 0  0  0  0   0  0 -1  1

    result.reserve(dim+2*(dim-dim/blockrows));
    int innerblockrowIter=0;
    for(int row = 0;row<dim;row++){
        result.startVec(row);
        if (innerblockrowIter==0){
            result.insertBack(row,row)=1;
            result.insertBack(row,row+1)=-1;
        }else if (innerblockrowIter+1==blockrows){
            result.insertBack(row,row-1)=-1;
            result.insertBack(row,row)=1;
        }else{
            result.insertBack(row,row-1)=-1;
            result.insertBack(row,row)=2;
            result.insertBack(row,row+1)=-1;
        }

        innerblockrowIter++;
        if (innerblockrowIter>=blockrows)
            innerblockrowIter=0;
    }
    result.finalize();
    return result;
}

SparseMatrix<double,RowMajor> xy_tikhonov(int Rows,int Cols){
//getestet
    int dim = Rows*Cols*2;
    int tmp = Cols;
    Cols = Rows;
    Rows = tmp;
    SparseMatrix<double,RowMajor> Rx = constraintBlockTikhonov(dim,Cols);
    SparseMatrix<double,RowMajor> hRy= constraintBlockTikhonov(dim,Rows);
    //;print,'w2h'
    SparseMatrix<double,RowMajor> T=matrixW2H(Rows,Cols);
    //;print,'Ry'
    SparseMatrix<double,RowMajor> Ry=T.transpose()*hRy*T;
    SparseMatrix<double,RowMajor> result = Rx+Ry;

    return result;
}

//L1_block(8,4)
//-1  -0.5  0    0    0    0    0    0
// 1   0   -0.5  0    0    0    0    0
// 0   0.5  0   -1    0    0    0    0
// 0   0    0.5  1    0    0    0    0
// 0   0    0    0   -1   -0.5  0    0
// 0   0    0    0    1    0   -0.5  0
// 0   0    0    0    0    0.5  0   -1
// 0   0    0    0    0    0    0.5  1


SparseMatrix<double,RowMajor> L1Block(int dim,int blockrows){
//getestet
    SparseMatrix<double,RowMajor> result(dim,dim);
    result.reserve(2*dim/blockrows + 2*(dim-dim/blockrows));
    int innerblockrowIter=0;
    for(int row = 0;row<dim;row++){
        result.startVec(row);
        if (innerblockrowIter==0){
            if(blockrows==2){
                result.insertBack(row,row)=-1;
                result.insertBack(row,row+1)=-1;
            }
            if(blockrows!=2){
                result.insertBack(row,row)=-1;
                result.insertBack(row,row+1)=-0.5;
                innerblockrowIter=0;
            }
        }else if (innerblockrowIter==1){
            if (blockrows==3){
                result.insertBack(row,row-1)=1;
                result.insertBack(row,row+1)=-1;
            }
            if (blockrows!=3){
                if(blockrows==2){
                    result.insertBack(row,row-1)=1;
                    result.insertBack(row,row)=1;
                }
                if(blockrows!=2){
                    result.insertBack(row,row-1)=1;
                    result.insertBack(row,row+1)=-0.5;
                }
            }
        }else if (innerblockrowIter+2==blockrows){
            result.insertBack(row,row-1)=0.5;
            result.insertBack(row,row+1)=-1;
        }else if (innerblockrowIter+1==blockrows){
            result.insertBack(row,row-1)=0.5;
            result.insertBack(row,row)=1;
        }else{
            result.insertBack(row,row-1)=0.5;
            result.insertBack(row,row+1)=-0.5;

        }

        innerblockrowIter++;
        if (innerblockrowIter>=blockrows)
            innerblockrowIter=0;
    }
    result.finalize();
    //std::cout << "L1Block nonzeros: " << result.nonZeros() << std::endl;
    return result;

}

//L1_block_transpose_diag(12,4)
//-1    0    0   -0.5  0    0    0    0    0    0    0    0
// 0   -1    0    0   -0.5  0    0    0    0    0    0    0
// 0    0   -1    0    0   -0.5  0    0    0    0    0    0
// 1    0    0    0    0    0   -0.5  0    0    0    0    0
// 0    1    0    0    0    0    0   -0.5  0    0    0    0
// 0    0    1    0    0    0    0    0   -0.5  0    0    0
// 0    0    0    0.5  0    0    0    0    0   -1    0    0
// 0    0    0    0    0.5  0    0    0    0    0   -1    0
// 0    0    0    0    0    0.5  0    0    0    0    0   -1
// 0    0    0    0    0    0    0.5  0    0    1    0    0
// 0    0    0    0    0    0    0    0.5  0    0    1    0
// 0    0    0    0    0    0    0    0    0.5  0    0    1

SparseMatrix<double,RowMajor> L1BlockTranspose(int dim,int blockrows){
    //getestet!
#if 0
    SparseMatrix<double,RowMajor> result(dim,dim);
    result.reserve(2*dim/blockrows + 2*(dim-dim/blockrows));
    int innerblockrowIter=0;
    int blockcount = dim/blockrows;
    for(int row = 0;row<dim;row++){
        result.startVec(row);
        if (row<blockcount){
            if (blockrows == 2){
                result.insertBack(row,row)=-1;
                result.insertBack(row,row+blockcount)=-1;
            }else{
                result.insertBack(row,row)=-1;
                result.insertBack(row,row+blockcount)=-0.5;
            }
        }else if ((row>=blockcount)&&((row<blockcount*2)) ){
            if (blockrows == 2){
                result.insertBack(row,row-blockcount)=1;
                result.insertBack(row,row)=1;
            }else{
                result.insertBack(row,row-blockcount)=1;
                result.insertBack(row,row+blockcount)=-0.5;
            }
        }else if ((row >= (dim-blockcount*2))&&(row < (dim-blockcount))){
            result.insertBack(row,row-blockcount)=0.5;
            result.insertBack(row,row+blockcount)=-1;
        }else if (row >= (dim-blockcount)){
            result.insertBack(row,row-blockcount)=0.5;
            result.insertBack(row,row)=1;

        }else{
            result.insertBack(row,row-blockcount)=0.5;
            result.insertBack(row,row+blockcount)=-0.5;
        }

        innerblockrowIter++;
        if (innerblockrowIter>=blockrows)
            innerblockrowIter=0;
    }
    result.finalize();
    return result;
#else
    SparseMatrix<double,RowMajor> result;
    int width=dim/blockrows;

    SparseMatrix<double,RowMajor> T=matrixW2H(blockrows,width,false);

    SparseMatrix<double,RowMajor> L1=L1Block(dim,blockrows);

    result = T.transpose()*L1*T;
     //std::cout << "L1BlockTranspose nonzeros: " << result.nonZeros() << std::endl;
    return result;
#endif
}



VectorXd nextstepOET(const VectorXd& xapriori,const SparseMatrix<double,RowMajor> SAinv,const  VectorXd& deltay,const SparseMatrix<double,RowMajor>SEinv, const SparseMatrix<double,RowMajor>K){
//nicht getestet!
    bool ok = true;

    SparseMatrix<double,RowMajor> Kt = K.transpose();
    SparseMatrix<double,RowMajor> SXinv = Kt*SEinv*K+SAinv;
    VectorXd                      y1 =  SXinv*xapriori;
    SparseMatrix<double,RowMajor> y2 =  Kt*SEinv;

//    std::cout << "SXinv rows,cols " << SXinv.rows() << ","<< SXinv.cols() << std::endl;
//    std::cout << "deltay rows,cols " << deltay.rows() << ","<< deltay.cols() << std::endl;
//    std::cout << "y2 rows,cols " << y2.rows() << ","<< y2.cols() << std::endl;
//    std::cout << "y1 rows,cols " << y1.rows() << ","<< y1.cols() << std::endl;

    VectorXd                      y = y1+VectorXd(y2*deltay);
    VectorXd result;


    // solve SXinv*result = y
    //SimplicialLLt< SparseMatrix<double,RowMajor> > solver;
    SuperLU< SparseMatrix<double,RowMajor> > solver;
    //ConjugateGradient< SparseMatrix<double,RowMajor> > solver;
    solver.compute(SXinv);
    if(solver.info()!=Success) {
      // decomposition failed
        ok = false;
        std::cout << "decomposition failed" << std::endl;
    }
    if (ok){
        result = solver.solve(y);
        if(solver.info()!=Success) {
          // solving failed
            ok = false;
            std::cout << "solving failed" << std::endl;
        }
    }

    return result;
}

float get2Norm(QPointF p){
    return sqrt(p.x()*p.x()+p.y()*p.y());
}

float getVecAngle(QPointF vector){

//                0 (0,1)
//                |
//                |
// (-1,0) 270-----+-----90 (1,0)
//                |
//                |
//               180 (0,-1)

    float length = get2Norm(vector);
    float x = vector.x();

    float angle = asin(x/length);
    angle *= 180/M_PI;
    if(vector.y() < 0)
        angle = 180 - angle;

    if(angle < 0)
        angle += 360;
    return angle;
}

QPointF getMedianPoint(QList<QPointF> Apriori){
    QMap<float,QPointF> angles;
    float distances[3];
    QPointF result;
    for(int i = 0;i<Apriori.count();i++){
        QPointF p = Apriori[i];
        float angle = getVecAngle(p);
        angles.insertMulti(angle,Apriori[i]);
    }
    if (Apriori.count() == 3){
        int n=0;
        float oldpos;
        float firstpos;
        {
            QMapIterator<float, QPointF> i(angles);
            while (i.hasNext()) {
                i.next();
                if (n>0){
                    distances[n-1]=i.key()-oldpos;
                }else{
                    firstpos = i.key();
                }
                oldpos = i.key();
                n++;
            }
        }
        distances[n-1] = 360-oldpos + firstpos;
        float highesdistance;
        int highesdistanceindex;
        for(int i = 0;i<n;i++){
            if((distances[i] > highesdistance) || (i == 0)){
                highesdistance = distances[i];
                highesdistanceindex=i;
            }
        }
        highesdistanceindex--;
        if (highesdistanceindex<0)
            highesdistanceindex=3-highesdistanceindex;

        QMapIterator<float, QPointF> i(angles);
        n=0;
        while (i.hasNext()) {
            i.next();
            if(n==highesdistanceindex){
                result = i.value();
                break;
            }
            n++;

        }
    }else{
        if(Apriori.count()==0){
            result = QPointF(0,0);
        }else if(Apriori.count() == 1){
            result = Apriori[0];
        }else if(Apriori.count() == 2){
            result = Apriori[0]+Apriori[1];
            result /= 2;
        }
    }
    return result;
}

QPointF getAvgPoint(QList<QPointF> Apriori){
    QPointF  result(0,0);
    int n=0;
    for(int i=0;i<Apriori.count();i++){
        if (!isnan(Apriori[i].x())){
            result += Apriori[i];
            n++;
        }
    }
    if (n>0)
        result /= n;
    return result;
}

float selectAndIntegrateCorridor(TRetrievalImage* retimg, TParamLine &corridor, QPointF MeanFluxDirection, float CorridorWidth,int index,float distance){
    float result=0;
    int points=0;
    //TSpectrumPlotter* sp = TSpectrumPlotter::instance(0);
    QRectF corridorframe;
    QPointF unitydirectionvector = MeanFluxDirection;
    result = sqrt(unitydirectionvector.x()*unitydirectionvector.x()+unitydirectionvector.y()*unitydirectionvector.y());
    unitydirectionvector /= result;
    result = 0;
    for (int row=0;row<retimg->getHeight();row++){
        for (int col=0;col<retimg->getWidth();col++){
            QPointF p = retimg->valueBuffer[row][col]->mirrorCoordinate->getAngleCoordinate();
            float dist = corridor.GetDistanceToPoint(p);
            if (dist < CorridorWidth){
                QPointF windvector = retimg->valueBuffer[row][col]->getWindVector();
                float windspeedprojection = unitydirectionvector.x()*windvector.x()+
                                            unitydirectionvector.y()*windvector.y();
                //windspeedprojection = 1;
                result += retimg->valueBuffer[row][col]->val*windspeedprojection;//FIXME! We shouldnt use the scalebycorr image for integrating
                if (index == 1){
                 //   sp->plotXYMarkerNoLine(p.x(),p.y(),"",0);
                }
                if ((p.x() < corridorframe.left()) ||(points==0)){
                    corridorframe.setLeft(p.x());
                }
                if ((p.x() > corridorframe.right()) ||(points==0)){
                    corridorframe.setRight(p.x());
                }
                if ((p.y() < corridorframe.top()) ||(points==0)){
                    corridorframe.setTop(p.y());
                }
                if ((p.y() > corridorframe.bottom()) ||(points==0)){
                    corridorframe.setBottom(p.y());
                }
                points++;
            }
        }
    }
    float corridorlength = sqrt(corridorframe.height()*corridorframe.height()+corridorframe.width()*corridorframe.width());
    result/=points;
    result*=(distance*sin(M_PI*corridorlength/180));

    return result;
}
VectorXd getAKDiag(SparseMatrix<double,RowMajor>  sainv,SparseMatrix<double,RowMajor> Seinv,SparseMatrix<double,RowMajor> K){
    SparseMatrix<double,RowMajor> KTSK = K.transpose()*Seinv*K;
    SparseMatrix<double,RowMajor> Sx = SparseMatrix<double,RowMajor>(KTSK) + sainv;
    VectorXd AKDiag(KTSK.cols());
    bool ok = true;
    // solve Sx*AK = KTSK
    //SimplicialLLt< SparseMatrix<double,RowMajor> > solver;
    SuperLU< SparseMatrix<double,RowMajor> > solver;
    // solve Sx*AK = KTSK
    //ConjugateGradient< SparseMatrix<double,RowMajor> > solver;
    solver.compute(Sx);
    if(solver.info()!=Success) {
      // decomposition failed
        ok = false;
        std::cout << "decomposition failed" << std::endl;
    }



    if (ok){
        QFile data("AK_transposed.txt");
        data.open(QFile::WriteOnly | QFile::Truncate);
        QTextStream datastream(&data);

        for(int c=0;c<KTSK.cols();c++){
            VectorXd KTSKcol(KTSK.rows());
            for(int r = 0;r<KTSK.rows();r++){
                KTSKcol(r) = KTSK.coeff(r,c);
            }
            VectorXd AKCol = solver.solve(KTSKcol);
            for(int r = 0;r<AKCol.rows();r++){
                datastream << QString::number(AKCol.coeff(r)) << "\t";
            }
            datastream << "\n";
            double v = AKCol(c);
            AKDiag(c) = v;
            //*DOF += v;
            if(solver.info()!=Success) {
              // solving failed
                ok = false;
                std::cout << "solving failed" << std::endl;
                break;
            }
        }
        data.close();
        //std::cout << "DOF:" << DOF << std::endl;
    }

    return AKDiag;

}

float calcDOF_(VectorXd AKDiag,int offset){
    float result=0;
    for(int i = offset;i< offset+(AKDiag.rows()/3);i++){
        result += AKDiag(i);
    }
    return result;
}

float calcDOF_x(VectorXd AKDiag){
    return calcDOF_(AKDiag,0);
}

float calcDOF_y(VectorXd AKDiag){
    return calcDOF_(AKDiag,(AKDiag.rows()/3));
}

float calcDOF_src(VectorXd AKDiag){
    return calcDOF_(AKDiag,2*(AKDiag.rows()/3));
}

//function AK_OET,xapriori,sainv,deltay,Seinv,Kmat,dof
//	;deltay=y-F(xapriori)
//	KT=transpose(Kmat)
//	Sx=Invert(KT#seinv#Kmat+sainv,/Double)
//	G=Sx#KT#seinv
//	AK=G#Kmat
//	dof=trace(AK)
//	print,'DOF:',dof
//	return,AK
//end
