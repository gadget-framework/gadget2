##########################################################################
# Common for all architecture and compiler options
##########################################################################
GCCWARNINGS = -Wimplicit -Wreturn-type -Wswitch -Wcomment -Wformat \
              -Wparentheses -Wpointer-arith -Wcast-qual -Wconversion \
              -Wreorder -Wwrite-strings -Wsynth -Wchar-subscripts \
              -Wuninitialized -pedantic -W

#DEFINE_FLAGS = -D GADGET_INLINE -D DEBUG -D INTERRUPT_HANDLER -g -O
DEFINE_FLAGS = -D GADGET_INLINE -D NDEBUG -D INTERRUPT_HANDLER -O3

##########################################################################
# Pick the appropriate compiler from the following switches
##########################################################################
# 1. Linux, or Solaris, without pvm, g++ compiler
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
#LIBDIRS = -L. -L/usr/local/lib -L$(PVMLIB)
#LIBRARIES = -lm -lvec -lpvm3 -lgpvm3 -lnsl
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS) -D GADGET_NETWORK -I$(PVMINCLUDE)
#OBJECTS = $(GADGETOBJECTS) $(SLAVEOBJECTS)
##########################################################################
# 3. Solaris, with pvm3, g++ compiler
#CXX = g++
#PVMDIR = $(PVM_ROOT)
#PVMINCLUDE = $(PVMDIR)/include
#PVMLIB = $(PVMDIR)/lib/$(PVM_ARCH)
#LIBDIRS = -L. -L/usr/local/lib -L$(PVMLIB)
#LIBRARIES = -lm -lvec -lpvm3 -lgpvm3 -lnsl
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS) -D GADGET_NETWORK -I$(PVMINCLUDE)
#OBJECTS = $(GADGETOBJECTS) $(SLAVEOBJECTS)
##########################################################################
# 4. Solaris, without pvm3, using CC compiler
#CXX = CC
#LIBDIRS = -L. -L/usr/local/lib
#LIBRARIES = -lm -lvec
#CXXFLAGS = $(DEFINE_FLAGS)
#OBJECTS = $(GADGETOBJECTS)
##########################################################################
# 5. HP-UX, without pvm3
#LIBDIRS = -L. -L/usr/local/lib
#LIBRARIES = -lm -lvec
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS) -D __cplusplus
#OBJECTS = $(GADGETOBJECTS)
##########################################################################
# 6. Linux or Solaris, without pvm3, g++ compiler running CONDOR
#CXX = condor_compile g++
#LIBDIRS = -L. -L/usr/local/lib
#LIBRARIES = -lm -lvec
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS)
#OBJECTS = $(GADGETOBJECTS)
##########################################################################
# The following functions are not ansi compatible, and so if the -ansi flag
# is used, they will either fail or return garbage.  These functions are
# the lgamma function used in file mathfunc.h, and
# the strdup and strcasecmp functions might not work correctly.
##########################################################################

VECTORS = addresskeepermatrix.o addresskeepervector.o intmatrix.o intvector.o \
	bandmatrixptrvector.o doublematrix.o doublevector.o parametervector.o \
	otherfoodptrvector.o baseclassptrvector.o popinfoindexvector.o \
	migvariableptrvector.o conversionindexptrvector.o likelihoodptrvector.o \
	popratiomatrix.o popratiovector.o popinfomatrix.o popinfovector.o \
	doubleindexvector.o predatorptrvector.o preyptrvector.o fleetptrvector.o \
	doublematrixptrmatrix.o doublematrixptrvector.o timevariableindexvector.o \
	agebandmatrixptrvector.o poppredatorptrvector.o popratioindexvector.o \
	formulamatrix.o formulavector.o charptrmatrix.o charptrvector.o \
	timevariablevector.o stockptrvector.o agebandmatrixratioptrvector.o \
	vectorofcharptr.o suitfuncptrvector.o tagptrvector.o printerptrvector.o \
	formulamatrixptrvector.o variableinfoptrvector.o agebandmatrixptrmatrix.o

