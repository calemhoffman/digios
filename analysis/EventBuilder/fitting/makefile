

all: testFit cuda_fit cuda_fit_2

testFit: testFit.cpp Matrix.h FindFit.h
	g++ testFit.cpp -o testFit -lgsl -lgslcblas -lm `root-config --cflags --glibs`

cuda_fit: cuda_fit.cu Matrix.h
	nvcc -lcublas cuda_fit.cu -o cuda_fit

cuda_fit_2: cuda_fit_2.cu 
	nvcc -lcublas cuda_fit_2.cu -o cuda_fit_2

.PHONY: clean
clean:
	-rm testFit cuda_fit
