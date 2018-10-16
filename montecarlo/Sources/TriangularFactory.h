#ifndef TriangularFactory_h
#define TriangularFactory_h

#include "PDFFactory.h"

class TriangularFactory: public PDFFactory {
  
  public:
    
    TriangularFactory(double c, double disp);
    ~TriangularFactory();
    
    virtual double f (double x) const;
    virtual PDF* create_default(unsigned int steps) const;
    
  private:
    
    TriangularFactory(const TriangularFactory& x);
    TriangularFactory& operator=(const TriangularFactory& x);
    
    //the triangle has vertex in center and base delta
    double center;
    double delta;
  
};


#endif