#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

#include "PDF.h"
#include "ParametricFit.h"
#include "LinearFit.h"

using namespace std;

class intFunc: public ParametricFit::Func {
    public:
	intFunc():
	  Func("intFunc"){}
	~intFunc(){}
	
	virtual unsigned int n_fix(){return 2;}
	virtual unsigned int n_unk(){return 2;}
	virtual double f(double x, vector<double>* v_fix, vector<double>* v_unk){
	  unsigned int N = (x > 0) ? x : -x;
	  double lambda = v_fix->at(0);
	  double d = v_fix->at(1);
	  double n_l = v_unk->at(0);
	  double theta_0 = v_unk->at(1);
	  double theta = acos((pow(n_l,2) - 1 - pow((lambda*N/(2*d*10000) + n_l - 1),2))/(2*(lambda*N/(2*d*10000) + n_l - 1)));
	  if(x < 0) theta = -theta;
	  
	  return theta + theta_0;
	}
};



int main (){
  string input_name, fix_name, ancora, vuota;
  unsigned int n_data = 0, n_fix = 0;
  vector<double>* xV = new vector<double>;
  vector<PDF*>* yVP = new vector<PDF*>;
  vector<double>* pattume = new vector<double>;
  vector<PDF*>* fix_par = new vector<PDF*>;
  
  cout << "Where are the fixed parameters? ";
  cin >> fix_name;
  ifstream f_in(fix_name.c_str());
  if(!f_in){
    cout << "ERROR opening input file" << endl;
    return -1;
  }
  
  cout << "Where are the data? ";
  cin >> input_name;
  ifstream in(input_name.c_str());
  if(!in){
    cout << "ERROR opening input file" << endl;
    return -1;
  }
  
  
  //get the data and the fixed parameters
  LinearFit lf;
  lf.setPrecision(100);
  while(lf.add(&in,xV,yVP))
    n_data++;
  while(lf.add(&f_in,pattume,fix_par))
    n_fix++;
  if(n_fix != 2) return -1;
  delete pattume;
  
  //prepare ParametricFit
  intFunc funz;
  ParametricFit pf(&funz);
  for(unsigned int i = 0; i < fix_par->size(); i++)
    pf.add_fixed_parameter(fix_par->at(i));
  pf.set_data(xV,yVP);
  
  unsigned int n_rep, seed;
  double min_value = 0;
  double n_l_min, n_l_max, theta_0_min, theta_0_max;
  PDF *n_l, *theta_0;
  
  do{
    ancora = vuota;    
    pf.delete_unknown_parameters();
    
    cout << "Insert min and max values for n_l: ";
    cin >> n_l_min >> n_l_max;
    pf.add_unknown_parameter(n_l_min,n_l_max,100,"n_l");
    
    cout << "Insert min and max values for theta_0: ";
    cin >> theta_0_min >> theta_0_max;
    pf.add_unknown_parameter(theta_0_min,theta_0_max,100,"theta_0");
    
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
    
    ancora = vuota;
    
    n_l = pf.get_unknown_PDF("n_l");
    theta_0 = pf.get_unknown_PDF("theta_0");
    
    cout << "n_l = " << n_l->mean() << " +- " << sqrt(n_l->var()) << endl;
    cout << "theta_0 = " << theta_0->mean() << " +- " << sqrt(theta_0->var()) << endl;
    
    n_l->print("n_l_G.txt");
    theta_0->print("theta_0_G.txt");
    
    cout << "Do you want to fit again? [y/n]: ";
    cin >> ancora;
  }while(ancora[0] == 'y');
  
  cout << endl << endl;
  return 0;
}
