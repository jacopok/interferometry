#ifndef Util_H
#define Util_H

#include <vector>

class Util{
  
  public:
    
    static unsigned int binary_search(const vector<double>* v, double x, unsigned int min, unsigned int max){
      if(min == max)
	return min;
      unsigned int center = (min + max)/2;
      if(v->at(center) >= x)
	return binary_search(v,x,min,center);
      return binary_search(v,x,center + 1,max);
    }
      
};

#endif
