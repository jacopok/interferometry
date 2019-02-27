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
	pair<int,double>* t;
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
	data.push( new pair<int,double>( step, sum ) );
	m_data.unlock();
	cv_data.notify_all();
}

void Analizer::analizeData( pair<int,double>* p ) {
// 	add data in graph
	tg->SetPoint(tg->GetN(),p->first,p->second);
//	cout<<"Added point "<<p->first<<" "<<p->second<<endl;
	if(painted) tg->GetXaxis()->SetLimits(p->first-100,p->first+100);
	receiver->drawSmall(tg);
	painted = true;
	if(p->second > 30) {
		if(cc > 5) fit(start,end);
		cc = 0;
	}
	else if(p->second < 15) {
		if(cc == 0) start = p->first;
		end = p->first;
		cc++;	
	}
}

void Analizer::fit(int start, int end) {
	func->SetParameters(1,0,-1);
	tg->Fit(func,"Q","",start,end);
	start = -(func->GetParameter(1))/2/(func->GetParameter(0));
	receiver->addPoint(cont,start);
	cont++;
}

void Analizer::draw(int start,int end) {
	if(start != end && painted) tg->GetXaxis()->SetLimits(start,end);
	receiver->drawSmall(tg);
	painted = true;
}
