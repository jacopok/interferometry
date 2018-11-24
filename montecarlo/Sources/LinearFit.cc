#include "LinearFit.h"

#include "PDF.h"
#include "PDFFactory.h"
#include "PDFFactoryManager.h"
#include "DataSimulator.h"
#include "ProgressBar.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cmath>

using namespace std;

LinearFit::LinearFit():
  data_x(new vector<double>),
  data_y(new vector<PDF*>),
  seed(1),
  precision(0),
  ab_steps(0),
  n(0),
  isSet(false),
  isAset(false),
  isBset(false),
  isIXset(false),
  isIYset(false),
  a(0),
  b(0),
  r(0){
}

LinearFit::~LinearFit(){
  if(a != 0) delete a;
  if(b != 0) delete b;
  data_x->clear();
  delete data_x;
  data_y->clear();
  delete data_y;
}

void LinearFit::setSeed(unsigned int i) {
  seed = i;
  checkSet();
  return;
}

void LinearFit::setPrecision(unsigned int i) {
  precision = i;
  checkSet();
  return;
}

void LinearFit::setAB_steps(unsigned int i) {
  ab_steps = i;
  checkSet();
  return;
}

void LinearFit::setN(unsigned int i) {
  n = i;
  checkSet();
  return;
}

void LinearFit::setA_range(double m, double M) {
  min_a = m;
  max_a = M;
  isAset = true;
  return;
}

void LinearFit::setB_range(double m, double M) {
  min_b = m;
  max_b = M;
  isBset = true;
  return;
}

void LinearFit::setIX_range(double m, double M) {
  min_ix = m;
  max_ix = M;
  isIXset = true;
  return;
}

void LinearFit::setIY_range(double m, double M) {
  min_iy = m;
  max_iy = M;
  isIYset = true;
  return;
}


void LinearFit::reset() {//reset only further settings
  isAset = isBset = isIXset = isIYset = false;
  return;
}


void LinearFit::checkSet() {
  if(precision*ab_steps*n == 0) isSet = false;
  else isSet = true;
  return;
}

PDF* LinearFit::getA() const{
  if(a == 0) return 0;
  PDF* A = new PDF(*a);
  return A;
}

PDF* LinearFit::getB() const{
  if(b == 0) return 0;
  PDF* B = new PDF(*b);
  return B;
}

bool LinearFit::add(ifstream* file, vector<double>* xV, vector<PDF*>* yVP) {
  double x, PDF_a, PDF_b;
  string PDF_type, PDF_name;
  PDFFactory* F;
  if(!(*file >> x >> PDF_type)) return false;
  
  if((PDF_type == "load")||(PDF_type == "LOAD")){
    *file >> PDF_name;
    xV->push_back(x);
    yVP->push_back(PDF::load(PDF_name));
    return true;
  }
  
  if(!(*file >> PDF_a >> PDF_b)) return false;
  
  F = PDFFactoryManager::create(PDF_type,PDF_a,PDF_b);//create the PDFFactory for the y data
  xV->push_back(x);
  yVP->push_back(F->create_default(precision));//create the PDF for the y data and store it
  delete F;
  
  yVP->at(yVP->size() - 1)->normalize();
  return true;
}

bool LinearFit::add(ifstream* file) {
  return add(file,data_x,data_y);
}

void LinearFit::setData(vector<double>* xV, vector<PDF*>* yVP) {
  data_x = new vector<double>(*xV);
  data_y = new vector<PDF*>(*yVP);
}

