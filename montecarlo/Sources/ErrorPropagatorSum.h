#ifndef ErrorPropagatorSum_h
#define ErrorPropagatorSum_h

#include "ErrorPropagator.h"

#include <vector>
using std::vector;

class PDF;

class ErrorPropagatorSum: public ErrorPropagator {
  
  public:
  
     ErrorPropagatorSum(const vector<PDF*>* vP);
     ~ErrorPropagatorSum();
     
     double f() const;
  
  private:
    
     ErrorPropagatorSum(const ErrorPropagatorSum& e);
     ErrorPropagatorSum& operator=(const ErrorPropagatorSum& e);
     
     vector<double>* coeffs;
};


#endif