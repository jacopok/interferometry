#ifndef CENTERMANAGER_H
#define CENTERMANAGER_H

using namespace std;

#include <TGraph2D.h>

class CenterManager {
private:
	TGraph2D *tg;
	int C[2];
	
public:
	CenterManager();
	int getCenter(int coord);
	void draw();
	void updateData(int row,int col,double* data);
	
	ClassDef(CenterManager,0);
}

#endif
