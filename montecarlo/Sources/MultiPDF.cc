#include "MultiPDF.h"

#include "PDF.h"

#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>

using namespace std;


MultiPDF::MultiPDF(const string& s):
  name(s),
  dimension(0),
  size(1),
  counters(new vector<unsigned int>),
  PDFs(new vector<PDF*>),
  values(new vector<double>),
  empty(true),
  CDF(new vector<double>){
    cout << "New MultiPDF: " << this << endl;
  }

MultiPDF::~MultiPDF(){
  clear();
  delete counters;
  delete PDFs;
  delete values;
  delete CDF;
}

MultiPDF::MultiPDF(const MultiPDF& p):
  name(p.name),
  dimension(p.dimension),
  size(p.size),
  counters(new vector<unsigned int>(*(p.counters))),
  PDFs(new vector<PDF*>(*(p.PDFs))),
  values(new vector<double>(*(p.values))),
  empty(p.empty),
  CDF(new vector<double>){
    cout << "New copied MultiPDF: " << this << endl;
  }
  
MultiPDF& MultiPDF::operator=(const MultiPDF& p){
  name = p.name;
  dimension = p.dimension;
  size = p.size;
  delete counters;
  counters = new vector<unsigned int>(*(p.counters));
  delete PDFs;
  PDFs = new vector<PDF*>(*(p.PDFs));
  delete values;
  values = new vector<double>(*(p.values));
  empty = p.empty;
  CDF = new vector<double>;

  return *this;
}

MultiPDF* MultiPDF::load(const string& filename){
  ifstream in(filename);
  if(!in){
    cout << "Failed to load MultiPDF from file " << filename << endl;
    return 0;
  }
  
  string s;
  unsigned int dim, st, siz = 1;
  double m, M;
  
  in >> s >> dim;
  MultiPDF* p = new MultiPDF(s);
  
  for(unsigned int u = 0; u < dim; u++){
    in >> s >> m >> M >> st;
    siz *= st;
    PDF* a = new PDF(m,M,st,s);
    p->add_PDF(a);
  }
  
  p->values->resize(siz,0);
  
  for(unsigned int b = 0; b < siz; b++)
    in >> p->values->at(b);
  
  return p;
}

MultiPDF* MultiPDF::merge(vector<PDF*>* vP, const string& s){
  //initialize p
  MultiPDF* p = new MultiPDF(s);
  for(unsigned int u = 0; u < vP->size(); u++)
    p->add_PDF(vP->at(u));
  p->initialize_counters();
  
  double v;
  do{
    v = 1;
    for(unsigned int u = 0; u < p->dimension; u++)
      v *= p->PDFs->at(u)->getValue(p->counters->at(u));
    *(p->access()) = v;
  }while(p->update_counters());
  
  p->normalize();
  return p;
}



void MultiPDF::initialize_counters() const{
  counters->clear();
  counters->resize(dimension,0);
  return;
}

bool MultiPDF::update_counters() const{
  counters->at(0)++;
  for(unsigned int u = 0; u < dimension; u++){
    if(counters->at(u) == PDFs->at(u)->getSteps()){
      if(u == dimension - 1) return false;
      counters->at(u) = 0;
      counters->at(u + 1)++;
    }
  }
  return true;
}
    
void MultiPDF::add_PDF(PDF* p){
  if(!empty){
    cout << "\nWARNING: zeroing MultiPDF " << name << " in order to add a PDF" << endl << endl;
    zero();
  }
  size *= p->getSteps();
  PDFs->push_back(new PDF(*p));
  dimension++;
  counters->push_back(0);

  return;
}

void MultiPDF::zero(){
  values->clear();
  CDF->clear();
  empty = true;
  return;
}

void MultiPDF::clear(){
  zero();
  dimension = 0;
  counters->clear();
  for(unsigned int u = 0; u < dimension; u++)
    delete PDFs->at(u);
  PDFs->clear();
  return;
}

bool MultiPDF::add(vector<double>* v){
  if(v->size() != dimension)
    return false;
  vector<unsigned int>* indexs = new vector<unsigned int>(dimension,0);
  int j;
  for(unsigned int u = 0; u < dimension; u++){
    j = PDFs->at(u)->getIndex(v->at(u));
    if(j < 0) return false;
    indexs->at(u) = j;
  }
  
  *access(indexs) += 1.0;
  return true;
}

