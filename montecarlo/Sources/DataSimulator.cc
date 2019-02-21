#include "DataSimulator.h"

#include "PDF.h"
#include "MultiPDF.h"

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

#include <TH1F.h>
#include <TFile.h>

using namespace std;

DataSimulator::DataSimulator(){
}

DataSimulator::~DataSimulator(){
}



double DataSimulator::simulate_one(const PDF* p){
  double maxValue = p->somma();
  double r = random()*maxValue/RAND_MAX;
  //cout << r << endl;
  return p->inverted_CDF_value(r);
}

vector<double>* DataSimulator::simulate_sample(const vector<PDF*>* vp){
  vector<double>* vData = new vector<double>;
  for(unsigned int i = 0; i < vp->size(); i++)
    vData->push_back(simulate_one(vp->at(i)));
  return vData;
}

vector<double>* DataSimulator::multi_simulate_one(const MultiPDF* p){
  double maxValue = p->somma();
  double r = random()*maxValue/RAND_MAX;
  //cout << r << endl;
  return p->inverted_CDF_value(r);
}


void DataSimulator::root_one(const PDF* p, unsigned int n, const string& fileName){
  unsigned int nBin = p->getSteps();
  const char* hName = p->getName().c_str();
  
  TH1F* h = new TH1F(hName, hName, nBin, p->getMin(), p->getMax());//create histogram
  
  for(unsigned int i = 0; i < n; i++)
    h->Fill(simulate_one(p));//fill histogram
  
  // save current working area
  TDirectory* currentDir = gDirectory;
  // open histogram file
  TFile* file = new TFile( fileName.c_str(), "CREATE" );
  
  h->Write();
  
  // close file
  file->Close();
  delete file;
  // restore working area
  currentDir->cd();
  
  return;
}

void DataSimulator::root_one(const PDF* p, unsigned int n, const string& fileName, unsigned int nBin, double min, double max){
  const char* hName = p->getName().c_str();
  
  TH1F* h = new TH1F(hName, hName, nBin, min, max);//create histogram
  
  for(unsigned int i = 0; i < n; i++)
    h->Fill(simulate_one(p));//fill histogram
  
  // save current working area
  TDirectory* currentDir = gDirectory;
  // open histogram file
  TFile* file = new TFile( fileName.c_str(), "CREATE" );
  
  h->Write();
  
  // close file
  file->Close();
  delete file;
  // restore working area
  currentDir->cd();
  
  return;
}