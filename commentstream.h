#ifndef commentstream_h
#define commentstream_h

#include "gadget.h"

extern void killComments(istream* const istrptr);

class CommentStream;
typedef CommentStream& (*__commentmanip)(CommentStream&);

/**
 * \class Whitespace
 * \brief This is the dummy class used to strip white space from any input stream
 */
class Whitespace {
public:
  /**
   * \brief This is the default Whitespace constructor
   */
  Whitespace() { v = 0; };
  /**
   * \brief This is the default Whitespace destructor
   */
  ~Whitespace() {};
  /**
   * \brief This operator will strip white space from a given input istream
   * \param istr is the istream to strip white space from
   * \param Ws is the Whitespace used to strip white space from the data
   */
  friend istream& operator >> (istream& istr, Whitespace& Ws);
private:
  /**
   * \brief This is a dummy object used when stripping the white space from any input files
   */
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
    (*istrptr) >> a;
    return *this;
  };
  CommentStream& operator >> (double& a) {
    killComments(istrptr);
    (*istrptr) >> a;
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
  void get(char* text, int length, char sep) { istrptr->get(text, length, sep); };
  CommentStream& seekg(streampos Pos) {
    istrptr->seekg(Pos);
    return *this;
  };
  streampos tellg() { return istrptr->tellg(); };
  int operator !() { return istrptr->fail(); };
  CommentStream& get(char& c);
  CommentStream& getLine(char* ptr, int len, char delim = '\n');
protected:
  istream* istrptr;
};
CommentStream& ws(CommentStream& ins);

#endif
