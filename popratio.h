#ifndef popratio_h
#define popratio_h

/**
 * \class PopRatio
 * \brief This is the class used to store information about the number of tagged fish in a population cell of a stock and ratio of the fish from that population cell that are tagged
 */
class PopRatio {
public:
  /**
   * \brief This is the PopRatio constructor
   */
  PopRatio();
  /**
   * \brief This is the PopRatio destructor
   */
  ~PopRatio() {};
  /**
   * \brief This is the number of tagged fish in the population cell
   */
  double* N;
  /**
   * \brief This is the ratio of tagged fish in the population cell
   */
  double R;
  PopRatio& operator += (const PopRatio& a);
  PopRatio& operator = (const PopRatio& a);
  void operator -= (double a);
  void operator *= (double a);
};

#endif