double MultiPDF::somma() const{
  if(empty){
    cout << "WARNING: MultiPDF " << name << " is empty: trying to normalize it will result in errors" << endl;
    return 0;
  }
  
  double sum = 0;
  for(unsigned int b = 0; b < size; b++)
    sum += values->at(b);
  
  for(unsigned int u = 0; u < dimension; u++)
    sum *= PDFs->at(u)->getDx();
  return sum;
}

bool MultiPDF::isnormalized() const{
  if(pow(somma() - 1,2) > 1E-14)
    return false;
  return true;
}

void MultiPDF::normalize(){
  if(isnormalized())
    return;
  double sum = somma();
  for(unsigned int b = 0; b < size; b++)
    values->at(b) /= sum;
  return;
}

void MultiPDF::build_CDF() const{
  if(empty){
    cout << "MultiPDF " << name << " is empty: cannot build CDF" << endl;
    return;
  }
  if(!isnormalized()){
    cout << "MultiPDF " << name << " is not normalized: cannot build CDF" << endl;
    return;
  }
  
  CDF->resize(size,0);
  double sum = 0;
  for(unsigned int b = 0; b < size; b++){
    sum += values->at(b);
    CDF->at(b) = sum;
  }
  return;
}

unsigned int MultiPDF::CDF_bigindex(double x) const{
  if(CDF->size() == 0)
    build_CDF();
  if(x <= 0) return 0;
  for(unsigned int b = 0; b < size; b++){
    if(x < CDF->at(b))
      return b;
  }
  return size;
}

vector<unsigned int>* MultiPDF::bigindex2indexs(unsigned int b) const{
  if(b >= size){
    cout << "MultiPDF " << name << "b >= size: cannot convert bigindex to index" << endl;
    return 0;
  }
  vector<unsigned int>* indexs = new vector<unsigned int>(dimension,0);
  unsigned int power = size;
  
  for(int u = dimension - 1; u >= 0; u--){
    power /= PDFs->at(u)->getSteps();
    indexs->at(u) = b/power;
    if(indexs->at(u) >= PDFs->at(u)->getSteps()){
      cout << "MultiPDF " << name << "error occurred while converting bigindex to index" << endl;
      return 0;
    }
    b -= indexs->at(u)*power;
  }
  return indexs;
}

vector<double>* MultiPDF::inverted_CDF_value(double x) const{
  vector<unsigned int>* indexs = bigindex2indexs(CDF_bigindex(x));
  vector<double>* v = new vector<double>(dimension,0);
  for(unsigned int u = 0; u < dimension; u++)
    v->at(u) = PDFs->at(u)->getMin() + (0.5 + indexs->at(u))*PDFs->at(u)->getDx();
  
  return v;
}

unsigned int MultiPDF::getDimension() const{
  return dimension;
}

unsigned int MultiPDF::getSize() const{
  return size;
}

vector<unsigned int>* MultiPDF::getCounters() const{
  return counters;
}

vector<PDF*>* MultiPDF::getPDFs() const{
  return PDFs;
}

double* MultiPDF::access() const{
  return access(counters);
}

double* MultiPDF::access(vector<unsigned int>* indexs) const{  
  if(indexs->size() != dimension){
    cout << "Failed to access data in MultiPDF " << name << endl;
    return 0;
  }
  if(empty){
    values->resize(size,0);
    empty = false;
  }
  
  unsigned int bigindex = 0;
  unsigned int power = 1;
  for(unsigned int u = 0; u < dimension; u++){
    bigindex += power * indexs->at(u);
    power *= PDFs->at(u)->getSteps();
  }
  
  if(bigindex >= size){
    cout << "Overflow while accessing data in MultiPDF " << name << endl;
    return 0;
  }
  
  return &(values->at(bigindex));
}

unsigned int MultiPDF::getAxis(const string& s) const{
  for(unsigned int u = 0; u < dimension; u++)
    if(PDFs->at(u)->getName() == s)
      return u;
  
  cout << "PDF " << s << " is not in MultiPDF " << name << endl;
  return -1;
}

MultiPDF* MultiPDF::integrate_along(const string& PDFname, const string& newname) const{
  return integrate_along(getAxis(PDFname),newname);
}

