#include "PDFFactoryManager.h"

#include "PDFFactory.h"
#include "GaussFactory.h"
#include "TriangularFactory.h"
#include "UniformFactory.h"
#include "DigitalFactory.h"

#include <iostream>

using namespace std;

PDFFactory* PDFFactoryManager::create(const string& type, double a, double b){
  switch(type[0]){
    case 'G':
    case 'g':
      return new GaussFactory(a,b);
    case 'T':
    case 't':
      return new TriangularFactory(a,b);
    case 'U':
    case 'u':
      return new UniformFactory(a,b);
    case 'B':
    case 'b':
      return new UniformFactory(a - b/2,a + b/2);
    case 'O':
    case 'o':
      return new DigitalFactory(a,0.05*b,0.03);
    
    default:
      cout << "no PDF avaiable with name " << type << endl;
      return 0;
  }
}