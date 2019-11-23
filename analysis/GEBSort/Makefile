

PLTFMOPT=Makefile.$(shell uname)$(shell getconf LONG_BIT)
include $(PLTFMOPT)

#                  +------- link to where EPICS is on this machine
#                  |        see below
EPICS_ROOT="curEPICS"

# at ANL we use
#rm curEPICS; ln -s /global/devel6_sl62/base/base-3.14.12.1 curEPICS

# at gretina @ MSU we use
#rm curEPICS; ln -s /global/devel/base/R3.14.11 curEPICS

EPICS_LIBSUBDIR=linux-x86
EPICS_INCSUBDIR=Linux

#------------------------------------------ end modifications ---

EPICSLIBDIR=$(EPICS_ROOT)/lib
EPICSARCHLIBDIR=$(EPICSLIBDIR)/$(EPICS_LIBSUBDIR)
EPICSINCDIR=$(EPICS_ROOT)/include
EPICSOSINCDIR=$(EPICS_ROOT)/include/os/$(EPICS_INCSUBDIR)

CFLAGS=-I$(EPICSINCDIR) -I$(EPICSOSINCDIR) -I.

EPICSFLAGS=-L$(EPICSARCHLIBDIR) -lCom -Wl,"-rpath=$(EPICSARCHLIBDIR)"

LIBS=libgretina.

all:		
		make offline
		make G4
		make mkMap
		make GEBMerge
		make online
offline:	GEBSort_nogeb GEBMerge GEBSort_nogeb_trace
online:		GEBSort
G4:		G4toMode2_EU G4toMode2_USa G4toMode2_USb
		make G4toMode2_EU
		make G4toMode2_USa
		make G4toMode2_USb
G4toMode2:	
		make G4toMode2_EU
		make G4toMode2_USa
		make G4toMode2_USb

#-------------------


GEBHeader.c:	GTMerge.h gdecomp.h
GTPrint.c:	GTMerge.h gdecomp.h GEBSort.h
GTUtilsCC.c:	GTMerge.h gdecomp.h

GEBSort:	GTPrintCC.o GEBSort.o time_stampCC.o GEBHeaderCC.o get_a_seedCC.o\
		bin_mode2.o bin_mode1.o bin_mode3.o bin_rcnp.o bin_dgs.o spe_funCC.o \
		libgretina.a bin_gtcal.o
		$(CC) $^ -DHAVE_VXWORKS=1 $(ROOTLIB) $(EPICSFLAGS) -o $@ -lz -lNew
#		go sim_map
#		go sim_root


GEBSort.o:	GEBSort.cxx GTMerge.h GEBSort.h GEBClient.h GEBLink.h gretTapClient.h\
		UserInclude.h UserChat.h UserStat.h UserFunctions.h \
		UserDeclare.h UserInit.h UserRawEv.h UserGoodEv.h \
		UserPreCond.h UserEv.h UserExit.h gdecomp.h \
		permtable1.h permtable2.h permtable3.h permtable4.h \
		permtable5.h permtable6.h permtable7.h permtable8.h 
		$(CC) -DHAVE_VXWORKS=1 $(CCFLAG) $(ROOTINC) $(EPICSFLAGS) $(CFLAGS) -c GEBSort.cxx

GEBSort_nogeb:	GTPrintCC.o GEBSort_nogeb.o time_stampCC.o GEBHeaderCC.o get_a_seedCC.o\
		bin_mode2.o bin_mode1.o bin_mode3.o bin_dgs.o bin_rcnp.o bin_dgs.o \
		spe_funCC.o bin_gtcal.o
		$(CC) $^ -DHAVE_VXWORKS=0 $(ROOTLIB)  -o $@ -lz
#		./GEBSort_nogeb -input disk GTDATA/g4_mode1.out -rootfile GTDATA/test.root RECREATE -chat GEBSort.chat 


GEBSort_nogeb_trace:	GTPrintCC.o GEBSort_nogeb.o time_stampCC.o GEBHeaderCC.o get_a_seedCC.o\
		bin_mode2.o bin_mode1.o bin_mode3.o bin_dgs.o bin_rcnp_trace.o bin_dgs.o \
		spe_funCC.o bin_gtcal.o
		$(CC) $^ -DHAVE_VXWORKS=0 $(ROOTLIB)  -o $@ -lz