MultiPDF* MultiPDF::integrate_along(unsigned int i, const string& newname) const{
  if(empty){
    cout << "ERROR: MultiPDF " << name << " is empty: cannot integrate_along " << PDFs->at(i)->getName() << endl;
    return 0;
  }
  
  double dx = PDFs->at(i)->getDx();
  
  MultiPDF* p = new MultiPDF(newname);
  
  //initialize new MultiPDF
  for(unsigned int u = 0; u < dimension; u++){
    if(u == i) continue;
    p->add_PDF(PDFs->at(u));
  }
  
  initialize_counters();
  p->initialize_counters();
  
  //iterate over p->counters
  do{
    
    //synchronize counters
    unsigned int nu = 0;
    for(unsigned int u = 0; u < dimension; u++){
      if(u == i) continue;
      counters->at(u) = p->counters->at(nu);
      nu++;
    }
    
    //integrate
    counters->at(i) = 0;
    for(unsigned int j = 0; j < PDFs->at(i)->getSteps(); j++){
      *(p->access()) += *access();
      counters->at(i)++;
    }
    *(p->access()) *= dx;
    
  }while(p->update_counters());
  
  p->normalize();
  return p;
}


void MultiPDF::print(const string& filename) const{
  ofstream out(filename);
  if(!out){
    cout << "Failed to print MultiPDF " << name << endl;
    return;
  }
  
  out << '#';
  for(unsigned int u = 0; u < dimension; u++)
      out << PDFs->at(u)->getName() << '\t';
  out << "probability" << endl << endl;
 
  initialize_counters();
  //iterate
  do{
    //print data
    for(unsigned int u = 0; u < dimension; u++)
      out << PDFs->at(u)->getMin() + (counters->at(u) + 0.5)*PDFs->at(u)->getDx() << '\t';
    out << *access() << endl;
  }while(update_counters());
  
  return;
}

void MultiPDF::save(const string& filename) const{
  ofstream out(filename);
  if(!out){
    cout << "Failed to save MultiPDF " << name << endl;
    return;
  }
  
  out << name << '\t' << dimension << endl;
  for(unsigned int u = 0; u < dimension; u++)
    out << PDFs->at(u)->getName() << '\t' << PDFs->at(u)->getMin() << '\t' << PDFs->at(u)->getMax() << '\t' << PDFs->at(u)->getSteps() << endl;
  out << endl;
  for(unsigned int b = 0; b < size; b++)
    out << values->at(b) << endl;
  
  return;
}



PDF* MultiPDF::toPDF() const{
  if(dimension != 1){
    cout << "This MultiPDF (" << name << ") is not 1-dimensinal: cannot convert to PDF" << endl;
    return 0;
  }
  vector<double> v(size,0);
  for(unsigned int b = 0; b < size; b++)
    v[b] = values->at(b);
  
  return new PDF(PDFs->at(0)->getMin(),PDFs->at(0)->getMax(),v,PDFs->at(0)->getName());
}

double MultiPDF::correlation_index() const{
  if(dimension != 2){
    cout << "This MultiPDF (" << name << ") is not 2-dimensinal: cannot evaluate correlation index" << endl;
    return 0;
  }
  if(!isnormalized())
    cout << "WARNING: MultiPDF " << name << " is not normalized: correlation index may be inaccurate" << endl;
  
  PDF* x = integrate_along(PDFs->at(0)->getName(),"x")->toPDF();
  PDF* y = integrate_along(PDFs->at(1)->getName(),"y")->toPDF();
  double x_m = x->mean();
  double y_m = y->mean();
  double x_sig = sqrt(x->var());
  double y_sig = sqrt(y->var());
  
  //compute E(xy)
  double Exy = 0;
  initialize_counters();
  
  do{
    Exy += *access() * (PDFs->at(0)->getMin() + (0.5 + counters->at(0)) * PDFs->at(0)->getDx()) * 
	    (PDFs->at(1)->getMin() + (0.5 + counters->at(1)) * PDFs->at(1)->getDx());
  }while(update_counters());
  
  Exy *= PDFs->at(0)->getDx() * PDFs->at(1)->getDx();
  
  //correlation index
  double rho = (Exy / somma() - x_m * y_m) / (x_sig * y_sig);
  
  return rho;
}
