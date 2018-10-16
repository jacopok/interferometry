#ifndef ParametricFit_h
#define ParametricFit_h

#include <vector>
#include <map>
#include <string>

using namespace std;

class PDF;


class ParametricFit {
  
  public:
    
    enum mode {value, p_value};
    
    class Func {
      public:
	Func();
	virtual ~Func();
	
	virtual unsigned int n_arguments();//number of fixed and unknown parameters 
	virtual double f(double x, vector<double>* v_fix, vector<double>* v_unk);
	//v_fix stands for fixed_parameters, v_unk for unknown_parameters
	
    };
    
    ParametricFit(Func* g);
    ParametricFit();
    ~ParametricFit();
    
    void set_func(Func* g);
    void add_fixed_parameter(PDF* p);
    void delete_fixed_parameters();
    void add_unknown_parameter(double min, double max, unsigned int steps, string& name);
    void delete_unknown_parameters();
    void add_data(double x, PDF* y_PDF);
    void delete_data();
    
    PDF* get_unknown_PDF(string& name) const;
    
    bool isready() const;
    
    void fit(unsigned int n_rep, unsigned int seed, mode q);
  
    
  private:
    
    ParametricFit(const ParametricFit& p);
    ParametricFit& operator=(ParametricFit& p);
    
    Func* f;
    
    vector<PDF*>* fixed_parameters;
    vector<double>* unknown_parameters_min;
    vector<double>* unknown_parameters_max;
    vector<unsigned int>* unknown_parameters_steps;
    vector<string>* unknown_parameters_name;
    
    vector<double>* data_x;
    vector<PDF*>* data_y_PDF;
     
    map<string,PDF*>* unknown_parameters_PDFs;
    
  
};

#endif