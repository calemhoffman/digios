CC=g++
CFLAG= -std=c++17 -O2
# CFLAG= -std=c++17 -g -O0
ROOTFLAG=`root-config --cflags --glibs`

pLIBS = $(shell pkg-config --libs arrow parquet)
pINCLUDE = $(shell pkg-config --cflags arrow parquet)

# all: EventBuilder EventBuilder_Q
all: EventBuilder

# EventBuilder: EventBuilder.cpp BinaryReader.h Hit.h
#	$(CC) $(CFLAG) EventBuilder.cpp -o EventBuilder ${ROOTFLAG}

EventBuilder: EventBuilder_Q.cpp BinaryReader.h Hit.h
	$(CC) $(CFLAG) EventBuilder_Q.cpp -o EventBuilder ${ROOTFLAG}

PP: PP.cpp
	# $(CC) $(CFLAG) PP.cpp -o PP -larrow -lparquet
	$(CC) $(CFLAG) PP.cpp -o PP ${pINCLUDE} ${pLIBS}

clean:
	-rm EventBuilder
