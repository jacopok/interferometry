//produces a PDF that has a contribute of gain and another of digits: these two are considered indepent and with Uniform distribution
//It resembles a measurement made with a digital instrument (Oscilloscope or Multimeter)

#ifndef DigitalFactory_h
#define DigitalFactory_h

#include "PDFFactory.h"

class DigitalFactory: public PDFFactory {
  
  public:
    
    DigitalFactory(double v, double digit, double gain);
    ~DigitalFactory();
    
    virtual double f (double x) const;
    virtual PDF* create(double min, double max, unsigned int steps, const std::string& name = "Untitled_PDF") const;
    virtual PDF* create_default(unsigned int steps) const;
    
  private:
    
    DigitalFactory(const DigitalFactory& x);
    DigitalFactory& operator=(const DigitalFactory& x);
    
    //the triangle has vertex in center and base delta
    double V;
    double Digit;
    double Gain;
    
    double inf;
    double sup;
    
    double eta(double a, double b) const;
  
};


#endif