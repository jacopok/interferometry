#ifndef ANALIZERTHREAD_H
#define ANALIZERTHREAD_H

#include <TThread.h>
#include "Analisis.h"
#include <thread>
#include <vector>
#include <mutex>
#include <iostream>
#include <condition_variable>

using namespace std;

class AnalizerThread {
private:
	vector<thread*> thrds;
	vector<Analisis*>* toAnalize;
	mutex accessingVector;
	condition_variable waitForPush;
	void work();
	static bool kill;
public:
	AnalizerThread(int howmuch);
	~AnalizerThread();
	void addAnalisis(Analisis* aa);

	ClassDef(AnalizerThread,0);
};

#endif
