#include <stdio.h>
#include <math.h>
#include <cuda_runtime.h>

#include "timer.h"

#define N 1000     // Number of data points
#define P 4       // Number of parameters (A, T, R, B)
#define dF (N - P) // Degrees of freedom
#define THREADS 256
#define BLOCKS (N + THREADS - 1) / THREADS

float model(float x, float A, float T, float R, float B) {
  return A / (1.0f + expf((x - T) / R)) + B;
}

__global__ void compute_residuals_SSR_and_jacobian(float* y, float* residuals, float *SSR, float* J, float A, float T, float R, float B) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i >= N) return;

  float yi = y[i];
  float exp_term = expf((i - T) / R);

  if( exp_term > 1e20f ){

    float ri = yi - B;
    residuals[i] = ri;
    atomicAdd(SSR, ri * ri);

    // Compute partial derivatives (Jacobian)
    J[i * P + 0] = 0;                        // ∂f/∂A
    J[i * P + 1] = 0;        // ∂f/∂T
    J[i * P + 2] = 0; // ∂f/∂R
    J[i * P + 3] = 1.0f;                                // ∂f/∂B

  }else{
    float denom = (1.0f + exp_term);
    float denom2 = denom * denom;

    float fi = A / denom + B;
    float ri = yi - fi;
    residuals[i] = ri;
    atomicAdd(SSR, ri * ri);

    // Compute partial derivatives (Jacobian)
    J[i * P + 0] = 1.0f / denom;                        // ∂f/∂A
    J[i * P + 1] = (A * exp_term) / (R * denom2);        // ∂f/∂T
    J[i * P + 2] = (A * exp_term * (i - T)) / (R * R * denom2); // ∂f/∂R
    J[i * P + 3] = 1.0f;                                // ∂f/∂B

  }
}

// Reduction kernel to compute JᵗJ and Jᵗr
__global__ void compute_JTJ_JTr(float* J, float* residuals, float* JTJ, float* JTr) {
  __shared__ float sJTJ[P * P];
  __shared__ float sJTr[P];

  int tid = threadIdx.x;
  for (int i = tid; i < P * P; i += blockDim.x) sJTJ[i] = 0.0f;
  for (int i = tid; i < P; i += blockDim.x) sJTr[i] = 0.0f;
  __syncthreads();

  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < N) {
      float* Ji = &J[i * P];
      float ri = residuals[i];
      for (int j = 0; j < P; ++j) {
          atomicAdd(&sJTr[j], Ji[j] * ri);
          for (int k = 0; k < P; ++k) {
              atomicAdd(&sJTJ[j * P + k], Ji[j] * Ji[k]);
          }
      }
  }
  __syncthreads();

  if (tid < P * P) atomicAdd(&JTJ[tid], sJTJ[tid]);
  if (tid < P)     atomicAdd(&JTr[tid], sJTr[tid]);
}


// Host-side LMA update (solve (JᵗJ + λI) δ = Jᵗr)
float update_parameters(float* JTJ, float* JTr, float* new_params, const float * params, const float * y, float lambda) {
  // Solve PxP linear system using basic Gaussian elimination (naive, for demo)
  float A[P][5]; // PxP + rhs
  for (int i = 0; i < P; ++i)
    for (int j = 0; j < P; ++j)
      A[i][j] = JTJ[i * P + j] + (i == j ? lambda : 0.0f);
  for (int i = 0; i < P; ++i)
    A[i][P] = JTr[i];


  //print the augmented matrix A
  // printf("Augmented matrix A:\n");
  // for (int i = 0; i < P; ++i) {
  //   for (int j = 0; j < 5; ++j) {
  //     printf("%8.Pf ", A[i][j]);
  //   }
  //   printf("\n");
  // }

  // Gaussian elimination with partial pivoting
  for (int i = 0; i < P; ++i) {
    // Partial pivoting: find the row with the largest absolute value in column i
    int maxRow = i;
    float maxVal = fabsf(A[i][i]);
    for (int k = i + 1; k < P; ++k) {
      if (fabsf(A[k][i]) > maxVal) {
        maxVal = fabsf(A[k][i]);
        maxRow = k;
      }
    }
    // Swap rows if needed
    if (maxRow != i) {
      for (int j = 0; j <= P; ++j) {
        float tmp = A[i][j];
        A[i][j] = A[maxRow][j];
        A[maxRow][j] = tmp;
      }
    }
    float pivot = A[i][i];
    if (fabsf(pivot) < 1e-8f) pivot = 1e-8f; // Avoid division by zero
    for (int j = 0; j <= P; ++j)
      A[i][j] /= pivot;
    for (int k = 0; k < P; ++k) {
      if (k == i) continue;
      float factor = A[k][i];
      for (int j = 0; j <= P; ++j)
        A[k][j] -= factor * A[i][j];
    }
  }

  // Update parameters
  for (int i = 0; i < P; ++i) {
    new_params[i] = params[i] + A[i][P];
    // printf("Updated parameter %d: %.4f <- %.4f + %.4f\n", i, new_params[i], params[i], A[i][4]);
  }

  // calculate SSR
  float SSR = 0.0f;
  for (int i = 0; i < N; ++i) {
      float fi = model(i , new_params[0], new_params[1], new_params[2], new_params[3]);
      float ri = (y[i]  - fi);
      SSR += ri * ri;
  }
  return SSR;
    
}

