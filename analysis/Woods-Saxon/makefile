CC=g++

all: WSFit WSCal WSSearch WSMakeTree WSRange WSSearch2

WSSearch: WSSearch.cpp RK4.h WS.h
	$(CC) WSSearch.cpp -o WSSearch `root-config --cflags --glibs`

WSSearch2: WSSearch2.cpp RK4.h WS.h
	$(CC) WSSearch2.cpp -o WSSearch2 `root-config --cflags --glibs`

WSFit: WSFit.cpp RK4.h WS.h matrix.h matrix.cpp
	$(CC) WSFit.cpp -o WSFit `root-config --cflags --glibs`

WSCal: WSCal.cpp RK4.h WS.h
	$(CC) WSCal.cpp -o WSCal `root-config --cflags --glibs`

WSMakeTree: WSMakeTree.cpp WS.h RK4.h
	$(CC) WSMakeTree.cpp -o WSMakeTree `root-config --cflags --glibs`

WSRange: WSRange.cpp WS.h RK4.h
	$(CC) WSRange.cpp -o WSRange `root-config --cflags --glibs`

clean: 
	rm -fv WSFit WSCal WSSearch WSMakeTree WSRange WSSearch2
	rm -fv *.pcm *.d *.so
