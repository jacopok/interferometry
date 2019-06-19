#ifndef SPOKEPERSON_H
#define SPOKEPERSON_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <stdio.h>
#include "SpokerInterface.h"
#include <unistd.h>
#include <vector>
#include <iostream>

using namespace std;
using namespace cv;

class Spokeperson {
private:
	vector<string> serPortNames;
	SpokerInterface* inter;
	int currstep;
	FILE* serPort;
	VideoCapture* cap;
	Mat readed;
	Mat green;
	int channels[2];
	bool motorOnline;

	enum command : char {
		sUp = '+',
		sDown = '-',
		sReceiv = 'r',
	};
	void write(char c);

public:
	Spokeperson(SpokerInterface* interface);
	~Spokeperson();
	bool step(int delta);
	bool stepto(int abs);
	bool zero();
	double* read(ofstream* out = NULL, bool debug = false);
	int cstep();
	int cols();
	int rows();
	void showCam();

	ClassDef(Spokeperson,0);
};

#endif
