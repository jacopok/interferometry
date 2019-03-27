#include "MultiErrorPropagator.h"
#include "ProgressBar.h"
#include "DataSimulator.h"

#include "PDF.h"
#include "MultiPDF.h"
#include <iostream>

using namespace std;

MultiErrorPropagator::MultiErrorPropagator(const vector<MultiPDF*>* vm, const vector<PDF*>* v, const string& name):
  vMP(vm),
  vP(v),
  sim_sample(new map<string,double>),
  result(new MultiPDF(name)){
    for(unsigned int i = 0; i < vm->size(); i++)
      for(unsigned int u = 0; u < vm->at(i)->getDimension(); u++)
	sim_sample->insert(make_pair(vm->at(i)->getPDFs()->at(u)->getName(),0));
      
    for(unsigned int i = 0; i < v->size(); i++)
      sim_sample->insert(make_pair(v->at(i)->getName(),0));
}

MultiErrorPropagator::~MultiErrorPropagator(){
  delete sim_sample;
  delete result;
}

void MultiErrorPropagator::simulate_sample() const{
  vector<double>* ms;
  for(unsigned int i = 0; i < vMP->size(); i++){
    ms = DataSimulator::multi_simulate_one(vMP->at(i));
    for(unsigned int u = 0; u < vMP->at(i)->getDimension(); u++)
      sim_sample->at(vMP->at(i)->getPDFs()->at(u)->getName()) = ms->at(u);
    delete ms;
  }
  
  for(unsigned int i = 0; i < vP->size(); i++)
    sim_sample->at(vP->at(i)->getName()) = DataSimulator::simulate_one(vP->at(i));
}

MultiPDF* MultiErrorPropagator::getResult() const{
  return result;
}

void MultiErrorPropagator::propagation(unsigned int n, unsigned int seed) const{
  //clear previous propagation
  result->zero();
  srandom(seed);
  cout << "Starting simulation: (" << n << ")" << endl;
  
  for(unsigned int i = 0; i < n; i++){
    simulate_sample();
    result->add(f());
    ProgressBar::percentages(i,n);
  }
  
  result->normalize();
  
  return;
}