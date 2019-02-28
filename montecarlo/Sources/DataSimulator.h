#ifndef DataSimulator_h
#define DataSimulator_h

#include <vector>
#include <string>

using std::vector;
using std::string;

class PDF;
class MultiPDF;

class DataSimulator {
  
  public:
    
    DataSimulator();
    ~DataSimulator();
    
    static double simulate_one(const PDF* p);
    static vector<double>* simulate_sample(const vector<PDF*>* vp);
    
    static vector<double>* multi_simulate_one(const MultiPDF* p);
    
    /*
    static void root_one(const PDF* p, unsigned int n, const string& fileName);
    static void root_one(const PDF* p, unsigned int n, const string& fileName, unsigned int nBin, double min, double max);
    */
    
  private:
    
    DataSimulator(const DataSimulator& d);
    DataSimulator& operator=(const DataSimulator& d);
    
};


#endif