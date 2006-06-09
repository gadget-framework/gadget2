#include "commentstream.h"
#include "errorhandler.h"

extern ErrorHandler handle;

istream& operator >> (istream& istr, Whitespace& ws) {
  int c;
  while (istr.peek() == ' ' || istr.peek() == '\t' || istr.peek() == '\r' || istr.peek() == '\n')
    c = istr.get();

  if (!istr.eof() && istr.peek() == EOF)
    c = istr.get();
  return istr;
}

void killComments(istream* const istrptr) {
  if (istrptr->eof())
    return;
  Whitespace ws;
  *istrptr >> ws;
  if (istrptr->eof())
    return;

  char c;
  while (istrptr->peek() == chrComment) {
    while (istrptr->peek() != '\n' && !(istrptr->eof()))
      istrptr->get(c);
    *istrptr >> ws;
  }
}

CommentStream& ws(CommentStream& ins) {
  killComments(ins.istrptr);
  return ins;
}

int CommentStream::peek() {
  if (istrptr->peek() == chrComment) {
    killComments(istrptr);
    return '\n';
  } else if (istrptr->peek() == '\r') {
    char tmp;
    istrptr->get(tmp);   //JMB get the carriage return and discard it
  } else if (istrptr->peek() == '\\') {
    //attempting to read backslash will do nasty things to the input stream
    handle.logFileMessage(LOGFAIL, "backslash is an invalid character");
  }
  return istrptr->peek();
}

CommentStream& CommentStream::get(char& c) {
  if (istrptr->peek() == chrComment) {
    killComments(istrptr);
    c = '\n';
  } else if (istrptr->peek() == '\r') {
    char tmp;
    istrptr->get(tmp);  //JMB throw away carriage return ...
    istrptr->get(c);    //JMB ... to be left with end of line
  } else
    istrptr->get(c);
  return *this;
}

CommentStream& CommentStream::getLine(char* ptr, int length, char delim) {
  int i = 0;
  while ((i < length - 2) && (istrptr->peek() != delim) && (istrptr->peek() != chrComment)) {
    ptr[i] = istrptr->get();
    i++;
  }
  if (istrptr->peek() == delim)
    ptr[i++] = istrptr->get();
  ptr[i] = '\0';
  return *this;
}

CommentStream& CommentStream::operator >> (int& a) {
  killComments(istrptr);
  (*istrptr) >> a;
  return *this;
}

CommentStream& CommentStream::operator >> (double& a) {
  killComments(istrptr);
  (*istrptr) >> a;
  return *this;
}

CommentStream& CommentStream::operator >> (char* a) {
  killComments(istrptr);
  string s;
  (*istrptr) >> s;
  s.copy(a, string::npos);
  a[s.length()] = '\0';
  return *this;
}

CommentStream& CommentStream::operator >> (__commentmanip func) {
  (*func)(*this);
  return *this;
}
