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
   * \param ws is the Whitespace used to strip white space from the data
   */
  friend istream& operator >> (istream& istr, Whitespace& ws);
private:
  /**
   * \brief This is a dummy object used when stripping the white space from any input files
   */
  int v;
};

class CommentStream {
public:
  friend CommentStream& ws(CommentStream& ins);
  /**
   * \brief This is the default CommentStream constructor
   */
  CommentStream() { istrptr = NULL; };
  CommentStream(istream& istr) { istrptr = &istr; };
  /**
   * \brief This is the default CommentStream destructor
   */
  ~CommentStream() {};
  void setStream(istream& istr) { istrptr = &istr; };
  /**
   * \brief This operator will read data from the CommentStream and store it as an integer
   * \param a is the integer used to store the data that has been read
   */
  CommentStream& operator >> (int& a);
  /**
   * \brief This operator will read data from the CommentStream and store it as a double
   * \param a is the double used to store the data that has been read
   */
  CommentStream& operator >> (double& a);
  /**
   * \brief This operator will read data from the CommentStream and store it as a char*
   * \param a is the char* used to store the data that has been read
   */
  CommentStream& operator >> (char* a);
  CommentStream& operator >> (__commentmanip func);
  int peek();
  int eof() { return istrptr->eof(); };
  int fail() { return istrptr->fail(); };
  int operator !() { return istrptr->fail(); };
  void get(char* text, int length, char sep) { istrptr->get(text, length, sep); };
  CommentStream& seekg(streampos Pos) { istrptr->seekg(Pos); return *this; };
  streampos tellg() { return istrptr->tellg(); };
  CommentStream& get(char& c);
  CommentStream& getLine(char* ptr, int length, char delim = '\n');
protected:
  istream* istrptr;
};

CommentStream& ws(CommentStream& ins);

#endif
