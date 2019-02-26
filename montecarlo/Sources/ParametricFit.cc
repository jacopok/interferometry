#include "ParametricFit.h"

#include "PDF.h"
#include "MultiPDF.h"
#include "DataSimulator.h"
#include "ProgressBar.h"

#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <cmath>

using namespace std;


ParametricFit::ParametricFit(Func* g):
  f(g),
  fixed_parameters(new vector<PDF*>),
  extern_data_vectors(false),
  min_value(0),
  data_x(new vector<double>),
  data_y_PDF(new vector<PDF*>),
  unknown_MultiPDF(new MultiPDF("unknownMP")){
  }
  
ParametricFit::ParametricFit():
  ParametricFit(0){
  }
  
ParametricFit::~ParametricFit(){
  clear();
  delete fixed_parameters;
  delete data_x;
  delete data_y_PDF;
  delete unknown_MultiPDF;
}
    
    
void ParametricFit::set_func(Func* g){
  f = g;
  return;
}

void ParametricFit::add_fixed_parameter(PDF* p){
  fixed_parameters->push_back(p);
  return;
}

void ParametricFit::delete_fixed_parameters(){
  fixed_parameters->clear();
  return;
}

void ParametricFit::add_unknown_parameter(double min, double max, unsigned int steps, const string& name){
  unknown_MultiPDF->add_PDF(new PDF(min,max,steps,name));
  return;
}

void ParametricFit::delete_unknown_parameters(){
  for(unsigned int u = 0; u < unknown_MultiPDF->getDimension(); u++)
    delete unknown_MultiPDF->getPDFs()->at(u);
  unknown_MultiPDF->clear();
  return;
}

void ParametricFit::add_data(double x, PDF* y_PDF){
  if(extern_data_vectors){//copy data into *this in order not to modify extern data vectors;
    vector<double>* aux_x = data_x;
    vector<PDF*>* aux_y_PDF = data_y_PDF;
    data_x = 0;
    data_y_PDF = 0;
    delete data_x;
    delete data_y_PDF;
    data_x = new vector<double>(*aux_x);
    data_y_PDF = new vector<PDF*>(*aux_y_PDF);
    extern_data_vectors = false;
  }
  data_x->push_back(x);
  data_y_PDF->push_back(y_PDF);
  return;
}

void ParametricFit::set_data(vector<double>* xV, vector<PDF*>* yVP){//data are NOT copied into *this
  delete_data();
  extern_data_vectors = true;
  data_x = xV;
  data_y_PDF = yVP;
  return;
}

void ParametricFit::delete_data(){
  if(extern_data_vectors){//I don't want to delete extern data
    data_x = 0;
    delete data_x;
    data_x = new vector<double>;
    data_y_PDF = 0;
    delete data_y_PDF;
    data_y_PDF = new vector<PDF*>;
  }
  else{
    data_x->clear();
    data_y_PDF->clear();
  }
  extern_data_vectors = false;
  return;
}

void ParametricFit::clear(){
  f = 0;
  delete_fixed_parameters();
  delete_unknown_parameters();
  delete_data();
  return;
}

MultiPDF* ParametricFit::get_unknown_MultiPDF() const{
  return new MultiPDF(*unknown_MultiPDF);
}

bool ParametricFit::isready() const{
  if(f == 0){
    cout << "Fit FAILED: no given function" << endl;
    return false;
  }
  if(f->n_fix() != fixed_parameters->size()){
    cout << "Fit FAILED: number of fixed parameters doesn't match with the given function" << endl;
    return false;
  }
  if(f->n_unk() != unknown_MultiPDF->getDimension()){
    cout << "Fit FAILED: number of unknown parameters doesn't match with the given function" << endl;
    return false;
  }
  if(data_x->size() == 0){
    cout << "Fit FAILED: no given data" << endl;
    return false;
  }
  if(min_value >= 0.5){
    cout << "WARNING: min_value >= 0.5: expect large PDFs (if you are using a brute_force fit ignore this warning)" << endl;
  }
  
  return true;
}

void ParametricFit::set_min_value(double m){
  min_value = m;
  return;
}


