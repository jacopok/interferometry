#include "ParametricFit.h"

#include "PDF.h"
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
  unknown_parameters_min(new vector<double>),
  unknown_parameters_max(new vector<double>),
  unknown_parameters_steps(new vector<unsigned int>),
  unknown_parameters_name(new vector<string>),
  extern_data_vectors(false),
  min_value(0),
  data_x(new vector<double>),
  data_y_PDF(new vector<PDF*>),
  unknown_parameters_PDFs(new map<string,PDF*>){
  }
  
ParametricFit::ParametricFit():
  ParametricFit(0){
  }
  
ParametricFit::~ParametricFit(){
  clear();
  delete fixed_parameters;
  delete unknown_parameters_min;
  delete unknown_parameters_max;
  delete unknown_parameters_steps;
  delete unknown_parameters_name;
  delete data_x;
  delete data_y_PDF;
  delete unknown_parameters_PDFs;
}
    
    
void ParametricFit::set_func(Func* g){
  f = g;
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
  unknown_parameters_min->push_back(min);
  unknown_parameters_max->push_back(max);
  unknown_parameters_steps->push_back(steps);
  unknown_parameters_name->push_back(name);
  return;
}

void ParametricFit::delete_unknown_parameters(){
  unknown_parameters_min->clear();
  unknown_parameters_max->clear();
  unknown_parameters_steps->clear();
  unknown_parameters_name->clear();
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
  
  for(map<string,PDF*>::iterator it = unknown_parameters_PDFs->begin(); it != unknown_parameters_PDFs->end(); ++it)
    delete it->second;
  unknown_parameters_PDFs->clear();
}


PDF* ParametricFit::get_unknown_PDF(const string& name) const{
  return new PDF(*(unknown_parameters_PDFs->at(name))); //return a COPY of the PDF
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
  if(f->n_unk() != unknown_parameters_name->size()){
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
  for(map<string,PDF*>::iterator it = unknown_parameters_PDFs->begin(); it != unknown_parameters_PDFs->end(); ++it)
    delete it->second;
  unknown_parameters_PDFs->clear();
  
  //useful variables
  unsigned int n_unk = unknown_parameters_name->size();
  double sum = 0;
  double partial_sum = 0;
  vector<double>* unknown_parameters_dx = new vector<double>;
  PDF* auxPDF = 0;
  
  //counter for iterations amongst unknown_parameters
  vector<unsigned int> counters(n_unk,0);
  unsigned int maxcount = 1;
  unsigned int a_che_punto_siamo = 0;
  
  //vector of single values (to be plugged in f)
  vector<double> v_unk(n_unk,0);
  vector<double> v_fix(fixed_parameters->size(),0);
  
  //create vector of values for the unknown unknown_parameters_PDFs
  vector<vector<double>*> unk_values;
  unk_values.reserve(n_unk);
  for(unsigned int i = 0; i < n_unk; i++){
    unk_values.push_back(new vector<double>(unknown_parameters_steps->at(i),0));
    
    unknown_parameters_dx->push_back((unknown_parameters_max->at(i) - unknown_parameters_min->at(i))/unknown_parameters_steps->at(i));
    maxcount *= unknown_parameters_steps->at(i);
  }
  //set seed
  srandom(seed);
  
  //fit
  switch(q){
    case value:
    case p_value:
      
      double y;
      //iterate over unknown_parameters
      while(counters[n_unk - 1] < unknown_parameters_steps->at(n_unk - 1)){
      //while(a_che_punto_siamo < maxcount){
	
	//initialize single values for unknown_parameters
	for(unsigned int i = 0; i < n_unk; i++)
	  v_unk[i] = unknown_parameters_min->at(i) + counters[i]*unknown_parameters_dx->at(i);
	
	//iterate over fixed parameters
	sum = 0;
	for(unsigned int j = 0; j < n_rep; j++){
	  
	  //initialize fixed parameters
	  for(unsigned int k = 0; k < fixed_parameters->size(); k++)
	    v_fix[k] = DataSimulator::simulate_one(fixed_parameters->at(k));
	  
	  //iterate over data
	  partial_sum = 1;
	  for(unsigned int h = 0; h < data_x->size(); h++){
	    //get y
	    y = f->f(data_x->at(h),&v_fix,&v_unk);
	    
	    //compare with y_PDF
	    if(q == value)
	      partial_sum *= (min_value + data_y_PDF->at(h)->value(y));
	    else
	      partial_sum *= 10 * (min_value + data_y_PDF->at(h)->p_value(y));//the 10 factor helps with precision
	  }
	  
	  //update sum with the average value (p_value)
	  sum += partial_sum;//data_x->size();

	}
	
	//update unk_values
	for(unsigned int i = 0; i < n_unk; i++)
	  unk_values[i]->at(counters[i]) += sum/n_rep;
	
	
	//update counters
	for(unsigned int i = 0; i < n_unk; i++){
	  counters[i]++;
	  if((counters[i] < unknown_parameters_steps->at(i))||(i == n_unk - 1))
	    break;
	  else
	    counters[i] = 0;
	}
	
	//ProgressBar
	a_che_punto_siamo++;
	ProgressBar::percentages(a_che_punto_siamo,maxcount);
	
      }
      
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
	for(unsigned int &l : counters)
	  l = 0;
	partial_sum = 0;
	first_over_unk = true;
	
	//scan over unknown parameter to find the best n-uple with the mininum squares method
	while(counters[n_unk - 1] < unknown_parameters_steps->at(n_unk - 1)){
	//while(a_che_punto_siamo < maxcount){
	  
	  //initialize single values for unknown_parameters
	  for(unsigned int i = 0; i < n_unk; i++)
	    v_unk[i] = unknown_parameters_min->at(i) + counters[i]*unknown_parameters_dx->at(i);
	  
	  //evaluate the sum of squares
	  sum = 0;
	  for(unsigned int h = 0; h < data_x->size(); h++)
	    sum += pow( (yPoints[h] - f->f(data_x->at(h),&v_fix,&v_unk)) ,2);
	  
	  if(first_over_unk)
	    partial_sum = sum;
	  
	  //update best_counters
	  if(sum < partial_sum){
	    partial_sum = sum;
	    best_counters = counters;
	  }
	  
	  //update counters
	  for(unsigned int i = 0; i < n_unk; i++){
	    counters[i]++;
	    if((counters[i] < unknown_parameters_steps->at(i))||(i == n_unk - 1))
	      break;
	    else
	      counters[i] = 0;
	  }
	  
	  first_over_unk = false;
	}
	
	//update unk_values
	for(unsigned int i = 0; i < n_unk; i++)
	  unk_values[i]->at(best_counters[i]) += 1;
	
	//ProgressBar
	ProgressBar::percentages(j,n_rep);
      }
  }
      
  //create the unknown PDFs 
  for(unsigned int i = 0; i < n_unk; i++){
    auxPDF = new PDF(unknown_parameters_min->at(i),unknown_parameters_max->at(i),*(unk_values[i]),unknown_parameters_name->at(i));
    auxPDF->normalize();
    unknown_parameters_PDFs->insert(make_pair(unknown_parameters_name->at(i),new PDF(*auxPDF)));
    delete auxPDF;
  }

  return;
}

