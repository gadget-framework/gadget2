SLAVEOBJECTS = netdata.o slavecommunication.o pvmconstants.o

all: gadget-para

include ../src/Makefile

GADGET_NETWORK = -D GADGET_NETWORK
GADGET_CXXFLAGS += -I ../src/include/ -I ../src-para/include/
GADGET_LDFLAGS += -I ../src/include/ -I ../src-para/include/ -L ../src/

# Build common gadget sources here
%.o: ../src/%.cc
	$(CXX) $(CXXFLAGS) $(GADGET_OPTIM) $(GADGET_CXXFLAGS) $(GADGET_VERSION) $(GADGET_PLATFORM) $(GADGET_NETWORK) -c $< -o $@

gadget-para: $(SLAVEOBJECTS) $(GADGETOBJECTS) $(GADGETINPUT)
	$(CXX) -o gadget-para $+ $(LDFLAGS) $(GADGET_LDFLAGS)

.PHONY: all clean
