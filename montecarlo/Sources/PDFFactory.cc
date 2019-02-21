#include "PDFFactory.h"

#include "PDF.h"

#include <vector>

using namespace std;

PDFFactory::PDFFactory(){
}

PDFFactory::~PDFFactory(){
  
}


PDF* PDFFactory::create(double min, double max, unsigned int steps, const std::string& name) const{
  double dx = (max - min)/steps;
  vector<double> v;
  v.reserve(steps);
  
  for(unsigned int i = 0; i < steps; i++)
    v.push_back(f(min + (i + 0.5)*dx));
  
  return new PDF(min,max,v, name);
}