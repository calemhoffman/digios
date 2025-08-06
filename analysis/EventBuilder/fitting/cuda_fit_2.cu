#include <stdio.h>
#include <math.h>
#include <cuda_runtime.h>

#include "timer.h"

#define N 1024     // Number of data points
#define THREADS 256
#define BLOCKS (N + THREADS - 1) / THREADS

float model(float x, float A, float T, float R, float B) {
  return A / (1.0f + expf((x - T) / R)) + B;
}

__global__ void compute_residuals_SSR_and_jacobian(float* x, float* y, float* residuals, float *SSR, float* J, float A, float T, float R, float B) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i >= N) return;

  float xi = x[i];
  float yi = y[i];
  float exp_term = expf((xi - T) / R);
  float denom = (1.0f + exp_term);
  float denom2 = denom * denom;

  float fi = A / denom + B;
  float ri = yi - fi;
  residuals[i] = ri;
  atomicAdd(SSR, ri * ri);

  // Compute partial derivatives (Jacobian)
  J[i * 4 + 0] = -1.0f / denom;                        // ∂f/∂A
  J[i * 4 + 1] = (A * exp_term) / (R * denom2);        // ∂f/∂T
  J[i * 4 + 2] = (A * exp_term * (xi - T)) / (R * R * denom2); // ∂f/∂R
  J[i * 4 + 3] = -1.0f;                                // ∂f/∂B
  
}

// Reduction kernel to compute JᵗJ and Jᵗr
__global__ void compute_JTJ_JTr(float* J, float* residuals, float* JTJ, float* JTr) {
  __shared__ float sJTJ[4 * 4];
  __shared__ float sJTr[4];

  int tid = threadIdx.x;
  for (int i = tid; i < 4 * 4; i += blockDim.x) sJTJ[i] = 0.0f;
  for (int i = tid; i < 4; i += blockDim.x) sJTr[i] = 0.0f;
  __syncthreads();

  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < N) {
      float* Ji = &J[i * 4];
      float ri = residuals[i];
      for (int j = 0; j < 4; ++j) {
          atomicAdd(&sJTr[j], Ji[j] * ri);
          for (int k = 0; k < 4; ++k) {
              atomicAdd(&sJTJ[j * 4 + k], Ji[j] * Ji[k]);
          }
      }
  }
  __syncthreads();

  if (tid < 4 * 4) atomicAdd(&JTJ[tid], sJTJ[tid]);
    if (tid < 4)     atomicAdd(&JTr[tid], sJTr[tid]);
}


// Host-side LMA update (solve (JᵗJ + λI) δ = Jᵗr)
float update_parameters(float* JTJ, float* JTr, float* params, float lambda) {
  // Solve 4x4 linear system using basic Gaussian elimination (naive, for demo)
  float A[4][5]; // 4x4 + rhs
  for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 4; ++j)
          A[i][j] = JTJ[i * 4 + j] + (i == j ? lambda : 0.0f);
  for (int i = 0; i < 4; ++i)
      A[i][4] = JTr[i];

  // Gaussian elimination
  for (int i = 0; i < 4; ++i) {
      float pivot = A[i][i];
      for (int j = 0; j <= 4; ++j)
          A[i][j] /= pivot;
      for (int k = 0; k < 4; ++k) {
          if (k == i) continue;
          float factor = A[k][i];
          for (int j = 0; j <= 4; ++j)
              A[k][j] -= factor * A[i][j];
      }
  }

  // Update parameters
  for (int i = 0; i < 4; ++i) params[i] += A[i][4];

  // calculate SSR
  float SSR = 0.0f;
  for (int i = 0; i < N; ++i) {
      float fi = model(i * 0.01f, params[0], params[1], params[2], params[3]);
      float ri = (i * 0.01f - fi);
      SSR += ri * ri;
  }
  return SSR;
    
}

int main() {
    float *x, *y, *d_x, *d_y, *d_res, *d_J, *d_JTJ, *d_JTr, *d_SSR;
    float params[4] = { 5.0f, 5.0f, 1.0f, 0.5f }; // Initial guess: A, T, R, B

    x = new float[N];
    y = new float[N];

    // Generate synthetic data
    for (int i = 0; i < N; ++i) {
        x[i] = i * 0.01f;
        float true_val = 3.0f / (1.0f + expf((x[i] - 7.0f) / 0.8f)) + 1.0f;
        y[i] = true_val + 0.05f * ((rand() % 1000) / 1000.0f - 0.5f);
    }

    unsigned int time0 = getTime_us();

    // Allocate GPU memory
    cudaMalloc(&d_x, N * sizeof(float));
    cudaMalloc(&d_y, N * sizeof(float));
    cudaMalloc(&d_res, N * sizeof(float));
    cudaMalloc(&d_SSR, sizeof(float));
    cudaMalloc(&d_J, N * 4 * sizeof(float));
    cudaMalloc(&d_JTJ, 16 * sizeof(float));
    cudaMalloc(&d_JTr, 4 * sizeof(float));

    cudaMemcpy(d_x, x, N * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_y, y, N * sizeof(float), cudaMemcpyHostToDevice);

    float lambda = 1e-3f;

    unsigned int time1 = getTime_us();
    printf("Memory allocation and data transfer took %u us\n", time1 - time0 );

    for (int iter = 0; iter < 20; ++iter) {

      unsigned int time2 = getTime_us();

      compute_residuals_SSR_and_jacobian<<<BLOCKS, THREADS>>>(d_x, d_y, d_res, d_SSR, d_J, params[0], params[1], params[2], params[3]);
      compute_JTJ_JTr<<<BLOCKS, THREADS>>>(d_J, d_res, d_JTJ, d_JTr);

      cudaDeviceSynchronize();

      unsigned int time2a = getTime_us();
      printf("Iteration %d: Kernel execution took %u us\n", iter, time2a - time2);

      float JTJ[16], JTr[4], SSR;
      cudaMemcpy(JTJ, d_JTJ, sizeof(float) * 16, cudaMemcpyDeviceToHost);
      cudaMemcpy(JTr, d_JTr, sizeof(float) * 4, cudaMemcpyDeviceToHost);
      cudaMemcpy(&SSR, d_SSR, sizeof(float), cudaMemcpyDeviceToHost);

      unsigned int time2b = getTime_us();
      printf("Iteration %d: Data transfer took %u us\n", iter, time2b - time2a);

      float new_SSR = update_parameters(JTJ, JTr, params, lambda);
      unsigned int time3 = getTime_us();
      printf("Iteration %d: Time for kernel execution and parameter update: %u us\n", iter, time3 - time2);

      printf("Iteration %d: A = %.4f, T = %.4f, R = %.4f, B = %.4f | SSR: %f, new SSR: %f\n",
             iter, params[0], params[1], params[2], params[3], SSR, new_SSR);

    }

    printf("Fitted parameters:\nA = %.4f\nT = %.4f\nR = %.4f\nB = %.4f\n",
        params[0], params[1], params[2], params[3]);

    unsigned int time4 = getTime_us();
    printf("Total execution time: %u us\n", time4 - time0);

    // Cleanup
    cudaFree(d_x); cudaFree(d_y); cudaFree(d_res); cudaFree(d_J);
    cudaFree(d_JTJ); cudaFree(d_JTr);
    delete[] x; delete[] y;

    return 0;
}
