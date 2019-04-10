#include "matrix.h"

Matrix::Matrix()
{
    p = NULL;
    rows = 0;
    cols = 0;
}

Matrix::Matrix(const int row, const int col)
{
    p = NULL;
    if( row <= 0 && col <= 0 ) return;

    this->rows = row;
    this->cols = col;

    p = new double *[row];
    for ( int i =0 ; i < row ; i++){
        p[i] = new double [col];
    }
    for ( int i = 0; i < row ; i++ ){
        for (int j = 0; j < col; j++ ) {
            p[i][j] = 0 ;
        }
    }
}

Matrix::Matrix(const Matrix& a)
{
    rows = a.rows;
    cols = a.cols;
    p = new double*[a.rows];
    for (int r = 0; r < a.rows; r++){
        p[r] = new double[a.cols];

        // copy the values from the matrix a
        for (int c = 0; c < a.cols; c++){
            p[r][c] = a.p[r][c];
        }
    }
}

Matrix::Matrix(int row, int col, double v){

    this->rows = row;
    this->cols = col;

    p = new double *[row];
    for ( int i =0 ; i < row ; i++){
        p[i] = new double [col];
    }

    for ( int i = 0; i < row ; i++ ){
        for (int j = 0; j < col; j++ ) {
            if( i == j){
                p[i][j] = v ;
            }else{
                p[i][j] = 0;
            }
        }
    }
}

double &Matrix::operator()(const int r, const int c)
{
    if (p != NULL && r > 0 && r <= rows && c > 0 && c <= cols){
        return p[r-1][c-1];
    }else{
      printf(" index out of range. \n");
    }

    return p[0][0];
}

Matrix &Matrix::operator=(const Matrix &a)
{
    rows = a.rows;
    cols = a.cols;
    p = new double*[a.rows];
    for (int r = 0; r < a.rows; r++){
        p[r] = new double[a.cols];

        // copy the values from the matrix a
        for (int c = 0; c < a.cols; c++){
            p[r][c] = a.p[r][c];
        }
    }
    return *this;
}

Matrix::~Matrix()
{
    // clean up allocated memory
    for (int r = 0; r < this->rows; r++){
        delete this->p[r];
    }
    delete this->p;
    this->p = NULL;
}

Matrix Matrix::operator + (const Matrix &a)
{
    // check if the dimensions match
    if (a.rows == rows && a.cols == cols){
        Matrix res(a.rows, a.cols);

        for (int r = 0; r < a.rows; r++){
            for (int c = 0; c < a.cols; c++){
                res.p[r][c] = a.p[r][c] + p[r][c];
            }
        }
        return res;
    }

    // return an empty matrix (this never happens but just for safety)
    return Matrix();
}

Matrix Matrix::operator - (const Matrix &a)
{
    // check if the dimensions match
    if (a.rows == rows && a.cols == cols){
        Matrix res(a.rows, a.cols);

        for (int r = 0; r < a.rows; r++){
            for (int c = 0; c < a.cols; c++){
                res.p[r][c] = a.p[r][c] - p[r][c];
            }
        }
        return res;
    }else{
        // give an error
      printf("Dimensions does not match\n");
    }

    // return an empty matrix (this never happens but just for safety)
    return Matrix();
}

Matrix Matrix::operator * (const Matrix &a)
{
    // check if the dimensions match
    if (a.rows == cols){
        Matrix res(rows, a.cols);

        for (int r = 0; r < rows; r++){
            for (int c = 0; c < a.cols; c++){
                for (int k = 0; k < cols; k++){
                    res.p[r][c] += p[r][k] * a.p[k][c];
                }
            }
        }
        return res;
    }else{
        // give an error
        printf("Dimensions does not match\n");
    }

    // return an empty matrix (this never happens but just for safety)
    return Matrix();
}

Matrix Matrix::operator + (const double b)
{
    Matrix res = *this;

    for (int r = 0; r < rows; ++r){
        for (int c = 0; c < cols; c++){
            res.p[r][c] += b;
        }
    }
    return res;
}

Matrix Matrix::operator - (const double b)
{
    Matrix res = *this;
    res + (-b);
    return res;
}

Matrix Matrix::operator * (const double b)
{
    Matrix res = *this;
    for (int r = 0; r < rows; r++){
        for (int c = 0; c < cols; c++){
            res.p[r][c] *= b;
        }
    }
    return res;
}

Matrix Matrix::operator / (const double b)
{
    Matrix res = *this;
    res * (1/b);
    return res;
}


Matrix Matrix::Minor(const int row, const int col) const
{
    Matrix res;
    if (row > 0 && row <= rows && col > 0 && col <= cols){
        res = Matrix(rows - 1, cols - 1);

        // copy the content of the matrix to the minor, except the selected
        for (int r = 1; r <= (rows - (row >= rows)); r++){
            for (int c = 1; c <= (cols - (col >= cols)); c++){
                res(r - (r > row), c - (c > col)) = p[r-1][c-1];
            }
        }
    }else{
      printf("Index for minor out of range\n");
    }

    return res;
}

