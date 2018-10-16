#include "ErrorPropagator.h"
#include "ProgressBar.h"

#include "PDF.h"
#include <iostream>

using namespace std;

ErrorPropagator::ErrorPropagator(const vector<PDF*>* v):
  vP(v){
    
}

ErrorPropagator::~ErrorPropagator(){
}


PDF* ErrorPropagator::propagation(unsigned int n, unsigned int seed,
				  double min, double max, unsigned int steps, const string& name) const{
   
  vector<double> values(steps,0.);
  PDF* sim_pdf = new PDF(min, max, values, name);
  
  srandom(seed);
  cout << "Starting simulation: (" << n << ")" << endl;
  //filling the sim_PDF
  for(unsigned int i = 0; i < n; i++){
    sim_pdf->add(f());
    ProgressBar::percentages(i,n);
  }
  
  sim_pdf->normalize();
  
  return sim_pdf;
}