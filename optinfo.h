#ifndef optinfo_h
#define optinfo_h

#include "ecosystem.h"
#include "parameter.h"

class OptInfo {
public:
  OptInfo();
  virtual ~OptInfo();
  virtual void MaximizeLikelihood() = 0;
  virtual void Read(CommentStream& infile) = 0;
  int Read(CommentStream& infile, char* text);
protected:
  int nopt;
};

class OptInfoHooke : public OptInfo {
public:
  OptInfoHooke();
  virtual ~OptInfoHooke();
  virtual void Read(CommentStream& infile);
  int Read(CommentStream& infile, char* text);
  virtual void MaximizeLikelihood();
protected:
  int MaximumIterations;  //Maximum nr of iteration
  double Rho_begin;       //Reduction factor for step length
  double lambda;          //Inital step length
  double epsmin;          //Minimum step length
};

class OptInfoSimann : public OptInfo {
public:
  OptInfoSimann();
  virtual ~OptInfoSimann();
  virtual void Read(CommentStream& infile);
  int Read(CommentStream& infile, char* text);
  virtual void MaximizeLikelihood();
protected:
  double rt;
  double eps;
  int ns;
  int nt;
  double T;
  double cs;
  double vm;
  long maxim;
  int maxevl;
};

class OptInfoHookeAndSimann : public OptInfo {
public:
  OptInfoHookeAndSimann();
  virtual ~OptInfoHookeAndSimann();
  virtual void Read(CommentStream& infile);
  int Read(CommentStream& infile, char* text);
  virtual void MaximizeLikelihood();
protected:
  int HookeMaxIter;
  double Rho_begin;
  double lambda;
  double epsmin;
  int SimannMaxIter;
  double rt;
  double eps;
  int ns;
  int nt;
  double T;
  double cs;
  double vm;
  long maxim;
};

/* JMB - commented out the BFGS stuff
class OptInfoNRecipes : public OptInfo {
public:
  OptInfoNRecipes();
  virtual ~OptInfoNRecipes();
  virtual void Read(CommentStream& infile);
  int Read(CommentStream& infile, char* text);
  virtual void MaximizeLikelihood();
protected:
  double ftol;
  double gtol;
  int bfgs;
}; */

#endif
