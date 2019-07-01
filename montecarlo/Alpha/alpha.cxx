#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

#include "PDF.h"
#include "MultiPDF.h"
#include "LinearFit.h"
#include "PDFFactory.h"
#include "PDFFactoryManager.h"
#include "ErrorPropagator.h"

using namespace std;

class Alpha: public ErrorPropagator{
  public:
    Alpha(const vector<PDF*>* vp):
      ErrorPropagator(vp){}
    ~Alpha(){}
    
    double f() const{
      double a = sim_sample->at(0);
      return 1/a;
    }
};


int main (int argc, char* argv[]){
  string ancora;
  PDF *a, *b, *alpha;
  double chi, dof, rho;
  unsigned int datastep;

  LinearFit lf;
  
  ifstream in(argv[1]);
  if(!in){
    cout << "ERROR opening input file" << endl;
    return -1;
  }
  
  cout << "How many steps should the data PDFs have? ";
  cin >> datastep;
  
  //get the data
  lf.setPrecision(datastep);
  
  //enable MultiPDF
  bool emp = true;
  lf.enableMultiPDF(emp);
  
  while(lf.add(&in)){}
  
  double param_max, param_min;
  unsigned int a_step, b_step, n_rep, seed;
  
  do{
    
    cout << "Do you want to reset unknown paramters? [y/n] ";
    cin >> ancora;
    
    if(ancora[0] == 'y'){
      
      cout << "Insert filename with parameters (type 0) for setting them manually: ";
      cin >> ancora;
      ifstream params(ancora);
      if(!params){
	cout << "Insert min and max values for a and its steps: ";
	cin >> param_min >> param_max >> a_step;
	lf.setA_range(param_min,param_max);
	
	cout << "Insert min and max values for b and its steps: ";
	cin >> param_min >> param_max >> b_step;
	lf.setB_range(param_min,param_max);
	
	lf.setAB_steps(b_step);
      }
      else{
	params >> ancora >> param_min >> param_max >> a_step;
	lf.setA_range(param_min,param_max);
	params >> ancora >> param_min >> param_max >> b_step;
	lf.setB_range(param_min,param_max);
	lf.setAB_steps(b_step);
      }
    }
    else{
      cout << "Would you like to optimize parameters? [y/n] ";
      cin >> ancora;
      if(ancora[0] == 'y'){
	cout << "Optimizing parameters" << endl << endl;
	a->optimize();
	b->optimize();
      }
      lf.setA_range(a->getMin(),a->getMax());
      lf.setB_range(b->getMin(),b->getMax());
      lf.setAB_steps(b_step);
    }
    
    cout << "Insert number of repetition and seed: ";
    cin >> n_rep >> seed;
    lf.setN(n_rep);
    lf.setSeed(seed);
    cout << "Fitting via LinearFit" << endl;
    
    lf.fit();
    
    chi = lf.chi2();
    dof = lf.degrees_of_freedom();
    
    cout << "Chi2/dof = " << chi << '/' << dof << endl;
    
    a = lf.getA();
    b = lf.getB();
    
    a->print("a_G.txt");
    b->print("b_G.txt");
    
    cout << "Do you want to fit again? [y/n] ";
    cin >> ancora;
  }while(ancora[0] == 'y');
  
  if(emp){
      MultiPDF* ab = lf.getAB();
      rho = ab->correlation_index();
      cout << "Correlation coefficient: " << rho << endl;
      ab->save("ab_MPDF.txt");
  }
  
  do{
    b->modifying_routine();
    
    vector<PDF*> v{b};
    double alpha_min = 1/b->getMax(), alpha_max = 1/b->getMin();
    unsigned int alpha_step;
    Alpha prop(&v);
    
    cout << "How many steps should alpha have? ";
    cin >> alpha_step;
    cout << "Insert number of repetition and seed: ";
    cin >> n_rep >> seed;
    
    alpha = prop.propagation(n_rep,seed,alpha_min,alpha_max,alpha_step,"alpha");
    
    alpha->modifying_routine();
    
    cout << "Do you want to propagate again? [y/n] ";
    cin >> ancora;
  }while(ancora[0] == 'y');
  
  alpha->save("alpha_PDF.txt");
  
  
  ofstream out("results.txt");
  if(!out){
    cout << "Failed to print results" << endl;
    return -1;
  }

  out << "a = " << a->mean() << " +- " << sqrt(a->var()) << endl;
  out << "b = " << b->mean() << " +- " << sqrt(b->var()) << endl;
  out << "alpha = " << alpha->mean() << " +- " << sqrt(alpha->var()) << endl;
  
  out << "Chi2/dof = " << chi << '/' << dof << endl;
  if(emp)
      out << "rho = " << rho << endl;
  
  
  cout << endl << endl;
  return 0;
}
