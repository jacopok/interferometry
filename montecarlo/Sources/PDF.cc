#include "PDF.h"

#include <cmath>
#include <iostream>
#include <fstream>

/*
#include <TFile.h>
#include <TH1F.h>
*/

using namespace std;

PDF::PDF(double m, double M, const vector<double>& v, string s = "Untitled_PDF"):
   name(s),
   steps(v.size()),
   min(m),
   max(M),
   dx((M-m)/v.size()),
   values(v),
   CDF(new vector<vector<double>>){
     cout << "new PDF: " << s << ' ' << this << endl;
     if(m > M)
       cout << "WARNING: in " << this << " max < min" << endl << endl; 
     if(dx <= 1E-7)
       cout << "Too fine PDF: errors are to be expected" << this << endl << endl;
    
}

PDF::PDF(double m, double M, unsigned int st, string s = "empty_PDF"):
   name(s),
   steps(st),
   min(m),
   max(M),
   dx((M-m)/st),
   values(vector<double>(st,0)),
   CDF(new vector<vector<double>>){
     cout << "new PDF: " << s << ' ' << this << endl;
     if(m > M)
       cout << "WARNING: in " << this << " max < min" << endl << endl; 
     if(dx <= 1E-7)
       cout << "Too fine PDF: errors are to be expected" << this << endl << endl;
    
}


PDF::~PDF(){
  values.clear();
  delete CDF;
  cout << name << " deleted" << endl;
}

PDF* PDF::load(const string& fileName) {
  ifstream file (fileName);
  string Name;
  double Min;
  double Max;
  double val;
  vector<double> v;
  if((!file)||(!(file >> Name >> Min >> Max))){
    cout << "Cannot load PDF" << endl;
    return 0;
  }
  while(file >> val)
    v.push_back(val);
  return new PDF(Min,Max,v,Name);
}

PDF::PDF(const PDF& p):
   name(p.name),
   steps(p.steps),
   min(p.min),
   max(p.max),
   dx(p.dx),
   values(p.values),//if I copy CDF too, then when p is destroyed I will have problems
   CDF(new vector<vector<double>>){
     cout << "new copied PDF: " << name << ' ' << this << endl;
}

PDF& PDF::operator=(const PDF& p) {
  steps = p.steps;
  min = p.min;
  max = p.max;
  dx = p.dx;
  values = p.values;
  CDF->clear();
  return *this;
}

double PDF::getMin() const{
  return min;
}

double PDF::getMax() const{
  return max;
}

double PDF::getDx() const{
  return dx;
}

double PDF::getValue(unsigned int i) const{
  return values[i];
}

int PDF::getIndex(double x) const{
  if(x < min) return -1;
  if(x > max + dx) return -1;
  int j = (x - min)/dx;//i valori sono il bordo sinistro del bin
  return j;
}

unsigned int PDF::getSteps() const{
  return steps;
}

string PDF::getName() const {
  return name;
}

void PDF::rename(const string& n) {
  cout << name << " (" << this << ") has been renamed to " << n << endl;
  name = n;
  return;
}

double PDF::somma() const{
  double sum = 0;
  for(double v : values)
    sum += v;
  return sum*dx; //era equivalente sommare nel ciclo v*dx e poi ritornare sum
}

bool PDF::isnormalized() const{
  if(pow(somma() - 1,2) < 1E-9) return true;
  return false;
}

void PDF::normalize() {
  if(isnormalized()) return;
  
  double s = somma();
  if(s == 0)//skip empty PDFs
    return;
  
  for(double& v : values)
    v /= s;
  CDF->clear();
  return;
}

void PDF::smoothen(unsigned int f){
  unsigned int ff = f/2;
  unsigned int t = 2*ff + 1;
  vector<double> newValues(steps,0);
  double nv = 0;
  
  for(unsigned int i = 0; i < t; i++)	//initialize nv
    nv += values[i];
  
  for(unsigned int i = 0; i < steps; i++){
    if(i < ff){
      newValues[i] = values[i];
      continue;
    }
    if(steps - i < ff){
      newValues[i] = values[i];
      continue;
    }
    newValues[i] = nv/t;
    if(i + ff + 1 < steps){
      nv += values[i + ff + 1] - values[i - ff]; // update nv
      if(nv < 0) nv = 0;
    }
  }
  
  values = newValues;
  CDF->clear();
  return;
}

