#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char* argv[]){
  if(argc < 3){
    cout << "Please type ./shuffler [data_file_name] [new_data_file_name]" << endl << endl;
    return 0;
  }
  
  ifstream in(argv[1]);
  if(!in){
    cout << "Invalid input filename" << endl;
    return -1;
  }
  ofstream out(argv[2]);
  if(!out){
    cout << "Invalid output filename" << endl;
    return -1;
  }
  
  vector<string> vs;
  string s;
  while(getline(in,s))
    vs.push_back(s);
  
  unsigned int i = 0, j = vs.size() - 1, count = 0;
  bool lower = true;
  
  while(i <= j){
    if(lower){
      out << vs[i] << endl;
      i++;
    }
    else{
      out << vs[j] << endl;
      j--;
    }
    lower = !lower;
    count++;
  }
  
  if(count != vs.size()){
    cout << "Some data were lost" << endl;
    return -1;
  }
  
  cout << endl;
  return 0;
}