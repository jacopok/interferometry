#include "ParametricFit.h"

#include "PDF.h"
#include "DataSimulator.h"
#include "ProgressBar.h"

#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>

using namespace std;


ParametricFit::ParametricFit(Func* g):
  f(g),
  fixed_parameters(new vector<PDF*>),
  unknown_parameters_min(new vector<double>),
  unknown_parameters_max(new vector<double>),
  unknown_parameters_steps(new vector<unsigned int>),
  unknown_parameters_name(new vector<string>),
  data_x(new vector<double>),
  data_y_PDF(new vector<PDF*>),
  unknown_parameters_PDFs(new map<string,PDF*>){
  }
  
ParametricFit::ParametricFit():
  ParametricFit(0){
  }
  
ParametricFit::~ParametricFit(){
  f = 0;
  delete fixed_parameters;
  delete unknown_parameters_min;
  delete unknown_parameters_max;
  delete unknown_parameters_steps;
  delete unknown_parameters_name;
  delete unknown_parameters_PDFs;
  delete data_x;
  delete data_y_PDF;
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
  data_x->push_back(x);
  data_y_PDF->push_back(y_PDF);
  return;
}

void ParametricFit::set_data(vector<double>* xV, vector<PDF*>* yVP){
  delete_data();
  data_x = xV;
  data_y_PDF = yVP;
  return;
}

void ParametricFit::delete_data(){
  data_x->clear();
  data_y_PDF->clear();
  return;
}


PDF* ParametricFit::get_unknown_PDF(const string& name) const{
  return unknown_parameters_PDFs->at(name);
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
  
  return true;
}


void ParametricFit::fit(unsigned int n_rep, unsigned int seed, mode q){
  if(!isready()) return;
  
  //clean previous fit
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
      partial_sum = 0;
      for(unsigned int h = 0; h < data_x->size(); h++){
	//get y
	double y = f->f(data_x->at(h),&v_fix,&v_unk);
	
	//compare with y_PDF
	if(q == value)
	  partial_sum += data_y_PDF->at(h)->value(y);
	else
	  partial_sum += data_y_PDF->at(h)->p_value(y);
      }
      
      //update sum with the average value (p_value)
      sum += partial_sum/data_x->size();

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
  
  //create the unknown PDFs 
  for(unsigned int i = 0; i < n_unk; i++){
    auxPDF = new PDF(unknown_parameters_min->at(i),unknown_parameters_max->at(i),*(unk_values[i]),unknown_parameters_name->at(i));
    auxPDF->normalize();
    unknown_parameters_PDFs->insert(make_pair(unknown_parameters_name->at(i),new PDF(*auxPDF)));
    delete auxPDF;
  }

  return;
}

