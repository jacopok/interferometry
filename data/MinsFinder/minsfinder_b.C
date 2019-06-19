#include <iostream>
#include <TGraph.h>
#include <TF1.h>

#define num 3

using namespace std;

void minsfinder_b(char* inName,char* outName,double lb, double ub) {
	TGraph* tg = new TGraph(inName);
	TF1 func ("func","[0] * x * x + [1] * x + [2]");
	ifstream in(inName);
	ofstream out(outName);

	double start = 0, end = 0, cont = 0, t1, t2, fr = -1, sum = 0, wsum = 0;
	while( !in.eof() ) {
		in>>t1>>t2;
		if ( t2 < lb ) {
			if ( cont == 0) start = t1;
			cont ++;
			end = t1;
			sum += (100-t2)*(100-t2)*t1;
			wsum += (100-t2)*(100-t2);
		}
		if ( t2 > ub ) {
			if ( cont >= num ) {
				out<<++fr<<"\t"<< sum / wsum<<endl;
//				func.SetParameters(1,0,0);
//				tg->Fit(&func,"Q0","",min(start,end),max(start,end));
//				out<<++fr<<"\t"<< - func.GetParameter(1) / 2.0 / func.GetParameter(0) <<endl;
//				cout<<start<<" "<<end<<" "<< - func.GetParameter(1) / 2.0 / func.GetParameter(0) << " "<<tg->GetN()<< endl;
			}
			cont = 0;
			sum = 0;
			wsum = 0;
		}
	}
}
