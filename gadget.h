#ifndef gadget_h
#define gadget_h

/* A list of the standard header files that are needed */
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <fstream>
#include <strstream>
#include <iomanip>
#include <cmath>
#include <ctime>
/* older compilers need these to be declared in the old format
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream.h>
#include <strstream.h>
#include <iomanip.h>
#include <math.h>
#include <time.h>*/
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
const int pathmax = 1025;
const double rathersmall = 1e-10;
const double verysmall = 1e-100;
const int MAX_MORTALITY = 1000;
const double MAX_RATIO_CONSUMED = 0.95;
const double HUGE_VALUE = 1e+30;
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
const int smallwidth = 4;
const int printwidth = 8;
const int largewidth = 10;
const int fullwidth = 12;

/* Update the following line each time upgrades are implemented */
#define gadget_version "2.0.02-BETA"

#endif
