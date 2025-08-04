#ifndef FindFit_H
#define FindFit_H

// this is a stand alone file for Levenberg-Marquardt Algorithm (LMA) implementation

#include <vector>
#include <cstdlib>
#include <cmath>

#include "Matrix_zero.h"

class FindFit {
public:
  FindFit() = default; // Default constructor
  FindFit(std::vector<double> x, std::vector<double> y, std::vector<double> par){
    // Constructor to initialize the LMA with data and parameters
    n = x.size();
    p = par.size();
    dF = n - p; // degrees of freedom
    valX = x; // independent variable data
    par = par; // parameters to be optimized
    
    // Initialize matrices
    Y = Matrix(n, 1);
    for (int i = 0; i < n; ++i) {
      Y(i, 0) = y[i];
    }
  }
  ~FindFit(){}

  void SetData(const std::vector<double> &x, const std::vector<double> &y){
    n = x.size();
    valX = x;
    Y = Matrix(n, 1);
    for (int i = 0; i < n; ++i) {
      Y(i, 0) = y[i];
    }
    if( p > 0 ) dF = n - p; // degrees of freedom
  }

  void SetParameters(const std::vector<double> &par){
    p = par.size();
    this->par = par;
    error.resize(p, 0.0); // initialize error vector
    if( n > 0 ) dF = n - p; // degrees of freedom
  }


  std::vector<double> GetParameters() const { return par;}
  std::vector<double> GetError() const { return error; }
  double GetSSR() const { return SSR; }
  double GetDegreesOfFreedom() const { return dF; }

  
  void LMA(double tolerance = 1e-5, int max_iter = 10000, double lambdaUp = 10, double lambdaDown = 10) {

    // Levenberg-Marquardt Algorithm implementation

    lambda = -1; // initial lambda value
    SSR = 1e+12;
    int count = 0; // iteration count

    double lastSSR; // last sum of squared residuals
    std::vector<double> par_old; // store old parameters

    double deltaSSR; // change in SSR
    do{
      
      par_old = par; // save current parameters
      lastSSR = SSR; // save last SSR
      int ret = NonLinearRegression(lambdaUp, lambdaDown);
      
      if( ret < 0 ){
        printf("fail to compute the inverse of Haessian matrix (J^T*J + lambda I)\n");
        break; 
      }
      
      count++;
      deltaSSR = fabs(SSR - lastSSR);

    }while(count < max_iter && deltaSSR > tolerance * SSR && 1e+12 > lambda && lambda > 1e-12);


    if( count >= max_iter ){
      printf("Warning: LMA did not converge within the maximum number of iterations (%d)\n", max_iter);
    }

    if( deltaSSR <= tolerance * SSR ){
      printf("LMA converged after %d iterations with SSR = %f\n", count, SSR);
    }else{
      printf("LMA did not converge, last SSR = %f after %d iterations\n", SSR, count); 
    }

    //calculate errors;
    double var = SSR / dF; // variance
    for (int i = 0; i < p; ++i)  error[i] = sqrt(var * CoVar(i, i)); // standard error

    // print the final parameters
    printf("==================== Fitting result: \n");
    printf("SSR = %f\n", SSR);
    for (int i = 0; i < p; ++i) {
      printf("par[%d] = %f (%f)\n", i, par[i], error[i] );
    }
    printf("######################################## end of LMA\n");

  }

private:
  std::vector<double> valX; // independent variable data
  Matrix Y;
  int n = 0; // number of data points
  int p = 0; // number of parameters
  int dF = 0; // degrees of freedom
  std::vector<double> par;
  std::vector<double> error; // errors for parameters
  double lambda = -1; // initial lambda value for LMA
  double SSR = 0; // sum of squared residuals

  Matrix CoVar; // this is covariance / var, var = SSR / nDF
  // it is used to calculate the parameter update step
  // CoVar = inverse(H + lambda * I)

  double FitFunction(const double x, const std::vector<double> &par) {
    double A = par[0];
    double T = par[1];
    double R = par[2];
    double B = par[3];
    return A /(1 + exp((x-T)/R)) + B; 
  }

