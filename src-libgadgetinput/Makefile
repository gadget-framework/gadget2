GADGETINPUT = intvector.o doublevector.o charptrvector.o initialinputfile.o \
    commentstream.o parameter.o parametervector.o strstack.o \
    optinfoptrvector.o doublematrix.o runid.o global.o errorhandler.o

all: libgadgetinput.a

include ../src/GNUmakefile

GADGET_NETWORK = -D GADGET_NETWORK
GADGET_CXXFLAGS += -I ../src/include/ -I ../src-para/include/
GADGET_LDFLAGS += -I ../src/include/ -I ../src-para/include/ -L ../src/
override PKG_CXXFLAGS = $(GADGET_OPTIM) $(GADGET_CXXFLAGS) $(GADGET_PLATFORM) $(GADGET_NETWORK)

# Build common gadget sources here
%.o: ../src/%.cc
	$(CXX) $(CXXFLAGS) $(PKG_CXXFLAGS) -c $< -o $@

libgadgetinput.a: $(GADGETINPUT)
	ar rs $@ $(GADGETINPUT)

.PHONY: all clean
