#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <cstdio>

using namespace std;
using namespace cv;

int main() {
	string s;
	int ind;
	int t;
	Mat img;
	bool c = true;
	cout<<"Dataset Name with path: ";
	cin>>s;
	cout<<"Starting frame: ";
	cin>>ind;
	
	namedWindow("ImgViewer",1);
	while(c) {
		ifstream in(s+"_"+to_string(ind)+".gimli");
		if(!in.is_open()) {
			cout<<"Invalid frame.\nFrame: ";
			cin>>ind;
		} else {
			img.create(480,640,CV_8U);
			for(int i=0;i<480;i++) {
				for(int j=0;j<640;j++) {
					in >> t;
					img.at<uchar>(i,j) = t;
				}
			}
			imshow("ImgViewer", img);
			cout<<ind<<endl;
			int k = waitKey(0);
			if( k == 81) ind--;
			else if( k == 83) ind++;
			else c = false;
		}
	}
}