get_a_seedCC.o:	get_a_seed.c
		$(CC)  -c $(CCFLAG) -o $@ $^

GEBSort_nogeb.o:	GEBSort.cxx GTMerge.h GEBSort.h \
		UserInclude.h UserChat.h UserStat.h UserFunctions.h \
		UserDeclare.h UserInit.h UserRawEv.h UserGoodEv.h \
		UserPreCond.h UserEv.h UserExit.h gdecomp.h \
		permtable1.h permtable2.h permtable3.h permtable4.h \
		permtable5.h permtable6.h permtable7.h permtable8.h
		$(CC) -DHAVE_VXWORKS=0 $(CCFLAG) $(ROOTINC)  $(CFLAGS) -c GEBSort.cxx -o GEBSort_nogeb.o

time_stampCC.o:	time_stamp.c
		$(CC)  -c $(CCFLAG) -o time_stampCC.o time_stamp.c  

GEBHeaderCC.o:	GEBHeader.c
		$(CC) $(CCFLAG) -c -o $@ $^

GTPrintCC.o:	GTPrint.c
		$(CC) $(CCFLAG) -c -o $@  $^

bin_mode3.o:	bin_mode3.c
		$(CC) $(CCFLAG) -c -o $@  $^

bin_gtcal.o:	bin_gtcal.c
		$(CC) $(CCFLAG) -c -o $@  $^

bin_mode2.o:	bin_mode2.c
		$(CC) $(CCFLAG) -c -o $@  $^

bin_mode1.o:	bin_mode1.c
		$(CC) $(CCFLAG) -c -o $@  $^

bin_rcnp.o:	bin_rcnp.c
		$(CC) $(CCFLAG) $(ROOTINC) -c -o $@  $^

bin_rcnp_trace.o:	bin_rcnp_trace.c
		$(CC) $(CCFLAG) $(ROOTINC) -c -o $@  $^


bin_dgs.o:	bin_dgs.c
		$(CC) $(CCFLAG) -c -o $@  $^

GTPrint.o:	GTPrint.c GTMerge.h
		$(CC) $(CCFLAG) -c $^

time_stamp.o:	time_stamp.c
		$(cc)  -c $(ccFLAG) $^

GTUtilsCC.o:	GTUtils.c
		$(CC) $(CCFLAG) -c $^

libgretina.a:	GEBClient.o gretTapClient.o
		ar cr libgretina.a GEBClient.o gretTapClient.o

GEBClient.o:	GEBClient.c 
		$(CC) $(CCFLAG) $(EPICSFLAGS) $(CFLAGS) -c -o $@  $^

GEBClient.c:	GEBClient.h

gretTapClient.o:	gretTapClient.c 
			$(CC) $(CCFLAG) $(EPICSFLAGS) $(CFLAGS) -c -o $@  $^

gretTapClient.c:	GEBClient.h gretTapClient.h


#-------------------
# trackMain

trackMain:	trackMain.o ctktk.o setupTrack.o track.o \
		writeTrack_repeat.o writeTrack_addtrack.o ctkStats.o \
		ctkTrackOpt.o ctkPrTrkPar.o  \
		ctkinit.o  findVector.o \
		findAngle.o findCAngle.o ctkTrackOpt.o \
		printEvent.o ctksort.o splitCluster.o \
                combineCluster.o matchMaker.o \
		reCluster.o str_decomp.o getEvent.o
		$(cc) $(CCFLAGS) -o $@ $^ -lm

trackMain.o:	trackMain.c ctktk.c ctk.h gdecomp.h
		$(cc) $(CCFLAG) -c $^

ctkTrackOpt.o:	ctkTrackOpt.c
		$(cc) $(CCFLAG) -c $^

ctkPrTrkPar.o:	ctkPrTrkPar.c
		$(cc) $(CCFLAG) -c $^

ctkinit.o:	ctkinit.c
		$(cc) $(CCFLAG) -c $^

findVector.o:	findVector.c
		$(cc) $(CCFLAG) -c $^

findAngle.o:	findAngle.c
		$(cc) $(CCFLAG) -c $^

findCAngle.o:	findCAngle.c
		$(cc) $(CCFLAG) -c $^

ctksort.o:	ctksort.c
		$(cc) $(CCFLAG) -c $^

