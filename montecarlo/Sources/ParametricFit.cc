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
#include <fstream>
#include <cmath>

using namespace std;


ParametricFit::ParametricFit(Func* g):
  f(g),
  fixed_parameters(new vector<PDF*>),
  extern_data_vectors(false),
  misses(0),
  min_value(0),
  data_x(new vector<double>),
  data_y_PDF(new vector<PDF*>),
  data_y_means(new vector<double>),
  data_y_vars(new vector<double>),
  data_misses(new vector<unsigned int>),
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
  delete data_y_means;
  delete data_y_vars;
  delete data_misses;
  delete unknown_MultiPDF;
}
    
    
void ParametricFit::set_func(Func* g){
  f = g;
  return;
}

void ParametricFit::set_misses(unsigned int i){
  misses = i;
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
  unknown_MultiPDF->clear();
  delete unknown_MultiPDF;
  unknown_MultiPDF = new MultiPDF("unknownMP");
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
  data_y_means->push_back(y_PDF->mean());
  data_y_vars->push_back(y_PDF->var());
  data_misses->push_back(0);
  return;
}

void ParametricFit::set_data(vector<double>* xV, vector<PDF*>* yVP){//data are NOT copied into *this
  delete_data();
  extern_data_vectors = true;
  data_x = xV;
  data_y_PDF = yVP;
  delete data_misses;
  data_misses = new vector<unsigned int>(data_x->size(),0);
  fill_y_mv();
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
  data_y_means->clear();
  data_y_vars->clear();
  data_misses->clear();
  extern_data_vectors = false;
  return;
}

void ParametricFit::clear(){
  f = 0;
  misses = 0;
  delete_fixed_parameters();
  delete_unknown_parameters();
  delete_data();
  return;
}

void ParametricFit::reject_missed_data(unsigned int i){
  vector<double>* new_data_x = new vector<double>;
  vector<PDF*>* new_data_y_PDF = new vector<PDF*>;
  new_data_x->reserve(data_x->size());
  new_data_y_PDF->reserve(data_x->size());
  
  for(unsigned int h = 0; h < data_x->size(); h++){
    if(data_misses->at(h) > i)
      continue;
    new_data_x->push_back(data_x->at(h));
    new_data_y_PDF->push_back(new PDF(*(data_y_PDF->at(h))));
  }

  unsigned int rejected_data = data_x->size() - new_data_x->size();
  double perc = rejected_data*100.0/data_x->size();
  
  cout << '\n' << rejected_data << " data have been rejected (" << perc << "%)" << endl << endl;
  
  delete_data();
  data_x = new_data_x;
  data_y_PDF = new_data_y_PDF;
  delete data_misses;
  data_misses = new vector<unsigned int>(new_data_x->size(),0);
  fill_y_mv();
  
  return;
}

void ParametricFit::fill_y_mv(){
  if(data_y_means->size() == 0){
    data_y_means->reserve(data_x->size());
    data_y_vars->reserve(data_x->size());
    for(unsigned int h = 0; h < data_x->size(); h++){
      data_y_means->push_back(data_y_PDF->at(h)->mean());
      data_y_vars->push_back(data_y_PDF->at(h)->var());
    }
  }
  return;
}

MultiPDF* ParametricFit::get_unknown_MultiPDF() const{
  return new MultiPDF(*unknown_MultiPDF);
}

double ParametricFit::chi2(vector<double>* fix_par_values, vector<double>* unk_par_values) const{
  if(fix_par_values->size() != f->n_fix()){
    cout << "Cannot evaluate chi2: wrong number of fixed parameters";
    return -1;
  }
  if(unk_par_values->size() != f->n_unk()){
    cout << "Cannot evaluate chi2: wrong number of unknown parameters";
    return -1;
  }
  
  cout << "chi2: ";
  for(unsigned int k = 0; k < fixed_parameters->size(); k++)
    cout << fix_par_values->at(k) << endl;
  for(unsigned int u = 0; u < unk_par_values->size(); u++)
    cout << unk_par_values->at(u) << endl;
  
  double sum = 0, y =0;
  
  for(unsigned int h = 0; h < data_x->size(); h++){
    //get y
    y = f->f(data_x->at(h),fix_par_values,unk_par_values);
    //cout << "chi2: " << y << endl;
    sum += pow(data_y_means->at(h) - y,2)/data_y_vars->at(h);
  }
  return sum;
}

