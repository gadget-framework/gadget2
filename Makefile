##########################################################################
# The name of the final executable (eg gadget-paramin or gadget.exe)
GADGET = gadget
GADGETPARA = gadget-para

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
# GADGET_OPTIM = -D DEBUG
GADGET_OPTIM = -O3 -march=native

##########################################################################
# Detect platform
ifeq ($(OS),Windows_NT)
    GADGET_PLATFORM = -D WINDOWS
else
    GADGET_PLATFORM = -D NOT_WINDOWS
endif

###########################################################################
gadget:
	$(MAKE) -C src gadget GADGET_PLATFORM="$(GADGET_PLATFORM)" GADGET_OPTIM="$(GADGET_OPTIM)"
	mv src/gadget $(GADGET)

##########################################################################
# The following line is needed to create the gadget input library
# for newer versions of paramin.  To create this library, you need
# to type "make libgadgetinput.a" *before* you compile paramin
##########################################################################
libgadgetinput.a:
	$(MAKE) -C src libgadgetinput.a GADGET_PLATFORM="$(GADGET_PLATFORM)" GADGET_OPTIM="$(GADGET_OPTIM)"
	mv src/$@ $@

gadgetpara: CXX = mpic++
gadgetpara:
	$(MAKE) -C src gadget-para GADGET_PLATFORM="$(GADGET_PLATFORM)" GADGET_OPTIM="$(GADGET_OPTIM)"
	mv src/gadget-para $(GADGETPARA)

## you need root permission to be able to do this ...
install	:	$(GADGET)
		strip $(GADGET)
		cp $(GADGET) /usr/local/bin/
		mkdir -p /usr/local/man/man1/
		cp gadget.1 /usr/local/man/man1/

clean	:
	$(MAKE) -C src $@
	rm libgadgetinput.a $(GADGETPARA) $(GADGET) || true

.PHONY: gadget libgadgetinput.a gadgetpara install clean
