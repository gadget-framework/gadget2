##########################################################################
# Common for all architecture and compiler options
##########################################################################
GCCWARNINGS = -Wimplicit -Wreturn-type -Wswitch -Wcomment -Wformat=2 \
              -Wparentheses -Wpointer-arith -Wcast-qual -Wconversion \
              -Wreorder -Wwrite-strings -Wsynth -Wchar-subscripts \
              -Wuninitialized -W

#DEFINE_FLAGS = -D GADGET_INLINE -D DEBUG -D INTERRUPT_HANDLER -g -O
DEFINE_FLAGS = -D GADGET_INLINE -D NDEBUG -D INTERRUPT_HANDLER -O3

##########################################################################
# Pick the appropriate compiler from the following switches
##########################################################################
# 1. Linux or Solaris, without pvm, g++ compiler
CXX = g++
LIBDIRS = -L.  -L/usr/local/lib
LIBRARIES = -lm -lvec
CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS)
OBJECTS = $(GADGETOBJECTS)
##########################################################################
# 2. Linux, with pvm3, g++ compiler
#CXX = g++
#PVMDIR = $(PVM_ROOT)
#PVMINCLUDE = $(PVMDIR)/include
#PVMLIB = $(PVMDIR)/lib/$(PVM_ARCH)
#LIBDIRS = -L.  -L/usr/local/lib  -L$(PVMLIB)
#LIBRARIES = -lm -lvec -lsocket -lpvm3 -lgpvm3 -lnsl
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS) -D GADGET_NETWORK -I$(PVMINCLUDE)
#OBJECTS = $(GADGETOBJECTS) $(SLAVEOBJECTS)
##########################################################################
# 3. Solaris, with pvm3, g++ compiler
#CXX = g++
#PVMDIR = $(PVM_ROOT)
#PVMINCLUDE = $(PVMDIR)/include
#PVMLIB = $(PVMDIR)/lib/$(PVM_ARCH)
#LIBDIRS = -L.  -L/usr/local/lib  -L$(PVMLIB)
#LIBRARIES = -lm -lvec -lpvm3 -lgpvm3 -lnsl
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS) -D GADGET_NETWORK -I$(PVMINCLUDE)
#OBJECTS = $(GADGETOBJECTS) $(SLAVEOBJECTS)
##########################################################################
# 4. Solaris, without pvm3, using CC compiler
#CXX = CC
#LIBDIRS = -L.  -L/usr/local/lib
#LIBRARIES = -lm -lvec
#CXXFLAGS = $(DEFINE_FLAGS)
#OBJECTS = $(GADGETOBJECTS)
##########################################################################
# 5. HP-UX, without pvm3
#LIBDIRS = -L.  -L/usr/local/lib
#LIBRARIES = -lm -lvec
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS) -D __cplusplus
#OBJECTS = $(GADGETOBJECTS)
##########################################################################
# 6. Linux or Solaris, without pvm3, g++ compiler running CONDOR
#CXX = condor_compile g++
#LIBDIRS = -L.  -L/usr/local/lib
#LIBRARIES = -lm -lvec
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS)
#OBJECTS = $(GADGETOBJECTS)
##########################################################################
# The following functions are not ansi compatible, and so if the -ansi flag
# is used, they will either fail or return garbage.  These functions are
# the lgamma function used in file mathfunc.h, and
# the strdup and strcasecmp functions might not work correctly.
##########################################################################

VECTORS = addr_keepmatrix.o addr_keepvector.o charptrmatrix.o charptrvector.o \
	bandmatrixptrmatrix.o bandmatrixptrvector.o doublematrix.o doublevector.o \
	otherfoodptrvector.o baseclassptrvector.o popinfoindexvector.o \
	migvariableptrvector.o conversionindexptrvector.o likelihoodptrvector.o \
	popratiomatrix.o popratiovector.o popinfomatrix.o popinfovector.o \
	doubleindexvector.o predatorptrvector.o preyptrvector.o fleetptrvector.o \
	doublematrixptrmatrix.o doublematrixptrvector.o intmatrix.o intvector.o \
	agebandmatrixptrvector.o poppredatorptrvector.o popratioindexvector.o \
	formulaindexvector.o stockptrvector.o formulamatrix.o formulavector.o \
	timevariablematrix.o timevariablevector.o timevariableindexvector.o \
	vectorofcharptr.o suitfuncptrvector.o tagptrvector.o printerptrvector.o \
	formulamatrixptrvector.o variableinfoptrvector.o agebandmatrixratio.o

