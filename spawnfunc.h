#ifndef spawnfunc_h
#define spawnfunc_h

#include "commentstream.h"
#include "keeper.h"
#include "timevariablevector.h"

/**
 * \class SpawnFunc
 * \brief This is the base class used to calculate the spawning level
 * \note This will always be overridden by the derived classes that actually calculate the spawning level
 */
class SpawnFunc {
public:
  /**
   * \brief This is the default SpawnFunc constructor
   */
  SpawnFunc();
  /**
   * \brief This is the default SpawnFunc destructor
   */
  virtual ~SpawnFunc();
  /**
   * \brief This will set the name of the spawning function
   * \param suitfuncName is the name of the spawning function
   */
  void setName(const char* suitfuncName);
  /**
   * \brief This will return the name of the spawning function
   * \return name
   */
  const char* getName();
  const TimeVariableVector& getConstants() const;
  void readConstants(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper);
  void updateConstants(const TimeClass* const TimeInfo);
  int constantsHaveChanged(const TimeClass* const TimeInfo);
  /**
   * \brief This will return the spawning level that has been calculated
   * \param len is the length of the spawner
   * \return 0 (will be overridden in derived classes)
   */
  virtual double calculate(double len) = 0;
  /**
   * \brief This will return the number of constants used to calculate the spawning value
   * \return number
   */
  int noOfConstants();
protected:
  /**
   * \brief This is the name of the spawning function
   */
  char* name;
  /**
   * \brief This is the vector of the spawning function constants
   */
  TimeVariableVector coeff;
};

/**
 * \class ConstSpawnFunc
 * \brief This is the class used to calculate a constant spawning level
 */
class ConstSpawnFunc : public SpawnFunc {
public:
  /**
   * \brief This is the default ConstSpawnFunc constructor
   */
  ConstSpawnFunc();
  /**
   * \brief This is the default ConstSpawnFunc destructor
   */
  virtual ~ConstSpawnFunc();
  /**
   * \brief This will return the spawning level that has been calculated
   * \param len is the length of the spawner
   * \return spawning level
   */
  virtual double calculate(double len);
};

/**
 * \class ExpSpawnFunc
 * \brief This is the class used to calculate the spawning level based on an exponential function of length
 */
class ExpSpawnFunc : public SpawnFunc {
public:
  /**
   * \brief This is the default ExpSpawnFunc constructor
   */
  ExpSpawnFunc();
  /**
   * \brief This is the default ExpSpawnFunc destructor
   */
  virtual ~ExpSpawnFunc();
  /**
   * \brief This will return the spawning level that has been calculated
   * \param len is the length of the spawner
   * \return spawning level
   */
  virtual double calculate(double len);
};

/**
 * \class StraightSpawnFunc
 * \brief This is the class used to calculate the spawning level based on a linear function of length
 */
class StraightSpawnFunc : public SpawnFunc {
public:
  /**
   * \brief This is the default StraightSpawnFunc constructor
   */
  StraightSpawnFunc();
  /**
   * \brief This is the default StraightSpawnFunc destructor
   */
  virtual ~StraightSpawnFunc();
  /**
   * \brief This will return the spawning level that has been calculated
   * \param len is the length of the spawner
   * \return spawning level
   */
  virtual double calculate(double len);
};

#endif
