#ifndef hasname_h
#define hasname_h

#include "gadget.h"

/**
 * \class HasName
 * \brief This is the base class for any object that has a name
 */
class HasName {
public:
  /**
   * \brief This is the default HasName constructor
   */
  HasName() { name = new char[1]; name[0] = '\0'; };
  /**
   * \brief This is the default HasName destructor
   * \note This will free all the memory allocated
   */
  virtual ~HasName() { delete[] name; };
  /**
   * \brief This is the HasName constructor for a specified name
   * \param givenname is a text string containing the name of the object to be created
   */
  HasName(const char* givenname) {
    name = new char[strlen(givenname) + 1];
    strcpy(name, givenname);
  };
  /**
   * \brief This will return a null terminated text string containing the name of the object
   * \return name
   */
  const char* getName() const { return name; };
private:
  /**
   * \brief This is the null terminated name of the object
   */
  char* name;
};

#endif
