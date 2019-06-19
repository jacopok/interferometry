#include "TriangularFactory.h"

#include <cmath>

using namespace std;


TriangularFactory::TriangularFactory(double c, double disp):
  center(c),
  delta(disp){
}

TriangularFactory::~TriangularFactory(){
}


double TriangularFactory::f (double x) const{
  if(x < center - delta/2)	return 0;
  if(x > center + delta/2) 	return 0;
  double h = 2.0/delta;
  if(x < center) 
	return h*(1.0 + 2*(x - center)/delta);
  return h*(1.0 - 2*(x - center)/delta);
}

PDF* TriangularFactory::create_default(unsigned int steps) const{
  return create(center - delta/2,center + delta/2,steps,"default_Triangular");
}
