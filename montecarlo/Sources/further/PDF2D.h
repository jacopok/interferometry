#ifndef PDF2D_h
#define PDF2D_h

#include <vector>
#include <string>
#include <map>

using namespace std;

class PDF;


class PDF2D {
  
  public:
    
    PDF2D(double x_m, double x_M, unsigned int x_s, const string& x_n,
	  double y_m, double y_M, unsigned int y_s, const string& y_n); //empty PDF2D
    PDF2D(double x_m, double x_M, const string& x_n, double y_m, double y_M, const string& y_n, const vector<vector<double>>& v);
    static PDF2D* load(const string& filename);
    ~PDF2D();
    
    double somma() const;
    bool isnormalized() const;
    void normalize();
    
    void compute_marginals()
    PDF* marginal(const string& name) const;
    PDF* marginal(unsigned int index) const;
    //WARNING: this function do NOT return a copy of the PDFs, instead they give access to `marginals`
    
    void print(const string& filename) const;
    void save(const string& filename) const;
    
  private:
    
    double x_min;
    double x_max;
    unsigned int x_steps;
    double dx;
    string x_name;
    
    double y_min;
    double y_max;
    unsigned int y_steps;
    double dy;
    string y_name
    
    vector<vector<double>>* values;
    
    bool uptodate;
    vector<PDF*>* marginals;
    
  
};

#endif