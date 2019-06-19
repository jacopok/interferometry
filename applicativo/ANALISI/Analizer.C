#include "Analizer.h"

ClassImp(Analizer);

bool Analizer::kill = false;

Analizer::Analizer(PointReceiver* r) {
	receiver = r;
	cont = 0;
	thrds = new thread(&Analizer::minimize,this);
	painted = false;
	tg = new TGraph();

	cc = 0;
	start = 0;
	end = 0;

	func = new TF1("fitting","[a]*x*x + [b]*x + [c]");
}

void Analizer::reset(string folder) {

	cont = 0;	

	if(out) delete out;
	out = new ofstream(string("data/")+folder+"_sinus.gimli");

	tg->Set(0);

	cc = 0;
	start = 0;
	end = 0;

	func = new TF1("fitting","[a]*x*x + [b]*x + [c]");
}

Analizer::~Analizer() {
	kill = true;
	m_data.lock();
	m_data.unlock();
	cv_data.notify_all();
	thrds -> join();
	delete thrds;
}

void Analizer::minimize() {
	pair<int,double> t;
	while(1) {
		unique_lock<mutex> ul(m_data);
		if(data.size() == 0) {
			cv_data.wait(ul);
		}
		if(kill) {
			cout<<"Thread Killed\n";
			ul.unlock();
			return;
		}
		if(data.size() > 0) {
			t=data.front();
			data.pop();
			ul.unlock();
			analizeData( t );
		} else {
			ul.unlock();
		}
	}
}

void Analizer::take(double step, int cx, int cy, int cols, double* d) {
	double sum = 	(d[cx   +  cy   *cols]
			+d[cx+1 +  cy   *cols]
			+d[cx-1 +  cy   *cols]
			+d[cx   + (cy+1)*cols]
			+d[cx+1 + (cy+1)*cols]
			+d[cx-1 + (cy+1)*cols]
			+d[cx   + (cy-1)*cols]
			+d[cx+1 + (cy-1)*cols]
			+d[cx-1 + (cy-1)*cols] ) / 9 ;
	if(out) *out<<step<<" "<<sum<<endl;
	m_data.lock();
	data.push( pair<int,double>{ step, sum } );
//	cout<<"Pushed "<<step<<endl;
	m_data.unlock();
	cv_data.notify_all();
	delete[] d;
//	cout<<"Cleaned "<<step<<endl;
}

void Analizer::analizeData( pair<int,double> p ) {
// 	add data in graph
//	cout<<"Starting "<<p.first<<endl;
	tg->SetPoint(tg->GetN(),p.first,p.second);
//	cout<<"Added "<<p.first<<endl;
//	cout<<"Added point "<<p.first<<" "<<p.second<<endl;
	if( tg->GetN() < 300 ) {
		if(painted) tg->GetXaxis()->SetLimits(p.first-100,p.first+100);
		receiver->drawSmall(tg);
		painted = true;
	}
	if(p.second > 75) {
		if(cc > 4) fit(start,end);
		start = p.first;
		cc = 0;
	}
	else if(p.second < 70) {
		if(cc == 0) start = p.first;
		end = p.first;
		cc++;	
	}
//	cout<<"Done "<<p.first<<endl;
}

void Analizer::fit(int start, int end) {
//	cout<<"Fitting "<<start<<endl;
	if(start > end) swap(start,end);
	func->SetParameters(1,1,-1);
	Int_t fitStatus = tg->Fit(func,"Q0","",start,end);
//	cout<<fitStatus<<endl;
	if( fitStatus == 0 || fitStatus == 1 ) {
		start = -(func->GetParameter(1))/2.0/(func->GetParameter(0));
//		cout<<"Fitting "<<start<<endl;
		receiver->addPoint(cont,start);
	}
//	cout<<"Fitting "<<start<<endl;
//	cout<<start<<" "<<end<<endl;
	cont++;
}

void Analizer::draw(int start,int end) {
	if(start != end && painted) tg->GetXaxis()->SetLimits(start,end);
	receiver->drawSmall(tg);
	painted = true;
}
