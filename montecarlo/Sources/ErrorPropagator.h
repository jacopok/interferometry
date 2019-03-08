#ifndef ErrorPropagator_h
#define ErrorPropagator_h

#include <vector>
#include <string>
using std::string;
using std::vector;

class PDF;

class ErrorPropagator {
  
  public:
  
      ErrorPropagator(const vector<PDF*>* v);
      virtual ~ErrorPropagator();
      
      //function of the PDFs in vP
      virtual double f() const = 0;
      
      //builds the PDF of the aleatory variable f
      PDF* propagation(unsigned int n, unsigned int seed,
		       double min, double max, unsigned int steps, const string& name) const;
      //n is the number of simulations to create the PDF
  
  protected:
    
    ErrorPropagator(const ErrorPropagator& e);
    ErrorPropagator& operator=(const ErrorPropagator& e);
    
    const vector<PDF*>* vP;
    mutable vector<double>* sim_sample;
};




#endif