void LinearFit::clearData(){
  data_x->clear();
  data_y->clear();
  return;
}

    
void LinearFit::fit(const string& fileName, unsigned int start_line, unsigned int end_line) {
  if(!isSet){
    cout << "Cannot fit: object not set" << endl << "Please set all the simulation parameters and try again" << endl;
    return;
  }
  ifstream file (fileName);
  if(!file){
    cout << "Cannot open " << fileName << endl;
    return;
  }
  if(a != 0) delete a;
  if(b != 0) delete b;
  
  bool line_range = (start_line < end_line);
  vector<double>* xV = new vector<double>;
  vector<PDF*>* yVP = new vector<PDF*>;
 
  
  //get data from file
  if(line_range){
    unsigned int n_lines = end_line - start_line + 1, j = 0;
    xV->reserve(n_lines);
    yVP->reserve(n_lines);
    
    string pattume;		//skip first lines
    while(j < start_line){
      getline(file,pattume);
      j++;
    }
    
    //cout << "entering for" << endl;
    for(unsigned int i = 0; i < n_lines; i++){
      if(!( add(&file, xV, yVP) )){
	cout << "Format file error in file " << fileName << endl;
	return;
      }
    }
  }
  else{
    bool enter = false;
    //cout << "entering while" << endl;
    while(add(&file, xV, yVP))
      enter = true;
    
    if(!enter){
      cout << "Format file error in file " << fileName << endl;
      return;
    }
  }
  
  //fit
  fit(xV,yVP);
  
  for(unsigned int i = 0; i < xV->size(); i++)//deallocate memory for no more useful objects
    delete yVP->at(i);
  delete yVP;
  delete xV;
  
  return;
}

void LinearFit::fit(vector<double>* xV,vector<PDF*>* yVP){
  if(!isSet){
    cout << "Cannot fit: object not set" << endl << "Please set all the simulation parameters and try again" << endl;
    return;
  }
  
   if((!isAset)&&(!isBset)){
    cout << "Welcome to LinearFit: some variables have been set compile-time, but you need now to set others: " << endl;
    cout << "Please enter min and max values for a: ";
    cin >> min_a >> max_a;
    cout << "Please enter min and max values for b: ";
    cin >> min_b >> max_b;
  }
  cout << "Starting simulation (" << n << ')' << endl << endl;
  //starting simulating data and fit
  srandom(seed);
  vector<double>* yV;
  double* ab;
  vector<double> values_a(ab_steps,0.);
  vector<double> values_b(ab_steps,0.);
  double dx_a = (max_a - min_a)/ab_steps;
  double dx_b = (max_b - min_b)/ab_steps;
  
  a = new PDF(min_a,max_a,values_a,"a");
  b = new PDF(min_b,max_b,values_b,"b");
  
  //fill vectors of values for a and b
  for(unsigned int i = 0; i < n; i++){
    yV = DataSimulator::simulate_sample(yVP);
    ab = fit_sample(xV,yV);
    //cout << ab[0] << '\t' << ab[1] << endl;
    if((ab[0] <= max_a + dx_a)&&(ab[0] >= min_a)&&(ab[1] <= max_b + dx_b)&&(ab[1] >= min_b)){
      a->add(ab[0]);
      b->add(ab[1]);
    }
    delete[] ab;
    delete yV;
    
    ProgressBar::percentages(i,n);
  }
  

  a->normalize();
  b->normalize();
  
  return;
}

void LinearFit::fit() {
  fit(data_x,data_y);
  return;
}

