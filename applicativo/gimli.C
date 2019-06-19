#include <fstream>
#include <cstring>
#include <mutex>
#include <ctime>
#include "ANALISI/Analizer.h"
#include "ANALISI/PointReceiver.h"
#include "ANALISI/CenterManager.h"
#include "GUI/MyGUI.h"
#include "GUI/Observer.h"
#include "BIGGRAPH/BigGraph.h"
#include <TGMsgBox.h>

class Interface : public Observer, public PointReceiver, public SpokerInterface {
	ofstream* massout=NULL,*out=NULL;
	MyGUI* mg=NULL;
	CenterManager* cm=NULL;
	BigGraph* bg=NULL;
	Spokeperson* spk=NULL;
	mutex gui;
	mutex mout;
	Analizer* anal=NULL;
	void rubbishClear();
public:
	Interface();
	~Interface();
	void click(const char* s);
	void addPoint(int nfrange, int passo);
	void drawSmall(TGraph* tg);
	void working(bool status);
	void setCStep(int cstep);
	void noSerPort();
};

Interface::Interface() {
	mg = new MyGUI();
	mg->attach(this);
	mg->setFolder("fold"+to_string(time(NULL)));
	bg = new BigGraph();
	spk= new Spokeperson(this);
	anal = new Analizer(this);
	cm = new CenterManager();
	spk->read();
	cm->updateData(spk->rows(),spk->cols(),spk->read());
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
	if(cm) delete cm;
	if(bg) delete bg;
	if(anal) delete anal;
}

void Interface::setCStep(int cstep) {
	mg->setStepInfo(to_string(cstep).c_str());
	gui.lock();
	mg->canvUpdate();	
	gui.unlock();
}

void Interface::click(const char* s) {
	if(strcmp(s,"info")==0) {
		cm->updateData(spk->rows(),spk->cols(),spk->read());
		char temp[100];
		sprintf(temp,"Center: %d,%d",cm->getCenter(0),cm->getCenter(1));
		mg->setGradInfo(temp);
	}
	if(strcmp(s,"plot1")==0) {
		gui.lock();
		mg->goToCanv(0);
		cm->draw();
		mg->canvUpdate();
		gui.unlock();
	}
	if(strcmp(s,"plot2")==0) {
		gui.lock();
		mg->goToCanv(0);
		anal->draw();
		mg->canvUpdate();
		gui.unlock();
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
		int start = mg->getStart(), end = mg->getEnd(), step = mg->getStep();
		anal->reset(mg->getFolder());
		if( (start < end && step > 0) || (start > end && step < 0)) {
			spk->stepto(start);
			while( (spk->cstep() <= end && step > 0) || (spk->cstep() >= end && step < 0) ) {
				if(mg->massiveSaving()) {
					string filename = string("data/")+mg->getFolder()+"_"+to_string(spk->cstep())+".gimli";
					massout = new ofstream(filename);
					anal->take(spk->cstep(),cm->getCenter(0),cm->getCenter(1),spk->cols(),spk->read(massout));
				}
				else
					anal->take(spk->cstep(),cm->getCenter(0),cm->getCenter(1),spk->cols(),spk->read() );
				if(massout) delete massout;
				massout=NULL;

				spk->step(step);
			}
		}
		mg->setFolder("fold"+to_string(time(NULL)));
	}	
}

void Interface::addPoint(int nfrange, int passo) {
	mout.lock();
//	cout<<"Mout"<<endl;
	*out<<nfrange<<"\t"<<passo<<"\n";
	out->flush();
	mout.unlock();
	gui.lock();
//	cout<<"Mbg"<<endl;
	bg->addPoint(nfrange,passo);
	mg->goToCanv(1);
	bg->draw();
	mg->canvUpdate();
	gui.unlock();
//	cout<<"Bye"<<endl;
}

void Interface::drawSmall(TGraph* tg) {
	gui.lock();
	mg->goToCanv(0);
	tg->Draw();
	mg->canvUpdate();
	gui.unlock();
}

void gimli() {
	//THREAD SAFETY
//	ROOT::EnableThreadSafety();
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");

	new Interface();
}
