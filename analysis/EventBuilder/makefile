CC=g++
CFLAG= -std=c++17 -O2 -pthread  -lgsl -lgslcblas -lm
# CFLAG= -std=c++17 -g -O0
ifeq ($(shell uname), Darwin)
CFLAG += -w -I/opt/homebrew/opt/gsl/include -L/opt/homebrew/opt/gsl/lib
endif
ROOTFLAG=`root-config --cflags --glibs`

pLIBS = $(shell pkg-config --libs arrow parquet)
pINCLUDE = $(shell pkg-config --cflags arrow parquet)

#---- mapping header compiled into EventBuilder, e.g.
#       make EventBuilder MAPPING=../working/mapping_alpha.h
#     the stamp file is rewritten only when MAPPING changes, and the old binary removed,
#     so the rebuild does not depend on 1-sec timestamp resolution
MAPPING ?= ../working/mapping.h
ifeq ($(strip $(MAPPING)),)
override MAPPING = ../working/mapping.h
endif
# symlinks resolved, so a symlinked header counts as the same mapping as its target
MAPPING_ABS = $(or $(realpath $(MAPPING)),$(abspath $(MAPPING)))
STAMP = .mapping.stamp
# evaluated when the makefile is read (before make checks any target):
# if MAPPING differs from the stamp, rewrite the stamp and remove the old binary
_MAPPING_CHECK := $(shell echo "$(MAPPING_ABS)" | cmp -s - $(STAMP) || { echo "$(MAPPING_ABS)" > $(STAMP); rm -f EventBuilder; })

all: EventBuilder

EventBuilder: EventBuilder.cpp BinaryReader.h Hit.h $(MAPPING_ABS) $(STAMP)
	$(CC) EventBuilder.cpp -o EventBuilder -DMAPPING_FILE='"$(MAPPING_ABS)"' $(CFLAG) ${ROOTFLAG}

clean:
	-rm EventBuilder $(STAMP)

.PHONY: all clean
