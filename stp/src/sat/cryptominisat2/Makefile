TOP = ../../..
include $(TOP)/scripts/Makefile.common

MTL       = mtl
MTRAND    = MTRand
SOURCES   = Logger.cpp Solver.cpp PackedRow.cpp \
	    XorFinder.cpp VarReplacer.cpp \
	    FindUndef.cpp ClauseCleaner.cpp RestartTypeChooser.cpp \
	    FailedVarSearcher.cpp PartFinder.cpp \
	    Subsumer.cpp PartHandler.cpp XorSubsumer.cpp \
	    Gaussian.cpp MatrixFinder.cpp StateSaver.cpp \
	    ClauseAllocator.cpp UselessBinRemover.cpp \
	    OnlyNonLearntBins.cpp
OBJECTS   = $(SOURCES:.cpp=.o)
LIB       = libminisat.a
CFLAGS    += -I../.. -I$(MTL) -I$(MTRAND) -DEXT_HASH_MAP -ffloat-store $(CFLAGS_M32) -c 
EXEC      = minisat
LFLAGS    = -lz

all: $(LIB) #$(EXEC)
lib: $(LIB)

$(LIB): $(OBJECTS)
	rm -f $@
	ar cq $@ $(OBJECTS)
	ranlib $@
	cp $(LIB) ../
	cp $(OBJECTS) ../

clean:
	rm -f $(OBJECTS) $(LIB)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