GADGETOBJECTS = parameter.o predator.o growermemberfunctions.o predatoraggregator.o \
	growthimplement.o predatorindex.o abstrpredstdinfo.o abstrpreystdinfo.o \
	predatorprinter.o abstrpreystdinfobylength.o predatoroveraggregator.o \
	actionattimes.o predatoroverprinter.o addr_keep.o predprey.o agebandm.o \
	predpreystdageprinter.o agebandmatrix.o predpreystdlengthprinter.o grow.o \
	agebandmatrixmemberfunctions.o predpreystdprinter.o area.o predstdinfo.o \
	bandmatrix.o predstdinfobylength.o binarytree.o prey.o logcatchfunction.o \
	preyoveraggregator.o checkconversion.o preyoverprinter.o catchstatistics.o \
	preystdinfo.o preystdinfobylength.o catchdistribution.o print.o runid.o \
	commentstream.o printinfo.o readfunc.o conversion.o readmain.o readmatrix.o \
	ecosystem.o readword.o renewal.o mortpredlength.o mortprey.o mortprinter.o \
	errorhandler.o extravector.o fleet.o sibyageonstep.o fleetpreyaggregator.o \
	sibylengthonstep.o formula.o grower.o growthcalc.o simulation.o hooke.o \
	simann.o sionstep.o initialcond.o spawner.o initialize.o stochasticdata.o \
	lengthpredator.o stockaggregator.o lengthprey.o sibylengthandageonstep.o \
	stockdistribution.o linearpredator.o stockfullprinter.o linearregression.o \
	stockmemberfunctions.o livesonareas.o stockpredator.o stockpredstdinfo.o \
	loglinearregression.o main.o stockprey.o mainfiles.o stockpreyfullprinter.o \
	stockpreystdinfo.o stockpreystdinfobylength.o stockprinter.o lenstock.o \
	stockstdprinter.o maturity.o stomachcontent.o maturitya.o variableinfo.o \
	maturityb.o maturityc.o suitfunc.o maturityd.o suits.o migration.o keeper.o \
	surveyindices.o migrationpenalty.o tagdata.o  stock.o readaggregation.o \
	migvariables.o time.o multinomial.o totalpredator.o naturalm.o optinfo.o \
	transition.o otherfood.o mathfunc.o understocking.o poppredator.o tags.o \
	vectorusingkeeper.o popstatistics.o boundlikelihood.o timevariable.o \
	biomassprinter.o cannibalism.o likelihoodprinter.o formatedprinting.o \
	strstack.o lennaturalm.o catchintons.o agebandmatrixratiomemberfunctions.o \
	formatedstockprinter.o formatedpreyprinter.o formatedchatprinter.o taggrow.o \
	interruptinterface.o interrupthandler.o initialinputfile.o popratio.o

SLAVEOBJECTS = netdata.o slavecommunication.o pvmconstants.o

GADGETINPUT = initialinputfile.o vectorofcharptr.o charptrvector.o \
	commentstream.o parameter.o doubleindexvector.o mathfunc.o \
	intvector.o doublevector.o intmatrix.o doublematrix.o

LDFLAGS = $(CXXFLAGS) $(LIBDIRS) $(LIBRARIES)

CC = $(CXX)
gadget	:	$(OBJECTS) libvec.a
	$(CXX) -o gadget $(OBJECTS) $(LDFLAGS)

libvec.a: $(VECTORS)
	ar r libvec.a $?
install: gadget
	mv gadget ../../bin
##########################################################################
# The following line is needed to create the gadget input library
# for newer versions of paramin.  To create this library, you need
# to type "make libgadgetinput.a" *before* you compile paramin
##########################################################################
libgadgetinput.a: $(GADGETINPUT)
	ar r libgadgetinput.a $?
clean:
	rm -f $(OBJECTS) $(VECTORS) libvec.a libgadgetinput.a
depend:
	$(CXX) -M -MM $(CXXFLAGS) *.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