void PDF::coarse(unsigned int f) {
  if((f == 0)||(f > steps)){
    cout << "Cannot coarse " << this << " with factor " << f << endl;
    return;
  }
  unsigned int newSteps = steps/f;
  double newDx = (max - min)/newSteps;
  vector<double> newValues(newSteps,0);//*dx/newDx
  int j = steps - 1;
  for(int i = newSteps - 1; i >= 0; i--){
    while(min + i*newDx <= min + j*dx){
      newValues[i] += values[j];
      j--;
      values.pop_back();
    }
    newValues[i] *= dx/newDx;
  }
  
  steps = newSteps;
  dx = newDx;
  values = newValues;
  CDF->clear();
  return;
}

PDF* PDF::optimize(double thr) {//removes zeros from the front and the back of values
  //find max_value
  double max_value = 0, soglia = 0;
  for(unsigned int i = 0; i < steps; i++){
    if(values[i] > max_value)
      max_value = values[i];
  }
  soglia = max_value*thr;
  
  vector<double> newValues;
  unsigned int n1 = 0, i = 0, n2 = steps;//=values.size()
  while(values[i] <= soglia){
    n1++;
    i++;
  }
  if(n1 != 0) n1--;
  i = steps - 1;
  while(values[i] <= soglia){
    n2--;
    i--;
  }
  if(n2 != steps) n2++;
  //set new min and max
  min += n1*dx;
  max -= (steps - n2)*dx;
  //build newValues
  for(i = n1; i < n2; i++)
    newValues.push_back(values[i]);
  //set new parameters
  steps = newValues.size();
  values = newValues;
  CDF->clear();
  
  return this;
}

void PDF::modifying_routine(){
  string ancora;
  unsigned int f;
  double thr;
  cout << name << endl;
  
  do{
      cout << "Would you like to manually optimize (o) coarse (c) smoothen (s) or proceed (p)? ";
      cin >> ancora;
      
      switch(ancora[0]){
	case 'o':
	  cout << "Insert optimization threshold (preferred < 1e-12) ";
	  cin >> thr;
	  optimize(thr);
	  normalize();
	  break;
	  
	case 'c':
	  optimize();
	  cout << "Insert coarsing factor: ";
	  cin >> f;
	  coarse(f);
	  normalize();
	  break;
	  
	case 's':
	  optimize();
	  cout << "Insert smooting factor: ";
	  cin >> f;
	  smoothen(f);
	  normalize();
	  break;
      }
      
      print();
      
      cout << "Would you like to do other modifications (y/n) ";
      cin >> ancora;
    }while(ancora[0] == 'y');
    
  return;
}

PDF* PDF::traslate(double l) {
  min += l;
  max += l;
  CDF->clear();
  return this;
}

bool PDF::add(double x, double val) {
  if(x < min) return false;
  if(x > max + dx) return false;
  
  unsigned int j = (x - min)/dx;
  values[j] += val;
  if(CDF->size() != 0)
    CDF->clear();
  return true;
}

double PDF::value( double x ) const{
  if(x < min) return 0;
  if(x > max + dx) return 0;
  
  unsigned int j = (x - min)/dx;//i valori sono il bordo sinistro del bin
  return values[j];
}

double PDF::probability( double a, double b) const{
  if(a > b){
    cout << "Warning: swapping extremes of integrations for " << this << endl;
    double temp = a;
    a = b;
    b = temp;
  }
  unsigned int i = (a > min) ? (a - min)/dx : 0;
  unsigned int j = (b - min)/dx;
  if(j >= steps) j = steps - 1;
  
  double sum = 0;
  for(unsigned int u = i; u <= j; u++)
    sum += values[u];
  
  return sum*dx;
}

double PDF::p_value(double x) const{
  if(x < min) return 0;
  if(x > max) return 0;
  
  double p1 = probability(min,x);
  double p2 = probability(x,max);
  
  return (p1 > p2) ? p2 : p1;
}

