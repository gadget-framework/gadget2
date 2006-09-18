##########################################################################
# Common for all architecture and compiler options
##########################################################################
GCCWARNINGS = -Wimplicit -Wreturn-type -Wswitch -Wcomment -Wformat \
              -Wparentheses -Wpointer-arith -Wcast-qual -Wconversion \
              -Wreorder -Wwrite-strings -Wsynth -Wchar-subscripts \
              -Wuninitialized -W -pedantic

#DEFINE_FLAGS = -D DEBUG -D INTERRUPT_HANDLER -g -O
DEFINE_FLAGS = -D NDEBUG -D INTERRUPT_HANDLER -O2 -s

##########################################################################
# The name of the final executable (eg gadget-paramin or gadget.exe)
GADGET = gadget
##########################################################################
# Pick the appropriate compiler from the following switches
##########################################################################
# 1. Linux, or Cygwin, or Solaris, without pvm3, g++ compiler
CXX = g++
LIBDIRS = -L. -L/usr/local/lib
LIBRARIES = -lm
CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS)
OBJECTS = $(GADGETINPUT) $(GADGETOBJECTS)
##########################################################################
# 2. Linux, or Cygwin, or Solaris, with pvm3, g++ compiler
#CXX = g++
#PVMDIR = $(PVM_ROOT)
#PVMINCLUDE = $(PVMDIR)/include
#PVMLIB = $(PVMDIR)/lib/$(PVM_ARCH)
#LIBDIRS = -L. -L/usr/local/lib -L$(PVMLIB)
#LIBRARIES = -lm -lpvm3 -lnsl
## note that some (older) linux machines also require the -lsocket library
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS) -D GADGET_NETWORK -I$(PVMINCLUDE)
#OBJECTS = $(GADGETINPUT) $(GADGETOBJECTS) $(SLAVEOBJECTS)
##########################################################################
# 3. Solaris, without pvm3, using CC compiler
#CXX = CC
#LIBDIRS = -L. -L/usr/local/lib
#LIBRARIES = -lm
#CXXFLAGS = $(DEFINE_FLAGS)
#OBJECTS = $(GADGETINPUT) $(GADGETOBJECTS)
##########################################################################
# 4. Linux or Solaris, without pvm3, g++ compiler running CONDOR
#CXX = condor_compile g++
#LIBDIRS = -L. -L/usr/local/lib
#LIBRARIES = -lm
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS)
#OBJECTS = $(GADGETINPUT) $(GADGETOBJECTS)
##########################################################################
# 5. Windows using services for unix
#CXX = g++
#LIBDIRS = -L. -L/usr/local/lib
#LIBRARIES = -lm
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS)
#OBJECTS = $(GADGETINPUT) $(GADGETOBJECTS)
#CPPFLAGS = -I/usr/local/include #-I/dev/fs/C/x/SDK/opt/gcc.3.3/include/c++/3.3/backward
##########################################################################

GADGETOBJECTS = gadget.o ecosystem.o initialize.o simulation.o fleet.o otherfood.o \
    area.o time.o keeper.o maininfo.o printinfo.o runid.o stochasticdata.o \
    timevariable.o formula.o readaggregation.o readfunc.o readmain.o readword.o \
    actionattimes.o livesonareas.o lengthgroup.o conversionindex.o selectfunc.o \
    suitfunc.o suits.o popinfo.o popinfomemberfunctions.o popratio.o popstatistics.o \
    interruptinterface.o agebandmatrix.o agebandmatrixmemberfunctions.o tags.o \
    interrupthandler.o agebandmatrixratio.o agebandmatrixratiomemberfunctions.o \
    migrationarea.o rectangle.o ludecomposition.o multinomial.o regressionline.o \
    stock.o stockmemberfunctions.o renewal.o spawner.o stray.o transition.o \
    grow.o grower.o growermemberfunctions.o growthcalc.o taggrow.o maturity.o \
    initialcond.o migration.o naturalm.o lengthprey.o prey.o stockprey.o \
    effortpredator.o lengthpredator.o linearpredator.o numberpredator.o \
    quotapredator.o predator.o poppredator.o stockpredator.o totalpredator.o \
    recaggregator.o stockpreyaggregator.o predatorpreyaggregator.o \
    predatoraggregator.o predatoroveraggregator.o preyoveraggregator.o \
    fleetpreyaggregator.o stockaggregator.o fleeteffortaggregator.o \
    predatoroverprinter.o predatorpreyprinter.o preyoverprinter.o \
    stockfullprinter.o stockpreyfullprinter.o stockpreyprinter.o stockprinter.o \
    predatorprinter.o stockstdprinter.o likelihoodprinter.o summaryprinter.o \
    catchdistribution.o catchinkilos.o catchstatistics.o understocking.o \
    recapture.o recstatistics.o stockdistribution.o stomachcontent.o sionstep.o \
    boundlikelihood.o surveydistribution.o migrationpenalty.o surveyindices.o \
    sibyageonstep.o sibyfleetonstep.o sibylengthonstep.o sibyeffortonstep.o \
    optinfobfgs.o optinfohooke.o optinfosimann.o bfgs.o hooke.o simann.o \
    addresskeepervector.o addresskeepermatrix.o intmatrix.o doublematrix.o \
    agebandmatrixptrvector.o agebandmatrixptrmatrix.o agebandmatrixratioptrvector.o \
    doublematrixptrvector.o doublematrixptrmatrix.o timevariablevector.o \
    formulavector.o formulamatrix.o formulamatrixptrvector.o charptrmatrix.o \
    popinfovector.o popinfomatrix.o popinfoindexvector.o suitfuncptrvector.o \
    popratiovector.o popratiomatrix.o popratioindexvector.o fleetptrvector.o \
    baseclassptrvector.o conversionindexptrvector.o likelihoodptrvector.o \
    predatorptrvector.o preyptrvector.o printerptrvector.o stockptrvector.o \
    migrationareaptrvector.o rectangleptrvector.o otherfoodptrvector.o \
    tagptrvector.o optinfoptrvector.o

SLAVEOBJECTS = netdata.o slavecommunication.o pvmconstants.o

GADGETINPUT = intvector.o doublevector.o charptrvector.o initialinputfile.o \
    commentstream.o parameter.o parametervector.o errorhandler.o strstack.o

LDFLAGS = $(CXXFLAGS) $(LIBDIRS) $(LIBRARIES)

gadget	:	$(OBJECTS)
		$(CXX) -o $(GADGET) $(OBJECTS) $(LDFLAGS)

## you need root permission to be able to do this ...
install	:	$(GADGET)
		strip $(GADGET)
		cp $(GADGET) /usr/local/bin/
		cp gadget.1 /usr/local/man/man1/

##########################################################################
# The following line is needed to create the gadget input library
# for newer versions of paramin.  To create this library, you need
# to type "make libgadgetinput.a" *before* you compile paramin
##########################################################################
libgadgetinput.a	:	$(GADGETINPUT)
		ar rs libgadgetinput.a $?

clean	:
		rm -f $(OBJECTS) libgadgetinput.a

depend	:
		$(CXX) -M -MM $(CXXFLAGS) *.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
