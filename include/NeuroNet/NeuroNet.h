//---------------------------------------------------------------------------
#ifndef NeuroNetH
#define NeuroNetH
//---------------------------------------------------------------------------
#include <vector>

#include "..\Util\errMessage.h"
#include "..\Util\TemplateFileRec.h"

#define CN_CIRCLE       3   // Число проходов при расчете сигнала у НС

using namespace std;
struct    tpTrainingPair
{

	inline  tpTrainingPair() {}
	tpTrainingPair(const tpTrainingPair &val);
	void    operator=(const tpTrainingPair &val);
	int SizeInFile() { return SizeVectFile(_vectIn)+SizeVectFile(_vectOut); }
	int ReadFile(int handle) { ReadVectFile(handle, _vectIn);  ReadVectFile(handle, _vectOut); return 0; }
	int WriteFile(int handle) { WriteVectFile(handle, _vectIn);  WriteVectFile(handle, _vectOut); return 0; }

	std::vector<double> _vectIn, _vectOut;
};

void    WriteFileArrTP(int handle,vector<tpTrainingPair> &v0);
void    ReadFileArrTP(int handle,vector<tpTrainingPair> &v0);

struct tpSinapse
{
    int _nbOut;
    double _weight;
};

class clNeuron
{
friend class clNeuroNet;
public :
    clNeuron();
//    clNeuron(clNeuron &val);
//    ~clNeuron();
//    void    New(double threshold=0);
//    void    Free();
    void    WriteFile(int handle);
    void    ReadFile(int handle);
    void    operator =(const clNeuron &val);
    void    AddSinapse(int nbOut,double weight);
    void    Propogate(clNeuroNet *net);
    int     Check(int nbOut);

    vector<tpSinapse> _sinapse;
    double _axon;//,_threshold;
};

class clNeuroNet
{
friend class clNeuron;
friend class clLayersBPNet;
public :
    clNeuroNet();
    ~clNeuroNet();
    void    New(int cnIn,int cnOut,int cnN);
    void    Clear();
    void    WriteFile(int handle);
    void    ReadFile(int handle);
    void    operator =(clNeuroNet &val);
    void    AddSinapse(int nbOut,int nbIn,double weight);
    void    SetWeightSinapse(int nbOut,int nbIn,double weight);
    bool    Empty();
    void    Propogate(vector<double> &vectIn,vector<double> &vOut);
    inline  clNeuron *PtrNeuron(int nb)     {return &_neurons.begin()[0]+nb;}
    inline  int CnAllNeurons()         {return _cnIn+_cnOut+_cnEx;}
    inline  int CnIn()                  {return _cnIn-1;}
    inline  int CnOut()                 {return _cnOut-1;}
//private :
    vector<clNeuron> _neurons;
    int _cnIn,_cnOut,_cnEx,_cnCicle;
};

class clLayersBPNet : public clNeuroNet
{
public :
    clLayersBPNet();
    void    InitFullConnection(vector<int> &v0);

	void    WriteFile(int handle);
	void    ReadFile(int handle);
    void    PrepareForBP();
	void    operator =(clLayersBPNet &val);
    void    Propogate(vector<double> &vectIn,vector<double> &vOut);
    inline  int CnLayers()      {return (int)_layers.size();}
	inline  int CnNeurons(int nbL)      { return (int)_layers[nbL].size(); }
	inline  int NbNeuron(int nbL, int nbN)   { return (int)_layers[nbL][nbN]; }
    void    LearningBP(vector<tpTrainingPair>  &arrTP);
    void    LearningBP(tpTrainingPair  &tp);
    double  Error(vector<tpTrainingPair>  &arrTP);
	double  ErrorMultW(vector<tpTrainingPair>  &arrTP);
	void    LearningGradDes(vector<tpTrainingPair>  &arrTP, vector<clNeuron> &deltaEr);

    double _alfa,_nu,_maxWeight;
private :
    vector<vector <int> > _layers;
    vector<clNeuron> _sinOut,_delta;
};

extern bool glAxon0;

#define VAL_1_FORNN     0.9
#define VAL_0_FORNN     0.1

inline  double  NNDouble01ToVal(double val)     {return val*(VAL_1_FORNN-VAL_0_FORNN)+VAL_0_FORNN;}
inline  double  NNValToDouble01(double val)     {return (val-VAL_0_FORNN)/(VAL_1_FORNN-VAL_0_FORNN);}

//---------------------------------------------------------------------------
#endif
