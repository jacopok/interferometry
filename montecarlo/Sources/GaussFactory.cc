#include "GaussFactory.h"

#include <cmath>

using namespace std;


GaussFactory::GaussFactory(double m, double sig):
  mu(m),
  sigma(sig){
}

GaussFactory::~GaussFactory(){
}


double GaussFactory::f (double x) const{
  return (exp(-pow(x - mu,2)/(2*sigma*sigma)))/(sigma*sqrt(2*M_PI));
}

PDF* GaussFactory::create_default(unsigned int steps) const{
  return create(mu - 5*sigma, mu + 5*sigma,steps,"default_Gauss");
}