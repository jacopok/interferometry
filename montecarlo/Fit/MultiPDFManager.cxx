#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>

#include "PDF.h"
#include "MultiPDF.h"

using namespace std;

int main(int argc, char* argv[]){
  MultiPDF* total = MultiPDF::load(argv[1]);
  
  vector<string> names;
  
  names = vector<string>{"n_l","theta_0"};
  
  MultiPDF* nt = total->subMultiPDF(&names,"nt");
  cout << nt->correlation_index();
  nt->print("nt_G.txt");
  
  names = vector<string>{"n_l","gamma"};
  
  MultiPDF* r = total->subMultiPDF(&names,"r");
  PDF* n_l_1 = r->integrate_along("gamma","n_l_1",0.000041,0.000045)->toPDF();
  n_l_1->print("n_l_1_G.txt");
  
  cout << endl;
  return 0;
}