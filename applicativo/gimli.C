#include <fstream>
#include <cstring>
#include <mutex>
#include <ctime>
#include "ANALISI/Analisis.h"
#include "ANALISI/MinsReceiver.h"
#include "GUI/MyGUI.h"
#include "GUI/Observer.h"
#include "BIGGRAPH/BigGraph.h"
#include <TGMsgBox.h>

class Interface : public Observer, public MinsReceiver, public SpokerInterface {
	ofstream* massout=NULL,*out=NULL;
	MyGUI* mg=NULL;
	Analisis* a=NULL;
	BigGraph* bg=NULL;
	Spokeperson* spk=NULL;
	mutex mbg;
	mutex mout;
	AnalizerThread* athr=NULL;
	bool grad = false;
	void rubbishClear();
public:
	Interface();
	~Interface();
	void click(const char* s);
	void mins(int id, vector<double>* mins);
//	void stepto(int a,bool relative=true);
	void setCStep(int cstep);
	void noSerPort();
};

Interface::Interface() {
	mg = new MyGUI();
	mg->attach(this);
	mg->setFolder("fold"+to_string(time(NULL)));
	bg = new BigGraph();
	spk=new Spokeperson(this);
	athr=new AnalizerThread(5);
	a = new Analisis(0);
	spk->read();
	a->take(spk->cols(),spk->rows(),spk->read());
}

void Interface::noSerPort() {
	new TGMsgBox(gClient->GetRoot(),mg,"ERROR","UNABLE TO CONNECT TO SERIAL PORT\nPlease restart the script.",kMBIconStop);
}

Interface::~Interface() {
	rubbishClear();
}

void Interface::rubbishClear() {
	if(massout) delete massout;
	if(out) out->flush();
	cout<<"Data flushed\n";
	if(out) delete out;
	if(a) delete a;
	if(bg) delete bg;
	if(athr) delete athr;
}

void Interface::setCStep(int cstep) {
	mg->setStepInfo(to_string(cstep).c_str());
}

void Interface::click(const char* s) {
	if(strcmp(s,"grad")==0) {
		a->findGrad();
		string str = "Grad: "+to_string(a->getGrad(0))+";"+to_string(a->getGrad(1))+")";
		mg->setGradInfo(str.c_str());
		grad = true;
	}
	if(strcmp(s,"plot1")==0) {
		mg->goToCanv(0);
		a->drawR();
		mg->canvUpdate();
	}
	if(strcmp(s,"plot2")==0) {
		mg->goToCanv(0);
		a->drawP();
		mg->canvUpdate();
	}
	if(strcmp(s,"zero")==0) {
		spk->zero();
	}
	if(strcmp(s,"next")==0) {
		spk->step(1);
	}
	if(strcmp(s,"prev")==0) {
		spk->step(-1);
	}
	if(strcmp(s,"goto")==0) {
		spk->stepto(mg->getGoTo());
	}
	if(strcmp(s,"close")==0) {
		rubbishClear();
	}
	if(strcmp(s,"webcam")==0) {
		spk->showCam();
	}
	if(strcmp(s,"anal")==0) {
		if(out) delete out;
		out = new ofstream(string("data/")+mg->getFolder()+"_mins.gimli");
		if(!grad) click("grad");
		int start = mg->getStart(), end = mg->getEnd(), step = mg->getStep();
		if( (start < end && step > 0) || (start > end && step < 0)) {
			spk->stepto(start);
			while( (spk->cstep() <= end && step > 0) || (spk->cstep() >= end && step < 0) ) {
				Analisis *t = new Analisis(spk->cstep(),this,a->getGrad(0),a->getGrad(1));
				if(mg->massiveSaving()) {
					string filename = string("data/")+mg->getFolder()+"_"+to_string(spk->cstep())+".gimli";
					massout = new ofstream(filename);
					t->take(spk->cols(),spk->rows(),spk->read(massout));
				}
				else
					t->take(spk->cols(),spk->rows(),spk->read());
				athr->addAnalisis(t);
				delete massout;
				spk->step(step);
			}
		}
	}	
}

void Interface::mins(int id, vector<double>* mins) {
	mout.lock();
	for(vector<double>::iterator d = mins->begin();d< mins->end();d++) *out<<id<<"\t"<<*d<<"\n";
	mout.unlock();
	mbg.lock();
	bg->addPoint(id,mins);
	mg->goToCanv(1);
	bg->draw();
	mg->canvUpdate();
	mbg.unlock();
}

void gimli() {
	//THREAD SAFETY
	ROOT::EnableThreadSafety();
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");

	new Interface();
}
