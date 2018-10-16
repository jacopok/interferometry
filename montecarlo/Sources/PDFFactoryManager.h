#ifndef PDFFactoryManager_h
#define PDFFactoryManager_h

#include <string>


class PDFFactory;

class PDFFactoryManager {
  
  public:
     
    static PDFFactory* create(const std::string& type, double a, double b);
  
  
  
};



#endif