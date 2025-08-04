#ifndef MATRIX_ZERO_H
#define MATRIX_ZERO_H

/*
A simple matrix class (0-based indexing)
Author: Adapted for 0-based indexing from Ryan Tang's Matrix.h

Features:
  - create a 2D matrix with custom size
  - get/set the cell values (0-based)
  - use operators +, -, *, /
  - use functions Ones(), Zeros(), Diag(), Det(), Inv(), Size()
  - print the content of the matrix
*/

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <vector>

class Exception{
public:
  const char* msg;
  Exception(const char* arg) : msg(arg){}
};

class Matrix{
private:
  int nRow, nCol;
  double** p;

  public:

  Matrix() : nRow(0), nCol(0), p(nullptr) {}
  Matrix(int row_count, int col_count) : nRow(row_count), nCol(col_count) {
    if (nRow > 0 && nCol > 0) {
      p = new double*[nRow];
      for (int r = 0; r < nRow; r++) {
        p[r] = new double[nCol];
        for (int c = 0; c < nCol; c++) p[r][c] = 0;
      }
    } else {
      p = nullptr;
    }
  }
  Matrix(const Matrix& a) : nRow(a.nRow), nCol(a.nCol) {
    if (nRow > 0 && nCol > 0) {
      p = new double*[nRow];
      for (int r = 0; r < nRow; r++) {
        p[r] = new double[nCol];
        for (int c = 0; c < nCol; c++) p[r][c] = a.p[r][c];
      }
    } else {
      p = nullptr;
    }
  }
  Matrix& operator=(const Matrix& a) {
    if (this == &a) return *this;
    if (p) {
      for (int r = 0; r < nRow; r++) delete[] p[r];
      delete[] p;
    }
    nRow = a.nRow;
    nCol = a.nCol;
    if (nRow > 0 && nCol > 0) {
      p = new double*[nRow];
      for (int r = 0; r < nRow; r++) {
        p[r] = new double[nCol];
        for (int c = 0; c < nCol; c++) p[r][c] = a.p[r][c];
      }
    } else {
      p = nullptr;
    }
    return *this;
  }
  ~Matrix() {
    if (p) {
      for (int r = 0; r < nRow; r++) delete[] p[r];
      delete[] p;
    }
    p = nullptr;
  }

  // 0-based index operator
  double& operator()(int r, int c) {
    if (p && r >= 0 && r < nRow && c >= 0 && c < nCol) {
      return p[r][c];
    } else {
      throw Exception("Subscript out of range");
    }
  }
  double operator()(int r, int c) const {
    if (p && r >= 0 && r < nRow && c >= 0 && c < nCol) {
      return p[r][c];
    } else {
      throw Exception("Subscript out of range");
    }
  }

  int GetNRow() const { return nRow; }
  int GetNCol() const { return nCol; }

  void Print() const {
    if (p) {
      printf("[");
      for (int r = 0; r < nRow; r++) {
        if (r > 0) printf(" ");
        for (int c = 0; c < nCol - 1; c++) printf("%+.3e, ", p[r][c]);
        if (r < nRow - 1) printf("%+.3e;\n", p[r][nCol - 1]);
        else printf("%+.3e]\n", p[r][nCol - 1]);
      }
    } else {
      printf("[ ]\n");
    }
  }

  // Element-wise operations
  Matrix& Add(double v) {
    for (int r = 0; r < nRow; r++)
      for (int c = 0; c < nCol; c++) p[r][c] += v;
    return *this;
  }
  Matrix& Subtract(double v) { return Add(-v); }
  Matrix& Multiply(double v) {
    for (int r = 0; r < nRow; r++)
      for (int c = 0; c < nCol; c++) p[r][c] *= v;
    return *this;
  }
  Matrix& Divide(double v) {
    if (v == 0) throw Exception("Division by zero");
    return Multiply(1.0 / v);
  }

  // Static creation functions
  static Matrix Ones(int rows, int cols) {
    Matrix res(rows, cols);
    for (int r = 0; r < rows; r++)
      for (int c = 0; c < cols; c++) res(r, c) = 1;
    return res;
  }
  static Matrix Zeros(int rows, int cols) {
    return Matrix(rows, cols);
  }
  static Matrix Diag(int n) {
    Matrix res(n, n);
    for (int i = 0; i < n; i++) res(i, i) = 1;
    return res;
  }
  // Add Minor method for determinant/inverse
  Matrix Minor(int row, int col) const {
    Matrix res(nRow - 1, nCol - 1);
    for (int r = 0, rr = 0; r < nRow; r++) {
      if (r == row) continue;
      for (int c = 0, cc = 0; c < nCol; c++) {
        if (c == col) continue;
        res(rr, cc) = p[r][c];
        cc++;
      }
      rr++;
    }
    return res;
  }
  double Det() const {
    if (nRow != nCol) throw Exception("Matrix must be square");
    if (nRow == 1) return p[0][0];
    if (nRow == 2) return p[0][0] * p[1][1] - p[0][1] * p[1][0];
    double d = 0;
    for (int c = 0; c < nCol; c++) {
      double sign = (c % 2 == 0) ? 1 : -1;
      d += sign * p[0][c] * this->Minor(0, c).Det();
    }
    return d;
  }
  Matrix Transpose() const {
    Matrix res(nCol, nRow);
    for (int r = 0; r < nRow; r++)
      for (int c = 0; c < nCol; c++)
        res(c, r) = p[r][c];
    return res;
  }

