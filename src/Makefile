# Compile gadget objects
# NB: Used for both R and top-level Makefile

CXXFLAGS += -O3 -g0  # Override /etc/R/Makeconf settings

CXXFLAGS += -D INTERRUPT_HANDLER -I include/
LDFLAGS += -L. -L/usr/local/lib -I include/ -lm

ifneq ($(OS),Windows_NT)
  ifneq ($(R_OSTYPE),windows)
    CXXFLAGS += -D NOT_WINDOWS
  endif
endif

GADGETOBJECTS = gadget.o ecosystem.o initialize.o simulation.o fleet.o otherfood.o \
    area.o time.o keeper.o maininfo.o printinfo.o runid.o global.o stochasticdata.o \
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
    recaggregator.o predatorpreyaggregator.o stockpreyaggregator.o \
    predatoraggregator.o predatoroveraggregator.o preyoveraggregator.o \
    fleetpreyaggregator.o stockaggregator.o fleeteffortaggregator.o stockstdprinter.o \
    predatorprinter.o predatoroverprinter.o predatorpreyprinter.o preyoverprinter.o \
    stockfullprinter.o stockpreyfullprinter.o stockpreyprinter.o stockprinter.o \
    likelihoodprinter.o summaryprinter.o boundlikelihood.o migrationpenalty.o \
    catchdistribution.o catchinkilos.o catchstatistics.o understocking.o \
    recapture.o recstatistics.o stockdistribution.o stomachcontent.o \
    surveydistribution.o surveyindices.o sionstep.o sibyacousticonstep.o \
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
    tagptrvector.o optinfoptrvector.o errorhandler.o modelvariablevector.o \
    stockvariable.o modelvariable.o migrationproportion.o proglikelihood.o

SLAVEOBJECTS = netdata.o slavecommunication.o pvmconstants.o

GADGETINPUT = intvector.o doublevector.o charptrvector.o initialinputfile.o \
    commentstream.o parameter.o parametervector.o strstack.o 

EXTRAINPUT = optinfoptrvector.o doublematrix.o runid.o global.o errorhandler.o

all: gadget

gadget: $(GADGETOBJECTS) $(GADGETINPUT)
	$(CXX) -o gadget $+ $(LDFLAGS)

libgadgetinput.a: $(GADGETINPUT) $(EXTRAINPUT)
	ar rs $@ $+

gadget-para: CXXFLAGS += -D GADGET_NETWORK
gadget-para: $(GADGETINPUT) $(GADGETOBJECTS) $(SLAVEOBJECTS)
	$(CXX) -o gadget-para $+ $(LDFLAGS)

clean:
	rm -f *.o libgadgetinput.a gadget gadgetpara

.PHONY: clean