double PDF::compatibility(double x) const{
  double lambda = mean() - x;
  lambda = (lambda > 0) ? lambda : -lambda;
  
  return lambda/sqrt(var());
}

double PDF::compatibility(const PDF& p) const{
  double lambda = mean() - p.mean();
  lambda = (lambda > 0) ? lambda : -lambda;
  
  return lambda/sqrt(var() + p.var());
}

double PDF::overlap(const PDF& p) const{
  double m = (min > p.min) ? min : p.min;
  double M = (max < p.max) ? max : p.max;
  double D = (dx < p.dx) ? dx : p.dx;
  double sum = 0.;
  double x, v, vp;
  unsigned int i = 0;
  
  while(m + i*D < M){
    x = m + (i + 0.5)*D;
    v = value(x);
    vp = p.value(x);
    
    sum += (v < vp) ? v : vp;
    i++;
  }
  
  return sum*D;
}

double PDF::mean() const{
  double sum = 0.;
  for(unsigned int i = 0; i < steps; i++)
    sum += (min + (i + 0.5)*dx)*values[i]*dx;
  sum /= somma();
  return sum;
}

double PDF::mode() const{
  double maxv = 0;
  unsigned int j = 0;
  for(unsigned int i = 0; i < values.size(); i++)
    if(values[i] > maxv){
      maxv = values[i];
      j = i;
    }
  
  return (min + (j + 0.5)*dx);
}

double PDF::median() const{
  if(CDF->size() == 0)
    build_CDF();
  
  double med = 0.5*somma();
  unsigned int i = 0;
  for(i = 0; i < CDF->at(1).size(); i++)
    if(CDF->at(1).at(i) >= med)
      break;
     
  return CDF->at(0).at(i);
}

double PDF::var() const{
  double sum1 = 0., sum2 = 0., sum0 = somma();
  for(unsigned int i = 0; i < steps; i++){
    sum1 += (min + (i + 0.5)*dx)*values[i]*dx;
    sum2 += pow(min + (i + 0.5)*dx, 2)*values[i]*dx;
  }
  sum1 /= sum0;
  sum2 /= sum0;
  return sum2 - sum1*sum1;
}

vector<double>* PDF::central_ordering_IC(double CL) const{
  if((CL <=0)||(CL >= 1)){
    cout << "Invalid CL for " << this << endl;
    return 0;
  }
  if(CDF->size() == 0)
    build_CDF();
  
  CL *= somma();
  double med = 0.5*somma();
  vector<double>* IC = new vector<double>;
  IC->reserve(2);
  unsigned int i = 0;
  for(i = 0; i < CDF->at(1).size(); i++){
    if(CDF->at(1).at(i) >= med - CL/2){
      IC->push_back(CDF->at(0).at(i));
      break;
    }
  }
  for(unsigned int j = i; j < CDF->at(1).size(); j++){
    if(CDF->at(1).at(j) >= med + CL/2){
      IC->push_back(CDF->at(0).at(j));
      break;
    }
  }
  return IC;
}


double PDF::CDF_value( double x ) const{
  if(x < min) return 0;
  if(x > max + dx) return 1;
  
  unsigned int j = (x - min)/dx;
  double sum = 0;
  for(unsigned int i = 0; i <= j; i++)
    sum += values[i];
  sum *= dx;
  
  if(sum > 1){
    cout << "Error in " << this << ": failed evaluating CDF for x = " << x << endl;
    cout << "Probably non-normalized PDF" << endl << endl;
  }
  
  return sum;
}

double PDF::inverted_CDF_value(double x) const{
  if(x <= 0) return min + 0.5*dx;
  if(CDF->size() == 0)
    build_CDF();
  for(unsigned int i = 0; i < steps; i++)
    if(CDF->at(1).at(i) >= x)//look for data in the CDF
      return CDF->at(0).at(i);
  return max + 0.5*dx;
}

void PDF::build_CDF() const{
  cout << "Building CDF for " << name << endl;
  CDF->clear();//precaution
  vector<double> aux;
  CDF->push_back(aux);
  CDF->push_back(aux);//now *CDF contains 2 empty vectors
  
  CDF->at(0).reserve(steps);// x values
  CDF->at(1).reserve(steps);// CDF values
  
  double sum = 0;
  for(unsigned int i = 0; i < steps; i++){
    CDF->at(0).push_back(min + (i + 0.5)*dx); //insert central value of the bin for x
    sum += values[i];
    CDF->at(1).push_back(sum*dx);
  }
  
  return;
}

