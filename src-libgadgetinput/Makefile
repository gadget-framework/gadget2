GADGETINPUT = intvector.o doublevector.o charptrvector.o initialinputfile.o \
    commentstream.o parameter.o parametervector.o strstack.o \
    optinfoptrvector.o doublematrix.o runid.o global.o errorhandler.o

all: libgadgetinput.a

include ../src/Makefile

GADGET_NETWORK = -D GADGET_NETWORK
GADGET_CXXFLAGS += -I ../src/include/ -I ../src-para/include/
GADGET_LDFLAGS += -I ../src/include/ -I ../src-para/include/ -L ../src/

# Build common gadget sources here
%.o: ../src/%.cc
	$(CXX) $(CXXFLAGS) $(GADGET_OPTIM) $(GADGET_CXXFLAGS) $(GADGET_VERSION) $(GADGET_PLATFORM) $(GADGET_NETWORK) -c $< -o $@

libgadgetinput.a: $(GADGETINPUT)
	ar rs $@ $(GADGETINPUT)

.PHONY: all clean
