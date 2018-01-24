#include "runid.h"
#include "gadget.h"

extern int gethostname(char*, int);

RunID::RunID() {
  hostname = new char[MaxStrLength];
  strncpy(hostname, "", MaxStrLength);
  timestring = new char[MaxStrLength];
  strncpy(timestring, "", MaxStrLength);

#ifdef NOT_WINDOWS
  if (uname(&host) != -1)
    strcpy(hostname, host.nodename);
#else
  strcpy(hostname, getenv("COMPUTERNAME"));
#endif
  if (time(&runtime))
    strcpy(timestring, ctime(&runtime));
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
