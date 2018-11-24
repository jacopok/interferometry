#include "DigitalFactory.h"

#include "PDF.h"

#include <cmath>
#include <vector>
#include <iostream>

using namespace std;

DigitalFactory::DigitalFactory(double v, double digit, double gain):
  V(v),
  Digit(digit),
  Gain(gain),
  inf((V - Digit > 0) ? (V - Digit)*(1 - Gain) : (V - Digit)*(1 + Gain)),
  sup((V + Digit > 0) ? (V + Digit)*(1 + Gain) : (V + Digit)*(1 - Gain)){
}

DigitalFactory::~DigitalFactory(){
}


double DigitalFactory::f (double x) const{
  //tirvial cases
  if(Gain == 0){//uniform in [V - Digit: V + Digit]
    if(x < V - Digit)	return 0;
    if(x > V + Digit) 	return 0;
    return 1/(2*Digit);
  }
  if(Digit == 0){//uniform in [V*(1 - Gain): V*(1 + Gain)]
    if(x < V*(1 - Gain)) return 0;
    if(x > V*(1 + Gain)) return 0;
    return 1/(2*V*Gain);
  }
  
  //proper digital PDF
  if(x == 0) return 1/(2*Digit);
  if(x > 0){
    if(V - Digit > 0) return eta(x/(V + Digit),x/(V - Digit));
    //now V - Digit <= 0
    if(V + Digit > 0) return eta(x/(V + Digit),10); //10 > 1 + Gain
    return 0;
  }
  //now x < 0
  if(V + Digit < 0) return eta(x/(V - Digit),x/(V + Digit));
  //now V + Digit >= 0
  if(V - Digit < 0) return eta(x/(V - Digit),10); //10 > 1 + Gain
  return 0;
}

PDF* DigitalFactory::create(double min, double max, unsigned int steps, const std::string& name) const {
  PDF* dig = PDFFactory::create(min,max,steps,name);
  dig->normalize();
  return dig;
}

PDF* DigitalFactory::create_default(unsigned int steps) const {  
  return create(inf,sup,steps,"default_Digital");
}

double DigitalFactory::eta(double a, double b) const {
  if(b <= 1 - Gain) return 0;
  if(a >= 1 + Gain) return 0;
  //now a < 1 + Gain, b > 1 - Gain
  if((a <= 1 - Gain)&&(b <= 1 + Gain)) return (b - 1 + Gain)/(4*Gain*Digit);
  if((a <= 1 - Gain)&&(1 + Gain < b)) return 1/(2*Digit);
  //now a > 1 - Gain
  if(b <= 1 + Gain) return (b - a)/(4*Gain*Digit);
  if(1 + Gain < b) return (1 + Gain - a)/(4*Gain*Digit);
  //if it proceeds there must be an error
  cout << "DigitalFactory::eta> ERROR occured" << endl;
  return 0;
}

