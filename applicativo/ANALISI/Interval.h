#include "Rtypes.h"

#ifndef Interval_H
#define Interval_H
class Interval {
public:
	double a,b;

	bool operator<(const Interval& i) const {
		return a < i.a;
	}

	Interval(double aa=0,double bb=0) : a(aa),b(bb) {}
	ClassDef(Interval,0);
};

#endif