void ParametricFit::fit(unsigned int n_rep, unsigned int seed, mode q){
  if(!isready()) return;
  
  //clean previous fit
  unknown_MultiPDF->zero();
  
  //useful variables
  unsigned int n_unk = unknown_MultiPDF->getDimension();
  double sum = 0;
  double partial_sum = 0;
  unsigned int a_che_punto_siamo = 0, maxcount = unknown_MultiPDF->getSize();
  
  //vector of single values (to be plugged in f)
  vector<double> v_unk(n_unk,0);
  vector<double> v_fix(fixed_parameters->size(),0);
  
  srandom(seed);
  
  //fit
  switch(q){
    case value:
    case p_value:
      
      double y, yv;
      double offset;
      //iterate over unknown_parameters
      unknown_MultiPDF->initialize_counters();
      do{
      //while(a_che_punto_siamo < maxcount){
	
	//initialize single values for unknown_parameters
	for(unsigned int u = 0; u < n_unk; u++)
	  v_unk[u] = unknown_MultiPDF->getPDFs()->at(u)->getMin() + (0.5 + unknown_MultiPDF->getCounters()->at(u))*unknown_MultiPDF->getPDFs()->at(u)->getDx();
	
	//iterate over fixed parameters
	sum = 0;
	for(unsigned int j = 0; j < n_rep; j++){
	  
	  //initialize fixed parameters
	  for(unsigned int k = 0; k < fixed_parameters->size(); k++)
	    v_fix[k] = DataSimulator::simulate_one(fixed_parameters->at(k));
	  
	  //iterate over data
	  partial_sum = 1;
	  
	  if(min_value == 0){
	    for(unsigned int h = 0; h < data_x->size(); h++){
	      //get y
	      y = f->f(data_x->at(h),&v_fix,&v_unk);
	      
	      //compare with y_PDF
	      if(q == value)
		yv = data_y_PDF->at(h)->value(y); 
	      else{
		yv = 10 * data_y_PDF->at(h)->p_value(y);//the 10 factor helps with precision
	      }
	      
	      partial_sum *= yv;
	      if(yv == 0) break;
	    }
	  }
	  else{
	    offset = 1;
	    for(unsigned int h = 0; h < data_x->size(); h++){
	      //get y
	      y = f->f(data_x->at(h),&v_fix,&v_unk);
	      
	      //compare with y_PDF
	      if(q == value){
		partial_sum *= (min_value + data_y_PDF->at(h)->value(y));
		offset *= min_value;
	      }
	      else{
		partial_sum *= 10 * (min_value + data_y_PDF->at(h)->p_value(y));//the 10 factor helps with precision
		offset *= 10 * min_value;
	      }
	    }
	    partial_sum -= offset;
	  }
	  //update sum with the average value (p_value)
	  sum += partial_sum;//data_x->size();

	}
	
	//update the MultiPDF
	*(unknown_MultiPDF->access()) += sum/n_rep;
	
	//ProgressBar
	a_che_punto_siamo++;
	ProgressBar::percentages(a_che_punto_siamo,maxcount);
	
      }while(unknown_MultiPDF->update_counters());
      
      break;
    
    case brute_force:
      
      vector<double> yPoints(data_x->size(),0);
      vector<unsigned int> best_counters(n_unk,0);
      bool first_over_unk;
      
      //iterate over repetitions
      for(unsigned int j = 0; j < n_rep; j++){
	
	//initialize fixed parameters
	for(unsigned int k = 0; k < fixed_parameters->size(); k++)
	    v_fix[k] = DataSimulator::simulate_one(fixed_parameters->at(k));
	
	//initialize y data
	for(unsigned int h = 0; h < data_x->size(); h++)
	  yPoints[h] = DataSimulator::simulate_one(data_y_PDF->at(h));
	
	//initialize counters for scanning
	unknown_MultiPDF->initialize_counters();	
	//scan over unknown parameter to find the best n-uple with the mininum squares method
	do{
	//while(a_che_punto_siamo < maxcount){
	  
	  //initialize single values for unknown_parameters
	  for(unsigned int u = 0; u < n_unk; u++)
	    v_unk[u] = unknown_MultiPDF->getPDFs()->at(u)->getMin() + (0.5 + unknown_MultiPDF->getCounters()->at(u))*unknown_MultiPDF->getPDFs()->at(u)->getDx();
	  
	  //evaluate the sum of squares
	  sum = 0;
	  for(unsigned int h = 0; h < data_x->size(); h++)
	    sum += pow( (yPoints[h] - f->f(data_x->at(h),&v_fix,&v_unk)) ,2);
	  
	  if(first_over_unk)
	    partial_sum = sum;
	  
	  //update best_counters
	  if(sum < partial_sum){
	    partial_sum = sum;
	    best_counters = *(unknown_MultiPDF->getCounters());
	  }
	  
	  first_over_unk = false;
	}while(unknown_MultiPDF->update_counters());
	
	//update the MultiPDF
	*(unknown_MultiPDF->access(&best_counters)) += 1.0;
	
	//ProgressBar
	ProgressBar::percentages(j,n_rep);
      }
  }
      
  unknown_MultiPDF->normalize();

  return;
}