  Matrix Inv() const {
    if (nRow != nCol) throw Exception("Matrix must be square");
    double d = Det();
    if (d == 0) throw Exception("Matrix is singular");
    if (nRow == 1) {
      Matrix res(1, 1);
      res(0, 0) = 1.0 / p[0][0];
      return res;
    }
    if (nRow == 2) {
      Matrix res(2, 2);
      res(0, 0) = p[1][1];
      res(0, 1) = -p[0][1];
      res(1, 0) = -p[1][0];
      res(1, 1) = p[0][0];
      return (1.0 / d) * res;
    }
    Matrix res(nRow, nCol);
    for (int r = 0; r < nRow; r++) {
      for (int c = 0; c < nCol; c++) {
        double sign = ((r + c) % 2 == 0) ? 1 : -1;
        res(c, r) = sign * Minor(r, c).Det(); // note transpose
      }
    }
    return (1.0 / d) * res;
  }


  // Singular Value Decomposition (Jacobi method for symmetric matrices)
  // Decomposes this matrix as U * S * U^T
  void SVD_square(Matrix& U, Matrix& S) const {
    if (nRow != nCol) throw Exception("SVD: Only square matrices supported in this simple implementation.");
    int n = nRow;
    U = Matrix::Diag(n); // Start with identity
    S = *this;           // Copy of original

    const int maxIter = 100;
    const double eps = 1e-10;

    for (int iter = 0; iter < maxIter; ++iter) {
      bool changed = false;
      for (int p = 0; p < n - 1; ++p) {
        for (int q = p + 1; q < n; ++q) {
          double app = S(p, p), aqq = S(q, q), apq = S(p, q);
          if (fabs(apq) < eps) continue;
          double phi = 0.5 * atan2(2 * apq, aqq - app);
          double c = cos(phi), s = sin(phi);

          // Jacobi rotation
          for (int k = 0; k < n; ++k) {
            double S_pk = S(p, k), S_qk = S(q, k);
            S(p, k) = c * S_pk - s * S_qk;
            S(q, k) = s * S_pk + c * S_qk;
          }
          for (int k = 0; k < n; ++k) {
            double S_kp = S(k, p), S_kq = S(k, q);
            S(k, p) = c * S_kp - s * S_kq;
            S(k, q) = s * S_kp + c * S_kq;
          }
          for (int k = 0; k < n; ++k) {
            double U_kp = U(k, p), U_kq = U(k, q);
            U(k, p) = c * U_kp - s * U_kq;
            U(k, q) = s * U_kp + c * U_kq;
          }
          changed = true;
        }
      }
      if (!changed) break;
    }
    // After rotations, S diagonal contains singular values
    // U columns are singular vectors
    // For true SVD, singular values should be non-negative
    for (int i = 0; i < n; ++i) {
        if (S(i, i) < 0) {
            for (int k = 0; k < n; ++k) U(k, i) = -U(k, i);
            S(i, i) = -S(i, i);
        }
        for (int j = 0; j < n; ++j) {
            if (i != j) S(i, j) = 0.0;
        }
    }
  }

};

//&##########################################################
// Matrix addition
inline Matrix operator+(const Matrix& a, const Matrix& b) {
  if (a.GetNRow() != b.GetNRow() || a.GetNCol() != b.GetNCol())
    throw Exception("Dimensions do not match");
  Matrix res(a.GetNRow(), a.GetNCol());
  for (int r = 0; r < a.GetNRow(); r++)
    for (int c = 0; c < a.GetNCol(); c++)
      res(r, c) = a(r, c) + b(r, c);
  return res;
}
// Matrix subtraction
inline Matrix operator-(const Matrix& a, const Matrix& b) {
  if (a.GetNRow() != b.GetNRow() || a.GetNCol() != b.GetNCol())
    throw Exception("Dimensions do not match");
  Matrix res(a.GetNRow(), a.GetNCol());
  for (int r = 0; r < a.GetNRow(); r++)
    for (int c = 0; c < a.GetNCol(); c++)
      res(r, c) = a(r, c) - b(r, c);
  return res;
}
// Matrix multiplication
inline Matrix operator*(const Matrix& a, const Matrix& b) {
  if (a.GetNCol() != b.GetNRow())
    throw Exception("Dimensions do not match for multiplication");
  Matrix res(a.GetNRow(), b.GetNCol());
  for (int r = 0; r < a.GetNRow(); r++)
    for (int c = 0; c < b.GetNCol(); c++) {
      res(r, c) = 0;
      for (int k = 0; k < a.GetNCol(); k++)
        res(r, c) += a(r, k) * b(k, c);
    }
  return res;
}
// Scalar multiplication
inline Matrix operator*(const Matrix& a, double b) {
  Matrix res = a;
  res.Multiply(b);
  return res;
}
inline Matrix operator*(double b, const Matrix& a) {
  Matrix res = a;
  res.Multiply(b);
  return res;
}
// Scalar addition/subtraction
inline Matrix operator+(const Matrix& a, double b) {
  Matrix res = a;
  res.Add(b);
  return res;
}
inline Matrix operator-(const Matrix& a, double b) {
  Matrix res = a;
  res.Subtract(b);
  return res;
}
// Scalar division
inline Matrix operator/(const Matrix& a, double b) {
  Matrix res = a;
  res.Divide(b);
  return res;
}

// Determinant (recursive, for small matrices)
inline double Det(const Matrix& a) { return a.Det(); }

// Transpose
inline Matrix Transpose(const Matrix& a) { return a.Transpose();}

// Inverse (adjugate method for small matrices)
inline Matrix Inv(const Matrix& a) {  return a.Inv();}

inline std::vector<Matrix> SVD(const Matrix& a) {
  if (a.GetNRow() != a.GetNCol()) throw Exception("SVD: Only square matrices supported in this simple implementation.");
  Matrix U, S;
  a.SVD_square(U, S);
  return {U, S};
}

#endif
