#include "AnalizerThread.h"

ClassImp(AnalizerThread);

bool AnalizerThread::kill = false;

AnalizerThread::AnalizerThread(int howmuch) {
	toAnalize = new vector<Analisis*>();
	for(int i=0; i<howmuch; i++) {
		thrds.push_back(new thread(&AnalizerThread::work,this));
	}
}

AnalizerThread::~AnalizerThread() {
	kill = true;
	accessingVector.lock();
	accessingVector.unlock();
	waitForPush.notify_all();
	for(vector<thread*>::iterator t = thrds.begin();t<thrds.end();t++) {
		(*t)->join();
		delete *t;
	}
	thrds.clear();
	for(vector<Analisis*>::iterator t = toAnalize->begin();t<toAnalize->end();t++) {
		delete *t;
	}
	delete toAnalize;
}

void AnalizerThread::work() {
	while(1) {
		unique_lock<mutex> ul(accessingVector);
		if(toAnalize->size() == 0) {
			waitForPush.wait(ul);
		}
		if(kill) {
			cout<<"Thread Killed\n";
			ul.unlock();
			return;
		}
		if(toAnalize->size() > 0) {
			Analisis* anal = toAnalize->at(0);
			toAnalize->erase(toAnalize->begin());
			ul.unlock();
			anal->computeMins();			
		} else {
			ul.unlock();
		}	
	}
}

void AnalizerThread::addAnalisis(Analisis* aa) {
	accessingVector.lock();
	toAnalize->push_back(aa);
	accessingVector.unlock();
	waitForPush.notify_all();
}
