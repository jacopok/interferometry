//produces a PDF that is a convolution of two triangular PDFs with fullwidth d1,d2

#ifndef QbicFactory_h
#define QbicFactory_h

#include "PDFFactory.h"

class QbicFactory: public PDFFactory {
  
  public:
    
    QbicFactory(double c, double d1, double d2);
    ~QbicFactory();
    
    virtual double f (double x) const;
    virtual PDF* create(double min, double max, unsigned int steps, const std::string& name = "Untitled_PDF") const;
    virtual PDF* create_default(unsigned int steps) const;
    
  private:
    
    QbicFactory(const QbicFactory& x);
    QbicFactory& operator=(const QbicFactory& x);
    
    //the triangle has vertex in center and base delta
    double center;
    double D1;
    double D2;
    
    double inf;
    double sup;
    
  
};


#endif