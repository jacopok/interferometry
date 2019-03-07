#ifndef PDF_h
#define PDF_h

#include <vector>
#include <string>
#include <fstream>

using namespace std;

class PDF {
  
  public:
  
    //constructors & destructors
    PDF(double m, double M, const vector<double>& v, string s);
    PDF(double m, double M, unsigned int st, string s); //empty PDF
    ~PDF();
    static PDF* load(const string& fileName);
    PDF(const PDF& p);
    PDF& operator=(const PDF& p);
    
    //access to private members
    double getMin() const;
    double getMax() const;
    double getDx() const;
    double getValue(unsigned int i) const;
    int getIndex(double x) const;
    unsigned int getSteps() const;
    string getName() const;
    void rename(const string& n);
    
    double somma() const;
    
    //functions to slightly modify the PDF
    bool isnormalized() const;
    void normalize();
    void smoothen(unsigned int f); //each value is replaced with the mean of the nearest f values
    void coarse(unsigned int f); //the number of values is divided by f
    PDF* optimize(); //removes zeros from the front and the back of values
    void modifying_routine(); //interactive usage of normalize, smoothen, coarse, optimize
    PDF* traslate(double l); //traslate the whole PDF along the x-axis
    bool add(double x, double val = 1.0); //if x is fuond in values, values[i] += val: it will be used for building simulated PDFs
    
    //statistical properties
    double value(double x) const;
    double probability(double a, double b) const;
    double p_value(double x) const;
    double compatibility(double x) const;
    double compatibility(const PDF& p) const;
    double overlap(const PDF& p) const; //returns the area beneath both *this and p
    double mean() const;
    double mode() const;
    double median() const;
    double var() const;//returns the variance
    vector<double>* central_ordering_IC(double CL) const;//returns the Confidence interval with central ordering rule
    
    //functions dealing with the Cumulative Density Function (CDF)
    double CDF_value(double x) const;//returns the value of the CDF in position x
    double inverted_CDF_value (double x) const;//returns the value of the inverted CDF in position x
    void build_CDF() const;//builds the entire CDF for the Monte Carlo simulation and saves it in CDF
    vector<vector<double>>* get_CDF() const;
    
    //operations between PDFs
    PDF operator*(double l) const;
    PDF operator/(double l) const;
    PDF& operator*=(double l);
    PDF& operator/=(double l);
    PDF operator*(double (*prior) (double)) const;
    PDF operator*(const PDF& p) const;
    PDF operator+(const PDF& p) const;
    
    
    void print() const;
    void print(const string& fileName) const;
    void save(const string& fileName) const;
    
    /*
    //builds a root TH1F to plot the PDF
    void root(const string& fileName) const;
    */
    
    
  private:
    
    PDF();
    
    string name;
    unsigned int steps;
    double min;
    double max;
    double dx;
    vector<double> values;
    mutable vector<vector<double>>* CDF;
    
};



#endif