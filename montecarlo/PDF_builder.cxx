#include <iostream>
#include <cmath>
#include <string>
#include "PDF.h"
#include "PDFFactoryManager.h"
#include "PDFFactory.h"
#include "QbicFactory.h"

int main(int argc, char* argv[]){
		
		string type = "qbic";
		string filename = "bp3p3_-10s50.txt";
		double a = -825.2479112437524;
		double b = 19.535982024711696;
		double c = 20.87260966294067;
		int steps = 50;
		
		QbicFactory* F = new QbicFactory(a,b,c);
		
		PDF* p = F->create_default(steps);
		
		p->print(filename);
		
	return 0;
}
