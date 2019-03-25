#ifndef MultiPDF_h
#define MultiPDF_h

#include <vector>
#include <string>
#include <map>

using namespace std;

class PDF;


class MultiPDF {
  
  public:
    
    MultiPDF(const string& s);
    ~MultiPDF();
    MultiPDF(const MultiPDF& p);
    MultiPDF& operator=(const MultiPDF& p);
    
    static MultiPDF* load(const string& filename);
    static MultiPDF* merge(const vector<PDF*>* vP, const string& s);//transforms a vector of independent PDFs into a MultiPDF
    
    void initialize_counters() const;
    bool update_counters() const;
    
    void rename(const string& s);
    
    void add_PDF(PDF* p);//adds only the domain of the PDF: i.e. the MultiPDF remains empty 
    void zero();
    void clear();
    
    bool add(vector<double>* v, double val = 1.0);//increases by 1 the point corresponding to the coordinates in v (if found)
    double somma() const;
    bool isnormalized() const;
    void normalize();
    vector<double>* mean() const;
    
    void build_CDF() const;
    unsigned int CDF_bigindex(double x) const;
    vector<unsigned int>* bigindex2indexs(unsigned int b) const; //converts bigindex to indexs
    vector<double>* inverted_CDF_value(double x) const;
    
    unsigned int getDimension() const;
    unsigned int getSize() const;
    vector<unsigned int>* getCounters() const;
    vector<PDF*>* getPDFs() const;
    
    double* access() const;// = access(counters)
    double* access(vector<unsigned int>* indexs) const;
    unsigned int getAxis(const string& s) const;
    
    MultiPDF* integrate_along(const string& PDFname, const string& newname, double min = 0, double max = -1) const;
    MultiPDF* integrate_along(unsigned int i, const string& newname, double min = 0, double max = -1) const;
    
    MultiPDF* subMultiPDF(vector<string>* PDFnames, const string& newname) const;
    MultiPDF* subMultiPDF(vector<unsigned int>* indexs, const string& newname) const;
    
    
    void print(const string& filename) const;
    void print() const;
    void save(const string& filename) const;
    void save() const;
    
    //functions that work only with specific dimensions
    
    //dimension = 1
    PDF* toPDF() const;
    
    //dimension = 2
    double correlation_index() const;
    
  private:
    
    string name;
    unsigned int dimension;
    unsigned int size;
    mutable vector<unsigned int>* counters;//vector with indeces of the MultiPDF: very useful when scanning the MultiPDF
    vector<PDF*>* PDFs;// vector with the steps of the PDFs
    vector<double>* values;// all the data
    mutable bool empty;
    
    mutable vector<double>* CDF;
  
};

#endif