#include <iostream>
#include <cmath>
#include "PDF.h"

using namespace std;

int main(int argc, char* argv[]){
  if(argc < 2){
    cout << "Please type ./PDF_reader [PDF_file_name]" << endl << endl;
    return 0;
  }
  
  PDF* a = PDF::load(argv[1]);
  cout << endl << a->getName() << " = " << a->mean() << " +- " << sqrt(a->var()) << endl;
  
  cout << endl;
  return 0;
}