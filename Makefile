ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs) -lMinuit
ROOTGLIBS     = $(shell root-config --glibs)

CXX           = gcc -fPIC
LD            = gcc
LDFLAGS       = -g
SOFLAGS       = -shared

CXXFLAGS       = $(ROOTCFLAGS) -fPIC
INCLUDE_FLAGS  = 
LDLIBS         = $(ROOTLIBS)
GLIBS          = $(ROOTGLIBS)

EXE           = norm

INC 	      = norm.h

SRC	      = norm.cxx

OBJS          = norm.o

LIB           = libNORM.so

all: 	      $(LIB)

$(LIB):	      $(INC) $(SRC)
	      @echo "####### Generating dictionary"
	      @rootcint -f normDict.cxx -c -p $(CXXFLAGS) \
	      $(INCLUDE_FLAGS) -I. $(INC) LinkDef.h

	      @echo "####### Building library $(LIB)"
	      @$(CXX) $(SOFLAGS) $(CXXFLAGS) $(ROOTLIBS) $(INCLUDE_FLAGS) -I. $(SRC) \
	      normDict.cxx -o $(LIB) $(ROOTLIBS)
	      
	      @echo  "####### Removing generated dictionary"
	      @rm -f normDict.cxx normDict.h
	      @rm -f *.o

clean:
	      @rm -f $(OBJS) $(EXE) normDict.cxx normDict.h $(LIB)
