#include <cuda_runtime.h>
#include <curand_kernel.h>
#include <iostream>
#include <vector>
#include <cmath>

#include "Matrix.h" // Simple matrix class for small matrices
#include "timer.h"

// to compile nvcc cuda_fit.cu -o cuda_fit -lcublas


#include <cublas_v2.h>


__global__ void computeYf(float* dx, float *dY, float* dYf, const float *para, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    float A = para[0];
    float T = para[1];
    float R = para[2];
    float B = para[3];
    if (idx < n) {
        // Load parameters from device memory
        float model = A / (1.0f + expf((dx[idx] - T) / R)) + B;
        dYf[idx] = dY[idx] - model; // Y - f(p_0)
    }
}

__global__ void computeJacobian(float* dx, float* dJ, const float * para, int n, int p) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    float A = para[0];
    float T = para[1];
    float R = para[2];
    if (idx < n) {
        float exp_term = expf((dx[idx] - T) / R);
        float denom = 1.0f + exp_term;
        // Fill the Jacobian matrix J
        if( exp_term < 1e+20 ) {
            dJ[idx * p + 0] = 1.0f / denom; // dA
            dJ[idx * p + 1] = A * exp_term / (R * denom * denom); // dT
            dJ[idx * p + 2] = (A * (dx[idx] - T) * exp_term) / (R * R * denom * denom); // dR
        }else{
            dJ[idx * p + 0] = 0.0f; // dA
            dJ[idx * p + 1] = 0.0f; // dT
            dJ[idx * p + 2] = 0.0f; // dR
        }
        dJ[idx * p + 3] = 1.0f; // dB
    }
}


