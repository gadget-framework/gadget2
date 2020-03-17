# NB: This should *not* be included in the R package
# This is for compiling gadget outside R machinery

# Load common R / not-R Makevars
include ../src/Makevars

# Replace enough of R's Makeconf to compile
.cc.o:
	$(CXX) $(CXXFLAGS) $(PKG_CXXFLAGS) -c $< -o $@
