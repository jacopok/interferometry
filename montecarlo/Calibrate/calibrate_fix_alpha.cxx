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
#include "ErrorPropagator.h"

using namespace std;

class nu: public ParametricFit::Func {
    public:
	nu():
	  Func("intFunc"){}
	~nu(){}
	
	virtual unsigned int n_fix(){return 2;}
	virtual unsigned int n_unk(){return 3;}// theta_0,n_l,gamma = lambda/2d
	virtual double f(double x, vector<double>* v_fix, vector<double>* v_unk){
	  
	  double alpha = v_fix->at(0);
	  double n_l = v_fix->at(1);
	  double gamma = v_unk->at(0);
	  double theta_0 = v_unk->at(1);
	  double N_0 = v_unk->at(2);
	  double st;
	  if(x > 0)
	    st = acos((pow(n_l,2) - 1 - pow((gamma*(x - N_0) + n_l - 1),2))/(2*(gamma*(x - N_0) + n_l - 1)));
	  else{
	    st = -acos((pow(n_l,2) - 1 - pow((gamma*(-x - N_0) + n_l - 1),2))/(2*(gamma*(-x - N_0) + n_l - 1)));
	  }
	  
	  return alpha*st + theta_0;
	}
};



int main (int argc, char* argv[]){
  if(argc != 3){
    cout << "\nThis programm fits GIMLI data having a known PDF for n_l in order to find gamma." << endl;
    cout << "In order to use it type ./calibrate [data_file] [alpha_file]." << endl;
    return 0;
  }
  
  string ancora, session;
  PDF *n_l, *theta_0, *gamma, *N_0, *alpha;
  MultiPDF *gN, *gt, *offsets;
  unsigned int n_data = 0, datastep = 50, misses = 0, max_miss = 0;
  vector<double>* xV = new vector<double>;
  vector<PDF*>* yVP = new vector<PDF*>, *couple = new vector<PDF*>;
  vector<double>* pattume = new vector<double>;
  vector<string> names;
  double c1, c2, c3, chi, dof;
  
  cout << "Name this session: ";
  cin >> session;
  
  LinearFit lf;
  
  ifstream in(argv[1]);
  if(!in){
    cout << "ERROR opening input file" << endl;
    return -1;
  }
  
  
  cout << "How many steps should the alpha PDF have? ";
  cin >> datastep;
  lf.setPrecision(datastep);
  ifstream alphafile(argv[2]);
  if(!alphafile){
    cout << "ERROR opening alpha" << endl;
    return -1;
  }
  lf.add(&alphafile,pattume,couple);
  alpha = couple->at(0);
  alpha->rename("alpha");
  if(alpha->getSteps() > datastep)
    alpha->coarse(alpha->getSteps()/datastep);
  alpha->print();
  
  cout << "How many steps should the data PDFs have? ";
  cin >> datastep;
  
  //get the data
  lf.setPrecision(datastep);
  
  while(lf.add(&in,xV,yVP))
    n_data++;

  delete pattume;
  
  
  //prepare ParametricFit
  nu funz;
  ParametricFit pf(&funz);
  pf.add_fixed_parameter(alpha);
  pf.set_data(xV,yVP);

  pf.print_data((session + "_rough_data.txt").c_str());
  
  unsigned int n_rep, seed;
  double min_value = 0;
  double param_min, param_max;
  unsigned int n_l_step, gamma_step, theta_0_step, N_0_step;
  MultiPDF *total;
  
  cout << "Would you like to load n_l? [y/n] ";
  cin >> ancora;
  if(ancora[0] == 'n'){
    cout << "Insert type (Gauss/Box/Triangular), mean and width for n_l and its steps: ";
    cin >> ancora >> param_min >> param_max >> n_l_step;
    PDFFactory* F = PDFFactoryManager::create(ancora,param_min,param_max);
    n_l = F->create_default(n_l_step);
    n_l->rename("n_l");
  }
  else{
    cout << "Insert filename (_PDF.txt)";
    cin >> ancora;
    n_l = PDF::load(ancora);
  }
  pf.add_fixed_parameter(n_l);
  
  do{
    if(misses > 0){
      cout << "Do you want to reject missed data? [y/n] ";
      cin >> ancora;
      if(ancora[0] == 'y'){
	cout << "Insert maximum number of tolerable misses ";
	cin >> max_miss;
	pf.reject_missed_data(max_miss);
	pf.print_data((session + "_rough_data.txt").c_str());
      }
    }
    
    cout << "Do you want to reset unknown paramters? [y/n] ";
    cin >> ancora;
    
    pf.delete_unknown_parameters();
    
    if(ancora[0] == 'y'){
      
      cout << "Insert filename with parameters (type 0) for setting them manually: ";
      cin >> ancora;
      ifstream params(ancora);
      if(!params){
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
	params >> ancora >> param_min >> param_max >> gamma_step;
	pf.add_unknown_parameter(param_min,param_max,gamma_step,"gamma");
	params >> ancora >> param_min >> param_max >> theta_0_step;
	pf.add_unknown_parameter(param_min,param_max,theta_0_step,"theta_0");
	params >> ancora >> param_min >> param_max >> N_0_step;
	pf.add_unknown_parameter(param_min,param_max,N_0_step,"N_0");
      }
    }
    else{
      cout << "Would you like to optimize parameters? [y/n] ";
      cin >> ancora;
      if(ancora[0] == 'y'){
	cout << "Optimizing parameters" << endl << endl;
	gamma->optimize();
	theta_0->optimize();
	N_0->optimize();
      }
      pf.add_unknown_parameter(gamma->getMin(),gamma->getMax(),gamma_step,"gamma");
      pf.add_unknown_parameter(theta_0->getMin(),theta_0->getMax(),theta_0_step,"theta_0");
      pf.add_unknown_parameter(N_0->getMin(),N_0->getMax(),N_0_step,"N_0");
    }
    
    cout << "Insert maximum number of missable data: ";
    cin >> misses;
    pf.set_misses(misses);
    cout << "Insert mode: value (v), p-value (p), gauss (g), or brute_force (b) (WARNING: brute_force takes time): ";
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
      case 'g': pf.fit(n_rep,seed,ParametricFit::gauss);
		break;
      case 'b': pf.fit(n_rep,seed,ParametricFit::brute_force);
		break;
    }
    
    total = pf.get_unknown_MultiPDF();
    
    
    offsets = total->integrate_along("gamma","offsets");
    gN = total->integrate_along("theta_0","gN");
    gt = total->integrate_along("N_0","gt");
    
    c1 = gN->correlation_index();
    c2 = gt->correlation_index();
    c3 = offsets->correlation_index();
    chi = pf.chi2();
    dof = pf.degrees_of_freedom();
    
    gamma = gN->integrate_along("N_0","gamma")->toPDF();
    theta_0 = offsets->integrate_along("N_0","theta_0")->toPDF();
    N_0 = offsets->integrate_along("theta_0","N_0")->toPDF();
    
    cout << "n_l = " << n_l->mean() << " +- " << sqrt(n_l->var()) << endl;
    cout << "gamma = " << gamma->mean() << " +- " << sqrt(gamma->var()) << endl;
    cout << "theta_0 = " << theta_0->mean() << " +- " << sqrt(theta_0->var()) << endl;
    cout << "N_0 = " << N_0->mean() << " +- " << sqrt(N_0->var()) << endl << endl;
    
    n_l->print((session + "_n_l_G.txt").c_str());
    gamma->print((session + "_gamma_G.txt").c_str());
    theta_0->print((session + "_theta_0_G.txt").c_str());
    N_0->print((session + "_N_0_G.txt").c_str());
    
    cout << "Correlation coefficient between N_0 and gamma = " << c1 << endl;
    cout << "Correlation coefficient between theta_0 and gamma = " << c2 << endl;
    cout << "Correlation coefficient between N_0 and theta_0 = " << c3 << endl << endl;
    cout << "Chi2/dof = " << chi << '/' << dof << endl;
    
    pf.print_residuals((session + "_residuals.txt").c_str());
    
    if(misses > 0)
      pf.print_misses((session + "_misses.txt").c_str());
    
    cout << "Would you like to print the MultiPDFs? [y/n] ";
    cin >> ancora;
    if(ancora[0] == 'y'){
      cout << "printing gN" << endl;
      gN->print((session + "_gN_G.txt").c_str());
      cout << "printing gt" << endl;
      gt->print((session + "_gt_G.txt").c_str());
      cout << "printing offsets" << endl;
      offsets->print((session + "_offsets_G.txt").c_str());
    }
    
    cout << "Do you want to fit again? [y/n]: ";
    cin >> ancora;
  }while(ancora[0] == 'y');
  
  gamma->rename((session + "_gamma").c_str());
  
  gamma->modifying_routine();
  
  gamma->save((session + "_gamma_PDF.txt").c_str());
  
  ofstream out((session + "_results.txt").c_str());
  if(!out){
    cout << "Failed to print results" << endl;
    return -1;
  }

  out << "n_l = " << n_l->mean() << " +- " << sqrt(n_l->var()) << endl;
  out << "gamma = " << gamma->mean() << " +- " << sqrt(gamma->var()) << endl;
  out << "theta_0 = " << theta_0->mean() << " +- " << sqrt(theta_0->var()) << endl;
  out << "N_0 = " << N_0->mean() << " +- " << sqrt(N_0->var()) << endl << endl;
  out << "Correlation coefficient between N_0 and gamma = " << c1 << endl;
  out << "Correlation coefficient between theta_0 and gamma = " << c2 << endl;
  out << "Correlation coefficient between N_0 and theta_0 = " << c3 << endl << endl;
  out << "Chi2/dof = " << chi << '/' << dof << endl;
    
  cout << "saving total" << endl;
  total->save((session + "_total_MPDF.txt").c_str());
  
  cout << endl << endl;
  return 0;
}
