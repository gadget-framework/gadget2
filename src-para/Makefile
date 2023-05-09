SLAVEOBJECTS = netdata.o slavecommunication.o pvmconstants.o

all: ../gadget-para

include ../src/GNUmakefile

GADGET_NETWORK = -D GADGET_NETWORK
GADGET_CXXFLAGS += -I ../src/include/ -I ../src-para/include/
GADGET_LDFLAGS += -I ../src/include/ -I ../src-para/include/ -L ../src/
override PKG_CXXFLAGS = $(GADGET_OPTIM) $(GADGET_CXXFLAGS) $(GADGET_PLATFORM) $(GADGET_NETWORK)

# Build common gadget sources here
%.o: ../src/%.cc
	$(CXX) $(CXXFLAGS) $(PKG_CXXFLAGS) -c $< -o $@

../gadget-para: $(SLAVEOBJECTS) $(GADGETOBJECTS)
	$(CXX) -o $@ $+ $(LDFLAGS) $(GADGET_LDFLAGS)

clean:
	rm -f *.o ../gadget-para

.PHONY: all clean
