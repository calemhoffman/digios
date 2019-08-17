CC=g++

all: WSFit WSCal WSSearch WSMakeTree WSRange

WSSearch: WSSearch.cpp RK4.h WS.h
	$(CC) WSSearch.cpp -o WSSearch `root-config --cflags --glibs`

WSFit: WSFit.cpp RK4.h WS.h matrix.h matrix.cpp
	$(CC) WSFit.cpp -o WSFit `root-config --cflags --glibs`

WSCal: WSCal.cpp RK4.h WS.h
	$(CC) WSCal.cpp -o WSCal `root-config --cflags --glibs`

WSMakeTree: WSMakeTree.cpp WS.h RK4.h
	$(CC) WSMakeTree.cpp -o WSMakeTree `root-config --cflags --glibs`

WSRange: WSRange.cpp WS.h RK4.h
	$(CC) WSRange.cpp -o WSRange `root-config --cflags --glibs`

clean: 
	rm -fv WSFit WSCal WSSearch WSMakeTree WSRange
	rm -fv *.pcm *.d *.so
