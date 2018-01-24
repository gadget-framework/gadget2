#ifndef interruptinterface_h
#define interruptinterface_h

/**
 * \class InterruptInterface
 * \brief This is the class used to communicate with the user after an interrupt
 */
class InterruptInterface {
public:
  /**
   * \brief This is the InterruptInterface constructor
   */
  InterruptInterface() {};
  /**
   * \brief This is the default InterruptInterface destructor
   */
  ~InterruptInterface() {};
  /**
   * \brief This is the function used to communicate with the user
   * \return 0 to quit, else 1 to continue simulation
   */
  int menu();
private:
  /**
   * \brief This is the function used to print a menu to communicate with the user
   */
  void printMenu();
};

#endif
