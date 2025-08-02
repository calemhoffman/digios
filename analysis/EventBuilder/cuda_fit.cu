#include <cuda_runtime.h>
#include <curand_kernel.h>
#include <iostream>
#include <vector>
#include <cmath>

// to compile nvcc cuda_fit.cu -o cuda_fit

// CUDA kernel to compute the model: A/(1+exp(-(x-T)/R)) + B
__global__ void computeModel(float* x, float* y_pred, float A, float T, float R, float B, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        y_pred[idx] = A / (1.0f + expf((x[idx] - T) / R)) + B;
    }
}

// CUDA kernel to compute the loss (mean squared error)
__global__ void computeLoss(float* y_true, float* y_pred, float* loss, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        float diff = y_true[idx] - y_pred[idx];
        atomicAdd(loss, diff * diff);
    }
}

// CUDA kernel to compute gradients for A, T, R, B
__global__ void computeGradients(float* x, float* y_true, float* y_pred, float* gradA, float* gradT, float* gradR, float* gradB, float A, float T, float R, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        float exp_term = expf((x[idx] - T) / R);
        float denom = 1.0f + exp_term;
        float diff = y_pred[idx] - y_true[idx];

        // Partial derivatives (chain rule applied)
        float dA = 1.0f / denom;
        float dB = 1.0f;
        float dT = (A * exp_term) / (R * denom * denom);
        float dR = (A * (-T + x[idx]) * exp_term) / (R * R * denom * denom);

        atomicAdd(gradA, 2.0f * diff * dA);
        atomicAdd(gradT, 2.0f * diff * dT);
        atomicAdd(gradR, 2.0f * diff * dR);
        atomicAdd(gradB, 2.0f * diff * dB);
    }
}

// CUDA kernel to compute the diagonal of the Hessian (second derivatives)
__global__ void computeHessianDiag(float* x, float* y_true, float* y_pred, float* hessA, float* hessT, float* hessR, float* hessB, float A, float T, float R, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        float exp_term = expf((x[idx] - T) / R);
        float denom = 1.0f + exp_term;
        float denom2 = denom * denom;
        float diff = y_pred[idx] - y_true[idx];

        // Second derivatives (diagonal only)
        float dA = 1.0f / denom;
        float dT = (A * exp_term) / (R * denom2);
        float dR = (A * (x[idx] - T) * exp_term) / (R * R * denom2);
        float dB = 1.0f;

        atomicAdd(hessA, 2.0f * dA * dA);
        atomicAdd(hessT, 2.0f * dT * dT);
        atomicAdd(hessR, 2.0f * dR * dR);
        atomicAdd(hessB, 2.0f * dB * dB);
    }
}

