#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

int main() {
  double n_l, lambda, d, theta_max, theta_0, Delta_theta;
  string filename;
  
  cout << "Insert theoretical values for n_l, lambda [um], d [cm]: ";
  cin >> n_l >> lambda >> d;
  cout << "Insert value for theta_max [deg] and theta_0 [deg]: ";
  cin >> theta_max >> theta_0;
  cout << "Insetr value for Delta_theta [rad]: ";
  cin >> Delta_theta;
  
  theta_max *= M_PI/180; //convert theta_max and theta_0 to rad
  theta_0 *= M_PI/180;
  
  cout << "Insert the name of the file in which to put the data [no extention]: ";
  cin >> filename;
  
  string outname, logname;
  outname = filename + ".txt";
  logname = filename + "_log.txt"; 
  
  ofstream out(outname.c_str());
  if(!out){
    cout << "ERROR opening out" << endl;
    return -1;
  }
    
  cout << "Simulating data" << endl;
  
  double theta = 0, theta_tmp = 0;
  int i = 1;
  string PDF_type = "Uniform";
  
  do{
    //evaluate theta
    theta_tmp = theta;
    theta = acos((pow(n_l,2) - 1 - pow((lambda*i/(2*d*10000) + n_l - 1),2))/(2*(lambda*i/(2*d*10000) + n_l - 1)));
    out <<  i << '\t' << PDF_type << '\t' << theta_0 + theta - Delta_theta/2 << '\t' << theta_0 + theta + Delta_theta/2 << endl;
    out << -i << '\t' << PDF_type << '\t' << theta_0 - theta - Delta_theta/2 << '\t' << theta_0 - theta + Delta_theta/2 << endl;
    i++;
  }while((theta < theta_max)&&(theta > theta_tmp + Delta_theta));
  out.close();
  
  cout << 2*(i - 1) << " data have been produced" << endl;
  
  ofstream log(logname.c_str());
  if(!log){
    cout << "ERROR opening log" << endl;
    return -1;
  }
  log << "n_l\t\t\t" << n_l << endl;
  log << "lambda [um]\t\t" << lambda << endl;
  log << "d [cm]\t\t\t" << d << endl;
  log << "theta max_th [rad]\t" << theta_max << endl;
  log << "theta max_exp [rad]\t" << theta << endl;
  log << "theta 0 [rad]\t\t" << theta_0 << endl;
  log << "Delta theta [rad]\t" << Delta_theta << endl;
  log << "N data\t\t\t" << 2*(i - 1) << endl;
  
  cout << endl << endl;
  return 0;
}