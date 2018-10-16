#ifndef UniformFactory_h
#define UniformFactory_h

#include "PDFFactory.h"

class UniformFactory: public PDFFactory {
  
  public:
    
    UniformFactory(double a, double b);
    ~UniformFactory();
    
    virtual double f (double x) const;
    virtual PDF* create_default(unsigned int steps) const;
    
  private:
    
    UniformFactory(const UniformFactory& x);
    UniformFactory& operator=(const UniformFactory& x);
    
    double inf;
    double sup;
  
};


#endif
