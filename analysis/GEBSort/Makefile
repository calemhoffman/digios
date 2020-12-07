

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

all:		GEBMerge EventBuilder EventBuilder_trace
offline:	EventBuilder GEBMerge 

#-------------------

time_stamp.o:	time_stamp.c
		$(cc)  -c $(ccFLAG) $^

GEBMerge.c:	GTMerge.h GTMerge_readnew.h 

GEBMerge:	GEBMerge.c time_stamp.o
		$(cc) $(ccFLAG) -o $@ -lm $^ -lz

#-------------------

GTPrintCC.o:	GTPrint.c
		$(CC) $(CCFLAG) -c -o $@  $^

time_stampCC.o:	time_stamp.c
		$(CC)  -c $(CCFLAG) -o time_stampCC.o time_stamp.c  

GEBHeaderCC.o:	GEBHeader.c
		$(CC) $(CCFLAG) -c -o $@ $^

bin_rcnp.o:	bin_rcnp.c
		$(CC) $(CCFLAG) $(ROOTINC) -c -o $@  $^

GEBSort_nogeb.o:	GEBSort.cxx GTMerge.h GEBSort.h gdecomp.h 
		$(CC) -DHAVE_VXWORKS=0 $(CCFLAG) $(ROOTINC)  $(CFLAGS) -c GEBSort.cxx -o GEBSort_nogeb.o


EventBuilder:	GTPrintCC.o GEBSort_nogeb.o time_stampCC.o GEBHeaderCC.o  bin_rcnp.o
		$(CC) $^ -DHAVE_VXWORKS=0 $(ROOTLIB)  -o $@ -lz
#		./GEBSort_nogeb -input disk GTDATA/g4_mode1.out -rootfile GTDATA/test.root  -chat GEBSort.chat 


#-------------------

bin_rcnp_trace.o:	bin_rcnp_trace.c
		$(CC) $(CCFLAG) $(ROOTINC) -c -o $@  $^

EventBuilder_trace:	GTPrintCC.o GEBSort_nogeb.o time_stampCC.o GEBHeaderCC.o bin_rcnp_trace.o
		$(CC) $^ -DHAVE_VXWORKS=0 $(ROOTLIB)  -o $@ -lz


#-------------------

clean:		

		rm -f *.o
		rm -f core.*
		rm -f *%
		rm -f a.out
		rm -f *.gch
		rm -f EventBuilder EventBuilder_trace GEBMerge

debug:		gdb -d ./ GEBSort `ls core.*`

