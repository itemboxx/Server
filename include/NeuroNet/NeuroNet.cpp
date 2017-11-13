//---------------------------------------------------------------------------
#pragma hdrstop

#include <io.h>
#include <math.h>

#include "..\Util\UtilForVect.h"
#include "NeuroNet.h"

#define DEFTRESHOLD         0.3

double FuncNeuro(double x);
bool glAxon0=true;
//---------------------------------------------------------------------------
tpTrainingPair::tpTrainingPair(const tpTrainingPair &val)
{
    _vectIn=val._vectIn;
    _vectOut=val._vectOut;
}
//---------------------------------------------------------------------------
void    tpTrainingPair::operator=(const tpTrainingPair &val)
{
    _vectIn=val._vectIn;
    _vectOut=val._vectOut;
}
//---------------------------------------------------------------------------
void    WriteFileArrTP(int handle,vector<tpTrainingPair> &v0)
{
	int cn = (int)v0.size();
    _write(handle,&cn,sizeof(cn));
    for(int i=0;i<cn;i++)
    {
        VectDoubleWriteFile(v0[i]._vectIn,handle);
        VectDoubleWriteFile(v0[i]._vectOut,handle);
    }
}
//---------------------------------------------------------------------------
void    ReadFileArrTP(int handle,vector<tpTrainingPair> &v0)
{
    int cn;
    _read(handle,&cn,sizeof(cn));
    v0.resize(cn);
    for(int i=0;i<cn;i++)
    {
        VectDoubleReadFile(v0[i]._vectIn,handle);
        VectDoubleReadFile(v0[i]._vectOut,handle);
    }
}
//---------------------------------------------------------------------------
//***************************************************************************
//***************************************************************************
clNeuroNet::clNeuroNet()
{
    _cnIn=_cnOut=_cnEx=0;
    _cnCicle=CN_CIRCLE;
}
//---------------------------------------------------------------------------
clNeuroNet::~clNeuroNet()
{
    Clear();
}
//---------------------------------------------------------------------------
void    clNeuroNet::New(int cnIn,int cnOut,int cnN)
{
    _cnIn=cnIn+1;
    _cnOut=cnOut;
    _cnEx=cnN;
    int cnA=_cnIn+_cnOut+_cnEx;
    _neurons.resize(cnA);
//    for(int i=0;i<_neurons.size();i++)
//        _neurons[i].New(DEFTRESHOLD);
}
//---------------------------------------------------------------------------
void    clNeuroNet::Clear()
{
//    for(int i=0;i<_neurons.size();i++)
//        _neurons[i].Free();
    _cnIn=_cnOut=_cnEx=0;
    _neurons.clear();
}
//---------------------------------------------------------------------------
void    clNeuroNet::operator =(clNeuroNet &val)
{
    Clear();
    _cnIn=val._cnIn;
    _cnOut=val._cnOut;
    _cnEx=val._cnEx;
    _cnCicle=val._cnCicle;
    _neurons.resize(val._neurons.size());
    for(size_t i=0;i<_neurons.size();i++)
        _neurons[i]=val._neurons[i];
}
//---------------------------------------------------------------------------
void    clNeuroNet::AddSinapse(int nbOut,int nbIn,double weight)
{
    _neurons[nbIn].AddSinapse(nbOut,weight);
}
//---------------------------------------------------------------------------
void    clNeuroNet::SetWeightSinapse(int nbOut,int nbIn,double weight)
{
    for(unsigned j=0;j<_neurons[nbIn]._sinapse.size();j++)
        if(_neurons[nbIn]._sinapse[j]._nbOut==nbOut)
        {
            _neurons[nbIn]._sinapse[j]._weight=weight;
            return;
        }
}
//---------------------------------------------------------------------------
bool    clNeuroNet::Empty()
{
    return (_cnIn+_cnOut==0);
}
//---------------------------------------------------------------------------
void clNeuroNet::Propogate(vector<double> &vectIn,vector<double> &v0)
{
//    double d0[100];

    int cnF=_cnIn+_cnOut;
    int cnA=cnF+_cnEx;
	_neurons[0]._axon = 1.;
    for(int i=1;i<_cnIn;i++)
        _neurons[i]._axon=vectIn[i-1];
     for(int i=_cnIn;i<cnA;i++)
            _neurons[i]._axon=0;
/*
    for(int i0=0;i0<cnA;i0++)
        d0[i0]=_neurons[i0]._axon;
*/
    for(int c=0;c<_cnCicle;c++)
    {
        for(int i=cnF;i<cnA;i++)
            _neurons[i].Propogate(this);
		for (int i = _cnIn; i<cnF; i++)
			_neurons[i].Propogate(this);
/*
    for(int i0=0;i0<cnA;i0++)
        d0[i0]=_neurons[i0]._axon;
*/
    }
    v0.resize(_cnOut);
    for(int i=0;i<_cnOut;i++)
        v0[i]=_neurons[i+_cnIn]._axon;
}
//---------------------------------------------------------------------------
void    clNeuroNet::WriteFile(int handle)
{
    _write(handle,&_cnIn,sizeof(int));
    _write(handle,&_cnOut,sizeof(int));
    _write(handle,&_cnEx,sizeof(int));
    _write(handle,&_cnCicle,sizeof(int));
	int cn = (int)_neurons.size();
    _write(handle,&cn,sizeof(int));
    for(int i=0;i<cn;i++)
        _neurons[i].WriteFile(handle);
}
//---------------------------------------------------------------------------
void    clNeuroNet::ReadFile(int handle)
{
    _read(handle,&_cnIn,sizeof(int));
    _read(handle,&_cnOut,sizeof(int));
    _read(handle,&_cnEx,sizeof(int));
    _read(handle,&_cnCicle,sizeof(int));
    int cn;
    _read(handle,&cn,sizeof(int));
    _neurons.resize(cn);
    for(int i=0;i<cn;i++)
        _neurons[i].ReadFile(handle);
}
//---------------------------------------------------------------------------
//***************************************************************************
//***************************************************************************
clNeuron::clNeuron()
{
    _axon=0;
//    _threshold=DEFTRESHOLD;
}
//---------------------------------------------------------------------------
void    clNeuron::operator =(const clNeuron &val)
{
    _axon=val._axon;
//    _threshold=val._threshold;
    _sinapse=val._sinapse;
}
//---------------------------------------------------------------------------
void    clNeuron::AddSinapse(int nbOut,double weight)
{
#ifdef  ERR_MESSAGE
    //int nb=Check(nbOut);
   // if(nb!=-1)
    //    ErrMessage("Уже есть такой нейрон","clNeuron::AddSinapse");
#endif
    tpSinapse s;
    s._nbOut=nbOut;
    s._weight=weight;
    _sinapse.push_back(s);
}
//---------------------------------------------------------------------------
void    clNeuron::Propogate(clNeuroNet *net)
{
//    _axon=-_threshold;
    _axon=0;
    for(size_t i=0;i<_sinapse.size();i++)
    {
        tpSinapse *ptr=&_sinapse[i];
        _axon+=net->_neurons[ptr->_nbOut]._axon*ptr->_weight;
    }
    _axon=FuncNeuro(_axon);
}
//---------------------------------------------------------------------------
int     clNeuron::Check(int nbOut)
{
    for(size_t i=0;i<_sinapse.size();i++)
        if(nbOut == _sinapse[i]._nbOut)
			return (int)i;
    return -1;
}
//---------------------------------------------------------------------------
void    clNeuron::WriteFile(int handle)
{
	int cn = (int)_sinapse.size();
    _write(handle,&cn,sizeof(int));
	if (cn > 0)
		_write(handle,&_sinapse.begin()[0],cn*sizeof(tpSinapse));
}
//---------------------------------------------------------------------------
void    clNeuron::ReadFile(int handle)
{
    int cn;
    _read(handle,&cn,sizeof(int));
    _sinapse.resize(cn);
	if (cn > 0)
	    _read(handle,&_sinapse.begin()[0],cn*sizeof(tpSinapse));
}
//---------------------------------------------------------------------------
double FuncNeuro(double x)
{
    if(x < -10) return 0;
	return 1/(1+exp(-x));
}
//---------------------------------------------------------------------------
//***************************************************************************
//***************************************************************************
clLayersBPNet::clLayersBPNet()
{
    _alfa=0;
    _nu=0.1;
    _maxWeight=1000;
}
//---------------------------------------------------------------------------
void    clLayersBPNet::WriteFile(int handle)
{
	_write(handle, &_alfa, sizeof(double));
	_write(handle, &_nu, sizeof(double));
	_write(handle, &_maxWeight, sizeof(double));


	int cn = (int)_layers.size();
	_write(handle, &cn, sizeof(int));
	for (int i = 0; i < cn; i++)
	{
		int cnV = (int)_layers[i].size();
		_write(handle, &cnV, sizeof(int));
		if (cnV > 0)
			_write(handle, &_layers[i].begin()[0], cnV * sizeof(int));
	}

	cn = (int)_sinOut.size();
	_write(handle, &cn, sizeof(int));
	for (int i = 0; i<cn; i++)
		_sinOut[i].WriteFile(handle);

	cn = (int)_delta.size();
	_write(handle, &cn, sizeof(int));
	for (int i = 0; i<cn; i++)
		_delta[i].WriteFile(handle);
	clNeuroNet::WriteFile(handle);
}
void    clLayersBPNet::ReadFile(int handle)
{
	_read(handle, &_alfa, sizeof(double));
	_read(handle, &_nu, sizeof(double));
	_read(handle, &_maxWeight, sizeof(double));


	int cn;
	_read(handle, &cn, sizeof(int));
	_layers.resize(cn);
	for (int i = 0; i < cn; i++)
	{
		int cnV;
		_read(handle, &cnV, sizeof(int));
		_layers[i].resize(cnV);
		if (cnV > 0)
		_read(handle, &_layers[i].begin()[0], cnV * sizeof(int));
	}

	_read(handle, &cn, sizeof(int));
	_sinOut.resize(cn);
	for (int i = 0; i<cn; i++)
		_sinOut[i].ReadFile(handle);

	_read(handle, &cn, sizeof(int));
	_delta.resize(cn);
	for (int i = 0; i<cn; i++)
		_delta[i].ReadFile(handle);
	clNeuroNet::ReadFile(handle);
}
//---------------------------------------------------------------------------
void    clLayersBPNet::InitFullConnection(vector<int> &v0)
{
    Clear();
    int cn=0;
    for(unsigned i=1;i<v0.size()-1;i++)
        cn+=v0[i];
    New(v0[0],v0[v0.size()-1],cn);
    _cnCicle=3;
    _layers.resize(v0.size());
    _layers[0].resize(_cnIn);
    for(int i=0;i<_cnIn;i++)
        _layers[0][i]=i;
	int nb = _cnIn, nbOut = (int)v0.size() - 1;
    _layers[nbOut].resize(_cnOut);
    for(int i=0;i<_cnOut;i++)
        _layers[nbOut][i]=nb++;
    for(unsigned i=1;i<v0.size()-1;i++)
    {
        _layers[i].resize(v0[i]);
        for(int j=0;j<v0[i];j++)
            _layers[i][j]=nb++;
    }
    for(unsigned k=1;k<_layers.size();k++)
        for(unsigned i=0;i<_layers[k].size();i++)
        {
            clNeuron *ptrN=PtrNeuron(_layers[k][i]);
            for(unsigned i1=0;i1<_layers[k-1].size();i1++)
                ptrN->AddSinapse(_layers[k-1][i1],1-2.*(double)rand()/RAND_MAX);
        }
    for(unsigned k=2;k<_layers.size();k++)
        for(unsigned i=0;i<_layers[k].size();i++)
        {
            clNeuron *ptrN=PtrNeuron(_layers[k][i]);
            ptrN->AddSinapse(0,1-2.*(double)rand()/RAND_MAX);
        }
}
//---------------------------------------------------------------------------
void    clLayersBPNet::LearningBP(vector<tpTrainingPair>  &arrTP)
{
    for(unsigned i=0;i<arrTP.size();i++)
        LearningBP(arrTP[i]);
}
//---------------------------------------------------------------------------
void    clLayersBPNet::LearningBP(tpTrainingPair  &tp)
{
    vector <double> v0;
    Propogate(tp._vectIn,v0);
	int nbL = (int)_layers.size() - 1;
    for(int i=0;i<_cnOut;i++)
    {
        int nb=_layers[nbL][i];
        double out=v0[i];
		_delta[nb]._axon = out*(1 - out)*(tp._vectOut[i] - out);
    }
    for(int k=nbL-1;k>=0;k--)
        for(unsigned i=0;i<_layers[k].size();i++)
        {
            int nb=_layers[k][i];
            double sum=0;
            for(unsigned j=0;j<_sinOut[nb]._sinapse.size();j++)
            {
                int nb1=_sinOut[nb]._sinapse[j]._nbOut;
                sum+=_sinOut[nb]._sinapse[j]._weight*_delta[nb1]._axon;
            }
            _delta[nb]._axon=_neurons[nb]._axon*(1-_neurons[nb]._axon)*sum;
        }
    for(unsigned i=0;i<_delta.size();i++)
        for(unsigned j=0;j<_delta[i]._sinapse.size();j++)
        {
            int nb1=_delta[i]._sinapse[j]._nbOut;
            _delta[i]._sinapse[j]._weight=_alfa*_delta[i]._sinapse[j]._weight+(1-_alfa)*(_nu)*_delta[nb1]._axon*_neurons[i]._axon;
            double d0=fabs(_sinOut[i]._sinapse[j]._weight);
            if(d0>_maxWeight)
            {
                double d1=_sinOut[i]._sinapse[j]._weight+_delta[i]._sinapse[j]._weight;
                if(fabs(d1)>d0)
                    _delta[i]._sinapse[j]._weight/=1.2;
            }
            _sinOut[i]._sinapse[j]._weight+=_delta[i]._sinapse[j]._weight;
            SetWeightSinapse(i,nb1,_sinOut[i]._sinapse[j]._weight);
        }
}
//---------------------------------------------------------------------------
void clLayersBPNet::Propogate(vector<double> &vectIn,vector<double> &v0)
{
    _neurons[0]._axon=1;
    for(int i=1;i<_cnIn;i++)
        _neurons[i]._axon=vectIn[i-1];

    for(unsigned k=1;k<_layers.size();k++)
        for(unsigned i=0;i<_layers[k].size();i++)
        {
//            int nb=_layers[k][i];
            _neurons[_layers[k][i]].Propogate(this);
        }
    v0.resize(_cnOut);
    for(int i=0;i<_cnOut;i++)
        v0[i]=_neurons[i+_cnIn]._axon;
}
//---------------------------------------------------------------------------
double  clLayersBPNet::Error(vector<tpTrainingPair>  &arrTP)
{
    double err=0;
    vector <double> v0;
    for(unsigned i=0;i<arrTP.size();i++)
    {
        Propogate(arrTP[i]._vectIn,v0);
        for(unsigned j=0;j<v0.size();j++)
        {
            double d=v0[j];
            d-=arrTP[i]._vectOut[j];
            //err+=d*d*arrTP[i]._vectIn[j+2];
			err += d*d;
        }
    }
    err/=arrTP.size();
    err/= v0.size();
    return sqrt(err);
	/*int handle;
	handle = _open("C:\\data\\log.txt", _O_TEXT | _O_WRONLY | _O_CREAT| _O_APPEND, _S_IREAD | _S_IWRITE);
	for (unsigned i = 0; i < arrTP.size(); i++)
	{
		int imax = 0, cn0 = 0;
		double	max = 0;
		double errDouble;
		Propogate(arrTP[i]._vectIn, v0);
		for (unsigned j = 0; j < v0.size(); j++)
			if (max < v0[j])
			{
				max = v0[j];
				imax = j;
			}
		max = 0;
		for (unsigned j = 0; j < arrTP[i]._vectOut.size(); j++)
			if (max < arrTP[i]._vectOut[j])
			{
				max = arrTP[i]._vectOut[j];
				cn0 = j;
			}
		if (imax != cn0)
		{
			CString str, str1;
			err += 1;
			str.Format("%i",imax);
			str += " instead ";
			str1.Format("%i",cn0);
			str += str1 + "\n";
			int sizeStr = strlen(str);
			_write(handle, str, sizeStr);
		}
	}
	err/=arrTP.size();
	_close(handle);
	return err;*/
}
//---------------------------------------------------------------------------
double  clLayersBPNet::ErrorMultW(vector<tpTrainingPair>  &arrTP)
{
	double err = 0;
	vector <double> v0;
	for (unsigned i = 0; i<arrTP.size(); i++)
	{
		Propogate(arrTP[i]._vectIn, v0);
		for (unsigned j = 0; j<v0.size(); j++)
		{
			double d = v0[j];
			d -= arrTP[i]._vectOut[j];
			err += d*d* NNValToDouble01(arrTP[i]._vectIn[j + 2]);
			//err += d*d;
		}
	}
	err /= arrTP.size();
	err /= v0.size() / 10;
	return sqrt(err);
	/*int handle;
	handle = _open("C:\\data\\log.txt", _O_TEXT | _O_WRONLY | _O_CREAT| _O_APPEND, _S_IREAD | _S_IWRITE);
	for (unsigned i = 0; i < arrTP.size(); i++)
	{
	int imax = 0, cn0 = 0;
	double	max = 0;
	double errDouble;
	Propogate(arrTP[i]._vectIn, v0);
	for (unsigned j = 0; j < v0.size(); j++)
	if (max < v0[j])
	{
	max = v0[j];
	imax = j;
	}
	max = 0;
	for (unsigned j = 0; j < arrTP[i]._vectOut.size(); j++)
	if (max < arrTP[i]._vectOut[j])
	{
	max = arrTP[i]._vectOut[j];
	cn0 = j;
	}
	if (imax != cn0)
	{
	CString str, str1;
	err += 1;
	str.Format("%i",imax);
	str += " instead ";
	str1.Format("%i",cn0);
	str += str1 + "\n";
	int sizeStr = strlen(str);
	_write(handle, str, sizeStr);
	}
	}
	err/=arrTP.size();
	_close(handle);
	return err;*/
}
//---------------------------------------------------------------------------
void    clLayersBPNet::PrepareForBP()
{
    _sinOut.resize(_neurons.size());
    for(unsigned k=0;k<_layers.size()-1;k++)
        for(unsigned i=0;i<_layers[k].size();i++)
        {
            int nb=_layers[k][i];
            for(unsigned i1=0;i1<_layers[k+1].size();i1++)
            {
                int nb1=_layers[k+1][i1];
                for(unsigned j=0;j<_neurons[nb1]._sinapse.size();j++)
                    if(_neurons[nb1]._sinapse[j]._nbOut==nb)
                    {
                        tpSinapse sin;
                        sin._nbOut=nb1;
                        sin._weight=_neurons[nb1]._sinapse[j]._weight;
                        _sinOut[nb]._sinapse.push_back(sin);
                        break;
                    }
            }
        }
    _delta=_sinOut;
    for(unsigned i=0;i<_delta.size();i++)
        for(unsigned j=0;j<_delta[i]._sinapse.size();j++)
            _delta[i]._sinapse[j]._weight=0;
}
void clLayersBPNet::operator=(clLayersBPNet & val)
{
	Clear();
	_cnIn = val._cnIn;
	_cnOut = val._cnOut;
	_cnEx = val._cnEx;
	_cnCicle = val._cnCicle;
	_neurons.resize(val._neurons.size());
	for (size_t i = 0; i<_neurons.size(); i++)
		_neurons[i] = val._neurons[i];
	_alfa = val._alfa;
	_nu = val._nu;
	_maxWeight = val._maxWeight;
	_layers = val._layers;
	_sinOut.resize(val._sinOut.size());
	for (size_t i = 0; i<_sinOut.size(); i++)
		_sinOut[i] = val._sinOut[i];
	_delta.resize(val._delta.size());
	for (size_t i = 0; i<_delta.size(); i++)
		_delta[i] = val._delta[i];
	
}
//---------------------------------------------------------------------------
/*void    clLayersBPNet::LearningGradDes(vector<tpTrainingPair>  &arrTP)
{

	double err = Error(arrTP), dx = _nu;
	double max = 0;
	for (unsigned i = 0; i<_neurons.size(); i++)
		for (unsigned j = 0; j<_neurons[i]._sinapse.size(); j++)
		{
			double w0 = _neurons[i]._sinapse[i]._weight;
			_neurons[i]._sinapse[i]._weight += _nu;
			double e0 = Error(arrTP);
			_delta[i]._sinapse[j]._weight = err - e0;
			if (max<fabs(e0 - err)) max = fabs(err - e0);
			_neurons[i]._sinapse[i]._weight = w0;
		}
	for (unsigned i = 0; i<_neurons.size(); i++)
		for (unsigned j = 0; j<_neurons[i]._sinapse.size(); j++)
			_neurons[i]._sinapse[i]._weight += _nu*_delta[i]._sinapse[j]._weight / max;
}*/
//---------------------------------------------------------------------------
void    clLayersBPNet::LearningGradDes(vector<tpTrainingPair>  &arrTP, vector<clNeuron> &deltaEr)
{

	double err = Error(arrTP), dx = _nu;
	double max = 0;
	for (unsigned i = 0; i<_neurons.size(); i++)
		for (unsigned j = 0; j<_neurons[i]._sinapse.size(); j++)
		{
			double w0 = _neurons[i]._sinapse[j]._weight;
			_neurons[i]._sinapse[j]._weight += _nu;
			double e0 = Error(arrTP);
			deltaEr[i]._sinapse[j]._weight = err - e0;
			if (max<fabs(e0 - err)) max = fabs(err - e0);
			_neurons[i]._sinapse[j]._weight = w0;
		}
	for (unsigned i = 0; i<_neurons.size(); i++)
		for (unsigned j = 0; j<_neurons[i]._sinapse.size(); j++)
			_neurons[i]._sinapse[j]._weight += _nu*deltaEr[i]._sinapse[j]._weight / max;
}
//---------------------------------------------------------------------------