getEvent.o:	getEvent.c
		$(cc) $(CCFLAG) -c $^

ctktk.o:	ctktk0.c 
		$(cc) $(CCFLAG) -c ctktk.c

ctktk.c:	ctktk0.c ctk.h

reCluster.o:	reCluster.c ctk.h	
		$(cc) $(CCFLAG) -c $^

splitCluster.o: splitCluster.c ctk.h
		$(cc) $(CCFLAG) -c $^

combineCluster.o: combineCluster.c ctk.h
		$(cc) $(CCFLAG) -c $^

str_decomp.o:	str_decomp.c
		$(cc) -c $(CCFLAG) $^

matchMaker.o:	matchMaker.c  ctk.h
		$(cc) $(CCFLAG) -c $^

track.o:	track.c ctk.h
		$(cc) $(CCFLAG) -c $^	

writeTrack_repeat.o:	writeTrack_repeat.c ctk.h
			$(cc) $(CCFLAG) -DWRITETRACK -c $^		

writeTrack_addtrack.o:	writeTrack_addtrack.c ctk.h
			$(cc) $(CCFLAG) -DWRITETRACK -c $^		

readTrack.o:	writeTrack.c ctk.h
		$(cc) $(CCFLAG) -DREADTRACK -o readTrack.o $^		

setupTrack.o:	setupTrack.c ctk.h 
		$(cc) $(CCFLAG) -c $^	

ctkStats.o:	ctkStats.c ctk.h
		$(cc) $(CCFLAG) -c $^

printEvent.o:	printEvent.c ctk.h
		$(cc) $(CCFLAG) -c $^

#-------------------

GEBMerge.c:	GTMerge.h GTMerge_readnew.h pbuf.h

GEBMerge:	GEBMerge.c spe_fun.o \
		get_a_seed.o time_stamp.o\
		spe_fun.o 
		$(cc) $(ccFLAG) -o $@ -lm -lz $^

#-------------------

listTS:		listTS.c gdecomp.h
		$(cc) $(CCFLAGS) -o $@ $^

G4toMode2_EU:	G4toMode2.c spe_fun.o printEvent.o get_a_seed.o str_decomp.o
		$(cc) $(CCFLAGS) -D USGEANTFORMAT=0 -D ASCIIDATA=1 -o $@ $^ -lm

G4toMode2_USa:	G4toMode2.c spe_fun.o printEvent.o get_a_seed.o str_decomp.o
		$(cc) $(CCFLAGS) -D USGEANTFORMAT=1 -D ASCIIDATA=1 -o $@ $^ -lm

G4toMode2_USb:	G4toMode2.c spe_fun.o printEvent.o get_a_seed.o str_decomp.o
		$(cc) $(CCFLAGS) -D USGEANTFORMAT=1 -D ASCIIDATA=0 -o $@ $^ -lm

spe_fun.o:	spe_fun.c
		$(cc) -c  $(ccFLAG) $^

spe_funCC.o:	spe_fun.c
		$(CC) -c -o spe_funCC.o $(CCFLAG) $^

get_a_seed.o:	get_a_seed.c 
		$(cc) -c $(ccFLAG) $^

G4toMode2.c:	gdecomp.h

G4toMode2_old.c:	gdecomp.h

#-------------------

zzip:		zzip.c
		$(cc) $(CCFLAG) -o $@ $^ -lz

zunzip:		zunzip.c
		$(cc) $(CCFLAG) -o $@ $^ -lz

mkMap:		mkMap.c
		$(cc) $(CCFLAG) -o $@ $^ -lz

inv3x3:		inv3x3.c
		$(cc) $(CCFLAG) -o $@ $^ -lm
		$@

#-------------------

format_2_3:	format_2_3.c
		$(cc) $(CCFLAGS) -o $@ $^

format_1_3:	format_1_3.c
		$(cc) $(CCFLAGS) -o $@ $^
#		$@ GTDATA/Global.dat Global_mod.dat

#-------------------

clean:		

		rm -f *.o
		rm -f core.*
		rm -f *%
		rm -f a.out
		rm -f *.gch
		rm -f G4toMode2_EU G4toMode2_US GEBMerge GEBSort 
		rm -f GEBSort_nogeb listTS mkMap trackMain

debug:		gdb -d ./ GEBSort `ls core.*`

