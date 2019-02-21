#ifndef MultiErrorPropagator_h
#define MultiErrorPropagator_h

#include <vector>
#include <string>
#include <map>
using std::map;
using std::string;
using std::vector;

class PDF;
class MultiPDF;

class MultiErrorPropagator {
  
  public:
  
      MultiErrorPropagator(const vector<MultiPDF*>* vm, const vector<PDF*>* v, const string& name);
      virtual ~MultiErrorPropagator();
      
      //function of the PDFs in vP
      virtual vector<double>* f() const = 0;
      
      void simulate_sample() const;
      
      MultiPDF* getResult() const;//gives access to result in order to initialize it
      
      //builds the MultiPDF of the aleatory variable f
      void propagation(unsigned int n, unsigned int seed) const;
      //n is the number of simulations to create the MultiPDF
  
  protected:
    
    MultiErrorPropagator(const MultiErrorPropagator& e);
    MultiErrorPropagator& operator=(const MultiErrorPropagator& e);
    
    const vector<MultiPDF*>* vMP;
    const vector<PDF*>* vP;
    
    mutable map<string,double>* sim_sample;
    
    MultiPDF* result;
};


#endif