double ParametricFit::chi2() const{
  vector<double>* fix_par_values = new vector<double>;
  for(unsigned int k = 0; k < fixed_parameters->size(); k++)
    fix_par_values->push_back(fixed_parameters->at(k)->mean());
  vector<double>* unk_par_values = unknown_MultiPDF->mean();
  return chi2(fix_par_values,unk_par_values);
}

unsigned int ParametricFit::degrees_of_freedom() const{
  return data_x->size() - unknown_MultiPDF->getDimension() - fixed_parameters->size();
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
  delete data_misses;
  data_misses = new vector<unsigned int>(data_x->size(),0);
  
  //useful variables
  unsigned int n_unk = unknown_MultiPDF->getDimension();
  double sum = 0;
  double partial_sum = 0, coeff = 1, c1 = 0, c2 = 0;
  unsigned int a_che_punto_siamo = 0, maxcount = unknown_MultiPDF->getSize();
  MultiPDF* fixed_MultiPDF;
  
  //vector of single values (to be plugged in f)
  vector<double> v_unk(n_unk,0);
  vector<double> v_fix(fixed_parameters->size(),0);
  
  srandom(seed);
  
  //fit
  switch(q){
    case value:
    case p_value:
    case gauss:
      
      if(n_rep == 0){
	fixed_MultiPDF = MultiPDF::merge(fixed_parameters,"fixed_MultiPDF");
	for(unsigned int k = 0; k < fixed_parameters->size(); k++)
	  coeff *= fixed_parameters->at(k)->getDx();
	//fixed_MultiPDF->print("fixed_G.txt");
      }
    
      //iterate over unknown_parameters
      unknown_MultiPDF->initialize_counters();
      do{
      //while(a_che_punto_siamo < maxcount){
	
	//initialize single values for unknown_parameters
	for(unsigned int u = 0; u < n_unk; u++)
	  v_unk[u] = unknown_MultiPDF->getPDFs()->at(u)->getMin() + (0.5 + unknown_MultiPDF->getCounters()->at(u))*unknown_MultiPDF->getPDFs()->at(u)->getDx();
	
	//iterate over fixed parameters
	sum = 0;
	if(n_rep > 0){
	  for(unsigned int j = 0; j < n_rep; j++){
	    
	    //initialize fixed parameters
	    for(unsigned int k = 0; k < fixed_parameters->size(); k++)
	      v_fix[k] = DataSimulator::simulate_one(fixed_parameters->at(k));
	    
	    //iterate over data
	    c1 = data_iterator(&v_fix,&v_unk,q);
	    //cout << c1 << endl;
	    sum += c1;//data_x->size();
	    //cout << sum << endl;
	  }
	}
	else{
	  fixed_MultiPDF->initialize_counters();
	  do{
	    //initialize fixed parameters
	    for(unsigned int k = 0; k < fixed_parameters->size(); k++)
	      v_fix[k] = fixed_parameters->at(k)->getMin() + (0.5 + fixed_MultiPDF->getCounters()->at(k))*fixed_parameters->at(k)->getDx();
	    //iterate over data
	    if((c2 = *(fixed_MultiPDF->access())) != 0){
	      c1 = data_iterator(&v_fix,&v_unk,q);
	      //cout << fixed_MultiPDF->getCounters()->at(0) << '\t' << c2*coeff*c1 << endl;
	      sum += c2*coeff*c1;
	    } 
	  }while(fixed_MultiPDF->update_counters());
	}
	
	//update the MultiPDF
	if(n_rep > 0)
	  *(unknown_MultiPDF->access()) += sum/n_rep;
	else{
	  *(unknown_MultiPDF->access()) += sum;
	}
		
	//ProgressBar
	a_che_punto_siamo++;
	ProgressBar::percentages(a_che_punto_siamo,maxcount);
	
      }while(unknown_MultiPDF->update_counters());
      
      if(n_rep == 0)
	delete fixed_MultiPDF;
      
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

double ParametricFit::data_iterator(vector<double>* v_fix, vector<double>* v_unk, mode q) const{
  double partial_sum = 0, y = 0, yv = 0;
  unsigned int missed = 0;
  vector<unsigned int> hs;
  hs.reserve(misses);
  
  
  for(unsigned int h = 0; h < data_x->size(); h++){
    //get y
    y = f->f(data_x->at(h),v_fix,v_unk);
    
    //compare with y_PDF
    if(q == gauss){
      yv = pow(y - data_y_means->at(h),2)/data_y_vars->at(h);
      partial_sum += 1 + min_value - 0.5*yv;
      /*if(yv < 25)
	partial_sum += 7 + 0.5*(11 - yv);*/
      continue;
    }
    else if(q == value)
      yv = data_y_PDF->at(h)->value(y)*data_y_PDF->at(h)->getDx()*data_y_PDF->at(h)->getSteps(); 
    else{
      yv = data_y_PDF->at(h)->p_value(y)*data_y_PDF->at(h)->getSteps();
    }
    
    if(yv <= 0){ 
      missed++;
      partial_sum += log(min_value);
      hs.push_back(h);
    }
    else{
      partial_sum += log(yv);
    }
    if(missed > misses)
      return 0;
  }

  if(missed > 0){
    for(unsigned int m = 0; m < hs.size(); m++)//update data_misses
      data_misses->at(hs[m])++;
    /*cout << "Missed " << missed << "data trying to fit with parameters: " << flush;
    for(unsigned int k = 0; k < v_fix->size(); k++)
      cout << v_fix->at(k) << " " << flush;
    cout << "; " << flush;
    for(unsigned int u = 0; u < v_unk->size(); u++)
      cout << v_unk->at(u) << " " << flush;
    cout << endl;*/
  } 
  //cout << partial_sum << endl;
  return exp(partial_sum);
}

void ParametricFit::print_misses(const string& filename) const{
  ofstream mis(filename);
  if(!mis){
    cout << "Failed to print misses" << endl;
    return;
  }
  mis << "#x\ty\tmisses" << endl << endl;
  for(unsigned int h = 0; h < data_x->size(); h++)
    mis << data_x->at(h) << '\t' << data_y_means->at(h) << '\t' << data_misses->at(h) << endl;
  
  return;
}

void ParametricFit::print_data(const string& filename) const{
  ofstream rough(filename);
  if(!rough){
    cout << "Failed to print data onto " << filename << endl;
    return;
  }
  
  for(unsigned int h = 0; h < data_x->size(); h++)
    rough << data_x->at(h) << '\t' << data_y_means->at(h) << '\t' << sqrt(data_y_vars->at(h)) << endl;
  
  return;
}

void ParametricFit::print_residuals(const string& filename) const{
  ofstream residuals(filename);
  if(!residuals){
    cout << "Failed to print residuals onto " << filename << endl;
    return;
  }
  
  vector<double>* fix_par_values = new vector<double>;
  for(unsigned int k = 0; k < fixed_parameters->size(); k++)
    fix_par_values->push_back(fixed_parameters->at(k)->mean());
  vector<double>* unk_par_values = unknown_MultiPDF->mean();
  double res = 0;
  for(unsigned int h = 0; h < data_x->size(); h++){
    res = data_y_means->at(h) - f->f(data_x->at(h),fix_par_values,unk_par_values);
    residuals << data_x->at(h) << '\t' << res << '\t' << sqrt(data_y_vars->at(h)) << endl;
  }
  delete fix_par_values;
  
  return;
}
