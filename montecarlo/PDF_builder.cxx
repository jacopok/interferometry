#include <iostream>
#include <cmath>
#include <string>
#include "PDF.h"
#include "PDFFactoryManager.h"
#include "PDFFactory.h"
#include "QbicFactory.h"

int main(int argc, char* argv[]){
		
		string type = "qbic";
		string filename = "bp3p3_-13s50.txt";
		double a = -964.7479112437524;
		double b = 16.62916748424382;
		double c = 16.315325283124277;
		int steps = 50;
		
		QbicFactory* F = new QbicFactory(a,b,c);
		
		PDF* p = F->create_default(steps);
		
		p->print(filename);
		
	return 0;
}
