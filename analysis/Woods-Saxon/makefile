CC=g++

all: WSCompare WSCal WSSearch WSSearchTree WSRange WSSearch2

WSSearch: WSSearch.cpp RK4.h WS.h
	$(CC) WSSearch.cpp -o WSSearch `root-config --cflags --glibs`

WSSearch2: WSSearch2.cpp RK4.h WS.h
	$(CC) WSSearch2.cpp -o WSSearch2 `root-config --cflags --glibs`

WSCompare: WSCompare.cpp RK4.h WS.h matrix.h matrix.cpp
	$(CC) WSCompare.cpp -o WSCompare `root-config --cflags --glibs`

WSCal: WSCal.cpp RK4.h WS.h
	$(CC) WSCal.cpp -o WSCal `root-config --cflags --glibs`

WSSearchTree: WSSearchTree.cpp WS.h RK4.h
	$(CC) WSSearchTree.cpp -o WSSearchTree `root-config --cflags --glibs`

WSRange: WSRange.cpp WS.h RK4.h
	$(CC) WSRange.cpp -o WSRange `root-config --cflags --glibs`

clean: 
	rm -fv WSCompare WSCal WSSearch WSMakeTree WSRange WSSearch2
	rm -fv *.pcm *.d *.so
