#include "ErrorPropagatorSum.h"

#include "DataSimulator.h"
#include "PDF.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

ErrorPropagatorSum::ErrorPropagatorSum(const vector<PDF*>* vP):
  ErrorPropagator(vP),
  coeffs(new vector<double>){
  
    cout << "\n\nWelcome to ErrorPropagatorSum: this (" << this << ") object contains these PDFs:" << endl;
    for(unsigned int i = 0; i < vP->size(); i++)
      cout << vP->at(i)->getName() << endl;
    cout << "Please insert coefficients for the linear combination:" << endl;
    double a = 0;
    for(unsigned int i = 0; i < vP->size(); i++){
      cin >> a;
      coeffs->push_back(a);
    }
    if(vP->size() != coeffs->size())
      cout << "ERROR" << endl;
    else cout << "Object successfully built!" << endl << endl;
}

ErrorPropagatorSum::~ErrorPropagatorSum(){
  delete coeffs;
}

double ErrorPropagatorSum::f() const{
  //vector<double>* sim_v = DataSimulator::simulate_sample(vP);
  double sum = 0;
  for(unsigned int i = 0; i < coeffs->size(); i++)
    sum += coeffs->at(i)*DataSimulator::simulate_one(vP->at(i));
  //delete sim_v;
  
  return sum;
}