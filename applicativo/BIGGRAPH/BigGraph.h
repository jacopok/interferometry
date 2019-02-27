#ifndef BIGGRAPH_H
#define BIGGRAPH_H

#include <vector>
#include <TGraph.h>

class BigGraph {
	TGraph *tg;

public:
	BigGraph();
	void draw();
	void addPoint(double x, double y);
	int getN();

	ClassDef(BigGraph,0);
};

#endif