PDF** LinearFit::intersec(const string& fileN1, const string& fileN2) {
  if(!isSet){
    cout << "Cannot fit: object not set" << endl << "Please set all the simulation parameters and try again" << endl;
    return 0;
  }
  ifstream file1 (fileN1);
  if(!file1){
    cout << "Cannot open " << fileN1 << endl;
    return 0;
  }
  ifstream file2 (fileN2);
  if(!file1){
    cout << "Cannot open " << fileN2 << endl;
    return 0;
  }
  
  
  vector<double>* xV1 = new vector<double>;
  vector<double>* xV2 = new vector<double>;
  vector<PDF*>* yVP1 = new vector<PDF*>;
  vector<PDF*>* yVP2 = new vector<PDF*>;
  vector<double>* yV1;
  vector<double>* yV2;
  
  bool enter = false;
    //cout << "entering while" << endl;
    while(add(&file1, xV1, yVP1))
      enter = true;
    
    if(!enter){
      cout << "Format file error in file " << fileN1 << endl;
      return 0;
    }
    enter = false;
    //cout << "entering while" << endl;
    while(add(&file2, xV2, yVP2))
      enter = true;
      
    if(!enter){
      cout << "Format file error in file " << fileN2 << endl;
      return 0;
    }
    
  if((!isIXset)&&(!isIYset)){
    cout << "Welcome to LinearFit: some variables have been set compile-time, but you need now to set others: " << endl;
    cout << "Please enter min and max values for the intersection x: ";
    cin >> min_ix >> max_ix;
    cout << "Please enter min and max values for intersection y: ";
    cin >> min_iy >> max_iy;
  }
  cout << "Starting simulation (" << n << ')' << endl << endl;
  //starting simulating data and fit
  srandom(seed);
  double* ab;
  double* cd;
  double* xy = new double [2];
  vector<double> values_ix(ab_steps,0.);
  vector<double> values_iy(ab_steps,0.);
  double dx_ix = (max_ix - min_ix)/ab_steps;
  double dx_iy = (max_iy - min_iy)/ab_steps;
 
  PDF** I = new PDF*[2];
  I[0] = new PDF(min_ix,max_ix,values_ix,"ix");
  I[1] = new PDF(min_iy,max_iy,values_iy,"iy");
  
  //fill vectors of values for ix and iy
  for(unsigned int i = 0; i < n; i++){
    yV1 = DataSimulator::simulate_sample(yVP1);
    ab = fit_sample(xV1,yV1);
    yV2 = DataSimulator::simulate_sample(yVP2);
    cd = fit_sample(xV2,yV2);
    //cout << ab[0] << '\t' << ab[1] << '\t' << cd[0] << '\t' << cd[1] << endl;
    xy[0] = (ab[0] - cd[0])/(cd[1] - ab[1]);
    xy[1] = ab[0] + ab[1]*xy[0];
    //cout << xy[0] << '\t' << xy[1] << endl;
    if((xy[0] <= max_ix + dx_ix)&&(xy[0] >= min_ix)&&(xy[1] <= max_iy + dx_iy)&&(xy[1] >= min_iy)){
      I[0]->add(xy[0]);
      I[1]->add(xy[1]);
    }
    delete[] ab;
    delete[] cd;
    delete yV1;
    delete yV2;
    
    ProgressBar::percentages(i,n);
  }
  delete[] xy;
  
  
  I[0]->normalize();
  I[1]->normalize();
  
  for(unsigned int i = 0; i < xV1->size(); i++){//deallocate memory for no more useful objects
    delete yVP1->at(i);
    delete yVP2->at(i);
  }
  delete yVP1;
  delete yVP2;
  delete xV1;
  delete xV2;
  
  return I;
}

PDF** LinearFit::intersec(const string& fileName, double retta_a, double retta_b) {
  if(!isSet){
    cout << "Cannot fit: object not set" << endl << "Please set all the simulation parameters and try again" << endl;
    return 0;
  }
  ifstream file (fileName);
  if(!file){
    cout << "Cannot open " << fileName << endl;
    return 0;
  }
  if(a != 0) delete a;
  if(b != 0) delete b;
  
  
  vector<double>* xV = new vector<double>;
  vector<PDF*>* yVP = new vector<PDF*>;
  vector<double>* yV;
  
  //get data from file
  bool enter = false;
  //cout << "entering while" << endl;
  while(add(&file, xV, yVP))
      enter = true;
  
  if(!enter){
    cout << "Format file error in file " << fileName << endl;
    return 0;
  }

  
  if((!isIXset)&&(!isIYset)){
    cout << "Welcome to LinearFit: some variables have been set compile-time, but you need now to set others: " << endl;
    cout << "Please enter min and max values for a: ";
    cin >> min_ix >> max_ix;
    cout << "Please enter min and max values for b: ";
    cin >> min_iy >> max_iy;
  }
  cout << "Starting simulation (" << n << ')' << endl << endl;
  //starting simulating data and fit
  srandom(seed);
  double* ab;
  double* xy = new double [2];
  vector<double> values_ix(ab_steps,0.);
  vector<double> values_iy(ab_steps,0.);
  double dx_ix = (max_ix - min_ix)/ab_steps;
  double dx_iy = (max_iy - min_iy)/ab_steps;
  
  PDF** I = new PDF*[2];
  I[0] = new PDF(min_ix,max_ix,values_ix,"ix");
  I[1] = new PDF(min_iy,max_iy,values_iy,"iy");
  
  
  //fill vectors of values for a and b
  for(unsigned int i = 0; i < n; i++){
    yV = DataSimulator::simulate_sample(yVP);
    ab = fit_sample(xV,yV);
    //cout << ab[0] << '\t' << ab[1] << endl;
    if(isAset && isBset && !((ab[0] <= max_a)&&(ab[0] >= min_a)&&(ab[1] <= max_b)&&(ab[1] >= min_b)))
      continue;
    
    xy[0] = (ab[0] - retta_a)/(retta_b - ab[1]);
    xy[1] = ab[0] + ab[1]*xy[0];
    //cout << xy[0] << '\t' << xy[1] << endl;
    if((xy[0] <= max_ix + dx_ix)&&(xy[0] >= min_ix)&&(xy[1] <= max_iy + dx_iy)&&(xy[1] >= min_iy)){
      I[0]->add(xy[0]);
      I[1]->add(xy[1]);
    }
    delete[] ab;
    delete yV;
    
    ProgressBar::percentages(i,n);
    
  }
  delete[] xy;
  
  
  I[0]->normalize();
  I[1]->normalize();
  
  for(unsigned int i = 0; i < xV->size(); i++)//deallocate memory for no more useful objects
    delete yVP->at(i);
  delete yVP;
  delete xV;
  
  return I;
}

