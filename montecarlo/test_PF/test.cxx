#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

#include "PDF.h"
#include "MultiPDF.h"
#include "PDFFactoryManager.h"
#include "PDFFactory.h"
#include "DataSimulator.h"

#include "LinearFit.h"
#include "ParametricFit.h"

using namespace std;

class linFunc: public ParametricFit::Func {
    public:
	linFunc():
	  Func("linFunc"){}
	~linFunc(){}
	
	virtual unsigned int n_fix(){return 0;}
	virtual unsigned int n_unk(){return 2;}
	virtual double f(double x, vector<double>* v_fix, vector<double>* v_unk){
	  return v_unk->at(0) + x * v_unk->at(1);
	}
};

int main () {
  //build 10 PFDs on the line y = x
  PDFFactory* F;
  vector<double> xv;
  vector<PDF*> yvp;
  unsigned int n_data;
  double noise_amplitude, pointwise_noise;
  string type;
  
  cout << "This programm compares LinearFit and ParametricFit\n\n";
  cout << "How many data do you want to simulate? (Gaussians with sigma=1 along the line y = x) ";
  cin >> n_data;
  
  cout << "Which type of noise do you want? (Gauss, Triangular) ";
  cin >> type;
  cout << "How much noise do you want to add to the data? ";
  cin >> noise_amplitude;
  F = PDFFactoryManager::create(type,0,noise_amplitude);
  PDF* noise = F->create_default(100);
  delete F;
  
  for(unsigned int i = 0; i < n_data; i++){
    pointwise_noise = DataSimulator::simulate_one(noise);
    F = PDFFactoryManager::create("Gauss",i + pointwise_noise,1);
    xv.push_back(i);
    yvp.push_back(F->create_default(500));
    delete F;
  }
  
  //visualize rough data
  ofstream rough_data("rough_data.txt");
  if(!rough_data){
    cout << "ERROR creating rough_data.txt" << endl;
    return -1;
  }
  for(unsigned int i = 0; i < xv.size(); i++)
    rough_data << xv[i] << '\t' << yvp[i]->mean() << '\t' << sqrt(yvp[i]->var()) << endl;
  rough_data.close();
  
  //fit via LinearFit
  LinearFit lf;
  lf.setAB_steps(100);
  lf.setPrecision(400);
  
  double a_min = -4, a_max = 4, b_min = 0, b_max = 2;
  string ancora, vuota;
  PDF *Alf, *Blf, *Apf, *Bpf;
  MultiPDF* ABlf;
  unsigned int n_rep = 0, seed = 0;
  
  do{
    ancora = vuota;
    cout << "Insert number of repetitions for LinearFit: ";
    cin >> n_rep;
    cout << "Insert min and max values for a: ";
    cin >> a_min >> a_max;
    cout << "Insert min and max values for b: ";
    cin >> b_min >> b_max;
    
    lf.setN(n_rep);
    lf.setA_range(a_min,a_max);
    lf.setB_range(b_min,b_max);
    
    lf.enableMultiPDF(true);
    
    cout << "Fitting via LinearFit" << endl;
    lf.fit(&xv,&yvp);
    
    Alf = lf.getA();
    Blf = lf.getB();
    ABlf = lf.getAB();
    
    cout << "a = " << Alf->mean() << " +- " << sqrt(Alf->var()) << endl;
    cout << "b = " << Blf->mean() << " +- " << sqrt(Blf->var()) << endl;
    
    Alf->print("Alf_G.txt");
    Blf->print("Blf_G.txt");
    ABlf->print("ABlf_G.txt");
    
    cout << "correlation index = " << ABlf->correlation_index() << endl << endl;
    
    cout << "Do you want to fit again? [y/n]: ";
    cin >> ancora;
  }while(ancora[0] == 'y');
  
  //fit via ParametricFit
  linFunc funz;
  ParametricFit pf(&funz);
  pf.add_unknown_parameter(a_min,a_max,100,"Apf");
  pf.add_unknown_parameter(b_min,b_max,100,"Bpf");
  pf.set_data(&xv,&yvp);
  double min_value = 0;
  MultiPDF* ABpf;
  
  do{
    ancora = vuota;
    cout << "Insert mode: value (v), p-value (p) or brute_force (b) (WARNING: brute_force takes time): ";
    cin >> ancora;
    cout << "Insert min_value: ";
    cin >> min_value;
    pf.set_min_value(min_value);
    cout << "Insert number of repetition and seed: ";
    cin >> n_rep >> seed;
    cout << "Fitting via ParametricFit" << endl;
    
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
    
    ABpf = pf.get_unknown_MultiPDF();
    Apf = ABpf->integrate_along("Bpf","Apf")->toPDF();
    Bpf = ABpf->integrate_along("Apf","Bpf")->toPDF();
    
    cout << "a = " << Apf->mean() << " +- " << sqrt(Apf->var()) << endl;
    cout << "b = " << Bpf->mean() << " +- " << sqrt(Bpf->var()) << endl;
    
    Apf->print("Apf_G.txt");
    Bpf->print("Bpf_G.txt");
    ABpf->print("ABpf_G.txt");
    
    cout << "correlation index = " << ABpf->correlation_index() << endl << endl;
    
    cout << "Do you want to fit again? [y/n]: ";
    cin >> ancora;
  }while(ancora[0] == 'y');
  
  pf.clear();
  
  cout << "Final results: " << endl << endl;
  
  cout << "Alf = " << Alf->mean() << " +- " << sqrt(Alf->var()) << endl;
  cout << "Blf = " << Blf->mean() << " +- " << sqrt(Blf->var()) << endl;
  cout << "Apf = " << Apf->mean() << " +- " << sqrt(Apf->var()) << endl;
  cout << "Bpf = " << Bpf->mean() << " +- " << sqrt(Bpf->var()) << endl;
  
  cout << endl << endl;
  return 0;
}