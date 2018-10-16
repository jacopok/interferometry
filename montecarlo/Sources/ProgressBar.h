#ifndef ProgressBar_h
#define ProgressBar_h

#include <iostream>
using namespace std;

class ProgressBar {
  
 public:
   
  static void bar(unsigned int i, unsigned int n) {
     if(i == 0)
      cout << "|                    |\n " << flush;
    
    if(!((i*20)%n)){
      cout << "*" << flush;
    }
    
    if(i == n - 1)
      cout << "*" << endl;
    
    return;
  }
  
  static void percentages(unsigned int i, unsigned int n) {
    if(i == 0)
      cout << "\t\t\t\t\t\t\t" << flush;
    
    if(!((i*100)%n)){
      if((i*100)/n > 0)
	cout << "\b\b" << flush;
      if((i*100)/n > 9)
	cout << '\b' << flush;
      cout << (i*100)/n << "%" << flush;
    }
    
    if(i == n - 1)
      cout << "\b\b\b100%" << endl;
    
    return;
  }
   
};


#endif