double* LinearFit::fit_sample(const vector<double>* xV, const vector<double>* yV) {
  if(xV->size() != yV->size()){
    cout << "fit_sample: ERROR" << endl;
    return 0;
  }
  double* ab = new double[2];
  double Ex = 0, Ex2 = 0, Exy = 0, Ey = 0;//"E" stands for "sum"
  unsigned int N = xV->size();
  
  for(unsigned int i = 0; i < N; i++){
    Ex  += xV->at(i);
    Ex2 += xV->at(i)*xV->at(i);
    Exy += xV->at(i)*yV->at(i);
    Ey  += yV->at(i);
  }
  
  double Delta = N*Ex2 - Ex*Ex;
  ab[0] = (Ex2*Ey - Ex*Exy)/Delta;
  ab[1] = (N*Exy - Ex*Ey)/Delta;
  
  return ab;
}

double LinearFit::chi2(vector<double>* xV,vector<PDF*>* yVP) const{
  if(a == 0){
    cout << "No fit has been done: cannot evaluate chi2" << endl;
    return -1;
  }
  
  double sum = 0;
  double a_m = a->mean();
  double b_m = b->mean();
  
  for(unsigned int i = 0; i < xV->size(); i++)
    sum += pow(yVP->at(i)->mean() - a_m - b_m*xV->at(i), 2)/yVP->at(i)->var();
  
  return sum;
}

double LinearFit::chi2() const{
  return chi2(data_x,data_y);
}

double LinearFit::rho(vector<double>* xV,vector<PDF*>* yVP) const{
  double x_m = 0, y_m = 0;
  unsigned int N = xV->size();
  vector<double>* yV = new vector<double>(N,0);
  
  for(unsigned int i = 0; i < N; i++){
    yV->at(i) = yVP->at(i)->mean();
    x_m += xV->at(i);
    y_m += yV->at(i);
  }
  x_m /= N;
  y_m /= N;
  
  double sum_x = 0, sum_y = 0, sum_xy = 0;
  for(unsigned int i = 0; i < N; i++){
    sum_x += pow(xV->at(i) - x_m, 2);
    sum_y += pow(yV->at(i) - y_m, 2);
    sum_xy += (xV->at(i) - x_m)*(yV->at(i) - y_m);
  }
  
  r = sum_xy/sqrt(sum_x*sum_y);
  
  return r;
}

double LinearFit::rho() const{
  return rho(data_x,data_y);
}

double LinearFit::T_N() const{
  return r*sqrt(degrees_of_freedom()/(1 - r*r));
}

int LinearFit::degrees_of_freedom() const{
  return data_x->size() - 2;
}