vector<vector<double>>* PDF::get_CDF() const{
  return CDF;
}


PDF PDF::operator*(double l) const {
  vector<double> newValues;
  for(double v : values)
    newValues.push_back(v*l);
  return PDF(min,max,newValues);
}
    
PDF PDF::operator/(double l) const {
  vector<double> newValues;
  for(double v : values)
    newValues.push_back(v/l);
  return PDF(min,max,newValues);
}

PDF& PDF::operator*=(double l) {
  for(double& v : values)
    v *= l;
  CDF->clear();
  return *this;
}

PDF& PDF::operator/=(double l) {
  for(double& v : values)
    v /= l;
  CDF->clear();
  return *this;
}

PDF PDF::operator*(double (*prior) (double)) const {
  vector<double> newValues;
  for(unsigned int i = 0; i < steps; i++)
    newValues.push_back(values[i]*prior(min + (i + 0.5)*dx));
  return PDF(min,max,newValues);
}

PDF PDF::operator*(const PDF& p) const {
  const PDF* finer = (p.dx < dx) ? &p : this;
  const PDF* coarser = (p.dx >= dx) ? &p : this;
  vector<double> newValues;
  
  for(unsigned int i = 0; i < finer->steps; i++)
    newValues.push_back( finer->values[i] * coarser->value(finer->min + (i + 0.5)*finer->dx) );
  
  return PDF(finer->min,finer->max,newValues);
}

PDF PDF::operator+(const PDF& p) const {
  const PDF* finer = (p.dx < dx) ? &p : this;
  const PDF* coarser = (p.dx >= dx) ? &p : this;
  double newMin = (p.min < min) ? p.min : min;
  double newMax = (p.max > max) ? p.max : max;
  double newDx = finer->dx;
  vector<double> newValues;
  unsigned int j = 0;
  double x;
  
  while(newMin + j*newDx <= newMax){
    x = newMin + (j + 0.5)*newDx;
    newValues.push_back( finer->value(x) + coarser->value(x) );
    j++;
  }
  return PDF(newMin,newMax,newValues);
}


void PDF::print_on_screen() const{
  cout << min - dx << '\t' << "0" << endl;
  for(unsigned int i = 0; i < steps; i++)
    cout << min + i*dx << '\t' << values[i] << endl;
  cout << max + dx << '\t' << "0" << endl;
  return;
}

void PDF::print(const string& fileName) const{
  ofstream file (fileName);
  if(!file){
    cout << "Cannot write on file " << fileName << endl;
    return;
  }
  file << min - dx << '\t' << "0" << endl;
  for(unsigned int i = 0; i < steps; i++)
    file << min + i*dx << '\t' << values[i] << endl;
  file << max + dx << '\t' << "0" << endl;
  return;
}

void PDF::print() const{
  print((name + "_G.txt").c_str());
  return;
}

void PDF::save(const string& fileName) const{
  ofstream file (fileName);
  if(!file){
    cout << this << " has not been saved" << endl;
    return;
  }
  file << name << endl;
  file << min << endl;
  file << max << endl;
  for(double v : values)
    file << v << endl;
  return;
}

void PDF::save() const{
  save((name + "_PDF.txt").c_str());
  return;
}

/*
void PDF::root(const string& fileName) const{
  unsigned int nBin = steps;
  const char* hName = name.c_str();
  
  TH1F* h = new TH1F(hName, hName, nBin, min, max);//create histogram
  
  for(unsigned int i = 0; i < steps; i++)
    h->SetBinContent(i + 1,values[i]);//fill histogram
  
  // save current working area
  TDirectory* currentDir = gDirectory;
  // open histogram file
  TFile* file = new TFile( fileName.c_str(), "CREATE" );
  
  h->Write();
  
  // close file
  file->Close();
  delete file;
  // restore working area
  currentDir->cd();
  
  cout << "TFile " << fileName << " built!" << endl;
  return;
}

*/