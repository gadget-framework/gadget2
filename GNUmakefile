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
GADGET_OPTIM = -O3 -march=native -Wall

##########################################################################
# Detect platform
ifeq ($(OS),Windows_NT)
    GADGET_PLATFORM = -D WINDOWS
else
    GADGET_PLATFORM = -D NOT_WINDOWS
endif

###########################################################################
all: gadget

gadget:
	$(MAKE) -C src all GADGET_PLATFORM="$(GADGET_PLATFORM)" GADGET_OPTIM="$(GADGET_OPTIM)"

##########################################################################
# The following line is needed to create the gadget input library
# for newer versions of paramin.  To create this library, you need
# to type "make libgadgetinput.a" *before* you compile paramin
##########################################################################
libgadgetinput.a:
	$(MAKE) -C src-libgadgetinput libgadgetinput.a GADGET_PLATFORM="$(GADGET_PLATFORM)" GADGET_OPTIM="$(GADGET_OPTIM)"

gadgetpara: CXX = mpic++
gadgetpara:
	$(MAKE) -C src-para all GADGET_PLATFORM="$(GADGET_PLATFORM)" GADGET_OPTIM="$(GADGET_OPTIM)"

## you need root permission to be able to do this ...
install	:	$(GADGET)
		strip $(GADGET)
		cp $(GADGET) /usr/local/bin/
		mkdir -p /usr/local/man/man1/
		cp gadget.1 /usr/local/man/man1/

clean	:
	$(MAKE) -C src $@
	$(MAKE) -C src-para $@
	$(MAKE) -C src-libgadgetinput $@
	rm libgadgetinput.a $(GADGETPARA) $(GADGET) || true

.PHONY: all gadget libgadgetinput.a gadgetpara install clean
