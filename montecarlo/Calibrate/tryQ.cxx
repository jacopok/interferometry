#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "QbicFactory.h"
#include "PDF.h"

using namespace std;

int main(int argc, char* argv[]){
  
  QbicFactory f1(1,1.001,1);
  PDF* q1 = f1.create_default(200);
  q1->print("q1_G.txt");
  
  QbicFactory f2(1,2,1);
  PDF* q2 = f2.create_default(200);
  q2->print("q2_G.txt");
  
  QbicFactory f3(1,5,1);
  PDF* q3 = f3.create_default(200);
  q3->print("q3_G.txt");
  
  cout << endl;
  return 0;
}