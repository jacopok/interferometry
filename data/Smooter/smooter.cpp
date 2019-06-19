#include <fstream>

using namespace std;

int main(int nargs, char* args[]) {
	ifstream in(args[1]);
	ofstream out(args[2]);
	
	double a,b,oa,ob;
	in>>a>>b;
	while(!in.eof()) {
		oa = a;
		ob = b;
		in>>a>>b;
		out<< (a+oa)/2.0 <<" "<< (b+ob)/2.0 << endl;
	}
}
