#ifndef naturalm_h
#define naturalm_h

#include "areatime.h"
#include "commentstream.h"
#include "timevariableindexvector.h"
#include "keeper.h"

class NaturalM {
public:
  /**
   * \brief This is the NaturalM constructor
   * \param infile is the CommentStream to read the NaturalM information from
   * \param minage is the minimum age of the stock
   * \param maxage is the maximum age of the stock
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  NaturalM(CommentStream& infile, int minage, int maxage,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default NaturalM destructor
   */
  ~NaturalM() {};
  const DoubleVector& ProportionSurviving(const TimeClass* const TimeInfo) const;
  const DoubleIndexVector& getMortality() const;
  /**
   * \brief This function will reset the NaturalM information
   * \param TimeInfo is the TimeClass for the current model
   */
  void Reset(const TimeClass* const TimeInfo);
  /**
   * \brief This function will print the summary NaturalM information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile);
protected:
  TimeVariableIndexVector mortality;
  DoubleVector proportion;
};

#endif
