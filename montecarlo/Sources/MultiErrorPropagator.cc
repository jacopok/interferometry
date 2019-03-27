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
  
  if(n > 0){
    srandom(seed);
    cout << "Starting simulation: (" << n << ")" << endl;
    
    for(unsigned int i = 0; i < n; i++){
      simulate_sample();
      result->add(f());
      ProgressBar::percentages(i,n);
    }
  }
  else{
    unsigned int a_che_punto_siamo = 0, maxcount = 1;
    vector<double>* ms;
    double val = 1;
    MultiPDF* pre = MultiPDF::merge(vP,"pre");
    vector<MultiPDF*>* all = new vector<MultiPDF*>(*vMP);
    all->push_back(pre);
    
    for(unsigned int i = 0; i < all->size(); i++){
      maxcount *= all->at(i)->getSize();
      all->at(i)->initialize_counters();
    }
    
    cout << "Starting simulation (" << maxcount << ")" << endl << endl;
    
    while(a_che_punto_siamo < maxcount){
      //fill sim_sample
      val = 1;
      for(unsigned int i = 0; i < all->size(); i++){
	ms = all->at(i)->coordinates();
	val *= *(all->at(i)->access());
	for(unsigned int u = 0; u < all->at(i)->getDimension(); u++)
	  sim_sample->at(all->at(i)->getPDFs()->at(u)->getName()) = ms->at(u);
	delete ms;
      }
      
      result->add(f(),val);
      
      //update
      for(unsigned int i = 0; i < all->size(); i++){
	if(all->at(i)->update_counters())
	  break;
	else{
	  all->at(i)->initialize_counters();
	}
      }
      
      ProgressBar::percentages(a_che_punto_siamo,maxcount);
      a_che_punto_siamo++;
    }
    
    all = 0;
    delete all;
    delete pre;
  }
  result->normalize();
  
  return;
}