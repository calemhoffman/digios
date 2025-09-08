CC=g++
CFLAG= -std=c++17 -O2 -pthread  -lgsl -lgslcblas -lm
# CFLAG= -std=c++17 -g -O0
ifeq ($(shell uname), Darwin)
CFLAG += -w -I/opt/homebrew/opt/gsl/include -L/opt/homebrew/opt/gsl/lib
endif
ROOTFLAG=`root-config --cflags --glibs`

pLIBS = $(shell pkg-config --libs arrow parquet)
pINCLUDE = $(shell pkg-config --cflags arrow parquet)

all: EventBuilder EventBuilder_Q EventBuilder_S

EventBuilder: EventBuilder.cpp BinaryReader.h Hit.h
	$(CC) $(CFLAG) EventBuilder.cpp -o EventBuilder ${ROOTFLAG}

EventBuilder_Q: EventBuilder_Q.cpp BinaryReader.h Hit.h
	$(CC) $(CFLAG) EventBuilder_Q.cpp -o EventBuilder_Q ${ROOTFLAG}

EventBuilder_S: EventBuilder_S.cpp BinaryReader.h Hit.h ../working/GeneralSortMapping.h
	$(CC) EventBuilder_S.cpp -o EventBuilder_S $(CFLAG) ${ROOTFLAG}

clean:
	-rm EventBuilder EventBuilder_Q EventBuilder_S
