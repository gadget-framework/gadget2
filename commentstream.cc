#include "commentstream.h"

istream& operator >> (istream& istr, Whitespace& ws) {
  int ch;
  while (istr.peek() == ' ' || istr.peek() == '\t' || istr.peek() == '\r' || istr.peek() == '\n')
    ch = istr.get();

  if (!istr.eof())
    if (istr.peek() == EOF)
      ch = istr.get();
  return istr;
}

void killComments(istream* const istrptr) {
  Whitespace Ws;
  if (istrptr->eof())
    return;
  char c;
  *istrptr >> Ws;
  if (istrptr->eof())
    return;

  while (istrptr->peek() == chrComment) {
    while (istrptr->peek() != '\n' && !(istrptr->eof()))
      istrptr->get(c);
    *istrptr >> Ws;
  }
}

CommentStream& ws(CommentStream& ins) {
  killComments(ins.istrptr);
  return ins;
}

CommentStream& CommentStream::get(char& c) {
  if (istrptr->peek() == chrComment) {
    killComments(istrptr);
    c = '\n';
  } else
    istrptr->get(c);
  return *this;
}

CommentStream& CommentStream::getline(char* ptr, int len, char delim) {
  int i = 0;
  while ((i < len - 2) && (istrptr->peek() != delim) && (istrptr->peek() != chrComment)) {
    ptr[i] = istrptr->get();
    i++;
  }
  if (istrptr->peek() == delim)
    ptr[i++] = istrptr->get();
  ptr[i] = '\0';
  return *this;
}
