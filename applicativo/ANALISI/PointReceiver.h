#ifndef POINTRECEIVER_H
#define POINTRECEIVER_H
#include <vector>
class PointReceiver {
public:
	virtual void addPoint(int nfrange, int passo) = 0;
	virtual void drawSmall(TGraph* tg) = 0;
};
#endif
