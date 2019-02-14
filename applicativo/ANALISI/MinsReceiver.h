#ifndef MINSRECEIVER_H
#define MINSRECEIVER_H
#include <vector>
class MinsReceiver {
public:
	virtual void mins(int id, vector<double>* mins) = 0;
};
#endif
