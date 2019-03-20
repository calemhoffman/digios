CC=g++

all: WSFit2 WSFit WSCal WSSearch

WSSearch: WSSearch.cpp RK4.h WS.h
	$(CC) WSSearch.cpp -o WSSearch `root-config --cflags --glibs`

WSFit2: WSFit2.cpp RK4.h WS.h
	$(CC) WSFit2.cpp -o WSFit2 `root-config --cflags --glibs`

WSFit: WSFit.cpp RK4.h WS.h
	$(CC) WSFit.cpp -o WSFit `root-config --cflags --glibs`

WSCal: WSCal.cpp RK4.h WS.h
	$(CC) WSCal.cpp -o WSCal `root-config --cflags --glibs`
