#include "runid.h"
#include "gadget.h"

extern int gethostname(char*, int);

RunId::RunId() {
  if (uname(&host) != -1)
    hostname = host.nodename;
  else
    hostname = "-nohostname-";
  time_t runtime;
  if (time(&runtime))
    timestring = ctime(&runtime);
  else
    timestring = "-notime-";
}

void RunId::print(ostream& o) {
  o << "Gadget version " << gadget_version << " running on " << hostname << sep << timestring;
  if (timestring[strlen(timestring) - 1] != '\n')
    o << endl;
  o.flush();
}

RunId::~RunId() {
}
