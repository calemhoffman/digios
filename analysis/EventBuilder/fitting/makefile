

all: testFit cuda_fit

testFit: testFit.cpp Matrix.h FindFit.h
	g++ testFit.cpp -o testFit 

cuda_fit: cuda_fit.cu Matrix.h
	nvcc -lcublas cuda_fit.cu -o cuda_fit

.PHONY: clean
clean:
	-rm testFit cuda_fit