Matrix Matrix::Transpose(){

    int r = this->rows;
    int c = this->cols;

    Matrix res = Matrix(c, r);
    for (int i = 0 ; i < c; i ++){
        for (int j = 0 ; j < r; j ++){
            res.p[i][j]= p[j][i];
        }
    }

    return res;
}

Matrix Matrix::Inverse(){
    Matrix res;

    Matrix a = *this;
    int rows = a.rows;
    int cols = a.cols;
    double d = a.Det();

    if (rows == cols && d != 0){
        if (rows == 1){
            res = Matrix(rows, cols);
            res(1, 1) = 1 / a(1, 1);
        }else if (rows == 2){
            res = Matrix(rows, cols);
            res(1, 1) = a(2, 2);
            res(1, 2) = -a(1, 2);
            res(2, 1) = -a(2, 1);
            res(2, 2) = a(1, 1);
            res = (1/d) * res;
        }else{
            // calculate inverse using gauss-jordan elimination
            //   http://mathworld.wolfram.com/MatrixInverse.html
            //   http://math.uww.edu/~mcfarlat/inverse.htm
            res = Matrix(rows, rows, 1);   // a diagonal matrix with ones at the diagonal
            Matrix ai = a;    // make a copy of Matrix

            for (int c = 1; c <= cols; c++){
                // element (c, c) should be non zero. if not, swap content
                // of lower rows
                int r;
                for (r = c; r <= rows && ai(r, c) == 0; r++){
                }
                if (r != c){
                    // swap rows
                    for (int s = 1; s <= cols; s++){
                        Swap(ai(c, s), ai(r, s));
                        Swap(res(c, s), res(r, s));
                    }
                }

                // eliminate non-zero values on the other rows at column c
                for (int r = 1; r <= rows; r++){
                    if(r != c){
                        // eleminate value at column c and row r
                        if (ai(r, c) != 0){
                            double f = - ai(r, c) / ai(c, c);

                            // add (f * row c) to row r to eleminate the value
                            // at column c
                            for (int s = 1; s <= cols; s++){
                                ai(r, s) += f * ai(c, s);
                                res(r, s) += f * res(c, s);
                            }
                        }
                    }else{
                        // make value at (c, c) one,
                        // divide each value on row r with the value at ai(c,c)
                        double f = ai(c, c);
                        for (int s = 1; s <= cols; s++){
                            ai(r, s) /= f;
                            res(r, s) /= f;
                        }
                    }
                }
            }
        }
    }else{
        printf(" fail to calculate inverse\n");
    }

    return res;
}

double Matrix::Det(){
    double d = 0;

    if( this->rows == this->cols){

        Matrix res = *this;

        if( res.rows == 1){
            d = res(1,1);
        }else if (res.rows == 2){
            d = res(1,1) * res(2,2) - res(1,2)* res(2,1);
        }else{
            for( int j = 1; j <= res.cols; j++){
                Matrix M = this->Minor(1,j);
                double det = M.Det();
                d += (j%2 + j%2 -1) * res(1,j) * det;
            }
        }
    }else{
      printf("Matrix must be square\n");
    }

    return d;
}



void Matrix::PrintM()
{
    TString tmp, msg;
    if( p != NULL){
        for (int i = 0 ; i < this->rows ; i++){
            msg = "[" ;
            for (int j = 0 ; j < this->cols-1; j++){
                tmp.Form("%7.3f, ",p[i][j]);
                msg += tmp;
            }
            tmp.Form("%7.3f]", p[i][this->cols-1]);
            msg += tmp;
            printf("%s\n", msg.Data());    
        }
    }else{
      printf("[]\n");
    }
}

void Matrix::PrintM(string str)
{
    TString msg;
    msg.Form("%s(%d,%d) = ", str.c_str(), this->rows, this->cols);
    printf("%s\n", msg.Data());
    PrintM();
}

void Matrix::PrintVector(string str)
{
    /// only for single row or single col matrix
    TString msg, tmp;
    msg.Form("%s(%d,%d) = ", str.c_str(), this->rows, this->cols);

    int n;
    if(this->rows == 1) n = this->cols;
    if(this->cols == 1) n = this->rows;

    if( p != NULL){
        msg += "[" ;
        for (int j = 0 ; j < n; j++){
            if(this->rows == 1) tmp.Form("%7.3f, ",p[0][j]);
            if(this->cols == 1) tmp.Form("%7.3f, ",p[j][0]);
            msg += tmp;
        }
        tmp.Form("]");
        msg += tmp;
    }else{
        msg += "[]";
    }

    printf("%s\n", msg.Data());
}
