#ifndef popinfo_h
#define popinfo_h

/**
 * \class PopInfo
 * \brief This is the class used store information about the number, and mean weight, of a population cell of a stock
 */
class PopInfo {
public:
  /**
   * \brief This is the PopInfo constructor
   */
  PopInfo();
  /**
   * \brief This is the PopInfo destructor
   */
  ~PopInfo() {};
  /**
   * \brief This is the number of the fish in the population cell
   */
  double N;
  /**
   * \brief This is the mean weight of the fish in the population cell
   */
  double W;
  PopInfo& operator += (const PopInfo& a);
  void operator -= (double a);
  void operator *= (double a);
  PopInfo operator * (double b);
  PopInfo& operator = (const PopInfo& a);
};

#endif
