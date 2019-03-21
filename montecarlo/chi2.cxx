#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char* argv[]){
  if(argc < 2){
    cout << "Please type ./chi2 [residuals_file_name]" << endl << endl;
    return 0;
  }
  
  ifstream in(argv[1]);
  if(!in){
    cout << "Invalid input filename" << endl;
    return -1;
  }
  
  double sum = 0;
  double x, y, sig;
  
  while(in >> x >> y >> sig)
    sum += y*y/(sig*sig);
  
  cout << "\n\nchi2 = " << sum << endl << endl;
  
  cout << endl;
  return 0;
}