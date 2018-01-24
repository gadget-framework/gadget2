#ifndef addresskeeper_h
#define addresskeeper_h

#include "gadget.h"

/**
 * \class AddressKeeper
 * \brief This is the class used to store the name, and memory adddress, of the variables used in the model simulation
 */
class AddressKeeper {
public:
  /**
   * \brief This is the default AddressKeeper constructor
   */
  AddressKeeper() { addr = 0; name = 0; };
  /**
   * \brief This is the AddressKeeper constructor that creates a copy of an existing AddressKeeper
   * \param initial is the AddressKeeper to copy
   */
  AddressKeeper(const AddressKeeper& initial)
    : addr(initial.addr), name(new char[strlen(initial.name) + 1])
    { strcpy(name, initial.name); };
  /**
   * \brief This is the default AddressKeeper destructor
   */
  ~AddressKeeper() {
    if (name != 0)
      delete[] name;
    name = 0;
  };
  /**
   * \brief This operator will set the address of the AddressKeeper equal to an existing address
   * \param address is the address to copy
   */
  void operator = (double* address) { addr = address; };
  /**
   * \brief This operator will set the address of the AddressKeeper equal to an existing vale
   * \param value is the value to copy
   */
  void operator = (double& value) { addr = &value; };
  /**
   * \brief This operator will check to see if the address for the AddressKeeper is equal to an existing address
   * \param value is the address to check
   * \return 1 if the two addresses are equal, 0 otherwise
   */
  int operator == (const double& value) { return (addr == &value); };
  /**
   * \brief This operator will check to see if the address for the AddressKeeper is equal to an existing address
   * \param address is the address to check
   * \return 1 if the two addresses are equal, 0 otherwise
   */
  int operator == (const double* address) { return (addr == address); };
  /**
   * \brief This operator will set the name of the AddressKeeper equal to an existing string
   * \param str is the string to copy
   */
  void operator = (const char* str) {
    if (name != 0)
      delete[] name;
    name = new char[strlen(str) + 1];
    strcpy(name, str);
    delete[] str;
  };
  /**
   * \brief This operator will set the AddressKeeper equal to an existing AddressKeeper
   * \param a is the AddressKeeper to copy
   */
  void operator = (const AddressKeeper a) {
    if (name != 0)
      delete[] name;
    name = new char[strlen(a.name) + 1];
    strcpy(name, a.name);
    addr = a.addr;
  };
  /**
   * \brief This will return a null terminated text string containing the name of the stored variable
   * \return name
   */
  const char* getName() const { return name; };
  /**
   * \brief This is the memory address where the value of the variable is stored
   */
  double* addr;  //JMB shouldnt this be private??
private:
  /**
   * \brief This is the null terminated name of the stored variable
   */
  char* name;
};

#endif
