#include "BigGraph.h"

ClassImp(BigGraph);

BigGraph::BigGraph() {
	tg = new TGraph();
}

void BigGraph::draw() {
	tg->Draw("A*");
}

void BigGraph::addPoint(double x,double y) {
	tg->SetPoint(tg->GetN(),x,y);
}

void BigGraph::addPoint(double x,vector<double>* y) {
	for(vector<double>::iterator d = y->begin(); d < y->end(); d++)
		tg->SetPoint(tg->GetN(),x,*d);
}

int BigGraph::getN() {
	return tg->GetN();
}