void computeUncertainties(std::vector<float>& x, std::vector<float>& y, float A, float T, float R, float B, float& sigmaA, float& sigmaT, float& sigmaR, float& sigmaB) {
    int n = x.size();
    float *d_x, *d_y, *d_y_pred, *d_hessA, *d_hessT, *d_hessR, *d_hessB;
    cudaMalloc(&d_x, n * sizeof(float));
    cudaMalloc(&d_y, n * sizeof(float));
    cudaMalloc(&d_y_pred, n * sizeof(float));
    cudaMalloc(&d_hessA, sizeof(float));
    cudaMalloc(&d_hessT, sizeof(float));
    cudaMalloc(&d_hessR, sizeof(float));
    cudaMalloc(&d_hessB, sizeof(float));

    cudaMemcpy(d_x, x.data(), n * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_y, y.data(), n * sizeof(float), cudaMemcpyHostToDevice);

    int threadsPerBlock = 256;
    int blocks = (n + threadsPerBlock - 1) / threadsPerBlock;

    // Compute model predictions
    computeModel<<<blocks, threadsPerBlock>>>(d_x, d_y_pred, A, T, R, B, n);
    cudaDeviceSynchronize();

    // Initialize Hessian diagonals
    cudaMemset(d_hessA, 0, sizeof(float));
    cudaMemset(d_hessT, 0, sizeof(float));
    cudaMemset(d_hessR, 0, sizeof(float));
    cudaMemset(d_hessB, 0, sizeof(float));

    // Compute Hessian diagonal
    computeHessianDiag<<<blocks, threadsPerBlock>>>(d_x, d_y, d_y_pred, d_hessA, d_hessT, d_hessR, d_hessB, A, T, R, n);
    cudaDeviceSynchronize();

    float hessA = 0.0f, hessT = 0.0f, hessR = 0.0f, hessB = 0.0f;
    cudaMemcpy(&hessA, d_hessA, sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(&hessT, d_hessT, sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(&hessR, d_hessR, sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(&hessB, d_hessB, sizeof(float), cudaMemcpyDeviceToHost);

    // Uncertainty estimation: sqrt(1 / Hessian)
    sigmaA = sqrtf(1.0f / hessA);
    sigmaT = sqrtf(1.0f / hessT);
    sigmaR = sqrtf(1.0f / hessR);
    sigmaB = sqrtf(1.0f / hessB);

    cudaFree(d_x);
    cudaFree(d_y);
    cudaFree(d_y_pred);
    cudaFree(d_hessA);
    cudaFree(d_hessT);
    cudaFree(d_hessR);
    cudaFree(d_hessB);
}

void fitCurve(std::vector<float>& x, std::vector<float>& y, float& A, float& T, float& R, float& B, float initial_lr = 0.2f) {
    int n = x.size();

    // Device memory
    float *d_x, *d_y, *d_y_pred, *d_loss, *d_gradA, *d_gradT, *d_gradR, *d_gradB;
    cudaMalloc(&d_x, n * sizeof(float));
    cudaMalloc(&d_y, n * sizeof(float));
    cudaMalloc(&d_y_pred, n * sizeof(float));
    cudaMalloc(&d_loss, sizeof(float));
    cudaMalloc(&d_gradA, sizeof(float));
    cudaMalloc(&d_gradT, sizeof(float));
    cudaMalloc(&d_gradR, sizeof(float));
    cudaMalloc(&d_gradB, sizeof(float));

    // Copy input data to device
    cudaMemcpy(d_x, x.data(), n * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_y, y.data(), n * sizeof(float), cudaMemcpyHostToDevice);

    // CUDA grid configuration
    int threadsPerBlock = 256;
    int blocks = (n + threadsPerBlock - 1) / threadsPerBlock;

    // Levenberg-Marquardt parameters
    float lambda = 1.0f; // damping factor
    float lambda_up = 5.0f;
    float lambda_down = 0.5f;
    float learning_rate = initial_lr;

    int nIterations = 0;
    double tolerance = 1e-3;
    float loss = 0.0f, old_loss = 0.0;
    float tolerance_check = 0.0f;
    
    do {
        // Initialize loss and gradients
        float gradA = 0.0f, gradT = 0.0f, gradR = 0.0f, gradB = 0.0f;
        cudaMemset(d_loss, 0, sizeof(float));
        cudaMemset(d_gradA, 0, sizeof(float));
        cudaMemset(d_gradT, 0, sizeof(float));
        cudaMemset(d_gradR, 0, sizeof(float));
        cudaMemset(d_gradB, 0, sizeof(float));

        // Compute model predictions
        computeModel<<<blocks, threadsPerBlock>>>(d_x, d_y_pred, A, T, R, B, n);
        cudaDeviceSynchronize();

        // Compute loss
        computeLoss<<<blocks, threadsPerBlock>>>(d_y, d_y_pred, d_loss, n);
        cudaDeviceSynchronize();
        cudaMemcpy(&loss, d_loss, sizeof(float), cudaMemcpyDeviceToHost);
        loss /= n;

        // Compute gradients
        computeGradients<<<blocks, threadsPerBlock>>>(d_x, d_y, d_y_pred, d_gradA, d_gradT, d_gradR, d_gradB, A, T, R, n);
        cudaDeviceSynchronize();
        cudaMemcpy(&gradA, d_gradA, sizeof(float), cudaMemcpyDeviceToHost);
        cudaMemcpy(&gradT, d_gradT, sizeof(float), cudaMemcpyDeviceToHost);
        cudaMemcpy(&gradR, d_gradR, sizeof(float), cudaMemcpyDeviceToHost);
        cudaMemcpy(&gradB, d_gradB, sizeof(float), cudaMemcpyDeviceToHost);

        // Save current parameters
        float A_old = A, T_old = T, R_old = R, B_old = B;

        // LM update: add damping to gradient step
        A -= learning_rate * gradA / (n + lambda);
        T -= learning_rate * gradT / (n + lambda);
        R -= learning_rate * gradR / (n + lambda);
        B -= learning_rate * gradB / (n + lambda);

        // Recompute loss with new parameters
        cudaMemset(d_loss, 0, sizeof(float));
        computeModel<<<blocks, threadsPerBlock>>>(d_x, d_y_pred, A, T, R, B, n);
        cudaDeviceSynchronize();
        computeLoss<<<blocks, threadsPerBlock>>>(d_y, d_y_pred, d_loss, n);
        cudaDeviceSynchronize();
        float new_loss = 0.0f;
        cudaMemcpy(&new_loss, d_loss, sizeof(float), cudaMemcpyDeviceToHost);
        new_loss /= n;

        // Adaptive lambda adjustment
        if (new_loss < loss) {
            // Accept update, decrease lambda
            lambda *= lambda_down;
            old_loss = new_loss;
        } else {
            // Reject update, increase lambda and revert parameters
            lambda *= lambda_up;
            A = A_old;
            T = T_old;
            R = R_old;
            B = B_old;
            continue;
        }

        tolerance_check = fabs(loss - old_loss)/ fabs(old_loss);

        if (nIterations % 10 == 0){
            printf(" %f, %f, %f|%f| A: %f, T: %f, R: %f, B: %f, lambda: %f\n", old_loss, loss, new_loss, tolerance_check, A, T, R, B, lambda);
        }

        if (nIterations > 50 && tolerance_check < tolerance) {
            printf("Convergence reached. num of iteration : %d | loss tolerance %f\n", nIterations, tolerance_check);
            break;
        }

    } while ( nIterations++ < 10000);

    //if not converged, print warning
    if (nIterations >= 10000) {
        printf("Warning: Maximum iterations reached without convergence. tolerance %f\n", tolerance_check);
    }

    // Clean up
    cudaFree(d_x);
    cudaFree(d_y);
    cudaFree(d_y_pred);
    cudaFree(d_loss);
    cudaFree(d_gradA);
    cudaFree(d_gradT);
    cudaFree(d_gradR);
    cudaFree(d_gradB);
}

int main() {

    // Data points
    std::vector<float> y = {
        15004, 15000, 14997, 14994, 14994, 14995, 14994, 14992, 14988, 14983, 14987, 14993, 14990, 14989, 14989, 14991, 14993, 15000, 15001, 15000, 15001, 15004, 14999, 14996, 14991, 14992, 14986, 14983, 14984, 14980, 14979, 14979, 14981, 14984, 14984, 14986, 14986, 14987, 14988, 14981, 14980, 14980, 14976, 14976, 14978, 14969, 14964, 14956, 14954, 14950, 14951, 14947, 14951, 14956, 14952, 14954, 14959, 14958, 14954, 14950, 14954, 14958, 14953, 14945, 14942, 14942, 14938, 14937, 14937, 14935, 14935, 14927, 14927, 14931, 14926, 14926, 14926, 14930, 14930, 14925, 14921, 14916, 14912, 14907, 14904, 14902, 14897, 14887, 14873, 14865, 14841, 14827, 14811, 14785, 14764, 14744, 14723, 14702, 14682, 14682, 14645, 14621, 14600, 14578, 14556, 14539, 14509, 14494, 14476, 14461, 14440, 14424, 14409, 14395, 14383, 14372, 14363, 14361, 14353, 14348, 14340, 14335, 14332, 14325, 14321, 14314, 14305, 14305, 14301, 14301, 14297, 14297, 14293, 14292, 14289, 14282, 14279, 14268, 14260, 14255, 14254, 14251, 14249, 14242, 14236, 14234, 14227, 14228, 14223, 14220, 14221, 14216, 14210, 14206, 14203, 14196, 14193, 14186, 14185, 14184, 14181, 14179, 14174, 14169, 14164, 14157, 14158, 14148, 14141, 14139, 14137, 14136, 14131, 14127, 14122, 14119, 14116, 14109, 14104, 14103, 14100, 14088, 14080, 14074, 14071, 14066, 14064, 14058, 14059, 14062, 14058, 14052, 14050, 14046, 14040, 14035, 14033, 14026, 14022, 14012, 14006, 14001, 14004, 13997, 13997, 13996, 13996, 13996, 13985, 13980, 13971, 13967, 13959, 13953, 13952, 13952, 13946, 13940, 13936, 13930, 13925, 13915, 13914, 13908, 13907, 13902, 13896, 13895, 13893, 13891, 13889, 13886, 13876, 13876, 13872, 13867, 13857, 13853, 13853, 13845, 13843, 13843, 13843, 13827, 13819, 13816, 13817, 13810, 13810, 13801, 13792, 13786, 13780, 13776, 13768, 13761, 13754, 13749, 13743, 13737, 13729, 13724, 13720, 13720, 13716, 13715, 13716, 13709, 13707, 13706, 13704, 13702, 13696, 13687, 13686, 13677, 13673, 13669, 13665, 13663, 13661, 13660, 13659, 13658, 13652, 13647, 13644, 13641, 13636, 13640, 13636, 13635, 13635, 13634, 13627, 13622, 13617, 13606, 13605, 13594, 13586, 13584, 13583, 13578, 13574, 13568, 13565, 13563, 13557, 13557, 13551, 13541, 13539, 13530, 13521, 13508, 13497, 13490, 13480, 13471, 13462, 13458, 13454, 13448, 13443, 13439, 13438, 13431, 13431, 13426, 13420, 13414, 13410, 13409, 13408, 13397, 13389, 13390, 13390, 13390, 13392, 13391, 13392, 13387, 13387, 13382, 13378, 13375, 13374, 13375, 13369, 13367, 13368, 13364, 13367, 13361, 13359, 13353, 13347, 13339, 13338, 13332, 13327, 13323, 13318, 13313, 13309, 13304, 13305, 13302, 13304, 13298, 13296, 13289, 13277, 13273, 13265, 13254, 13244, 13238, 13237, 13232, 13225, 13219, 13220, 13214, 13202, 13196, 13190, 13188, 13182, 13175, 13168, 13164, 13163, 13152, 13146, 13142, 13136, 13133, 13126, 13119, 13115, 13105, 13104, 13098, 13097, 13092, 13086, 13078, 13070, 13062, 13053, 13038, 13032, 13028, 13024, 13022, 13020, 13019, 13019, 13015, 13019, 13019, 13020, 13025, 13027, 13031, 13032, 13032, 13030, 13028, 13025, 13026, 13021, 13014, 13012, 13006, 13004, 12996, 12993, 12992, 12991, 12989, 12993, 12997, 13004, 13003, 13009, 13010, 13007, 13007, 13005, 13005, 13000, 13000, 13000, 12994, 12995, 12996, 12996, 12999, 13001, 12997, 12995, 12995, 12994, 12998, 13000, 12999, 13000, 13001, 12999, 12997, 12995, 12996, 12992, 12983, 12974, 12972, 12978, 12976, 12977, 12980, 12985, 12984, 12977, 12974, 12979, 12978, 12980, 12973, 12977, 12974, 12969, 12970, 12973, 12970, 12961, 12963, 12965, 12963, 12957, 12961, 12958, 12961, 12959, 12963, 12962, 12968, 12963, 12965, 12962, 12963, 12963, 12965, 12966, 12966, 12971, 12971, 12969, 12968, 12970, 12968, 12962, 12961, 12955, 12954, 12948, 12948, 12951, 12953, 12958, 12954, 12954, 12953, 12958, 12958, 12954, 12955, 12952, 12947, 12949, 12948, 12947, 12949, 12947, 12949, 12949, 12947, 12947, 12942, 12940, 12938, 12940, 12940, 12938, 12938, 12939, 12937, 12934, 12933, 12936, 12937, 12933, 12933, 12933, 12929, 12925, 12924, 12922, 12927, 12930, 12935, 12935, 12934, 12935, 12930, 12932, 12927, 12927, 12926, 12927, 12929, 12930, 12934, 12936, 12936, 12934, 12932, 12931, 12926, 12926, 12933, 12928, 12923, 12924, 12921, 12918, 12915, 12913, 12908, 12909, 12909, 12911, 12917, 12920, 12922, 12922, 12920, 12915, 12919, 12919, 12919, 12920, 12920, 12922, 12918, 12918, 12919, 12916, 12918, 12912, 12909, 12917, 12911, 12908, 12911, 12914, 12920, 12920, 12916, 12914, 12917, 12914, 12909, 12908, 12907, 12911, 12913, 12913, 12914, 12912, 12909, 12908, 12905, 12900, 12904, 12908, 12913, 12916, 12918, 12918, 12924, 12930, 12932, 12927, 12930, 12928, 12921, 12910, 12910, 12911, 12909, 12911, 12908, 12916, 12916, 12914, 12919, 12920, 12927, 12926, 12928, 12931, 12928, 12925, 12928, 12923, 12923, 12919, 12917, 12916, 12916, 12918, 12916, 12913, 12916, 12911, 12919, 12922, 12920, 12926, 12928, 12930, 12930, 12935, 12933, 12934, 12930, 12932, 12932, 12926, 12922, 12922, 12921, 12922, 12918, 12914, 12914, 12913, 12916, 12915, 12915, 12919, 12915, 12914, 12912, 12912, 12907, 12905, 12902, 12901, 12898, 12904, 12901, 12902, 12901, 12908, 12909, 12907, 12910, 12912, 12913, 12911, 12910, 12911, 12912, 12910, 12916, 12913, 12914, 12914, 12915, 12914, 12917, 12915, 12913, 12911, 12907, 12902, 12907, 12905, 12909, 12907, 12905, 12904, 12904, 12902, 12905, 12903, 12907, 12910, 12914, 12911, 12912, 12910, 12909, 12910, 12916, 12916, 12919, 12921, 12923, 12925, 12922, 12920, 12919, 12916, 12913, 12914, 12922, 12925, 12924, 12922, 12923, 12916, 12914, 12909, 12906, 12902, 12908, 12907, 12906, 12907, 12912, 12912, 12915, 12918, 12920, 12924, 12922, 12920, 12922, 12914, 12913, 12909, 12911, 12914, 12914, 12910, 12907, 12907, 12907, 12909, 12913, 12911, 12909, 12905, 12908, 12907, 12907, 12908, 12906, 12902, 12903, 12901, 12898, 12896, 12894, 12895, 12896, 12899, 12894, 12892, 12891, 12885, 12889, 12897, 12896, 12901, 12902, 12901, 12899, 12902, 12900, 12901, 12899, 12901, 12899, 12895, 12894, 12897, 12898, 12901, 12904, 12908, 12910, 12905, 12907, 12903, 12898, 12901, 12904, 12906, 12909, 12911, 12909, 12908, 12913, 12913, 12913, 12913, 12913, 12913, 12915, 12912, 12910, 12909, 12901, 12900, 12905, 12906, 12909, 12912, 12917, 12922, 12924, 12927, 12918, 12912, 12911, 12906, 12904, 12903, 12902, 12904, 12905, 12909, 12909, 12910, 12915, 12910, 12910, 12910, 12910
    };

    std::vector<float> x;
    for( int i = 0; i < y.size(); i++ ) x.push_back(i);

    // Initial parameter guesses
    float A = 3000.0, T = 160, R = 100.0, B = 12000;

    fitCurve(x, y, A, T, R, B);

    printf("Final parameters: (A, T, R, B) = %f, %f, %f, %f\n", A, T, R, B);
    // Compute uncertainties
    float sigmaA, sigmaT, sigmaR, sigmaB;
    computeUncertainties(x, y, A, T, R, B, sigmaA, sigmaT, sigmaR, sigmaB);
    printf("Uncertainties: (sigmaA, sigmaT, sigmaR, sigmaB) = %f, %f, %f, %f\n", sigmaA, sigmaT, sigmaR, sigmaB);

    return 0;
}