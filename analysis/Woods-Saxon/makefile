CC=g++

#all: WSCompare WSCal WSFit WSFitMC WSSearchTree WSRange WSFit2
all: WSCompare WSCal WSFit WSRange

WSFit: WSFit.cpp RK4.h WS.h
	$(CC) WSFit.cpp -o WSFit `root-config --cflags --glibs`
	
WSFitMC: WSFitMC.cpp RK4.h WS.h
	$(CC) WSFitMC.cpp -o WSFitMC `root-config --cflags --glibs`

WSFit2: WSFit2.cpp RK4.h WS.h
	$(CC) WSFit2.cpp -o WSFit2 `root-config --cflags --glibs`

WSCompare: WSCompare.cpp RK4.h WS.h matrix.h matrix.cpp
	$(CC) WSCompare.cpp -o WSCompare `root-config --cflags --glibs`

WSCal: WSCal.cpp RK4.h WS.h
	$(CC) WSCal.cpp -o WSCal `root-config --cflags --glibs`

WSSearchTree: WSSearchTree.cpp WS.h RK4.h
	$(CC) WSSearchTree.cpp -o WSSearchTree `root-config --cflags --glibs`

WSRange: WSRange.cpp WS.h RK4.h
	$(CC) WSRange.cpp -o WSRange `root-config --cflags --glibs`

clean: 
	rm -fv WSCompare WSCal WSFit WSSearchTree WSRange WSFit2
	rm -fv *.pcm *.d *.so
