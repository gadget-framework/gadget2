#ifndef gadget_h
#define gadget_h

/**
 * \mainpage Draft Gadget Documentation
 *
 * Gadget is the <b>G</b>lobally applicable <b>A</b>rea-<b>D</b>isaggregated <b>G</b>eneral <b>%E</b>cosystem <b>T</b>oolbox.  Gadget is a flexible and powerful tool for creating ecosystem models.  The program was developed for modelling marine ecosystems in a fisheries management and biology context, however there is nothing in the program that restricts it to fish, and models have been developed to examine marine mammal populations.  Gadget has been used to investigate the population dynamics of many stocks and stock complexes in Icelandic waters, the Barents Sea, the North Sea and the Irish and Celtic Seas.
 *
 * Gadget can run complicated statistical ecosystem models, which take many features of the ecosystem into account. Gadget works by running an internal model based on many parameters, and then comparing the data from the output of this model to ''real'' data to get a goodness-of-fit likelihood score.  The parameters can then be adjusted, and the model re-run, until an optimum is found, which corresponds to the model with the lowest likelihood score.
 *
 * Gadget allows you to include a number of features into your model: One or more species, each of which may be split into multiple stocks; multiple areas with migration between areas; predation between and within species; maturation; reproduction and recruitment; multiple commercial and survey fleets taking catches from the populations.
 *
 * For more information about Gadget, including links to download the software, please see the webpage that has been set up at http://www.hafro.is/gadget or email us at gadgethelp@hafro.is
 */

/* A list of the standard header files that are needed for Gadget */
/* Older compilers need these to be declared in the old format    */
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <signal.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/param.h>

/* This is a nasty hack to use the functions in the std namespace */
/* it would be much better to explicitly state the std namespace  */
/* when using the functions - eg std::strcmp() not just strcmp()  */
/* Older compilers will reject this so it needs to be removed     */
using namespace std;

/* Some compilers define the values for EXIT_SUCCESS and EXIT_FAILURE */
/* but to be sure that they are defined, they are also included here. */
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

/* Also defined are some of the constants used by Gadget */
const int MaxStrLength = 250;
const int LongString = 1025;
const double rathersmall = 1e-10;
const double verysmall = 1e-100;
const double verybig = 1e+10;
const double pivalue = 3.14159265359;
const int MAX_MORTALITY = 1000;
const double MAX_RATIO_CONSUMED = 0.95;
const char sep = ' ';

#ifndef TAB
#define TAB '\t'
#endif

/* Also defined are some of the printer constants used by Gadget */
const int lowprecision = 1;
const int smallprecision = 4;
const int printprecision = 6;
const int largeprecision = 8;
const int fullprecision = 15;
const int lowwidth = 4;
const int smallwidth = 8;
const int printwidth = 10;
const int largewidth = 12;
const int fullwidth = 18;

/* Update the following line each time upgrades are implemented */
#define gadgetversion "2.0.04"

#endif