int main() {
    float *y, *d_y, *d_res, *d_J, *d_JTJ, *d_JTr, *d_SSR;
    float params[P] = { 5.0f, 5.0f, 1.0f, 0.5f }; // Initial guess: A, T, R, B
    float new_params[P];

    float noise_level = 0.0f;

    y = new float[N];

    // Generate synthetic data
    for (int i = 0; i < N; ++i) {
      float true_val  = model(i, 10.0f, 503.0f, 80.0f, 1.0f);
      y[i] = true_val + noise_level * ((rand() % 1000) / 1000.0f - 0.5f);
    }

    unsigned int time0 = getTime_us();

    // Allocate GPU memory
    cudaMalloc(&d_y, N * sizeof(float));
    cudaMalloc(&d_res, N * sizeof(float));
    cudaMalloc(&d_SSR, sizeof(float));
    cudaMalloc(&d_J, N * P * sizeof(float));
    cudaMalloc(&d_JTJ, P * P * sizeof(float));
    cudaMalloc(&d_JTr, P * sizeof(float));

    cudaMemcpy(d_y, y, N * sizeof(float), cudaMemcpyHostToDevice);

    float lambda = 1.0f;

    unsigned int time1 = getTime_us();
    printf("Memory allocation and data transfer took %u us\n", time1 - time0 );

    int iter = 0;

    float JTJ[P*P], JTr[P], SSR;    
    do{
      // unsigned int time2 = getTime_us();

      // printf("----------------------- Iteration %d -----------------------\n", iter);
      // printf("Current parameters: A = %.4f, T = %.4f, R = %.4f, B = %.4f\n",
            // params[0], params[1], params[2], params[3]);
      
      cudaMemset(d_SSR, 0, sizeof(float));
      compute_residuals_SSR_and_jacobian<<<BLOCKS, THREADS>>>(d_y, d_res, d_SSR, d_J, params[0], params[1], params[2], params[3]);
      cudaDeviceSynchronize();

      cudaMemset(d_JTJ, 0, sizeof(float) * P * P);
      cudaMemset(d_JTr, 0, sizeof(float) * P);

      compute_JTJ_JTr<<<BLOCKS, THREADS>>>(d_J, d_res, d_JTJ, d_JTr);
      cudaDeviceSynchronize();

      // unsigned int time2a = getTime_us();
      // printf("Iteration %d: Kernel execution took %u us\n", iter, time2a - time2);

      cudaMemcpy( JTJ, d_JTJ, sizeof(float) * P * P, cudaMemcpyDeviceToHost);
      cudaMemcpy( JTr, d_JTr, sizeof(float) * P, cudaMemcpyDeviceToHost);
      cudaMemcpy(&SSR, d_SSR, sizeof(float), cudaMemcpyDeviceToHost);

      // unsigned int time2b = getTime_us();
      // printf("Iteration %d: Data transfer took %u us\n", iter, time2b - time2a);

      float new_SSR = update_parameters(JTJ, JTr, new_params, params, y, lambda);
      // unsigned int time3 = getTime_us();
      // printf("Iteration %d: Time for kernel execution and parameter update: %u us\n", iter, time3 - time2);
      
      if( fabs(new_SSR - SSR) < 1e-6f ) break;

      if (new_SSR < SSR) {
        lambda *= 0.1f;
        // printf("$$$$$$$$$ Accepting new parameters.\n");
        for (int i = 0; i < P; ++i) params[i] = new_params[i];
        SSR = new_SSR;
      } else {
        lambda *= 10.0f;
      }

      // unsigned int time4 = getTime_us();
      // printf("Iteration %d: A = %.4f, T = %.4f, R = %.4f, B = %.4f | SSR: %f, new SSR: %f | lambda : %.3e\n",
      //       iter, new_params[0], new_params[1], new_params[2], new_params[3], SSR, new_SSR, lambda);

      
    }while(iter++ < 200 && 1e+12 > lambda && lambda > 1e-12);

    double var = SSR / dF; // variance
    double error[P];      // standard deviation
    
    float JTJ_inv[P*P];
    // Invert JTJ matrix
    // Invert 4x4 matrix JTJ using Gauss-Jordan elimination
    for (int i = 0; i < P * P; ++i) JTJ_inv[i] = (i % (P + 1)) ? 0.0f : 1.0f; // Identity matrix
    float temp[P * P];
    for (int i = 0; i < P * P; ++i) temp[i] = JTJ[i];
    // Gauss-Jordan elimination
    for (int col = 0; col < P; ++col) {
      float pivot = temp[col * P + col];
      if (fabsf(pivot) < 1e-8f) pivot = 1e-8f;
      for (int j = 0; j < P; ++j) {
        temp[col * P + j] /= pivot;
        JTJ_inv[col * P + j] /= pivot;
      }
      for (int row = 0; row < P; ++row) {
        if (row == col) continue;
        float factor = temp[row * P + col];
        for (int j = 0; j < P; ++j) {
          temp[row * P + j] -= factor * temp[col * P + j];
          JTJ_inv[row * P + j] -= factor * JTJ_inv[col * P + j];
        }
      }
    }
      
    for (int i = 0; i < P; ++i) error[i] = sqrt(var * JTJ[i * P + i]);

    unsigned int time4 = getTime_us();
    printf("Total execution time: %u us\n", time4 - time0);
    
    printf("==================== Fitting result: \n");
    printf("Number of iteraciton: %d, SSR = %f\n", iter, SSR);
    for (int i = 0; i < P; ++i) {
    printf("par[%d] = %12.6f (%.6f)\n", i, params[i], error[i] );
    }
    printf("######################################## end of LMA\n");

    // Cleanup
    cudaFree(d_y); cudaFree(d_res); cudaFree(d_J);
    cudaFree(d_JTJ); cudaFree(d_JTr);
    delete[] y;

    return 0;
}
