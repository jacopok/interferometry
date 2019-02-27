#include "CenterManager.h"

ClassImp(CenterManager);

CenterManager::CenterManager() {
	tg = new TGraph2D();
	C[0] = 0;
	C[1] = 0;
}

int CenterManager::getCenter(int coord) {
	return (coord == 0 || coord == 1 ? C[coord] : 0);
}

void CenterManager::draw() {
	tg->Draw("CONT4Z");
}

void CenterManager::updateData(int row,int col,double* data) {
	tg->Clear();
	for(int i=0; i < row; i++) {
		for(int j=0; j<col; j++) {
			tg->SetPoint(j+i*col,j,i,data[j+i*col]);
		}
	}
	C[0] = row/2;
	C[1] = row/2;
}
	
