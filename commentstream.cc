#include "commentstream.h"
#include "errorhandler.h"
#include "global.h"

istream& operator >> (istream& istr, Whitespace& ws) {
  int c;
  while (istr.peek() == ' ' || istr.peek() == '\t' || istr.peek() == '\r' || istr.peek() == '\n')
    c = istr.get();
  return istr;
}

CommentStream& ws(CommentStream& ins) {
  ins.killComments();
  return ins;
}

void CommentStream::killComments() {
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

char CommentStream::peek() {
  if (istrptr->peek() == chrComment) {
    this->killComments();
    return '\n';
  } else if (istrptr->peek() == '\r') {
    char tmp;
    istrptr->get(tmp);   //JMB get the carriage return and discard it
  } else if (istrptr->peek() == '\\') {
    //attempting to read backslash will do nasty things to the input stream
    handle.logFileMessage(LOGFAIL, "backslash is an invalid character");
  } else if ((istrptr->peek() == '\'') || (istrptr->peek() == '\"')) {
    //attempting to read quote will do nasty things to the input stream
    handle.logFileMessage(LOGFAIL, "quote is an invalid character");
  }
  //JMB GCC 4.3 has a stricter implemenation of the C++ standard
  //so we need to cast this to a char to avoid generating warnings
  return (char)istrptr->peek();
}

CommentStream& CommentStream::get(char& c) {
  if (istrptr->peek() == chrComment) {
    this->killComments();
    c = '\n';
  } else if (istrptr->peek() == '\r') {
    char tmp;
    istrptr->get(tmp);  //JMB throw away carriage return ...
    istrptr->get(c);    //JMB ... to be left with end of line
  } else
    istrptr->get(c);
  return *this;
}

CommentStream& CommentStream::getLine(char* text, int length) {
  int i = 0;
  while ((i < length - 2) && (istrptr->peek() != chrComment)
          && (istrptr->peek() != '\n') && (istrptr->peek() != '\r'))
    text[i++] = (char)istrptr->get();

  if ((istrptr->peek() == '\n') || (istrptr->peek() == '\r'))
    text[i++] = (char)istrptr->get();
  text[i] = '\0';
  return *this;
}

CommentStream& CommentStream::operator >> (int& a) {
  this->killComments();
  (*istrptr) >> a;
  return *this;
}

CommentStream& CommentStream::operator >> (double& a) {
  this->killComments();
  (*istrptr) >> a;
  return *this;
}

CommentStream& CommentStream::operator >> (char* a) {
  this->killComments();
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