  Matrix GradFitFunction(const double x, const std::vector<double> &par) {
    Matrix grad(4,1);
    
    double A = par[0];
    double T = par[1];
    double R = par[2];
    double B = par[3];

    double exp_term = exp((x - T) / R);
    
    grad(0,0) = 1/(1 + exp_term); // derivative with respect to A
    grad(1,0) = A *  exp_term / (R * (1+ exp_term) * (1 + exp_term)); // derivative with respect to T
    grad(2,0) = A * (x - T) * exp_term / (R * R * (1 + exp_term) * (1 + exp_term)); // derivative with respect to R
    grad(3,0) = 1; // derivative with respect to B

    // Note: grad(1,1) is the derivative with respect to A, grad(2,1) with respect to T, etc.
    // Adjust the indices if your parameter vector is structured differently.
    return grad;
  }

  int NonLinearRegression(double lambdaUp = 10, double lambdaDown = 10) {
    // this function update lambda, parametets, SSR, and CoVar

    // in Taylor series, we have:
    // Y = f + J*(p-p0) + e, J = df/dp = Jacobian, n x p 
    // let J^T*J = H is the Hessian matrix, p x p
    // p = p0 + inverse(H).(J^T*(Y-f)),
    
    
    // in Gradient Descent, 
    // the SSR (sum of squared residuals) is minimized is
    // SSR = (Y - f)^T * (Y - f) 
    // the derivative of SSR with respect to p is
    // dSSR/dp = -2 * J^T * (Y - f)
    // p = p0 + a * J^T * (Y - f)

    // LM method is a combination of Gradient Descent and Newton's method
    // p = p0 + inverse(H + lambda * I) * J^T * (Y - f)
    // Newton's method is fast but unstable, while Gradient Descent is stable but slow

    Matrix J(n, p);  // n x p matrix for function values
    Matrix f(n ,1);  // n x 1 matrix for fitted values

    for( int i = 0; i < n; i++) {
      double x_val = valX[i];
      f(i, 0) = FitFunction(x_val, par);
      
      Matrix Frow = GradFitFunction(x_val, par); // p x 1 matrix for gradient
      for (int k = 0; k < p; k++)  J(i, k) = Frow(k, 0); 

    }

    Matrix Jt = Transpose(J);
    Matrix H = Jt * J;

    if( lambda == -1 ){ // if lambda is not set, calculate initial lambda
      lambda = 0;
      for(int i = 0 ; i < n ; i++){
        for(int j = 0 ; j < p ; j++){
          lambda += J(i,j)*J(i,j);
        }
      }
      lambda = sqrt(lambda/n/p);
      printf("====== ini lambda : %f \n", lambda);
    }

    Matrix DI(H.GetNRow(), H.GetNCol()); // identity matrix
    for( int i = 0; i < H.GetNRow() ; i++) DI(i,i) = lambda ;

    Matrix Q = H + DI;
    try{
      CoVar = Inv(Q);
    }catch( Exception err){
      return -1; // Error in matrix inversion
    }

    Matrix dY = Y - f;
    Matrix JtY = Jt * dY; // p x 1 matrix for gradient of SSR

    SSR = (Transpose(dY) * dY)(0, 0); // compute initial SSR

    Matrix dpar = CoVar * JtY; // p x 1 matrix for

    std::vector<double> par_new = par; // new parameters
    for (int i = 0; i < p; ++i)  par_new[i] += dpar(i, 0); // update parameters

    //====================== Compute new SSR
    Matrix fn(n, 1);
    for(int i = 0; i < n ; i++) {
      double x_val = valX[i];
      fn(i, 0) = FitFunction(x_val, par_new);
    }
    Matrix dYn = Y - fn;
    double new_SSR = (Transpose(dYn) * dYn)(0, 0); // new SSR

    double delta = new_SSR - SSR; // change in SSR
    if (delta > 0) { 
      lambda *= lambdaUp; // increase lambda if SSR increased, because grdient descent should weight more 
      return 0;
    }else{
      lambda /= lambdaDown; // decrease lambda if SSR decreased
      SSR = new_SSR; // update SSR
      par = par_new; // update parameters
    }

    return 1; // Return 0 for success, non-zero for failure
  }



};

#endif