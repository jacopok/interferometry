#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

#include "PDF.h"
#include "MultiPDF.h"
#include "ParametricFit.h"
#include "LinearFit.h"

using namespace std;

class nu: public ParametricFit::Func {
    public:
	nu():
	  Func("intFunc"){}
	~nu(){}
	
	virtual unsigned int n_fix(){return 0;}
	virtual unsigned int n_unk(){return 4;}// theta_0,n_l,gamma = lambda/2d
	virtual double f(double x, vector<double>* v_fix, vector<double>* v_unk){
	  
	  double n_l = v_unk->at(0);
	  double gamma = v_unk->at(1);
	  double theta_0 = v_unk->at(2);
	  double N_0 = v_unk->at(3);
	  double theta;
	  if(x > 0)
	    theta = acos((pow(n_l,2) - 1 - pow((gamma*(x - N_0) + n_l - 1),2))/(2*(gamma*(x - N_0) + n_l - 1)));
	  else{
	    theta = -acos((pow(n_l,2) - 1 - pow((gamma*(-x - N_0) + n_l - 1),2))/(2*(gamma*(-x - N_0) + n_l - 1)));
	  }
	  
	  return theta + theta_0;
	}
};



int main (){
  string input_name, ancora;
  unsigned int n_data = 0, datastep = 50;
  vector<double>* xV = new vector<double>;
  vector<PDF*>* yVP = new vector<PDF*>;
  vector<double>* pattume = new vector<double>;
  vector<string> names;
  
  cout << "Where are the data? ";
  cin >> input_name;
  ifstream in(input_name.c_str());
  if(!in){
    cout << "ERROR opening input file" << endl;
    return -1;
  }
  
  cout << "How many steps should the data PDFs have? ";
  cin >> datastep;
  
  //get the data
  LinearFit lf;
  lf.setPrecision(datastep);
  while(lf.add(&in,xV,yVP))
    n_data++;
  delete pattume;
  
  //prepare ParametricFit
  nu funz;
  ParametricFit pf(&funz);
  pf.set_data(xV,yVP);
  
  unsigned int n_rep, seed;
  double min_value = 0;
  double param_min, param_max;
  PDF *n_l, *theta_0, *gamma, *N_0;
  MultiPDF *total, *result, *offsets;
  
  do{
    cout << "Do you want to reset unknown paramters? [y/n] ";
    cin >> ancora;
    
    if(ancora[0] == 'y'){
      pf.delete_unknown_parameters();
      
      cout << "Insert min and max values for n_l and its steps: ";
      cin >> param_min >> param_max >> datastep;
      pf.add_unknown_parameter(param_min,param_max,datastep,"n_l");
      
      cout << "Insert min and max values for gamma and its steps: ";
      cin >> param_min >> param_max >> datastep;
      pf.add_unknown_parameter(param_min,param_max,datastep,"gamma");
      
      cout << "Insert min and max values for theta_0 and its steps: ";
      cin >> param_min >> param_max >> datastep;
      pf.add_unknown_parameter(param_min,param_max,datastep,"theta_0");
      
      cout << "Insert min and max values for N_0 and its steps: ";
      cin >> param_min >> param_max >> datastep;
      pf.add_unknown_parameter(param_min,param_max,datastep,"N_0");
    }
    
    cout << "Insert mode: value (v), p-value (p) or brute_force (b) (WARNING: brute_force takes time): ";
    cin >> ancora;
    cout << "Insert min_value: ";
    cin >> min_value;
    pf.set_min_value(min_value);
    cout << "Insert number of repetition and seed: ";
    cin >> n_rep >> seed;
    cout << "Fitting via ParametricFit" << endl;
    
    //fit
    switch(ancora[0]){
      default:
      case 'v': pf.fit(n_rep,seed,ParametricFit::value);
		break;
      case 'p': pf.fit(n_rep,seed,ParametricFit::p_value);
		break;
      case 'b': pf.fit(n_rep,seed,ParametricFit::brute_force);
		break;
    }
    
    total = pf.get_unknown_MultiPDF();
    
    names = vector<string>{"n_l","gamma"};
    result = total->subMultiPDF(&names,"result");
    names = vector<string>{"theta_0","N_0"};
    offsets = total->subMultiPDF(&names,"offsets");
    
    n_l = result->integrate_along("gamma","n_l")->toPDF();
    gamma = result->integrate_along("gamma","n_l")->toPDF();
    theta_0 = offsets->integrate_along("N_0","theta_0")->toPDF();
    N_0 = offsets->integrate_along("theta_0","N_0")->toPDF();
    
    cout << "n_l = " << n_l->mean() << " +- " << sqrt(n_l->var()) << endl;
    cout << "gamma = " << gamma->mean() << " +- " << sqrt(gamma->var()) << endl;
    cout << "theta_0 = " << theta_0->mean() << " +- " << sqrt(theta_0->var()) << endl;
    cout << "N_0 = " << N_0->mean() << " +- " << sqrt(N_0->var()) << endl << endl;
    
    cout << "Correlation coefficient between n_l and gamma = " << result->correlation_index() << endl;
    cout << "Correlation coefficient between N_0 and theta_0 = " << offsets->correlation_index() << endl << endl;
    
    n_l->print("n_l_G.txt");
    gamma->print("gamma_G.txt");
    theta_0->print("theta_0_G.txt");
    N_0->print("N_0_G.txt");
    result->print("result_G.txt");
    offsets->print("offsets.txt");
    
    cout << "Do you want to fit again? [y/n]: ";
    cin >> ancora;
  }while(ancora[0] == 'y');
  
  total->save("total_MPDF.txt");
  
  cout << endl << endl;
  return 0;
}
