#include "LTest.h"
#include "PDFFactoryManager.h"
#include "PDFFactory.h"
#include "GaussFactory.h"
#include "PDF.h"

#include <iostream>
#include <cmath>

using namespace std;


LTest::LTest():
  func(0),
  L(0),
  outdated(true),
  g(0){
    PDF* normalGauss = PDFFactoryManager::create("Gauss",0,1)->create(-10,10,10000,"normalGauss");
    g = normalGauss;
}
  
LTest::LTest(const vector<double>* x, const vector<PDF*>* y):
  xv(*x),
  yvP(*y),
  func(0),
  L(0),
  outdated(true),
  g(normalGauss){
    if(x->size() != y->size()){
      cout << "Warning: x and y vectors have different sizes!" << endl;
      cout << "Resizing vectors" << endl;
      unsigned int min_size = x->size();
      if(y->size() < min_size){
	min_size = y->size();
	xv.resize(min_size);
      }
      else
	yvP.resize(min_size);
    }
}
    
LTest::~LTest() {
  xv.clear();
  yvP.clear();
  delete g;
  func = 0;
}


double LTest::getL() {
  if(outdated)
    update();
  return L;
}

void LTest::add(double x, PDF* yP) {
  xv.push_back(x);
  yvP.push_back(yP);
  outdated = true;
  return;
}

bool LTest::add(ifstream* file, unsigned int precision) {
  outdated = true;
  double x, PDF_a, PDF_b;
  string PDF_type, PDF_name;
  if(!(*file >> x >> PDF_type)) return false;
  
  if((PDF_type == "load")||(PDF_type == "LOAD")){
    *file >> PDF_name;
    xv.push_back(x);
    yvP.push_back(PDF::load(PDF_name));
    return true;
  }
  
  if(!(*file >> PDF_a >> PDF_b)) return false;
  
  
  xv.push_back(x);
  yvP.push_back(PDFFactoryManager::create(PDF_type,PDF_a,PDF_b)->create_default(precision));//create the PDF for the y data and store it
  
  yvP[yvP.size() - 1]->normalize();
  return true;
}

void LTest::setFunc(double (*f) (double)) {
  func = f;
  outdated = true;
  return;
}
    
void LTest::reset() {
  xv.clear();
  yvP.clear();
  func = 0;
  outdated = true;
  return;
}


void LTest::update() {
  double sum = 0, r, yf;
  PDF* yP;
  //cout << "LTest: updating" << endl;
  
  for(unsigned int i = 0; i < xv.size(); i++){
    yf = func(xv[i]);
    yP = yvP[i];
    if(!yP->isnormalized()){
      cout << "LTest: normalizing " << yP << " (" << yP->getName() << ")" << endl;
      yP->normalize();
    }
    
    if( (yf < yP->getMin()) || (yf > yP->getMax() + yP->getDx()) ){
      sum += 1.0;
      continue;
    }
    
    r = 2.0*yP->probability(yP->getMin(),yf) - 1.0;
    cout << yP << " r = " << r << endl;
    sum += (r > 0) ? r : -r;
  }
  
  double l = (1 - sum/xv.size())/2.0;
  if(l <= 0) L = 1/0.;//fun way to write infinity
  else L = -(g->inverted_CDF_value(l));
  
  outdated = false;
  return;
}