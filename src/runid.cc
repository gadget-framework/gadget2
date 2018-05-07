#include "runid.h"
#include "gadget.h"
#include <omp.h>

extern int gethostname(char*, int);

RunID::RunID() {
  hostname = new char[MaxStrLength];
  strncpy(hostname, "", MaxStrLength);
  timestring = new char[MaxStrLength];
  strncpy(timestring, "", MaxStrLength);
  
  if (uname(&host) != -1)
    strcpy(hostname, host.nodename);
  if (time(&runtime))
    strcpy(timestring, ctime(&runtime));
  
  runtime2 = omp_get_wtime();
}

RunID::~RunID() {
  delete[] hostname;
  delete[] timestring;
}


void RunID::Print(ostream& o) {
  o << "Gadget version " << GADGETVERSION << " running on " << hostname << sep << timestring;
  o.flush();
}

void RunID::printTime(ostream& o) {
  time_t stoptime;
  o << difftime(time(&stoptime), runtime) << " seconds" << endl;
  o.flush();
}

double RunID::returnTime() {
  double t1;

  return (omp_get_wtime() - runtime2);
}

