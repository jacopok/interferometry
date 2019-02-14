#include "Spokeperson.h"

ClassImp(Spokeperson);

Spokeperson::Spokeperson(SpokerInterface* interface) {

	// Members initialization	
	inter = interface;
	currstep = 0;
	inter->setCStep(0);

	// Arduino loading
	serPortNames.push_back("/dev/ttyACM0");
	serPortNames.push_back("/dev/ttyUSB0");
	serPortNames.push_back("/dev/ttyUSB1");
	bool conn = false;
	for(int i=0;i<serPortNames.size() && conn == false;i++) {
		system((string("stty -F ")+serPortNames[i]+" -hupcl").c_str()); //PREVENT RESET
		system((string("stty -F ")+serPortNames[i]+" 9600").c_str()); //SET BAUDRATE
		serPort = fopen(serPortNames[i].c_str(), "r+");
		if (serPort == NULL)
			cout<<"NOT ABLE TO OPEN SERIAL PORT AT "<<serPortNames[i]<<endl;
	}
	if (!conn)
		inter->noSerPort();

	// Cam loading
	channels[0] = 1;
	channels[1] = 0;
	cap = new VideoCapture(0);
	*cap >> readed; // Load a sample image
	green.create(readed.rows,readed.cols,CV_8U);
	mixChannels(&readed, 1, &green, 1, channels, 1);
}

Spokeperson::~Spokeperson() {
	fclose(serPort);
}


bool Spokeperson::step(int delta) {
	for(int i=0;i<delta;i++) write(sUp);
	for(int i=0;i>delta;i--) write(sDown);
	currstep += delta;
	inter->setCStep(currstep);
	return true;
}

bool Spokeperson::stepto(int abs) {
	return step(abs-currstep);
}

bool Spokeperson::zero() {
	currstep = 0;
	inter->setCStep(currstep);
	return true;
}

int Spokeperson::cstep() {
	return currstep;
}

double* Spokeperson::read(ofstream* out) {
/*
	ifstream in("data.dat");
	double* data = new double[row*col];
	double t;
	for(int i=0;i<row;i++)
		for(int j=0;j<col;j++) {
			in>>t;
			if(out) *out<<t<<"\t";
			data[j+i*col] = t;
		}
	return data;*/
	double* data = new double[green.rows*green.cols];
	double t=0;
	*cap >> readed;
//	cvtColor(readed, green, COLOR_RGB2GRAY);
	mixChannels(&readed,  1, &green, 1, channels, 1);
	uchar* p = green.data;
	for(int i=0;i<green.rows;i++)
		for(int j=0;j<green.cols;j++) {
			t = (double)p[j+i*green.cols];
			if(out) *out<<t<<"\t";
			data[j+green.cols*i] = t;
		}
	return data;
}

int Spokeperson::rows() {
	return readed.cols;
}
int Spokeperson::cols() {
	return readed.rows;
}

void Spokeperson::write(char c) {
	fwrite(&c, sizeof(char), 1, serPort);
	fflush(serPort);
	usleep(200);
	/*char r='0';
	do {
		while(fread(&r, sizeof(char), 1, serPort)) clearerr(serPort); // READ OLD DATA
		fwrite(&c, sizeof(char), 1, serPort);
		fflush(serPort);
		fseek(serPort,0,SEEK_CUR);
		while(!fread(&r, sizeof(char), 1, serPort)) clearerr(serPort);
		cout<<r<<endl;
		cout.flush();
	} while(r!=sReceiv);*/
}

void Spokeperson::showCam() {
	namedWindow("Camera",1);
	while(1) {
		*cap >> readed;
		mixChannels(&readed,  1, &green, 1, channels, 1);
	//	cvtColor(readed, green, COLOR_RGB2GRAY);	
        	imshow("Camera", green);
	        if(waitKey(30) >= 0) break;
	}
	destroyWindow("Camera");
	int k = readed.type();
	cout<<k;
}

