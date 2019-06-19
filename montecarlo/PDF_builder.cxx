#include <iostream>
#include <cmath>
#include <string>
#include "PDF.h"
#include "PDFFactoryManager.h"
#include "PDFFactory.h"
#include "QbicFactory.h"

int main(int argc, char* argv[]){
		
		string type = "qbic";
		string filename = argv[2];
		double a = double(argv[3]);
		double b = double(argv[4]);
		double c = double(argv[5]);
		int steps = int(argv[6]);
		
		QbicFactory* F = new QbicFactory(a,b,c);
		
		PDF* p = F->create_default(steps);
		
		p->print(filename);
		
	return 0
}
