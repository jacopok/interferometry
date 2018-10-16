#include "PDFFactoryManager.h"

#include "PDFFactory.h"
#include "GaussFactory.h"
#include "TriangularFactory.h"
#include "UniformFactory.h"

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
    
    default:
      cout << "no PDF aveiable with name " << type << endl;
      return 0;
  }
}