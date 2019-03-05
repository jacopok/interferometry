#ifndef ParametricFit_h
#define ParametricFit_h

#include <vector>
#include <map>
#include <string>

using namespace std;

class PDF;
class MultiPDF;

class ParametricFit {
  
  public:
    
    enum mode {value, p_value, brute_force};
    
    class Func {
      public:
	Func(const string& n):
	  name(n){}
	virtual ~Func(){}
	
	string get_name() const{return name;}
	virtual unsigned int n_fix() = 0;//number of fixed parameters
	virtual unsigned int n_unk() = 0;//number of unknown parameters
	virtual double f(double x, vector<double>* v_fix, vector<double>* v_unk) = 0;
	//v_fix stands for fixed_parameters, v_unk for unknown_parameters
	
      private:
	string name;
    };
    
    ParametricFit(Func* g);
    ParametricFit();
    ~ParametricFit();
    
    void set_func(Func* g);
    void set_misses(unsigned int i);
    void add_fixed_parameter(PDF* p);
    void delete_fixed_parameters();
    void add_unknown_parameter(double min, double max, unsigned int steps,const string& name);
    void delete_unknown_parameters();
    void add_data(double x, PDF* y_PDF);
    void set_data(vector<double>* xV, vector<PDF*>* yVP);
    void delete_data();
    void clear();
    
    MultiPDF* get_unknown_MultiPDF() const;
    
    double chi2(vector<double>* fix_par_values, vector<double>* unk_par_values) const;
    double chi2() const;
        
    bool isready() const;
    void set_min_value(double m);
    
    void fit(unsigned int n_rep, unsigned int seed, mode q);
    
    void print_misses(const string& filename) const;
  
    
  private:
    
    ParametricFit(const ParametricFit& p);
    ParametricFit& operator=(ParametricFit& p);
    
    Func* f;
    
    vector<PDF*>* fixed_parameters;
    
    bool extern_data_vectors;
    unsigned int misses;
    double min_value;
    vector<double>* data_x;
    vector<PDF*>* data_y_PDF;
    vector<unsigned int>* data_misses;
     
    //map<string,PDF*>* unknown_parameters_PDFs;
    
    MultiPDF* unknown_MultiPDF;
    
    
    double data_iterator(vector<double>* v_fix, vector<double>* v_unk, mode q) const;
  
};

#endif