void NonLinearRegression(double tolerance = 1e-5, int max_iter = 10000, double lambdaUp = 10, double lambdaDown = 10){
    
    // Simulated data
    int n = 500;
    std::vector<float> x(n);
    std::vector<float> y(n);
    std::vector<float> para = {12.0f, 100.0f, 5.0f, 7.0f}; // Initial guess for A, T, R, B
    int p = para.size();
    
    const int dF = n - p; // degrees of freedom
    
    float maxNoise = 0.0f; // Maximum noise level
    for( int i = 0; i < n; i++ ) {
        x[i] = i;
        y[i] = 10.0f / (1 + expf((i - 200.0f) / 3.0f)) + 8.0f + (rand() % 100) / 100.0 * maxNoise; // Adding some noise
        y[i] = round(y[i] * 1000.0f) / 1000.0f; // Round to 3 decimal places
        // printf("{%.0f, %.3f},", x[i], y[i]);
    }
    // printf("\n");
    
    // cuda memory pointers
    float *dx, *dy;
    cudaMalloc(&dx, n * sizeof(float));
    cudaMalloc(&dy, n * sizeof(float));
    float *dYf, *dJ;
    cudaMalloc(&dYf, n * sizeof(float)); // Yf = Y - f(p_0)
    cudaMalloc(&dJ, n * p * sizeof(float)); // Jacobian matrix
    float * dpara_new;
    cudaMalloc(&dpara_new, p * sizeof(float)); // New parameters
    float *dH;
    cudaMalloc(&dH, p * p * sizeof(float)); // Hessian matrix
    float *dG;
    cudaMalloc(&dG, p * sizeof(float)); // Gradient vector

    cudaMemcpy(dx, x.data(), n * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(dy, y.data(), n * sizeof(float), cudaMemcpyHostToDevice);

    int threadsPerBlock = 1024;
    int blocks = (n + threadsPerBlock - 1) / threadsPerBlock;


    float lambda = 1.0f; // LMA factor
    float SSR = 0.0f;
    int count = 0; // count of iterations

    float new_SSR = 0.0f;

    Matrix H_inv(p, p); // Inverse of Hessian matrix;

    // Compute predicted values
    cudaMemset(dYf, 0, n * sizeof(float)); // Initialize dYf to zero
    computeYf<<<blocks, threadsPerBlock>>>(dx, dy, dYf, para.data(), n);
    cudaDeviceSynchronize(); // ensure kernel execution is complete

    
    // // Compute the SSR (mean squared error) in host
    std::vector<float> Yf(n);
    cudaMemcpy(Yf.data(), dYf, n * sizeof(float), cudaMemcpyDeviceToHost);
    SSR = 0.0f;
    for (int i = 0; i < n; i++){
        SSR += Yf[i] * Yf[i];
        // printf(" %d | %f - %f = %f \n", i, y[i], (para[0] / (1.0f + expf((x[i] - para[1]) / para[2])) + para[3]), Yf[i]);
    }
    printf("Initial SSR = %f\n", SSR);
    

    float deltaSSR = 0.f;

    //^=========================================

    do{

        // print current parameters
        printf("Iteration %d: SSR = %f,  {%f, %f, %f, %f}, Lambda = %.3e\n", count, SSR, para[0], para[1], para[2], para[3], lambda);

        // Compute Jacobian matrix J
        computeJacobian<<<blocks, threadsPerBlock>>>(dx, dJ, para.data(), n, p);
        cudaDeviceSynchronize(); 

        // CUDA matrix multiplication for H = J^T * J
        cublasHandle_t handle;
        cublasCreate(&handle);
        const float alpha = 1.0f; // Scaling factor for the matrix multiplication
        const float beta = 0.0f; 
                
        // dJ is stored as a n x p row-major matrix. CUBLAS expects column-major format,
        // so it sees dJ as a p x n matrix, which is equivalent to J^T.
        // To compute H = J^T * J, we can do (J^T) * (J^T)^T.
        // This corresponds to cublasSgemm with opA = CUBLAS_OP_N and opB = CUBLAS_OP_T.
        cublasSgemm(handle,
            CUBLAS_OP_N, // op(A) = A = J^T
            CUBLAS_OP_T, // op(B) = A^T = (J^T)^T = J
            p,           // m = rows of op(A) and H
            p,           // n = columns of op(B) and H
            n,           // k = inner dimension
            &alpha,
            dJ, p,       // A is p x n, lda = p
            dJ, p,       // B is p x n, ldb = p
            &beta,
            dH, p);      // C is p x p, ldc = p
        
        cublasDestroy(handle);

        // copy dH back to Host, since H is small matrix, 4x4
        std::vector<float> H_matrix(p * p);
        cudaMemcpy(H_matrix.data(), dH, p * p * sizeof(float), cudaMemcpyDeviceToHost);

        // Compute gradient: g = J^T * Yf 
        cublasHandle_t handle3;
        cublasCreate(&handle3);
        // To compute g = J^T * Yf, we can use the fact that dJ is already J^T in column-major format.
        // So we can use cublasSgemv with opA = CUBLAS_OP_N.
        cublasSgemv(handle3, CUBLAS_OP_N, p, n, &alpha, dJ, p, dYf, 1, &beta, dG, 1); // g = J^T * Yf
        cublasDestroy(handle3);

        // copy dG back to Host
        std::vector<float> G_vector(p);
        cudaMemcpy(G_vector.data(), dG, p * sizeof(float), cudaMemcpyDeviceToHost);

        // Convert H and G to Matrix class
        Matrix H(p, p);
        for (int i = 0; i < p; i++) {
            for (int j = 0; j < p; j++) {
                H(i, j) = H_matrix[i * p + j] + (i == j ? lambda : 0.0f); // Add lambda to diagonal elements
            }
        }
        Matrix G(p, 1);
        for (int i = 0; i < p; i++) {
            G(i, 0) = G_vector[i]; 
        }

        // Compute inverse of H using Matrix class
        if (std::isnan(Det(H)) || Det(H) == 0.0) {
            std::cout << "Hessian is singular or NaN, cannot compute inverse." << std::endl;
            break;
        }

        H_inv = Inv(H);

        // Update parameters: p = p - H_inv * G
        Matrix delta = H_inv * G; // delta = H_inv * G

        std::vector<float> para_new = para; // Copy current parameters
        for (int i = 0; i < p; i++) para_new[i] += delta(i, 0); // Update parameters    
        
        // Calculate the new SSR.
        // Compute predicted values
        cudaMemcpy(dpara_new, para_new.data(), p * sizeof(float), cudaMemcpyHostToDevice);

        computeYf<<<blocks, threadsPerBlock>>>(dx, dy, dYf, dpara_new, n);
        cudaDeviceSynchronize(); // ensure kernel execution is complete
        
        // Compute the SSR (mean squared error)
        new_SSR = 0.0f;
        cudaMemcpy(Yf.data(), dYf, n * sizeof(float), cudaMemcpyDeviceToHost);
        for (int i = 0; i < n; i++) new_SSR += Yf[i] * Yf[i];

        // printf("         new SSR = %f, parameters: [%f, %f, %f, %f]\n", new_SSR, para_new[0], para_new[1], para_new[2], para_new[3]);
        deltaSSR = fabs(SSR - new_SSR);

        if(  new_SSR < SSR ) {
            lambda /= lambdaDown; // Increase lambda, leaning towards Newton's method
            para = para_new; // Update parameters
            SSR = new_SSR; // Update SSR
            if( deltaSSR < tolerance ) break;

        } else {
            lambda *= lambdaUp; // Increase lambda
        }

        count++;

        // if( count % 10 == 0) {
            // printf("Iteration %d: SSR = %f, Parameters = [%f, %f, %f, %f], Lambda = %f\n", count, SSR, para[0], para[1], para[2], para[3], lambda);
        // }

    }while( count < 1000 && 1e+12 > lambda && lambda > 1e-12);

    
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
    std::vector<float> error(p, 0.0f);
    for (int i = 0; i < p; ++i)  error[i] = sqrt(var * H_inv(i, i)); // standard error

    // print the final parameters
    printf("==================== Fitting result: \n");
    printf("SSR = %f\n", SSR);
    for (int i = 0; i < p; ++i) {
      printf("par[%d] = %f (%f)\n", i, para[i], error[i] );
    }
    printf("######################################## end of LMA\n");


    cudaFree(dx);
    cudaFree(dy);
    cudaFree(dYf);
    cudaFree(dJ);
    cudaFree(dH);
    cudaFree(dG);
    cudaFree(dpara_new);

    return;

}

int main() {

    NonLinearRegression();

    // // Data points
    // std::vector<float> y = {
    //     15004, 15000, 14997, 14994, 14994, 14995, 14994, 14992, 14988, 14983, 14987, 14993, 14990, 14989, 14989, 14991, 14993, 15000, 15001, 15000, 15001, 15004, 14999, 14996, 14991, 14992, 14986, 14983, 14984, 14980, 14979, 14979, 14981, 14984, 14984, 14986, 14986, 14987, 14988, 14981, 14980, 14980, 14976, 14976, 14978, 14969, 14964, 14956, 14954, 14950, 14951, 14947, 14951, 14956, 14952, 14954, 14959, 14958, 14954, 14950, 14954, 14958, 14953, 14945, 14942, 14942, 14938, 14937, 14937, 14935, 14935, 14927, 14927, 14931, 14926, 14926, 14926, 14930, 14930, 14925, 14921, 14916, 14912, 14907, 14904, 14902, 14897, 14887, 14873, 14865, 14841, 14827, 14811, 14785, 14764, 14744, 14723, 14702, 14682, 14682, 14645, 14621, 14600, 14578, 14556, 14539, 14509, 14494, 14476, 14461, 14440, 14424, 14409, 14395, 14383, 14372, 14363, 14361, 14353, 14348, 14340, 14335, 14332, 14325, 14321, 14314, 14305, 14305, 14301, 14301, 14297, 14297, 14293, 14292, 14289, 14282, 14279, 14268, 14260, 14255, 14254, 14251, 14249, 14242, 14236, 14234, 14227, 14228, 14223, 14220, 14221, 14216, 14210, 14206, 14203, 14196, 14193, 14186, 14185, 14184, 14181, 14179, 14174, 14169, 14164, 14157, 14158, 14148, 14141, 14139, 14137, 14136, 14131, 14127, 14122, 14119, 14116, 14109, 14104, 14103, 14100, 14088, 14080, 14074, 14071, 14066, 14064, 14058, 14059, 14062, 14058, 14052, 14050, 14046, 14040, 14035, 14033, 14026, 14022, 14012, 14006, 14001, 14004, 13997, 13997, 13996, 13996, 13996, 13985, 13980, 13971, 13967, 13959, 13953, 13952, 13952, 13946, 13940, 13936, 13930, 13925, 13915, 13914, 13908, 13907, 13902, 13896, 13895, 13893, 13891, 13889, 13886, 13876, 13876, 13872, 13867, 13857, 13853, 13853, 13845, 13843, 13843, 13843, 13827, 13819, 13816, 13817, 13810, 13810, 13801, 13792, 13786, 13780, 13776, 13768, 13761, 13754, 13749, 13743, 13737, 13729, 13724, 13720, 13720, 13716, 13715, 13716, 13709, 13707, 13706, 13704, 13702, 13696, 13687, 13686, 13677, 13673, 13669, 13665, 13663, 13661, 13660, 13659, 13658, 13652, 13647, 13644, 13641, 13636, 13640, 13636, 13635, 13635, 13634, 13627, 13622, 13617, 13606, 13605, 13594, 13586, 13584, 13583, 13578, 13574, 13568, 13565, 13563, 13557, 13557, 13551, 13541, 13539, 13530, 13521, 13508, 13497, 13490, 13480, 13471, 13462, 13458, 13454, 13448, 13443, 13439, 13438, 13431, 13431, 13426, 13420, 13414, 13410, 13409, 13408, 13397, 13389, 13390, 13390, 13390, 13392, 13391, 13392, 13387, 13387, 13382, 13378, 13375, 13374, 13375, 13369, 13367, 13368, 13364, 13367, 13361, 13359, 13353, 13347, 13339, 13338, 13332, 13327, 13323, 13318, 13313, 13309, 13304, 13305, 13302, 13304, 13298, 13296, 13289, 13277, 13273, 13265, 13254, 13244, 13238, 13237, 13232, 13225, 13219, 13220, 13214, 13202, 13196, 13190, 13188, 13182, 13175, 13168, 13164, 13163, 13152, 13146, 13142, 13136, 13133, 13126, 13119, 13115, 13105, 13104, 13098, 13097, 13092, 13086, 13078, 13070, 13062, 13053, 13038, 13032, 13028, 13024, 13022, 13020, 13019, 13019, 13015, 13019, 13019, 13020, 13025, 13027, 13031, 13032, 13032, 13030, 13028, 13025, 13026, 13021, 13014, 13012, 13006, 13004, 12996, 12993, 12992, 12991, 12989, 12993, 12997, 13004, 13003, 13009, 13010, 13007, 13007, 13005, 13005, 13000, 13000, 13000, 12994, 12995, 12996, 12996, 12999, 13001, 12997, 12995, 12995, 12994, 12998, 13000, 12999, 13000, 13001, 12999, 12997, 12995, 12996, 12992, 12983, 12974, 12972, 12978, 12976, 12977, 12980, 12985, 12984, 12977, 12974, 12979, 12978, 12980, 12973, 12977, 12974, 12969, 12970, 12973, 12970, 12961, 12963, 12965, 12963, 12957, 12961, 12958, 12961, 12959, 12963, 12962, 12968, 12963, 12965, 12962, 12963, 12963, 12965, 12966, 12966, 12971, 12971, 12969, 12968, 12970, 12968, 12962, 12961, 12955, 12954, 12948, 12948, 12951, 12953, 12958, 12954, 12954, 12953, 12958, 12958, 12954, 12955, 12952, 12947, 12949, 12948, 12947, 12949, 12947, 12949, 12949, 12947, 12947, 12942, 12940, 12938, 12940, 12940, 12938, 12938, 12939, 12937, 12934, 12933, 12936, 12937, 12933, 12933, 12933, 12929, 12925, 12924, 12922, 12927, 12930, 12935, 12935, 12934, 12935, 12930, 12932, 12927, 12927, 12926, 12927, 12929, 12930, 12934, 12936, 12936, 12934, 12932, 12931, 12926, 12926, 12933, 12928, 12923, 12924, 12921, 12918, 12915, 12913, 12908, 12909, 12909, 12911, 12917, 12920, 12922, 12922, 12920, 12915, 12919, 12919, 12919, 12920, 12920, 12922, 12918, 12918, 12919, 12916, 12918, 12912, 12909, 12917, 12911, 12908, 12911, 12914, 12920, 12920, 12916, 12914, 12917, 12914, 12909, 12908, 12907, 12911, 12913, 12913, 12914, 12912, 12909, 12908, 12905, 12900, 12904, 12908, 12913, 12916, 12918, 12918, 12924, 12930, 12932, 12927, 12930, 12928, 12921, 12910, 12910, 12911, 12909, 12911, 12908, 12916, 12916, 12914, 12919, 12920, 12927, 12926, 12928, 12931, 12928, 12925, 12928, 12923, 12923, 12919, 12917, 12916, 12916, 12918, 12916, 12913, 12916, 12911, 12919, 12922, 12920, 12926, 12928, 12930, 12930, 12935, 12933, 12934, 12930, 12932, 12932, 12926, 12922, 12922, 12921, 12922, 12918, 12914, 12914, 12913, 12916, 12915, 12915, 12919, 12915, 12914, 12912, 12912, 12907, 12905, 12902, 12901, 12898, 12904, 12901, 12902, 12901, 12908, 12909, 12907, 12910, 12912, 12913, 12911, 12910, 12911, 12912, 12910, 12916, 12913, 12914, 12914, 12915, 12914, 12917, 12915, 12913, 12911, 12907, 12902, 12907, 12905, 12909, 12907, 12905, 12904, 12904, 12902, 12905, 12903, 12907, 12910, 12914, 12911, 12912, 12910, 12909, 12910, 12916, 12916, 12919, 12921, 12923, 12925, 12922, 12920, 12919, 12916, 12913, 12914, 12922, 12925, 12924, 12922, 12923, 12916, 12914, 12909, 12906, 12902, 12908, 12907, 12906, 12907, 12912, 12912, 12915, 12918, 12920, 12924, 12922, 12920, 12922, 12914, 12913, 12909, 12911, 12914, 12914, 12910, 12907, 12907, 12907, 12909, 12913, 12911, 12909, 12905, 12908, 12907, 12907, 12908, 12906, 12902, 12903, 12901, 12898, 12896, 12894, 12895, 12896, 12899, 12894, 12892, 12891, 12885, 12889, 12897, 12896, 12901, 12902, 12901, 12899, 12902, 12900, 12901, 12899, 12901, 12899, 12895, 12894, 12897, 12898, 12901, 12904, 12908, 12910, 12905, 12907, 12903, 12898, 12901, 12904, 12906, 12909, 12911, 12909, 12908, 12913, 12913, 12913, 12913, 12913, 12913, 12915, 12912, 12910, 12909, 12901, 12900, 12905, 12906, 12909, 12912, 12917, 12922, 12924, 12927, 12918, 12912, 12911, 12906, 12904, 12903, 12902, 12904, 12905, 12909, 12909, 12910, 12915, 12910, 12910, 12910, 12910
    // };

    // std::vector<float> x;
    // for( int i = 0; i < y.size(); i++ ) x.push_back(i);

    return 0;
}