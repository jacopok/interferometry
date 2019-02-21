#include "Point.h"
#include "Interval.h"
#include "MinsReceiver.h"
#include <vector>
#include <thread>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TLine.h>
#include <TF1.h>
#include <algorithm>
#include <fstream>
#include <cfloat>
#include <new>
#include <iostream>

using namespace std;

#ifndef Analisis_H
#define Analisis_H

#include <vector>
#include "Point.h"
#include "Interval.h"
#include <algorithm>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TF1.h>
#include "MinsReceiver.h"

using namespace std;

class Analisis {
private:
	int id;
	int col,row;
	double *data;
	double G[2];
	vector<Point> *pdata;
	TGraph *ptg;
	TGraph2D *tg;
	vector<Interval> *sep;
	vector<double>* mins;
	MinsReceiver *mrece;
	TF1 *fitFunction;

public:
	Analisis(int i, MinsReceiver *m = 0, double gradx = 0, double grady = 0);
	~Analisis();
	void take(int cols, int rows,double *readed);
	//void startAnalizing();
	void drawP();
	void drawR();
	double getGrad(int index);
	//void join();
	void findGrad();
	void computeMins();
	ClassDef(Analisis,0);
};

#endif

