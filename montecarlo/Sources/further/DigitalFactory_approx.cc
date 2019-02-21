#include "DigitalFactory.h"

#include "PDF.h"

#include <cmath>
#include <vector>

using namespace std;


DigitalFactory::DigitalFactory(double v, double digit, double gain):
  V(v),
  Digit(digit),
  Gain(gain),
  inf((V > 0) ? V*(1 - Gain) - Digit : V*(1 + Gain) - Digit),
  sup((V > 0) ? V*(1 + Gain) + Digit : V*(1 - Gain) + Digit){
}

DigitalFactory::~DigitalFactory(){
}


double DigitalFactory::f (double x) const{//uniform in [V - Digit: V + Digit]
  double n_min = (inf + Digit < sup - Digit) ? inf + Digit : sup - Digit;
  double n_max = (inf + Digit > sup - Digit) ? inf + Digit : sup - Digit;
  
  if(x < inf - Digit) return 0;
  if(x < n_min) return x - inf - Digit;
  if(x < n_max) return n_min - inf - Digit;
  if(x < sup + Digit) return sup + Digit - x;
  else return 0;  
}

PDF* DigitalFactory::create(double min, double max, unsigned int steps, const std::string& name) const {
  PDF* dig = PDFFactory::create(min,max,steps,name);
  dig->normalize();
  return dig;
}

PDF* DigitalFactory::create_default(unsigned int steps) const{
  return create(inf,sup,steps,"default_Digital");
}