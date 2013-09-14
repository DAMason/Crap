# Original Author:  Dongwook Jang
# $Id: Makefile,v 1.4 2013/04/01 09:53:24 yiiyama Exp $

# Starting with the bits from the SusyNtuple Makefile, but just modifying the targets...

#--------------------------------------------------
# Change file names for your need
#--------------------------------------------------
#TARGET  = CrapEXE

ROOTSYS  =/cern/root

SUSYPHODIR	= ../SUSYPhotonAnalysis/SusyNtuplizer/src
SUSYEVENT	= $(SUSYPHODIR)/SusyEvent.cc

DICTS		= Event_dict


GLOBALINC	= -I. -I$(shell $(ROOTSYS)/bin/root-config --incdir) -I $(SUSYPHODIR)
GLOBALLIBS	= $(shell $(ROOTSYS)/bin/root-config --libs) -Llib

CRAPSRCS	= CounterClass.cpp DiObjectPlotClass.cpp PhoHistClass.cpp
PHOSRCS		= PhoAnalysis/CrapPhoAnalysis.cc $(CRAPSRCS) $(SUSYEVENT)
PHOOBJ		= $(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(PHOSRCS))) 
PHOINC		= -I PhoAnalysis

LLSRCS		= LLAnalysis/CrapLLAnalysis.cc $(CRAPSRCS) $(SUSYEVENT)
LLOBJ		= $(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(LLSRCS)))
LLINC		= -I LLAnalysis

MAINSRC		= CrapMain.cc

#---------------------------------------------------
# compiler dependent flags
#---------------------------------------------------
ARCH         := $(shell $(ROOTSYS)/bin/root-config --arch)
#
ifeq ($(ARCH),linuxx8664gcc)
# Linux with egcs, gcc 2.9x, gcc 3.x (>= RedHat 5.2)
CXX           = g++
CXXFLAGS      = -O2 -Wall -fPIC
LD            = g++
LDFLAGS       = -O2
SOFLAGS       = -shared
endif

ifeq ($(ARCH),macosx64)
# Linux with egcs, gcc 2.9x, gcc 3.x (>= RedHat 5.2)
CXX           = clang++
CXXFLAGS      = -O2 -Wall -fPIC
LD            = clang++
LDFLAGS       = -O2
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linux)
# Linux with egcs, gcc 2.9x, gcc 3.x (>= RedHat 5.2)
CXX           = g++
CXXFLAGS      = -O2 -Wall -fPIC
LD            = g++
LDFLAGS       = -O2
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxkcc)
# Linux with the KAI compiler
CXX           = KCC --one_instantiation_per_object
CXXFLAGS      = -fPIC +K0 --backend -w --no_exceptions
LD            = KCC
LDFLAGS       = -O $(shell root-config --cflags)
SOFLAGS       =
endif

ifeq ($(ARCH),linuxicc)
# Linux with Intel icc compiler
CXX           = icc
CXXFLAGS      = -O
LD            = icc
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),win32)
# Windows with the VC++ compiler
CXX           = cl
CXXOPT        = -O2
#CXXOPT        = -Z7
CXXFLAGS      = $(CXXOPT) -G5 -GR -GX -MD -DWIN32 -D_WINDOWS -nologo \
                -DVISUAL_CPLUSPLUS -D_X86_=1 -D_DLL
LD            = link
LDOPT         = -opt:ref
#LDOPT         = -debug
LDFLAGS       = $(LDOPT) -pdb:none -nologo
SOFLAGS       = -DLL

LIBS  = $(shell root-config --nonew --libs)
endif

ifeq ($(ARCH),win32gcc)
# Windows with gcc
CXX           = g++
CXXFLAGS      = -O2 -march=pentium4 -mfpmath=sse -Wall -Woverloaded-virtual -I/usr/X11R6/include
LD            = g++
LDFLAGS       = -O2 -Wl,--enable-auto-import
SOFLAGS       = -shared -D_DLL -Wl,--export-all-symbols
endif


#-------------------------------------------
# Rules
#-------------------------------------------



all:  CrapExe

CrapExe: libPhoAnalysis.so libLLAnalysis.so CrapMain.o
	@@echo "Making CrapExe"
	@@$(CXX) $(CXXFLAGS) -o CrapExe $(GLOBALLIBS) -l CrapPhoAnalysis -l CrapLLAnalysis CrapMain.o
  
CrapLLAnalysis: libLLAnalysis.so

CrapPhoAnalysis: libPhoAnalysis.so

libPhoAnalysis.so: $(PHOOBJ) $(DICTS)
	@@echo "Making libPhoAnalysis.so..."
	@@$(CXX) $(CXXFLAGS) $(SOFLAGS) -o lib/libCrapPhoAnalysis.so $(GLOBALINC) $(PHOINC) $(GLOBALLIBS) $(PHOOBJ) $(DICTS).cc
	
libLLAnalysis.so: $(LLOBJ) $(DICTS)
	@@echo "Making libLLAnalysis.so..."
	@@$(CXX) $(CXXFLAGS) $(SOFLAGS) -o lib/libCrapLLAnalysis.so $(GLOBALINC) $(LLINC) $(GLOBALLIBS) $(LLOBJ) $(DICTS).cc
	
		
%.o: %.cc
	echo "Making $@..."
	@@$(CXX) $(CXXFLAGS) $(GLOBALINC) -c $^ -o $@
	
%.o: %.cpp
	echo "Making $@..."
	@@$(CXX) $(CXXFLAGS) $(GLOBALINC) -c $^ -o $@
	
$(DICTS):
	@@echo "Generating Dictionary..."
	@@$(ROOTSYS)/bin/rootcint -f $(DICTS).cc -c $(SUSYPHODIR)/SusyEvent.h $(SUSYPHODIR)/SusyNtuplizer_LinkDef.h


Debug: 	
	@@echo $(GLOBALLIBS) 

clean:
	@@/bin/rm -f $(TARGET) $(DICTS).* *_cc.d *_cc.so *~
