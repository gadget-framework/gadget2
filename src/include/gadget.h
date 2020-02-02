#ifndef gadget_h
#define gadget_h

/**
\mainpage Draft Gadget Documentation

Gadget is the <b>G</b>lobally applicable <b>A</b>rea <b>D</b>isaggregated <b>G</b>eneral <b>E</b>cosystem <b>T</b>oolbox.  Gadget is a flexible and powerful tool for creating ecosystem models.  The program was developed for modelling marine ecosystems in a fisheries management and biology context, however there is nothing in the program that restricts it to fish, and models have been developed to examine marine mammal populations.  Gadget has been used to investigate the population dynamics of many stocks and stock complexes in Icelandic waters, the Barents Sea, the North Sea and the Irish and Celtic Seas.

Gadget can run complicated statistical ecosystem models, which take many features of the ecosystem into account. Gadget works by running an internal model based on many parameters, and then comparing the data from the output of this model to ''real'' data to get a goodness-of-fit likelihood score.  The parameters can then be adjusted, and the model re-run, until an optimum is found, which corresponds to the model with the lowest likelihood score.

Gadget allows you to include a number of features into your model: One or more species, each of which may be split into multiple stocks; multiple areas with migration between areas; predation between and within species; maturation; reproduction and recruitment; multiple commercial and survey fleets taking catches from the populations.

For more information about Gadget, including links to download the software, please see the webpage that has been set up at http://www.hafro.is/gadget or email us at gadgethelp@hafro.is

This document is the developers manual for Gadget, and describes the classes, functions and variables from a programmers point of view.  Also included in this document is the \link standards Gadget Coding Standards \endlink which gives brief details of the layout of the C++ source files that developers should follow.
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
#include <vector>
#include <signal.h>
#include <unistd.h>
#ifdef NOT_WINDOWS
#include <sys/utsname.h>
#endif
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
const int MaxStrLength = 1025;                 /* 128 + 1 */
const int LongString = 16385;                  /* 16Kb + 1 */
const double verybig = 1e+10;
const double rathersmall = 1e-10;
/* machine accuracy for doubles is apparently 2.2204460492503131e-16 */
//const double verysmall = 4.4408921e-16;  /* twice machine accuracy */
const double verysmall = 1e-20;
const char chrComment = ';';
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
#define GADGETVERSION "2.2.00-BETA"

#endif

/**
\page standards Gadget Coding Standards

This section contains brief details of the Gadget coding standards, based on the proposals made at the 2000 DST2 meeting in Nantes, and documented in annex L7 of the DST2 progress report for that year.

\section name Naming Conventions

Class names should always begin with a capital letter, and class names consisting of multiple words should be written with each word capitalised.

Class members, variables and functions should always start with a lower case letter.  Additional words in the name should be separated by capitalising each word.  The use of underscores to separate words is discouraged.

Examples:
\verbatim
   #class# TimeClass {
     // ...
   }

   TimeClass* currentTime;

   #void# incrementTime(TimeClass currentTime) {
     // ...
   }
\endverbatim

\section space Spacing, Indentation and Brackets

Only spaces should be used for indentation, two spaces for each level of indentation.  A block opening curly bracket should be at the end of the line that defines the block.  The matching end bracket is placed on a line of its own, at the same level of indentation as the line that starts the block.  If an \c if block is followed by an \c else clause, the next block should be started on the same line as the closing curly bracket of the previous block.

An opening round bracket following a keyword (\c if, \c while, \c for, etc.) should be separated from the keyword with a space.  There should also be a space separating the closing round bracket and the opening curly bracket, if the curly brackets are required.  In function calls, the opening bracket for the function arguments should not be separated from the function name.

All mathematical and logical operators (=, \<, +, etc.) should be separated from the variables they are operating on by a space before and after the operator.  Similarly when variables are separated by a comma or a semi-colon, there should be a space after the separator.

Examples:
\verbatim
   #for# (i = 0; i < n; i++) {
     // ...
   }

   #if# (isDone()) {
     // ...
   } #else# #if# (something()) {
     // ...
   } #else# {
     // ...
   }

   doSomething(a, b, c);
\endverbatim

\section forloop Local Variables in For Loops

Due to the limitations of some compilers, variables defined and initialised in the opening of a \c for loop will not always have a local scope for that loop.  These variables should, therefore, be defined outside the scope of the loop.

Example:
\verbatim
   #int# i;
   #for# (i = 0; i < n; i++) {
     // ...
   }
\endverbatim

\section doc Method Documentation

This documentation has been generated from the source files by Doxygen (see http://www.doxygen.org for more information).  To ensure that Doxygen can document all the classes, functions and variables in Gadget, these have been described with ''doxygen style'' comments that have been placed in the header files for each class.  As a minimum, these comments include the \c \\brief command to give a brief description of the object, the \c \\param command to give a description of the parameters (if any) and the \c \\return command to give a description of the return value (if any).  More complicated objects will require more detailed descriptions.

Any non-trivial method should also have a short description in the code, in addition to any documentation in the header file.

Example:
\verbatim
  #**
   * \brief This will return the total number of timesteps that have taken place in the
      simulation from the start of the model simulation until a specified year and step
   * \param year is the specified year
   * \param step is the specified step
   * \return number of timesteps taken
   *#
  #int# TimeClass::calcSteps(#int# year, #int# step);
\endverbatim

\section cvs Version Control

To maintain version control of the source files, a CVS repository has been set up on the Marine Research Institute server in Reykjavik.  Any user with secure shell access to the MRI server can access this repository, and a web interface to this repository is also available to allow users to browse the source code online (see http://www.hafro.is/viewcvs/viewcvs.cgi/gadget/).  Any ''official'' version of the Gadget software will be tagged and released from the source code that is in the CVS repository.  The CVS repository should also contain any user documents and files external to the main source code that are distributed with the source code (for example, shell scripts to help analyse the output from a Gadget simulation).

\section compat Code Compatibility

Gadget is a program that has been developed on a Unix computing platform, and is regularly compiled and run on machines running versions of Linux, Solaris and Cygwin (a Unix emulator for Microsoft Windows machines, see http://www.cygwin.com for more information).  By regularly compiling and running Gadget on a variety of computing platforms, it is hoped that the source code is compatible with as many computing platforms as possible.  The list of computing platforms, and C++ compilers, that Gadget is regularly tested on is:

  \li Linux (Fedora 9), compiled using \c g++ version 4.3.0
  \li Linux (Ubuntu 8.04), compiled using \c g++ version 4.2.3
  \li Linux (Fedora 8), compiled using \c g++ version 4.1.2
  \li Cygwin (1.5.25-14), compiled using  \c g++ version 3.4.4-3

*/
