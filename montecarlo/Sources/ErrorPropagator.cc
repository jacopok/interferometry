#include "ErrorPropagator.h"

#include "ProgressBar.h"
#include "DataSimulator.h"
#include "MultiPDF.h"
#include "PDF.h"
#include <iostream>

using namespace std;

ErrorPropagator::ErrorPropagator(const vector<PDF*>* v):
  vP(v),
  sim_sample(new vector<double>){
    //cout << "New ErrorPropagator" << endl;
}

ErrorPropagator::~ErrorPropagator(){
  delete sim_sample;
}


PDF* ErrorPropagator::propagation(unsigned int n, unsigned int seed,
				  double min, double max, unsigned int steps, const string& name) const{
   
  vector<double> values(steps,0.);
  PDF* sim_pdf = new PDF(min, max, values, name);
  
  if(n > 0){
    srandom(seed);
    cout << "Starting simulation: (" << n << ")" << endl;
    //filling the sim_PDF
    for(unsigned int i = 0; i < n; i++){
      sim_sample = DataSimulator::simulate_sample(vP);
      sim_pdf->add(f());
      
      ProgressBar::percentages(i,n);
      delete sim_sample;
    }
    sim_sample = new vector<double>;
  }
  else{
    delete sim_sample;
    sim_sample = new vector<double>(vP->size(),0);
    MultiPDF* pre = MultiPDF::merge(vP,"pre");
    unsigned int maxcount = pre->getSize(), aChePuntoSiamo = 0;
    pre->initialize_counters();
    double coeff = 1.0;
    for(unsigned int u = 0; u < pre->getDimension(); u++)
      coeff *= vP->at(u)->getDx()*vP->at(u)->getSteps();
    
    do{
      for(unsigned int u = 0; u < pre->getDimension(); u++)
	sim_sample->at(u) = vP->at(u)->getMin() + (0.5 + pre->getCounters()->at(u))*vP->at(u)->getDx();
      
      sim_pdf->add(f(),*(pre->access()));
      
      aChePuntoSiamo++;
      ProgressBar::percentages(aChePuntoSiamo,maxcount);
    }while(pre->update_counters());
    
    delete pre;
    delete sim_sample;
    sim_sample = new vector<double>;
  }
  
  sim_pdf->normalize();
  
  return sim_pdf;
}