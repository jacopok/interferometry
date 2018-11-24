#include "PDF2D.h"

#include "PDF.h"

#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>

using namespace std;


PDF2D::PDF2D(double x_m, double x_M, unsigned int x_s, const string& x_n,
      double y_m, double y_M, unsigned int y_s, const string& y_n):
  x_min(x_m),
  x_max(x_M),
  x_steps(x_s),
  dx((x_M - x_m)/x_s),
  x_name(x_n),
  y_min(y_m),
  y_max(y_M),
  y_steps(y_s),
  dy((y_M - y_m)/y_s),
  y_name(y_n),
  values(new vector<vector<double>>),
  uptodate(true),
  marginals(new vector<PDF*>){
    
    vector<double> yy(y_s,0);
    values->reserve(x_s);
    for(unsigned int i = 0; i < x_s; i++)
      values->push_back(yy);
}

PDF2D::PDF2D(double x_m, double x_M, const string& x_n, double y_m, double y_M, const string& y_n, const vector<vector<double>>& v):
  x_min(x_m),
  x_max(x_M),
  x_steps(v.size()),
  dx((x_M - x_m)/v.size()),
  x_name(x_n),
  y_min(y_m),
  y_max(y_M),
  y_steps(v[0].size()),
  dy((y_M - y_m)/v[0].size()),
  y_name(y_n),
  values(new vector<vector<double>>(v)),
  uptodate(true),
  marginals(new vector<PDF*>){
}

static PDF2D* PDF2D::load(const string& filename){
  ifstream file (fileName);
  if(!file){
    cout << "Cannot write on file " << fileName << endl;
    return;
  }
  
  double x_m, x_M, y_m, y_M;
  unsigned int x_s, y_s;
  string x_n, y_n;
  
  if(!(file >> x_n >> x_m >> x_M >> x_s >> y_n >> y_m >> y_M >> y_s)){
    cout << "Format error in `" << filename << "`: loading failed" << endl;
    return 0;
  }
  
  double z = 0;
  vector<double> yy(y_s,0);
  vector<vector<double>> v(x_s,yy);
  
  for(unsigned int i = 0; i < x_s; i++){
    for(unsigned int j = 0; j < y_s; j++){
      if(file >> z)
	v.at(i).at(j) = z;
      else{
	cout << "Format error in `" << filename << "`: loading failed" << endl;
	return 0;
      }
    }
  }
  
  return new PDF2D(x_m, x_M, x_n, y_m, y_M, y_n, v);
}

PDF2D::~PDF2D(){
  delete values;
  if(marginals->size() == 2){
    delete marginals->at(0);
    delete marginals->at(1);
  }
  delete marginals;
}
    
double PDF2D::somma() const{
  double sum = 0;
  for(unsigned int i = 0; i < x_steps; i++)
    for(unsigned int j = 0; j < y_steps; j++)
      sum += values->at(i).at(j);
    
  sum *= dx*dy;
  return sum;
}

bool PDF2D::isnormalized() const{
  return(pow(somma() - 1,2) < 1E-10);
}

void PDF2D::normalize(){
  if(isnormalized()) return;
  double sum = somma();
  
  for(unsigned int i = 0; i < x_steps; i++)
    for(unsigned int j = 0; j < y_steps; j++)
      values->at(i).at(j) /= sum;
  
  uptodate = true;
    
  return;
}
    

void PDF2D::compute_marginals(){
  if(!uptodate)
    return;
  
  delete marginals->at(0);
  delete marginals->at(1);
  marginals->clear();
  
  vector<double> xv(x_steps,0);
  vector<double> yv(y_steps,0);
  
  for(unsigned int i = 0; i < x_steps; i++){
    for(unsigned int j = 0; j < y_steps; j++){
      xv.at(i) += values->at(i).at(j);
      yv.at(j) += values->at(i).at(j);
    }
  }
  
  marginals->push_back(new PDF(x_min,x_max,xv,x_name));
  marginals->push_back(new PDF(y_min,y_max,yv,y_name));
  
  marginals->at(0)->normalize();
  marginals->at(1)->normalize();
  
  uptodate = false;
  return;
}
    
PDF* PDF2D::marginal(const string& name) const{
  unsigned int index = 2;
  if(name == x_name)
    index = 0;
  if(name == y_name)
    index = 1;
  
  return marginal(index);
}

PDF* PDF2D::marginal(unsigned int index) const{
  if(index > 1)
    return 0;
  
  return marginals->at(index);//WARNING: this is not a copy: if PDF2D is destroyed also will be the returned marginal PDF
}


void PDF2D::print(const string& filename) const{
  ofstream file (fileName);
  if(!file){
    cout << "Cannot write on file " << fileName << endl;
    return;
  }
  
  for(unsigned int i = 0; i < x_steps; i++)
    for(unsigned int j = 0; j < y_steps; j++)
      file << x_min + i*dx << '\t' << y_min + j*dy << '\t' << values->at(i).at(j) << endl;
    
  return;
}

void PDF2D::save(const string& filename) const{
  ofstream file (fileName);
  if(!file){
    cout << "Cannot write on file " << fileName << endl;
    return;
  }
  
  file << x_name << '\t' << x_min << '\t' << x_max << '\t' << x_steps << endl;
  file << y_name << '\t' << y_min << '\t' << y_max << '\t' << y_steps << endl;
  
  for(unsigned int i = 0; i < x_steps; i++)
    for(unsigned int j = 0; j < y_steps; j++)
      file << values->at(i).at(j) << endl;
    
  return;
}