#ifndef commentstream_h
#define commentstream_h

#include "gadget.h"

const char chrComment = ';';
extern void killComments(istream* const istrptr);

class CommentStream;
typedef CommentStream& (*__commentmanip)(CommentStream&);

class Whitespace {
public:
  Whitespace() { v = 0; };
  ~Whitespace() {};
  friend istream& operator >> (istream& istr, Whitespace& ws);
private:
  int v;
};

class CommentStream {
public:
  friend CommentStream& ws(CommentStream& ins);
  CommentStream() { istrptr = NULL; };
  CommentStream(istream& istr) { istrptr = &istr; };
  ~CommentStream() {};
  void setStream(istream& istr) { istrptr = &istr; };
  CommentStream& operator >> (int& a) {
    killComments(istrptr);
    istrptr->operator >> (a);
    return *this;
  };
  CommentStream& operator >> (double& a) {
    killComments(istrptr);
    istrptr->operator >> (a);
    return *this;
  };
  CommentStream& operator >> (char* a) {
    killComments(istrptr);
    string s;
    (*istrptr) >> s;
    s.copy(a, string::npos);
    a[s.length()] = '\0';
    return *this;
  };
  CommentStream& operator >> (__commentmanip func) {
    (*func)(*this);
    return *this;
  };
  int peek() { return (istrptr->peek() == chrComment ? '\n' : istrptr->peek()); };
  int eof() { return istrptr->eof(); };
  int fail() { return istrptr->fail(); };
  int bad() { return istrptr->bad(); };
  int good() { return istrptr->good(); };
  void get(char* text, int length, char sep) { istrptr->get(text, length, sep); };
  CommentStream& seekg(streampos Pos) {
    istrptr->seekg(Pos);
    return *this;
  };
  streampos tellg() { return istrptr->tellg(); };
  int operator !() { return istrptr->fail(); };
  void makebad() { istrptr->clear(ios::badbit|istrptr->rdstate()); };
  CommentStream& get(char& c);
  CommentStream& getLine(char* ptr, int len, char delim = '\n');
protected:
  istream* istrptr;
};

#endif
