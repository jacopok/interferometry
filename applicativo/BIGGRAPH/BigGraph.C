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

int BigGraph::getN() {
	return tg->GetN();
}
