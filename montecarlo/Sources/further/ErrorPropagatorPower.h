#ifndef ErrorPropagatorPower_h
#define ErrorPropagatorPower_h

#include "ErrorPropagator.h"

#include <vector>
using std::vector;

class ErrorPropagatorPower: public ErrorPropagator {
  
  public:
  
     ErrorPropagatorPower(const vector<PDF*>* vp, vector<double>* c, vector<double>* exp);
     ~ErrorPropagatorPower();
     
     double f() const;
  
  private:
    
     ErrorPropagatorPower(const ErrorPropagatorPower& e);
     ErrorPropagatorPower& operator=(const ErrorPropagatorPower& e);
     
     vector<double>* coeffs;
     vector<double>* exponents;
};




#endif