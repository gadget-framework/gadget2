#ifndef commentstream_h
#define commentstream_h

#include "gadget.h"

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

/**
 * \class CommentStream
 * \brief This is the class used to strip comments (and whitespace) from any input stream
 */
class CommentStream {
public:
  friend CommentStream& ws(CommentStream& ins);
  /**
   * \brief This is the default CommentStream constructor
   */
  CommentStream() { istrptr = NULL; };
  /**
   * \brief This is the CommentStream constructor for a given input stream
   * \param istr is the given input stream
   */
  CommentStream(istream& istr) { istrptr = &istr; };
  /**
   * \brief This is the default CommentStream destructor
   */
  ~CommentStream() {};
  /**
   * \brief This function will store a given input stream
   * \param istr is the given input stream
   */
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
  /**
   * \brief This function will inspect (but not read) the next character in the input stream
   * \return the next character in the input stream
   */
  char peek();
  /**
   * \brief This function will check to see if the input stream has reached the end of file marker
   * \return 1 if the input stream has reached the end of file marker, 0 otherwise
   */
  int eof() { return istrptr->eof(); };
  /**
   * \brief This function will check to see if the input stream has failed
   * \return 1 if the input stream has failed, 0 otherwise
   */
  int fail() { return istrptr->fail(); };
  /**
   * \brief This function will check to see if the input stream has failed
   * \return 1 if the input stream has failed, 0 otherwise
   */
  int operator !() { return istrptr->fail(); };
  /**
   * \brief This function will find the specified position in the input stream
   * \param pos is the position in the input stream to be found
   */
  CommentStream& seekg(streampos pos) { istrptr->seekg(pos); return *this; };
  /**
   * \brief This function will return the current position in the input stream
   * \return pos, the current position in the input stream
   */
  streampos tellg() { return istrptr->tellg(); };
  /**
   * \brief This function will read the next character from the input stream
   * \param c will store the next character in the input stream
   */
  CommentStream& get(char& c);
  /**
   * \brief This function will read the next line from the input stream
   * \param text will store the next line in the input stream
   * \param length is the length of the line to be read from the input stream
   */
  CommentStream& getLine(char* text, int length);
protected:
  /**
   * \brief This function will remove the comments and whitespace from the input stream
   */
  void killComments();
  /**
   * \brief This is the input stream that will have the comments and whitespace removed
   */
  istream* istrptr;
};

CommentStream& ws(CommentStream& ins);

#endif
