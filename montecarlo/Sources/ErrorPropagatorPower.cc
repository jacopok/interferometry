#include "ErrorPropagatorPower.h"

#include "DataSimulator.h"

#include <cmath>

using namespace std;

ErrorPropagatorPower::ErrorPropagatorPower(const vector<PDF*>* vp, vector<double>* c, vector<double>* exp):
   ErrorPropagator(vp),
   coeffs(new vector<double>),
   exponents(new vector <double>){
     *coeffs = *c;
     *exponents = *exp;
     while(coeffs->size() > exponents->size())
       exponents->push_back(1);//resize exponents*/
}

ErrorPropagatorPower::~ErrorPropagatorPower(){
  delete coeffs;
  delete exponents;
}

double ErrorPropagatorPower::f() const {
  double sum = 0;
  for(unsigned int i = 0; i < coeffs->size(); i++){
    sum += coeffs->at(0)*pow( DataSimulator::simulate_one(vP->at(0)), exponents->at(0) );
  }
  
  return sum;
}