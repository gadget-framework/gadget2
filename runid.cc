#include "runid.h"
#include "gadget.h"

extern int gethostname(char*, int);

RunID::RunID() {
  if (uname(&host) != -1)
    hostName = host.nodename;
  else
    hostName = "-nohostname-";

  if (time(&runtime))
    timestring = ctime(&runtime);
  else
    timestring = "-notime-";
}

void RunID::print(ostream& o) {
  o << "Gadget version " << GADGETVERSION << " running on " << hostName << sep << timestring;
  if (timestring[strlen(timestring) - 1] != '\n')
    o << endl;
  o.flush();
}

void RunID::printElapsedTime(ostream& o) {
  time_t stoptime;
  o << difftime(time(&stoptime), runtime) << " seconds" << endl;
  o.flush();  
}
