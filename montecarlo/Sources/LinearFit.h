#ifndef LinearFit_h
#define LinearFit_h

#include <string>
#include <vector>

using namespace std;

class PDF;

class LinearFit {
  
  public:
    
    LinearFit();
    ~LinearFit();
    
    PDF* getA() const;
    PDF* getB() const;
    
    void setSeed(unsigned int i);
    void setPrecision(unsigned int i);
    void setAB_steps(unsigned int i);
    void setN(unsigned int i);
    void setA_range(double m, double M);
    void setB_range(double m, double M);
    void setIX_range(double m, double M);
    void setIY_range(double m, double M);
    
    void reset();//resets only further settings
    
    bool add(ifstream* file, vector<double>* xV, vector<PDF*>* yVP);
    bool add(ifstream* file);
    void setData(vector<double>* xV, vector<PDF*>* yVP);
    void clearData();
    
    static double* fit_sample(const vector<double>* xV, const vector<double>* yV);
    
    void fit(const string& fileName, unsigned int start_line = 0, unsigned int end_line = 0);//reads data from line start_line to end_line
    void fit(vector<double>* xV,vector<PDF*>* yVP);
    void fit();
    
    PDF** intersec(const string& fileN1, const string& fileN2);
    PDF** intersec(const string& fileName, double retta_a, double retta_b);// intersecates the file with a given line (y = a + b*x)
    
    double chi2(vector<double>* xV, vector<PDF*>* yVP) const;
    double chi2() const;
    double rho(vector<double>* xV, vector<PDF*>* yVP) const;
    double rho() const;
    double T_N() const;
    int degrees_of_freedom() const;
  
  private:
    
    LinearFit(const LinearFit& l);
    LinearFit& operator=(const LinearFit& l);
    
    //data vectors
    vector<double>* data_x;
    vector<PDF*>* data_y;
    
    //settings
    unsigned int seed;
    unsigned int precision;//number of steps of the PDFs of the simulated sample
    unsigned int ab_steps;//number of steps of the PDFs of a and b or ix and iy
    unsigned int n;
    bool isSet;
    
    //further settings
    double min_a;
    double max_a;
    double min_b;
    double max_b;
    bool isAset;
    bool isBset;
    
    double min_ix;
    double max_ix;
    double min_iy;
    double max_iy;
    bool isIXset;
    bool isIYset;
    
    //PDFs of the coefficients of the fit function f = a + b*x
    PDF* a;
    PDF* b;
    
    //correlation coefficients
    mutable double r;
    
    void checkSet();
};


#endif