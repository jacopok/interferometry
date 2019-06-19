#include "Rtypes.h"

#ifndef Point_H
#define Point_H

class Point {
public:
	double x,y;

	bool operator<(const Point& a) const {
		return x < a.x;
	}

	Point(double xi=0, double yi=0) : x(xi),y(yi) {}
	ClassDef(Point,0);
};

#endif
