/*	This test is useful when a fit seems already good and the T_N-test gives >99,9% 
	The LTest is the mean compatibility between a data set and a function fitting it
	If the fit is not good, this is not a very pregnant test
*/

#ifndef LTest_H
#define LTest_H

#include "PDF.h"


#include <vector>
#include <fstream>

using namespace std;


class LTest {
  
  public:
    
    LTest();
    LTest(const vector<double>* x, const vector<PDF*>* y);
    ~LTest();
    
    double getL();
    
    void add(double x, PDF* yP);
    bool add(ifstream* file, unsigned int precision = 1000);
    void setFunc(double (*f) (double));
    
    void reset();
  
  private:
    
    vector<double> xv;
    vector<PDF*> yvP;
    double (*func) (double);//function fitting data
    
    double L;//mean distance func-data in sigma units
    
    bool outdated;
    void update();
    
    PDF* g;
    
};

#endif