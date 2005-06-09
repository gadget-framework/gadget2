#ifndef lengthprey_h
#define lengthprey_h

#include "prey.h"

/**
 * \class LengthPrey
 * \brief This is the class used to model a simple length based prey
 * \note This class is only used to model non-dynamic prey (ie the prey from OtherFood)
 */
class LengthPrey : public Prey {
public:
  /**
   * \brief This is the LengthPrey constructor
   * \param lengths is the DoubleVector of length groups of the prey (only one length group)
   * \param areas is the IntVector of areas that the prey lives on
   * \param Energy is the energy content of the prey
   * \param givenname is the name of the prey
   */
  LengthPrey(const DoubleVector& lengths, const IntVector& areas, double Energy, const char* givenname);
  /**
   * \brief This is the default LengthPrey destructor
   */
  ~LengthPrey() {};
  /**
   * \brief This will calculate the amount of prey that is consumed for a given area and timestep
   * \param NumberInArea is the PopInfoVector giving the amount of prey in the area
   * \param area is the area that the prey consumption is being calculated on
   */
  virtual void Sum(const PopInfoVector& NumberInArea, int area);
};

#endif