GADGETOBJECTS = gadget.o parameter.o growermemberfunctions.o predatoraggregator.o \
	predatorindex.o abstrpredstdinfo.o abstrpreystdinfo.o catchdistribution.o \
	predatorprinter.o abstrpreystdinfobylength.o predatoroveraggregator.o \
	actionattimes.o predatoroverprinter.o addresskeeper.o predprey.o strstack.o \
	predpreystdageprinter.o agebandmatrix.o predpreystdlengthprinter.o grow.o \
	agebandmatrixmemberfunctions.o predpreystdprinter.o area.o predstdinfo.o \
	bandmatrix.o predstdinfobylength.o prey.o logcatchfunction.o popstatistics.o \
	preyoveraggregator.o lengthgroup.o preyoverprinter.o catchstatistics.o \
	preystdinfo.o preystdinfobylength.o timevariable.o boundlikelihood.o \
	commentstream.o printinfo.o readfunc.o conversionindex.o readmain.o keeper.o \
	ecosystem.o readword.o renewal.o mortpredator.o mortprey.o mortprinter.o \
	errorhandler.o pionstep.o fleet.o sibyageonstep.o fleetpreyaggregator.o \
	sibylengthonstep.o formula.o grower.o growthcalc.o simulation.o hooke.o \
	simann.o sionstep.o initialcond.o spawner.o initialize.o stochasticdata.o \
	lengthpredator.o stockaggregator.o lengthprey.o sibylengthandageonstep.o \
	stockdistribution.o linearpredator.o stockfullprinter.o linearregression.o \
	stockmemberfunctions.o livesonareas.o stockpredator.o stockpredstdinfo.o \
	loglinearregression.o time.o stockprey.o maininfo.o stockpreyfullprinter.o \
	stockpreystdinfo.o stockpreystdinfobylength.o stockprinter.o lenstock.o \
	stockstdprinter.o maturity.o suitfunc.o suits.o stomachcontent.o runid.o \
	migration.o variableinfo.o binarytree.o vectorusingkeeper.o recaggregator.o \
	surveyindices.o migrationpenalty.o recapture.o stock.o readaggregation.o \
	migvariables.o multinomial.o totalpredator.o understocking.o poppredator.o \
	transition.o otherfood.o mathfunc.o recstatistics.o selectfunc.o optinfo.o \
	biomassprinter.o cannibalism.o likelihoodprinter.o formatedprinting.o \
	lennaturalm.o catchintons.o interruptinterface.o interrupthandler.o tags.o \
	formatedstockprinter.o formatedpreyprinter.o formatedchatprinter.o \
	taggrow.o initialinputfile.o popratio.o naturalm.o predator.o popinfo.o \
	agebandmatrixratio.o ludecomposition.o popinfomemberfunctions.o \
	agebandmatrixratiomemberfunctions.o

SLAVEOBJECTS = netdata.o slavecommunication.o pvmconstants.o

GADGETINPUT = initialinputfile.o vectorofcharptr.o charptrvector.o \
	commentstream.o parameter.o parametervector.o doubleindexvector.o \
	intvector.o doublevector.o intmatrix.o doublematrix.o mathfunc.o

LDFLAGS = $(CXXFLAGS) $(LIBDIRS) $(LIBRARIES)

gadget	:	$(OBJECTS) libvec.a
	$(CXX) -o gadget $(OBJECTS) $(LDFLAGS)

libvec.a:	$(VECTORS)
	ar rs libvec.a $?

install	:	gadget
	cp gadget ../../bin

##########################################################################
# The following line is needed to create the gadget input library
# for newer versions of paramin.  To create this library, you need
# to type "make libgadgetinput.a" *before* you compile paramin
##########################################################################
libgadgetinput.a: $(GADGETINPUT)
	ar rs libgadgetinput.a $?

clean	:
	rm -f $(OBJECTS) $(VECTORS) libvec.a libgadgetinput.a

depend	:
	$(CXX) -M -MM $(CXXFLAGS) *.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
