#include "DigitalFactory.h"

#include "PDF.h"
#include "ErrorPropagator.h"
#include "UniformFactory.h"
#include "DataSimulator.h"

#include <cmath>
#include <vector>

using namespace std;

class ErrorPropagatorD: public ErrorPropagator {
  public:
    ErrorPropagatorD(const vector<PDF*>* vp):
      ErrorPropagator(vp){}
    ~ErrorPropagatorD(){}
    
    double f() const {
      double u = DataSimulator::simulate_one(vP->at(0));
      double k = DataSimulator::simulate_one(vP->at(1));
      return u*k;
    }
};


DigitalFactory::DigitalFactory(double v, double digit, double gain):
  V(v),
  Digit(digit),
  Gain(gain),
  inf((V - Digit > 0) ? (V - Digit)*(1 - Gain) : (V - Digit)*(1 + Gain)),
  sup((V + Digit > 0) ? (V + Digit)*(1 + Gain) : (V + Digit)*(1 - Gain)){
}

DigitalFactory::~DigitalFactory(){
}


double DigitalFactory::f (double x) const{//uniform in [V - Digit: V + Digit]
  if(x < V - Digit)	return 0;
  if(x > V + Digit) 	return 0;
  return 1/(2*Digit);
}

PDF* DigitalFactory::create(double min, double max, unsigned int steps, const std::string& name) const {
  if(Gain == 0)
    return PDFFactory::create(min,max,steps,name);
  
  PDF* u = PDFFactory::create(V - Digit,V + Digit,steps);
  UniformFactory kF(1 - Gain, 1 + Gain);
  PDF* k = kF.create_default(steps);
  
  vector<PDF*> vp {u,k};
  ErrorPropagatorD d(&vp);
  PDF* dig = d.propagation(100000,1,min,max,steps,name);
  
  delete u;
  delete k;
  
  return dig;
}

PDF* DigitalFactory::create_default(unsigned int steps) const{
  return create(inf,sup,steps,"default_Digital");
}