#include <iostream>
#include <fstream>
#include<string>
#include<vector>
#include<cmath>

#include "PDF.h"
#include "ErrorPropagator.h"

using namespace std;

class Average: public ErrorPropagator{
  public:
    Average(const vector<PDF*>* vp):
      ErrorPropagator(vp),
      ndata(vP->size()),
      inf(0),
      sup(0){
	unsigned int totstep = 1;
	for(unsigned int i = 0; i < ndata; i++){
	  inf += vP->at(i)->getMin();
	  sup += vP->at(i)->getMax();
	  totstep *= vP->at(i)->getSteps(); 
	}
	inf /= ndata;
	sup /= ndata;
	
	cout << "\nIf you propagate with n = 0, there will be " << totstep << " iterations" << endl << endl;
    }
    ~Average(){}
    
    double f() const{
      double sum = 0;
      for(unsigned int i = 0; i < ndata; i++)
	sum += sim_sample->at(i);
      return sum/ndata;
    }
    
    PDF* propagate_default(unsigned int n, unsigned int seed, unsigned int steps, const string& name) const{
      return ErrorPropagator::propagation(n,seed,inf,sup,steps,name);
    }
    
  private:
    unsigned int ndata;
    double inf;
    double sup;
};


int main(int argc, char* argv[]){
  vector<PDF*> vp;
  string PDF_name, ancora = "n";
  unsigned int nrep, seed, steps;
  PDF* avg;
  
  ifstream in(argv[1]);
  if(!in){
    cout << "Error opening input file" << endl;
    return -1;
  }
  
  while(in >> PDF_name)
    vp.push_back(PDF::load(PDF_name));
  
  Average a(&vp);
  
  cout << "Insert average PDF name ";
  cin >> PDF_name;
  
  do{
    cout << "Insert number of steps for the average PDF ";
    cin >> steps;
    cout << "Insert number of repetitions and seed ";
    cin >> nrep >> seed;
    
    if(ancora[0] == 'n')
      avg = a.propagate_default(nrep,seed,steps,PDF_name);
    else{
      avg = a.propagation(nrep,seed,avg->getMin(),avg->getMax(),steps,PDF_name);
    }
    
    cout << avg->getName() << " = " << avg->mean() << " +- " << sqrt(avg->var()) << endl << endl;
    
    avg->modifying_routine();
    
    cout << "Would you like to propagate again? [y/n] ";
    cin >> ancora;
  }while(ancora[0] == 'y');
  
  avg->save();
  
  cout << avg->getName() << " = " << avg->mean() << " +- " << sqrt(avg->var()) << endl << endl;
  
  cout << endl;
  return 0;
}