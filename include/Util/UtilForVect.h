//---------------------------------------------------------------------------

#ifndef UtilForVectH
#define UtilForVectH
//---------------------------------------------------------------------------
#include <vector>

using namespace std;

void    VectDoubleWriteFile(vector<double> &vect,int handle);
void    VectDoubleReadFile(vector<double> &vect,int handle);
double  VectDoubleAverDistanceAndDisp(vector<vector <double> > &val1,vector<vector <double> > &val2,double &disp);
double  VectDoubleDistance(vector<double> &vect1,vector<double> &vect2);
void    VectDoubleNorm(vector<double> &vect,double sum);
void    VectIntNorm(vector<int> &vect,int sum);
//---------------------------------------------------------------------------
#endif
