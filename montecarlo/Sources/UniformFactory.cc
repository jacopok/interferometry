#include "UniformFactory.h"

#include <cmath>

using namespace std;


UniformFactory::UniformFactory(double a, double b):
  inf(a),
  sup(b){
    if(a > b){
      inf = b;
      sup = a;
    }
}

UniformFactory::~UniformFactory(){
}


double UniformFactory::f (double x) const{
  if(x < inf) return 0;
  if(x > sup) return 0;
  return 1.0/(sup - inf);
}

PDF* UniformFactory::create_default(unsigned int steps) const{
  return create(inf,sup,steps,"default_Uniform");
}