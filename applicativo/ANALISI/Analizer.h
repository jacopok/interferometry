#ifndef ANALIZER_H
#define ANALIZER_H

#include <TThread.h>
#include "Point.h"
#include "PointReceiver.h"
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <TGraph.h>
#include <TF1.h>

using namespace std;

class Analizer {
private:
	thread *thrds; // where "find minimum" function run

	queue< pair<int,double>* > data;
	mutex m_data;
	condition_variable cv_data;
	void minimize();
	ofstream *out = NULL;
	TGraph *tg;
	TF1 *func;
	int direction;

	int cont,cc,start,end;

	PointReceiver* receiver;

	int center[2];

	static bool kill;
	bool painted;
public:
	Analizer(PointReceiver* r);
	~Analizer();
	void take(double step, int cx, int cy, int cols, double* d);
	void draw(int start =0, int end=0);
	void reset(string folder);
	void fit(int start,int end);
	void analizeData( pair<int,double>* d);
	ClassDef(Analizer,0);
};

#endif
