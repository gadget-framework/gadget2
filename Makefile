################################################################
# Common for all architecture and compiler options
################################################################
GCCWARNINGS = -Wimplicit -Wreturn-type -Wswitch -Wcomment -Wformat \
              -Wparentheses -Wpointer-arith -Wcast-qual -Wcast-align \
              -Wsynth -Woverloaded-virtual -Wbad-function-cast \
              -Wwrite-strings -Wconversion -Wchar-subscripts \
              -Wuninitialized -W -pedantic

DEFINE_FLAGS = -D INLINE_POPINFO_CC -D INLINE_VECTORS \
               -D NDEBUG -D INTERRUPT_HANDLER -O3

################################################################
# Pick the appropriate compiler from the following switches
################################################################
# 1. Linux without pvm, g++ compiler
CXX = g++
LIBDIRS = -L.  -L/usr/local/lib
LIBRARIES = -lm -lvec
CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS)
OBJECTS = $(GADGETOBJECTS)
################################################################
# 2. Solaris without pvm3, g++ compiler
#CXX = g++
#LIBDIRS = -L.  -L/usr/local/lib
#LIBRARIES = -lm -lvec
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS)
#OBJECTS = $(GADGETOBJECTS)
################################################################
# 3. Solaris without pvm3, using CC compiler
#CXX = CC
#LIBDIRS = -L.  -L/usr/local/lib
#LIBRARIES = -lm -lvec
#CXXFLAGS = $(DEFINE_FLAGS)
#OBJECTS = $(GADGETOBJECTS)
################################################################
# 4. HP-UX without pvm3
#LIBDIRS = -L.  -L/usr/local/lib
#LIBRARIES = -lm -lvec
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS) -D __cplusplus
#OBJECTS = $(GADGETOBJECTS)
################################################################
# 5. Solaris with pvm3, g++ compiler
#CXX = g++
#PVMDIR = $(PVM_ROOT)
#PVMINCLUDE = $(PVMDIR)/include
#PVMLIB = $(PVMDIR)/lib/SUN4SOL2
#LIBDIRS = -L.  -L/usr/local/lib  -L$(PVMLIB)
#LIBRARIES = -lm -lvec -lpvm3 -lgpvm3 -lnsl -lsocket
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS) -D GADGET_NETWORK -I$(PVMINCLUDE)
#OBJECTS = $(GADGETOBJECTS) $(SLAVEOBJECTS)
################################################################
# 6. Linux, with pvm, g++ compiler
#CXX = g++
#PVMDIR = $(PVM_ROOT)
#PVMINCLUDE = $(PVMDIR)/include
#PVMLIB = $(PVMDIR)/lib/LINUX
#LIBDIRS = -L.  -L/usr/local/lib -L$(PVMLIB)
#LIBRARIES = -lm -lvec -lpvm3 -lgpvm3 -lnsl
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS) -D GADGET_NETWORK -I$(PVMINCLUDE)
#OBJECTS = $(GADGETOBJECTS) $(SLAVEOBJECTS)
################################################################
# The following functions are not ansi compatible, and so if the
# -ansi flag is used, they will either fail or return garbage.
# the lgamma function used in file mathfunc.h, and
# the strdup and strcasecmp functions might not work correctly.
################################################################

VECTORS = addr_keepmatrix.o addr_keepvector.o intvector.o intmatrix.o \
	bandmatrixptrmatrix.o likelihoodptrvector.o bandmatrixptrvector.o \
	otherfoodptrvector.o baseclassptrvector.o popinfoindexvector.o \
	catchdataptrvector.o popinfomatrix.o charptrmatrix.o charptrvector.o \
	popinfovector.o conversionindexptrvector.o poppredatorptrvector.o \
	doubleindexvector.o predatorptrvector.o doublematrix.o preyptrvector.o \
	doublematrixptrmatrix.o printerptrvector.o doublematrixptrvector.o \
	sionstepptrvector.o doublevector.o stockpreyptrvector.o fleetptrvector.o \
	formulaindexvector.o stockptrvector.o formulamatrix.o formulavector.o \
	timevariablevector.o timevariablematrix.o timevariableindexvector.o \
	variableinfoptrvector.o catchdistributionptrvector.o suitfuncptrvector.o \
	formulamatrixptrvector.o vectorofcharptr.o agebandmatrixratio.o \
	popratiomatrix.o popratiovector.o popratioindexvector.o \
	agebandmatrixptrvector.o tagptrvector.o

GADGETOBJECTS = parameter.o predator.o growermemberfunctions.o predatoraggregator.o \
	growthimplement.o predatorindex.o abstrpredstdinfo.o abstrpreystdinfo.o \
	predatorprinter.o abstrpreystdinfobylength.o predatoroveraggregator.o \
	actionattimes.o predatoroverprinter.o addr_keep.o predprey.o agebandm.o \
	predpreystdageprinter.o agebandmatrix.o predpreystdlengthprinter.o grow.o \
	agebandmatrixmemberfunctions.o predpreystdprinter.o area.o predstdinfo.o \
	bandmatrix.o predstdinfobylength.o binarytree.o catch.o prey.o catchdata.o \
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
	stockstdprinter.o maturity.o stomachcontent.o maturitya.o strstack.o \
	maturityb.o maturityc.o suitfunc.o maturityd.o suits.o migration.o keeper.o \
	surveyindices.o migrationpenalty.o tagdata.o migvariableptrvector.o stock.o \
	migvariables.o time.o multinomial.o totalpredator.o naturalm.o optinfo.o \
	transition.o otherfood.o mathfunc.o understocking.o poppredator.o tags.o \
	vectorusingkeeper.o popstatistics.o boundlikelihood.o timevariable.o \
	biomassprinter.o cannibalism.o readaggregation.o likelihoodprinter.o \
	formatedcatchprinter.o formatedchatprinter.o formatedpreyprinter.o \
	formatedprinting.o formatedstockprinter.o lennaturalm.o logcatchfunction.o \
	logsionstep.o logsurveyindices.o interruptinterface.o interrupthandler.o \
	initialinputfile.o popratio.o taggrow.o agebandmatrixratiomemberfunctions.o

SLAVEOBJECTS = netdata.o slavecommunication.o pvmconstants.o

GADGETINPUT = initialinputfile.o vectorofcharptr.o charptrvector.o \
	commentstream.o parameter.o doubleindexvector.o \
	intvector.o doublevector.o intmatrix.o doublematrix.o

LDFLAGS = $(CXXFLAGS) $(LIBDIRS) $(LIBRARIES)

CC = $(CXX)
gadget	:	$(OBJECTS) libvec.a
	$(CXX) -o gadget $(OBJECTS) $(LDFLAGS)

libvec.a: $(VECTORS)
	ar r libvec.a $?
install: gadget
	mv gadget ../../bin
################################################################
# The following line is needed to create the gadget input library
# for newer versions of paramin.  To create this library, you need
# to type "make libgadgetinput.a" *before* you compile paramin
################################################################
libgadgetinput.a: $(GADGETINPUT)
	ar r libgadgetinput.a $?
clean:
	rm -f $(VECTORS) $(OBJECTS) libvec.a libgadgetinput.a
depend:
	$(CXX) -M -MM $(CXXFLAGS) *.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
