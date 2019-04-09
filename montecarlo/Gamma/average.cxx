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

class Sum: public ErrorPropagator{
  public:
    Sum(const vector<PDF*>* vp):
      ErrorPropagator(vp),
      ndata(vp->size()),
      inf(0),
      sup(0){
	unsigned int totstep = 1;
	for(unsigned int i = 0; i < ndata; i++){
	  inf += vP->at(i)->getMin();
	  sup += vP->at(i)->getMax();
	  totstep *= vP->at(i)->getSteps(); 
	}
	
	cout << "\nIf you propagate with n = 0, there will be " << totstep << " iterations" << endl << endl;
    }
    ~Sum(){}
    
    double f() const{
      double sum = 0;
      for(unsigned int i = 0; i < ndata; i++)
	sum += sim_sample->at(i);
      return sum;
    }
    
    PDF* propagate_default(unsigned int n, unsigned int seed, unsigned int steps, const string& name) const{
      return ErrorPropagator::propagation(n,seed,inf,sup,steps,name);
    }
    
  private:
    unsigned int ndata;
    double inf;
    double sup;
};

class Rescale: public ErrorPropagator{
  public:
    Rescale(const vector<PDF*>* vp, double f):
      ErrorPropagator(vp),
      factor(f),
      inf(0),
      sup(0){
	if(factor > 0){
	  inf = vP->at(0)->getMin()*factor;
	  sup = vP->at(0)->getMax()*factor;
	}
	if(factor < 0){
	  inf = vP->at(0)->getMax()*factor;
	  sup = vP->at(0)->getMin()*factor;
	}
	  
    }
    ~Rescale(){}
    
    double f() const{
      return sim_sample->at(0)*factor;
    }
    
    PDF* propagate_default(unsigned int n, unsigned int seed, unsigned int steps, const string& name) const{
      return ErrorPropagator::propagation(n,seed,inf,sup,steps,name);
    }
    
  private:
    double factor;
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
  
  cout << "Insert average PDF name ";
  cin >> PDF_name;
  cout << "Would you like to propagate iteratively? [y/n] ";
  cin >> ancora;
  
  if(ancora[0] == 'n'){
    Average a(&vp);
    
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
  }
  else{
    cout << "Insert number of steps for the average PDF ";
    cin >> steps;
    vector<PDF*> couple;
    bool first_time = true;
    PDF* aux;
    double factor = 1.0/vp.size();
    
    for(PDF* p: vp){
      if(first_time){
	couple.push_back(p);
	first_time = false;
	continue;
      }
      couple.push_back(p);
      Sum a(&couple);
      aux = a.propagate_default(0,1,steps,"intermediate_average");
      couple.clear();
      couple.push_back(aux);
    }
    Rescale r(&couple,factor);
    avg = r.propagate_default(0,1,steps,PDF_name);
  }
  avg->modifying_routine();
  
  avg->save();
  
  cout << avg->getName() << " = " << avg->mean() << " +- " << sqrt(avg->var()) << endl << endl;
  
  cout << endl;
  return 0;
}