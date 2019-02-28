#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

#include "PDF.h"
#include "MultiPDF.h"
#include "ParametricFit.h"
#include "LinearFit.h"
#include "PDFFactory.h"
#include "PDFFactoryManager.h"

using namespace std;

class nu: public ParametricFit::Func {
    public:
	nu():
	  Func("intFunc"){}
	~nu(){}
	
	virtual unsigned int n_fix(){return 1;}
	virtual unsigned int n_unk(){return 3;}// theta_0,n_l,gamma = lambda/2d
	virtual double f(double x, vector<double>* v_fix, vector<double>* v_unk){
	  
	  double n_l = v_fix->at(0);
	  double gamma = v_unk->at(0);
	  double theta_0 = v_unk->at(1);
	  double N_0 = v_unk->at(2);
	  double theta;
	  if(x > 0)
	    theta = acos((pow(n_l,2) - 1 - pow((gamma*(x - N_0) + n_l - 1),2))/(2*(gamma*(x - N_0) + n_l - 1)));
	  else{
	    theta = -acos((pow(n_l,2) - 1 - pow((gamma*(-x - N_0) + n_l - 1),2))/(2*(gamma*(-x - N_0) + n_l - 1)));
	  }
	  
	  return theta + theta_0;
	}
};



int main (int argc, char* argv[]){
  string ancora;
  PDF *n_l, *theta_0, *gamma, *N_0;
  MultiPDF *gN, *gt, *offsets;
  unsigned int n_data = 0, datastep = 50;
  vector<double>* xV = new vector<double>;
  vector<PDF*>* yVP = new vector<PDF*>;
  vector<double>* pattume = new vector<double>;
  vector<string> names;
  
  ifstream in(argv[1]);
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
  
  //plot rough data
  ofstream rough("rough_data.txt");
  if(!rough){
    cout << "Cannot open rough" << endl;
    return -1;
  }
  
  for(unsigned int i = 0; i < xV->size(); i++)
    rough << xV->at(i) << '\t' << yVP->at(i)->mean() << '\t' << sqrt(yVP->at(i)->var()) << endl;
  
  
  //prepare ParametricFit
  nu funz;
  ParametricFit pf(&funz);
  pf.set_data(xV,yVP);
  
  unsigned int n_rep, seed;
  double min_value = 0;
  double param_min, param_max;
  unsigned int n_l_step, gamma_step, theta_0_step, N_0_step;
  MultiPDF *total;
  
  cout << "Insert type (Gauss/Box/Triangular), meam and width for n_l and its steps: ";
  cin >> ancora >> param_min >> param_max >> n_l_step;
  PDFFactory* F = PDFFactoryManager::create(ancora,param_min,param_max);
  n_l = F->create_default(n_l_step);
  n_l->rename("n_l");
  pf.add_fixed_parameter(n_l);
  
  do{
    cout << "Do you want to reset unknown paramters? [y/n] ";
    cin >> ancora;
    
    pf.delete_unknown_parameters();
    
    if(ancora[0] == 'y'){
      
      cout << "Insert min and max values for gamma and its steps: ";
      cin >> param_min >> param_max >> gamma_step;
      pf.add_unknown_parameter(param_min,param_max,gamma_step,"gamma");
      
      cout << "Insert min and max values for theta_0 and its steps: ";
      cin >> param_min >> param_max >> theta_0_step;
      pf.add_unknown_parameter(param_min,param_max,theta_0_step,"theta_0");
      
      cout << "Insert min and max values for N_0 and its steps: ";
      cin >> param_min >> param_max >> N_0_step;
      pf.add_unknown_parameter(param_min,param_max,N_0_step,"N_0");
    }
    else{
      cout << "Optimizing parameters" << endl << endl;
      gamma->optimize();
      pf.add_unknown_parameter(gamma->getMin(),gamma->getMax(),gamma_step,"gamma");
      theta_0->optimize();
      pf.add_unknown_parameter(theta_0->getMin(),theta_0->getMax(),theta_0_step,"theta_0");
      N_0->optimize();
      pf.add_unknown_parameter(N_0->getMin(),N_0->getMax(),N_0_step,"N_0");
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
    
    
    offsets = total->integrate_along("gamma","offsets");
    gN = total->integrate_along("theta_0","gN");
    gt = total->integrate_along("N_0","gt");
    
    gamma = gN->integrate_along("N_0","gamma")->toPDF();
    theta_0 = offsets->integrate_along("N_0","theta_0")->toPDF();
    N_0 = offsets->integrate_along("theta_0","N_0")->toPDF();
    
    cout << "n_l = " << n_l->mean() << " +- " << sqrt(n_l->var()) << endl;
    cout << "gamma = " << gamma->mean() << " +- " << sqrt(gamma->var()) << endl;
    cout << "theta_0 = " << theta_0->mean() << " +- " << sqrt(theta_0->var()) << endl;
    cout << "N_0 = " << N_0->mean() << " +- " << sqrt(N_0->var()) << endl << endl;
    
    n_l->print("n_l_G.txt");
    gamma->print("gamma_G.txt");
    theta_0->print("theta_0_G.txt");
    N_0->print("N_0_G.txt");
    
    cout << "Correlation coefficient between N_0 and gamma = " << gN->correlation_index() << endl;
    cout << "Correlation coefficient between theta_0 and gamma = " << gt->correlation_index() << endl;
    cout << "Correlation coefficient between N_0 and theta_0 = " << offsets->correlation_index() << endl << endl;
    cout << "Chi2 = " << pf.chi2() << endl;
    
    cout << "printing gN" << endl;
    gN->print("gN_G.txt");
    cout << "printing gt" << endl;
    gt->print("gt_G.txt");
    cout << "printing offsets" << endl;
    offsets->print("offsets_G.txt");
    cout << "saving total" << endl;
    total->save("total_MPDF.txt");
    
    cout << "Do you want to fit again? [y/n]: ";
    cin >> ancora;
  }while(ancora[0] == 'y');
  
  gamma->save("gamma_PDF.txt");
  
  cout << endl << endl;
  return 0;
}
