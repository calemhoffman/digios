CC=g++
CFLAG= -std=c++17 -O2 
#CFLAG= -std=c++17 -O2 -I/opt/homebrew/opt/gsl/include -L/opt/homebrew/opt/gsl/lib -lgsl -lgslcblas -lm
# CFLAG= -std=c++17 -g -O0
ifeq ($(shell uname), Darwin)
CFLAG += -w
endif
ROOTFLAG=`root-config --cflags --glibs`

pLIBS = $(shell pkg-config --libs arrow parquet)
pINCLUDE = $(shell pkg-config --cflags arrow parquet)

all: EventBuilder EventBuilder_Q

EventBuilder: EventBuilder_S.cpp BinaryReader.h Hit.h
	$(CC) $(CFLAG) EventBuilder_S.cpp -o EventBuilder ${ROOTFLAG}

EventBuilder_Q: EventBuilder_Q.cpp BinaryReader.h Hit.h
	$(CC) $(CFLAG) EventBuilder_Q.cpp -o EventBuilder_Q ${ROOTFLAG}

PP: PP.cpp
	# $(CC) $(CFLAG) PP.cpp -o PP -larrow -lparquet
	$(CC) $(CFLAG) PP.cpp -o PP ${pINCLUDE} ${pLIBS}

clean:
	-rm EventBuilder
