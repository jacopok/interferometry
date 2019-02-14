#include "Analisis.h"

ClassImp(Analisis);

Analisis::Analisis(int i, MinsReceiver *m, double gradx, double grady) {
	id = i;
	col = 0;
	row = 0;
	mrece = m;
	data = 0;
	pdata = new vector<Point>();
	ptg = new TGraph();
	tg = new TGraph2D();
	sep = new vector<Interval>();
	mins = new vector<double>();
	G[0] = gradx;
	G[1] = grady;
	string name = "f"+to_string(id);
	fitFunction = new TF1(name.c_str(),"[a]*x*x + [b]*x + [c]");
}

Analisis::~Analisis() {
	delete[] data;
	delete pdata;
	delete ptg;
	delete tg;
	delete sep;
	mins->clear();
	delete mins;
}

void Analisis::take(int cols, int rows,double* readed) {
	double max=-DBL_MAX,min=DBL_MAX;
	col = cols;
	row = rows;
	data = readed;
	// NORMALIZATION
	for(int i=0;i<row;i++)
		for(int j=0;j<col;j++) {
			if(data[j+i*col]>max) max = data[j+i*col];
			if(data[j+i*col]<min) min = data[j+i*col];
		}
	for(int i=0;i<row;i++)
		for(int j=0;j<col;j++) {
			data[j+i*col] = (data[j+i*col]-min)*2/(max-min)-1;
			tg->SetPoint(j+i*col,j,i,data[j+i*col]);
		}
}

void Analisis::findGrad() {double grads[(col-4)*(row-4)][2];
	double tx,ty,sx=0,sy=0,sxx=0,syy=0;

	for(int i=2;i<row-2;i++)
		for(int j=2;j<col-2;j++) {
			tx = data[j+1+i*col] - data[j-1+i*col];
			ty = data[j+(i+1)*col] - data[j+(i-1)*col];

			//EQUIORIENTATION
			if(tx<0) { tx*=-1; ty*=-1; }

			//FIRST MEAN & STD
			sx += tx;
			sxx+= tx*tx;
			sy += ty;
			syy+= ty*ty;

			//SAVING
			grads[(i-2)*(col-4)+(j-2)][0] = tx;
			grads[(i-2)*(col-4)+(j-2)][1] = ty;
		}

	tx = sx / (col-4) / (row-4);
	ty = sy / (col-4) / (row-4);
	sxx = sqrt(sxx / (col-4) / (row-4)) * 2;
	syy = sqrt(syy / (col-4) / (row-4)) * 2;

	//MEAN WITH DATA REJECTION AT 2 SIGMA
	sx = 0;
	sy = 0;
	for(int i=0;i<(col-4)*(row-4);i++) {
		if(abs(grads[i][0]-tx)<sxx) sx += grads[i][0];
		if(abs(grads[i][1]-ty)<syy) sy += grads[i][1];
	}
	G[0] = sx / (col-4) / (row-4);
	G[1] = sy / (col-4) / (row-4);
}

void Analisis::computeMins() {
	for(int i=0;i<row;i++)
		for(int j=0;j<col;j++) {
			//PROJECTION: for this formula, Gx>0 is needed
			pdata->push_back( (Point){(j*G[0]+i*G[1])/sqrt(G[0]*G[0]+G[1]*G[1]) , data[j+i*col] });
			ptg->SetPoint(col*i+j, (j*G[0]+i*G[1])/sqrt(G[0]*G[0]+G[1]*G[1]),data[j+i*col]);
	}

	int cc = 0;
	double start = 0,end = 0;
	vector<Interval> sep;
	sort(pdata->begin(),pdata->end());
	for(vector<Point>::iterator i = pdata->begin(); i < pdata->end(); i++) {
		if(i->y > 0.5) {
			if(cc > 5) sep.push_back( (Interval) { start,end} );
			cc = 0;
		}
		else if(i->y < -0.5) {
			if(cc == 0) start = i->x;
			end = i->x;
			cc++;	
		}
	}

	for(vector<Interval>::iterator i = sep.begin(); i< sep.end(); i++) {
		fitFunction->SetParameters(1,0,-1);
		ptg->Fit(fitFunction,"Q","",i->a,i->b);
		start = -(fitFunction->GetParameter(1))/2/(fitFunction->GetParameter(0));
//		cout<<"Fitted function at analisis "<<id<<" between "<<i->a<<" and "<<i->b<<" with "<<ptg->GetN()<<" points obtaining "<<start<<endl;
		mins->push_back(start);
	}
	if(mrece!=0)
		mrece->mins(id,mins);
}

void Analisis::drawR() {
	tg->Draw("CONT4Z");
}


void Analisis::drawP() {
	if(G[0]==0&&G[1]==0) findGrad();
	computeMins();
	ptg->Draw();
	for(vector<double>::iterator d = mins->begin(); d < mins->end(); d++) {
		TLine *tld = new TLine(*d,-1.3,*d,1.3);
		tld->SetLineColor(kRed);
		tld->Draw();
	}
}

double Analisis::getGrad(int index) {
	if(index < 2 && index >= 0) return G[index];
	else return 0;
}
