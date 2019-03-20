#include "QbicFactory.h"

#include "PDF.h"
#include "TriangularFactory.h"
#include "ErrorPropagator.h"

#include <cmath>
#include <vector>
#include <iostream>

using namespace std;

class Convoluter: public ErrorPropagator{
  public:
    Convoluter(const vector<PDF*>* vp):
      ErrorPropagator(vp){}
    ~Convoluter(){}
    
    double f() const{
      double x = sim_sample->at(0);
      double y = sim_sample->at(1);
      return x + y;
    }
};

QbicFactory::QbicFactory(double c, double d1, double d2):
  center(c),
  D1(d1),
  D2(d2),
  inf(center - D1/2 - D2/2),
  sup(center + D1/2 + D2/2){
}

QbicFactory::~QbicFactory(){
}


double QbicFactory::f (double x) const{//unused
  //tirvial cases
  if(x < inf) return 0;
  if(x > sup) return 0;
  return 0;
}

PDF* QbicFactory::create(double min, double max, unsigned int steps, const std::string& name) const {
  TriangularFactory f1(center/2,D1);
  PDF* t1 = f1.create_default(4*steps);
  TriangularFactory f2(center/2,D2);
  PDF* t2 = f2.create_default(3*steps);
  vector<PDF*> v{t1,t2};
  
  Convoluter c(&v);
  unsigned int n = (steps > 250) ? 1e06 : 0;
  PDF* fin = c.propagation(n,1,min,max,2*steps,name);
  fin->coarse(2);
  
  return fin;
}

PDF* QbicFactory::create_default(unsigned int steps) const {  
  return create(inf,sup,steps,"default_Qbic");
}



