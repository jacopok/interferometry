#ifndef GaussFactory_h
#define GaussFactory_h

#include "PDFFactory.h"

class GaussFactory: public PDFFactory {
  
  public:
    
    GaussFactory(double m, double sig);
    ~GaussFactory();
    
    virtual double f (double x) const;
    virtual PDF* create_default(unsigned int steps) const;
    
  private:
    
    GaussFactory(const GaussFactory& x);
    GaussFactory& operator=(const GaussFactory& x);
    
    double mu;
    double sigma;
  
};


#endif