CC=g++
CFLAG= -std=c++17 -O2
# CFLAG= -std=c++17 -g -O0
ROOTFLAG=`root-config --cflags --glibs`

OBJS = ClassDigitizer.o ClassInfluxDB.o

all: EventBuilder

EventBuilder: EventBuilder.cpp BinaryReader.h Hit.h
	$(CC) $(CFLAG) EventBuilder.cpp -o EventBuilder ${ROOTFLAG}

clean:
	-rm EventBuilder
