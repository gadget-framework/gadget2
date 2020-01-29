##########################################################################
# The name of the final executable (eg gadget-paramin or gadget.exe)
GADGET = gadget
GADGETPARA = gadget-para
export GADGET GADGETPARA

##########################################################################
# Pick the appropriate compiler from the following switches
##########################################################################
# 1. Linux, or Cygwin, or Solaris, with MPI, mpic++ compiler
#CXX = mpic++
##########################################################################
# 2. Linux, Mac, Cgwin or Solaris, without MPI, using g++ compiler
CXX = g++
# CXX = clang++

export CXX
##########################################################################
# Enable optimisation / debugging
# CXXFLAGS = -D DEBUG
CXXFLAGS = -O3

ifneq ($(OS),Windows_NT)
	CXXFLAGS += -D NOT_WINDOWS 
endif

export CXXFLAGS
###########################################################################
$(GADGET):
	$(MAKE) -C src $(GADGET)

##########################################################################
# The following line is needed to create the gadget input library
# for newer versions of paramin.  To create this library, you need
# to type "make libgadgetinput.a" *before* you compile paramin
##########################################################################
libgadgetinput.a:
	$(MAKE) -C src libgadgetinput.a

gadgetpara: CXX = mpic++
gadgetpara:
	$(MAKE) -C src $(GADGETPARA)

## you need root permission to be able to do this ...
install	:	$(GADGET)
		strip src/$(GADGET)
		cp src/$(GADGET) /usr/local/bin/
		mkdir -p /usr/local/man/man1/
		cp gadget.1 /usr/local/man/man1/

clean	:
	$(MAKE) -C src $@

.PHONY: gadget libgadgetinput.a gadgetpara install clean
