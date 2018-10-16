#ifndef PDFFactory_h
#define PDFFactory_h

#include <string>

class PDF;

class PDFFactory {

  public:
    
    PDFFactory();
    virtual ~PDFFactory();
    
    PDF* create(double min, double max, unsigned int steps, const std::string& name) const;
    virtual PDF* create_default(unsigned int steps) const = 0;
  
  protected:
    
    virtual double f (double x) const = 0;
    

};



#endif