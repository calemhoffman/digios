#ifndef MATRIX_H
#define MATRIX_H

#include "TString.h"
#include <cmath>

using namespace std;

class Matrix {
public:

    Matrix();
    Matrix(const int row, const int col);
    Matrix(const Matrix& a);
    Matrix(int row, int col,  double v);
    Matrix& operator= (const Matrix& a);
    ~Matrix();

    double & operator () (const int r, const int c);

    // Matrix with Matrix
    Matrix operator + (const Matrix& a);
    Matrix operator - (const Matrix& a);
    Matrix operator * (const Matrix& a);

    // Matrix with double
    Matrix operator + (const double b);
    Matrix operator - (const double b);
    Matrix operator * (const double b);
    Matrix operator / (const double b);

    // double with Matrix
    friend Matrix operator + (const double b, const Matrix& a){
        Matrix res = a;
        return res + b;
    }

    friend Matrix operator - (const double b, const Matrix& a){
        Matrix res = -a;
        return res + b;
    }

    friend Matrix operator * (const double b, const Matrix& a){
        Matrix res = a;
        return res * b;
    }
    // operator unary minus
    friend Matrix operator - (const Matrix& a){
        Matrix res(a.rows, a.cols);

        for (int r = 0; r < a.rows; r++){
            for (int c = 0; c < a.cols; c++){
                res.p[r][c] = -a.p[r][c];
            }
        }

        return res;
    }

    //Matrix Self function
    Matrix Minor(const int row, const int col) const; //for Det()
    Matrix Transpose();
    Matrix Inverse();
    double Det();
    int GetRows(){return rows;}
    int GetCols(){return cols;}
    double Get(int r, int c){return p[r-1][c-1];}

    void PrintM();
    void PrintM(string str);
    void PrintVector(string str);

private:
    double **p;
    int rows, cols;

    //Axulliary function
    void Swap(double& a, double& b){
        double temp = a;
        a = b;
        b = temp;
    }

};

#endif // MATRIX_H
