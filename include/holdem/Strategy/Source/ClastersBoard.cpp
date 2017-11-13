#include <time.h>
#include <fcntl.h>
#include <io.h>
#include <sys\stat.h>

#include "..\..\..\Util\TemplateFileRec.h"
#include "ClastersBoard.h"
#include "..\..\..\card\naborCards.h"

using namespace std;

//int DivideCls(clClasters <int> &cls, double dCur, double r, double(*f) (int, int));
void	FindCenterTurnCls(vector <int> &vect);
void	FindCenterRiverCls(vector <int> &vect);
double	DistFlopHand(int nb, tpFloat0 evFlop100, tpFloat0 evflop50, tpFloat0 ev100, tpFloat0 ev50);
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	tpFlopHandUnit::Get2Centers(vector <tpFlopHandUnit> &vect, int &v1, int &v2)
{
	size_t dim = vect.size();
	
	unsigned r1 = random32(dim);
	unsigned r2 = random32(dim);
	unsigned r3 = random32(dim);
	double d12 = DistFlopHand(vect[r1], vect[r2]);
	double d13 = DistFlopHand(vect[r1], vect[r3]);
	double d23 = DistFlopHand(vect[r2], vect[r3]);
	if (d12 > d13 && d12 > d23)
	{
		v1 = r1;
		v2 = r2;
	}
	else if (d13 > d12 && d13 > d23)
	{
		v1 = r1;
		v2 = r3;
	}
	else 
	{
		v1 = r2;
		v2 = r3;
	}
	return;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	tpTurnHandUnit::Get2Centers(vector <tpTurnHandUnit> &vect, int &v1, int &v2)
{
	size_t dim = vect.size();

	unsigned r1 = random32(dim);
	unsigned r2 = random32(dim);
	unsigned r3 = random32(dim);
	double d12 = DistTurnHand(vect[r1], vect[r2]);
	double d13 = DistTurnHand(vect[r1], vect[r3]);
	double d23 = DistTurnHand(vect[r2], vect[r3]);
	if (d12 > d13 && d12 > d23)
	{
		v1 = r1;
		v2 = r2;
	}
	else if (d13 > d12 && d13 > d23)
	{
		v1 = r1;
		v2 = r3;
	}
	else
	{
		v1 = r2;
		v2 = r3;
	}
	return;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
double	DistFlopHand(int nb1, int nb2)
{
	double val = glVectFlopEV100[nb1] - glVectFlopEV100[nb2];
	double res = val*val;
	val = glVectFlopEV50[nb1] - glVectFlopEV50[nb2];
	res += val*val;

/*	nb1 = GetHandIndex(glVectFlopHand[nb1]._hand);
	nb2 = GetHandIndex(glVectFlopHand[nb2]._hand);
	val = (glVectEV100[nb1] - glVectEV100[nb2])*0.5;
	res += val*val;
	val = (glVectEV50[nb1] - glVectEV50[nb2])*0.5;
	res += val*val;*/

	return sqrt(res);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
double	DistFlopHand(int nb, tpFloat0 evFlop100, tpFloat0 evFlop50, tpFloat0 ev100, tpFloat0 ev50)
{
	double val = glVectFlopEV100[nb] - evFlop100;
	double res = val*val;
	val = glVectFlopEV50[nb] - evFlop50;
	res += val*val;
	nb = GetHandIndex(glVectFlopHand[nb]._hand);
	val = (glVectEV100[nb] - ev100)*0.5;
	res += val*val;
	val = (glVectEV50[nb] - ev50)*0.5;
	res += val*val;
	return sqrt(res);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	FindCenterFlopCls(vector <int> &vect)
{
	float ev100 = 0, ev50 = 0, evFlop100 = 0, evFlop50 = 0;
	size_t dim = vect.size();
	for (size_t i = 0; i < dim; i++)
	{
		int nb = vect[i];
		evFlop100 += glVectFlopEV100[nb];
		evFlop50 += glVectFlopEV50[nb];
		nb = GetHandIndex(glVectFlopHand[nb]._hand);
		ev100 += glVectEV100[nb];
		ev50 += glVectEV50[nb];
	}
	evFlop100 /= dim;
	evFlop50 /= dim;
	ev100 /= dim;
	ev50 /= dim;
	double min = 1000;
	int nbMin = -1;
	for (size_t i = 0; i < dim; i++)
	{
		double res = DistFlopHand(vect[i],evFlop100,evFlop50,ev100,ev50);
		if (res < min)
		{
			min = res;
			nbMin = i;
		}
	}
	if (nbMin > 0)
	{
		int temp = vect[0];
		vect[0] = vect[nbMin];
		vect[nbMin] = temp;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		FindCnCls(vector <int> &vect)
{
	int max = -1;
	int dim = vect.size();
	for (int i = 0; i < dim; i++)
		if (vect[i]>max)
			max = vect[i];
	return max + 1;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class T> void	CreateVectIndex(clClasters <T> &cls, vector <int> &index)
{
	size_t cn = cls.CnElements();
	index.resize(cn);
	int cnAll = 0,cnCls=cls.CnClasters();
	for (int i = 0; i < cnCls; i++)
	{
		int dim0 = cls[i].size();
		cnAll += dim0;
		for (int j = 0; j < dim0; j++)
			index[cls[i][j]] = i;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CalcFlopHandCls(int cnClsMax)
{
	clClasters <tpFlopHandUnit> cls;
	cls.Resize(1);
	size_t dim = glVectFlopHand.size();
	cls[0].resize(dim);
	for (size_t i = 0; i < dim; i++)
		cls[0][i] = i;
	double dist = cls.BuildBinTree(15);
	dist = 0.024;
	size_t cnCls = 0;
	while(cnCls < 300000)
	{
		time_t t0 = time(NULL);
		cnCls = cls.DivideBin(dist);
//		cls.RecalcCls(dist);
		t0 = time(NULL) - t0;
		clAnsiString as = (clAnsiString)"t=" + (clAnsiString)t0 + (clAnsiString)"; cnCls=" + (clAnsiString)(int)cnCls + (clAnsiString)"; dist=" + (clAnsiString)dist;
		glTrasser.WriteMessage(as);
		dist /= 1.5;

		vector <int> index;
		CreateVectIndex(cls, index);
		as = (clAnsiString)PATH_CLASTERS + (clAnsiString)"Flop" + (clAnsiString)(int)cnCls + (clAnsiString)".bin";
		int handle = _open(as.c_str(), O_RDWR | O_BINARY | O_CREAT);
		if (handle == -1)
			ErrMessage("Не открывается файл ",as);
		else
		{
			WriteVectFile<int>(handle, index);
			_close(handle);
		}
	}
	dist = 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*int DivideCls(clClasters <int> &clsIn, double dist, double r, double(*f) (int, int))
{
	clClasters <int> cls;
	vector <int> vB;
	int dim = clsIn.CnClasters();
	vB.resize(dim);
	for (int i = 0; i < dim; i++)
		vB[i] = clsIn[i][0];
	while (dim > 0)
	{
		vector <int> vNew;
		int m=-1, k = 0;
		int dim0 = clsIn[0].size();
		for (; k < dim0; k++)
			if (clsIn[0][k] >= 0)
			{
				m= clsIn[0][k];
				vNew.push_back(m);
				clsIn[0][k] = -1;
				break; 
			}
		if (m == -1) break;
		for (k++; k < dim0; k++)
			if (clsIn[0][k] >= 0 && f(m, clsIn[0][k]) < dist)
			{
				VectAddCheck<int>(vNew, clsIn[0][k]);
				clsIn[0][k] = -1;
			}
		for (int i = 1; i < dim; i++)
		{
			bool b0 = false;
			double d1 = f(m, vB[i]);
			if (d1 > r + dist)
			{
				d1 = f(vB[0], vB[i]);
				double d2 = f(vB[0], m);
				if (d1-d2>r+dist)
					continue;
				b0 = true;
			}
			dim0 = clsIn[i].size();
			for (k = 0; k < dim0; k++)
				if (clsIn[i][k] >= 0 && f(m, clsIn[i][k]) < dist)
				{
					if (b0)
					{
						d1 = f(vB[0], vB[i]);
						double d2 = f(vB[0], m);
						double d4 = f(m, vB[i]);
						int c = clsIn[i][k];
						double d3 = f(m, clsIn[i][k]);
						double d5 = f(vB[i], clsIn[i][k]);
						d1 = d1;
					}
					VectAddCheck<int>(vNew, clsIn[i][k]);
					clsIn[i][k]=-1;
				}
		}
		for (int i = dim-1; i >=0; i--)
		{
			int nb = -1;
			dim0 = clsIn[i].size();
			for (k = 0; k < dim0; k++)
				if (clsIn[i][k] >= 0)
				{
					nb = k;
				}
			if (nb == -1)
			{
				vB.erase(vB.begin() + i);
				clsIn.DeleteCls(i);
			}
			else
				clsIn[i].resize(nb+1);
		}
		cls.AddCheck(&vNew);
		dim = clsIn.CnClasters();
	}
	clsIn = cls;
	return clsIn.CnClasters();
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CalcTurnHandCls(int cnClsMax)
{
	clClasters <tpTurnHandUnit> cls;
	cls.Resize(1);
	size_t dim = glVectTurnHand.size();
	cls[0].resize(dim);
	for (size_t i = 0; i < dim; i++)
		cls[0][i] = i;

	time_t t0 = time(NULL);
	double dist = cls.BuildBinTree(20);
	t0 = time(NULL) - t0;
	clAnsiString as = (clAnsiString)"BuildTree t=" + (clAnsiString)t0;
	glTrasser.WriteMessage(as);
	dist = 0.035;
	size_t cnCls = 0;
	while (cnCls < cnClsMax)
	{
		t0 = time(NULL);
		cnCls = cls.DivideBin(dist);
		//		cls.RecalcCls(dist);
		t0 = time(NULL) - t0;
		as = (clAnsiString)"t=" + (clAnsiString)t0 + (clAnsiString)"; cnCls=" + (clAnsiString)(int)cnCls + (clAnsiString)"; dist=" + (clAnsiString)dist;
		glTrasser.WriteMessage(as);
		dist /= 1.5;

		vector <int> index;
		CreateVectIndex(cls, index);
		as = (clAnsiString)PATH_CLASTERS + (clAnsiString)"Turn" + (clAnsiString)(int)cnCls + (clAnsiString)".bin";
		int handle = _open(as.c_str(), O_RDWR | O_BINARY | O_CREAT);
		if (handle == -1)
			ErrMessage("Не открывается файл ", as);
		else
		{
			WriteVectFile<int>(handle, index);
			_close(handle);
		}
	}
	dist = 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
#define CN_CLS_DIV		50000
struct tpThreadAtr
{
	clClasters <int> _cls;
	bool	_stop;
	double _dist, _r;
};
unsigned long _stdcall CalcTurnClsThread(void *ptr)
{
/*	tpThreadAtr *atr = (tpThreadAtr *)ptr;
	clClasters <int> &cls=atr->_cls; 
	if (cls.CnClasters()<CN_CLS_DIV)
		DivideCls(cls, atr->_dist, atr->_r, DistTurnHand);
	else
	{
		clClasters <int> clsRes;
		while (cls.CnClasters() > 0)
		{
			if (cls.CnClasters() < CN_CLS_DIV)
			{
				DivideCls(cls, atr->_dist, atr->_r, DistTurnHand);
				clsRes += cls;
				break;
			}
			else
			{
				clClasters <int> clsT;
				clsT.Resize(CN_CLS_DIV);
				int nb0 = cls.CnClasters() - CN_CLS_DIV;
				for (int i = 0; i < CN_CLS_DIV; i++)
					clsT[i] = cls[nb0 + i];
				cls.Resize(nb0);
				DivideCls(clsT, atr->_dist, atr->_r, DistTurnHand);
				clsRes += clsT;
			}
		}
		cls = clsRes;
	}
	int cnCls = cls.CnClasters();
	for (int i = 0; i < cnCls; i++)
		FindCenterTurnCls(cls[i]);
	atr->_stop = true;*/
	return 0;
}
void	CalcTurnHandClsThread(clClasters <int> &cls, int cnClsMax,double dist)
{
	double r = dist;
	int cnCls = 0;
	int dim = cls.CnClasters();
	tpThreadAtr atr[CN_THREAD];
	int cn0 = 0;
	for (int i = 0; i < CN_THREAD; i++)
	{
		int nbF = cn0;
		cn0 += (dim - cn0) / (CN_THREAD - i);
		for (int j = nbF; j < cn0; j++)
			atr[i]._cls.AddCheck(&cls[j]);
	}

	while (cnCls < cnClsMax)
	{
		dist /= 2;
		time_t t0 = time(NULL);
		for (int i = 0; i < CN_THREAD; i++)
		{
			atr[i]._stop = false;
			atr[i]._dist = dist;
			atr[i]._r = r;
			CreateThread(NULL, 0, &CalcTurnClsThread, &atr[i], 0, NULL);
		}
		while (true)
		{
			int j = 0;
			for (; j < CN_THREAD; j++)
				if (!atr[j]._stop)
					break;
			if (j == CN_THREAD)
				break;
			Sleep(100);
		}
		cnCls = 0;
		for (int i = 0; i < CN_THREAD; i++)
			cnCls += atr[i]._cls.CnClasters();
		r = dist;
		t0 = time(NULL) - t0;
		clAnsiString as = (clAnsiString)"dist=" + (clAnsiString)dist + (clAnsiString)"; t=" + (clAnsiString)t0 + (clAnsiString)"сек; cnCls=" + (clAnsiString)cnCls;
		glTrasser.WriteMessage(as);

		int h0 = _open(FILE_CLASTERS_TURN, O_RDWR | O_BINARY | O_CREAT);
		if (h0 != -1)
		{
			_lseeki64(h0, 0, SEEK_END);
			vector <int> clsTurn;
			clsTurn.resize(glVectTurnHand.size());
			int cnAll = 0,nb=0;
			for (int i0 = 0; i0 < CN_THREAD; i0++)
			{
				clClasters <int> &clsA = atr[i0]._cls;
				for (int i = 0; i < clsA.CnClasters(); i++)
				{
					int dim0 = clsA[i].size();
					cnAll += dim0;
					for (int j = 0; j < dim0; j++)
						clsTurn[clsA[i][j]] = nb;
					nb++;
				}
			}
			if (cnAll == glVectTurnHand.size())
				WriteVectFile<int>(h0, clsTurn);
			else
				glTrasser.WriteMessage((clAnsiString)"Ошибка Кластеров");
			_close(h0);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
double	DistTurnHand(int nb1, int nb2)
{
	double val = glVectTurnEV100[nb1] - glVectTurnEV100[nb2];
	double res = val*val;
	val = glVectTurnEV50[nb1] - glVectTurnEV50[nb2];
	res += val*val;
	nb1 = GetFlopHandIndexFromTurnHandId(glVectTurnHand[nb1]._idBoardHand);
	nb2 = GetFlopHandIndexFromTurnHandId(glVectTurnHand[nb2]._idBoardHand);
	val = (glVectFlopEV100[nb1] - glVectFlopEV100[nb2])*0.5;
	res += val*val;
	val = (glVectFlopEV50[nb1] - glVectFlopEV50[nb2])*0.5;
	res += val*val;
	nb1 = GetHandIndex(glVectFlopHand[nb1]._hand);
	nb2 = GetHandIndex(glVectFlopHand[nb2]._hand);
	val = (glVectEV100[nb1] - glVectEV100[nb2])*0.3;
	res += val*val;
	val = (glVectEV50[nb1] - glVectEV50[nb2])*0.3;
	res += val*val;
	return sqrt(res);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void	FindCenterTurnCls(vector <int> &vect)
{
	float ev100 = 0, ev50 = 0, evFlop100 = 0, evFlop50 = 0, evTurn100 = 0, evTurn50 = 0;
	size_t dim = vect.size();
	for (size_t i = 0; i < dim; i++)
	{
		int nb = vect[i];
		evTurn100 += glVectTurnEV100[nb];
		evTurn50 += glVectTurnEV50[nb];
		nb = GetFlopHandIndexFromTurnHandId(glVectTurnHand[nb]._idBoardHand);
		evFlop100 += glVectFlopEV100[nb];
		evFlop50 += glVectFlopEV50[nb];
		nb = GetHandIndex(glVectFlopHand[nb]._hand);
		ev100 += glVectEV100[nb];
		ev50 += glVectEV50[nb];
	}
	evTurn100 /= dim;
	evTurn50 /= dim;
	evFlop100 /= dim;
	evFlop50 /= dim;
	ev100 /= dim;
	ev50 /= dim;
	double min = 1000;
	int nbMin = -1;
	for (size_t i = 0; i < dim; i++)
	{
		int nb1 = vect[i];
		double val = glVectTurnEV100[nb1] - evTurn100;
		double res = val*val;
		val = glVectTurnEV50[nb1] - evTurn50;
		res += val*val;
		nb1 = GetFlopHandIndexFromTurnHandId(glVectTurnHand[nb1]._idBoardHand);
		val = glVectFlopEV100[nb1] - evFlop100;
		res += val*val;
		val = glVectFlopEV50[nb1] - evFlop50;
		res += val*val;
		nb1 = GetHandIndex(glVectFlopHand[nb1]._hand);
		val = glVectEV100[nb1] - ev100;
		res += val*val;
		val = glVectEV50[nb1] - ev50;
		res += val*val;
		res = sqrt(res);
		if (res < min)
		{
			min = res;
			nbMin = i;
		}
	}
	if (nbMin > 0)
	{
		int temp = vect[0];
		vect[0] = vect[nbMin];
		vect[nbMin] = temp;
	}
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CalcRiverHandCls(int cnClsMax)
{
	clClasters <tpRiverHandUnit> cls;
	cls.Resize(1);
	size_t dim = glVectRiverEV.size();
	cls[0].resize(dim);
	for (size_t i = 0; i < dim; i++)
		cls[0][i] = i;

	time_t t0 = time(NULL);
	double dist = cls.BuildBinTree(20);
	t0 = time(NULL) - t0;
	clAnsiString as = (clAnsiString)"BuildTree t=" + (clAnsiString)t0;
	glTrasser.WriteMessage(as);
	dist = 0.06;
	size_t cnCls = 0;
	while (cnCls < cnClsMax)
	{
		t0 = time(NULL);
		cnCls = cls.DivideBin(dist);
		//		cls.RecalcCls(dist);
		t0 = time(NULL) - t0;
		as = (clAnsiString)"t=" + (clAnsiString)t0 + (clAnsiString)"; cnCls=" + (clAnsiString)(int)cnCls + (clAnsiString)"; dist=" + (clAnsiString)dist;
		glTrasser.WriteMessage(as);
		dist /= 1.5;

		vector <int> index;
		CreateVectIndex(cls, index);
		as = (clAnsiString)PATH_CLASTERS + (clAnsiString)"River" + (clAnsiString)(int)cnCls + (clAnsiString)".bin";
		int handle = _open(as.c_str(), O_RDWR | O_BINARY | O_CREAT);
		if (handle == -1)
			ErrMessage("Не открывается файл ", as);
		else
		{
			WriteVectFile<int>(handle, index);
			_close(handle);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
double	DistRiverHand(int nb1, int nb2)
{
	double val = glVectRiverEV[nb1] - glVectRiverEV[nb2];
	double res = val*val;
	nb1 = GetTurnHandIndexFromRiverHandIndex(nb1);
	nb2 = GetTurnHandIndexFromRiverHandIndex(nb2);
	val = (glVectTurnEV100[nb1] - glVectTurnEV100[nb2])*0.5;
	res += val*val;
	val = (glVectTurnEV50[nb1] - glVectTurnEV50[nb2])*0.5;
	res += val*val;
	nb1 = GetFlopHandIndexFromTurnHandId(glVectTurnHand[nb1]._idBoardHand);
	nb2 = GetFlopHandIndexFromTurnHandId(glVectTurnHand[nb2]._idBoardHand);
	val = (glVectFlopEV100[nb1] - glVectFlopEV100[nb2])*0.3;
	res += val*val;
	val = (glVectFlopEV50[nb1] - glVectFlopEV50[nb2])*0.3;
	res += val*val;
	nb1 = GetHandIndex(glVectFlopHand[nb1]._hand);
	nb2 = GetHandIndex(glVectFlopHand[nb2]._hand);
	val = (glVectEV100[nb1] - glVectEV100[nb2])*0.2;
	res += val*val;
	val = (glVectEV50[nb1] - glVectEV50[nb2])*0.2;
	res += val*val;
	return sqrt(res);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	FindCenterRiverCls(vector <int> &vect)
{
	float ev100 = 0, ev50 = 0, evFlop100 = 0, evFlop50 = 0, evTurn100 = 0, evTurn50 = 0,evRiver=0;
	size_t dim = vect.size();
	for (size_t i = 0; i < dim; i++)
	{
		int nb = vect[i];
		evRiver += glVectRiverEV[nb];
		nb = GetTurnHandIndexFromRiverHandIndex(nb);
		evTurn100 += glVectTurnEV100[nb];
		evTurn50 += glVectTurnEV50[nb];
		nb = GetFlopHandIndexFromTurnHandId(glVectTurnHand[nb]._idBoardHand);
		evFlop100 += glVectFlopEV100[nb];
		evFlop50 += glVectFlopEV50[nb];
		nb = GetHandIndex(glVectFlopHand[nb]._hand);
		ev100 += glVectEV100[nb];
		ev50 += glVectEV50[nb];
	}
	evRiver /= dim;
	evTurn100 /= dim;
	evTurn50 /= dim;
	evFlop100 /= dim;
	evFlop50 /= dim;
	ev100 /= dim;
	ev50 /= dim;
	double min = 1000;
	int nbMin = -1;
	for (size_t i = 0; i < dim; i++)
	{
		int nb1 = vect[i];
		double val = glVectRiverEV[nb1] - evRiver;
		double res = val*val;
		nb1 = GetTurnHandIndexFromRiverHandIndex(nb1);
		val = glVectTurnEV100[nb1] - evTurn100;
		res += val*val;
		val = glVectTurnEV50[nb1] - evTurn50;
		res += val*val;
		nb1 = GetFlopHandIndexFromTurnHandId(glVectTurnHand[nb1]._idBoardHand);
		val = glVectFlopEV100[nb1] - evFlop100;
		res += val*val;
		val = glVectFlopEV50[nb1] - evFlop50;
		res += val*val;
		nb1 = GetHandIndex(glVectFlopHand[nb1]._hand);
		val = glVectEV100[nb1] - ev100;
		res += val*val;
		val = glVectEV50[nb1] - ev50;
		res += val*val;
		res = sqrt(res);
		if (res < min)
		{
			min = res;
			nbMin = i;
		}
	}
	if (nbMin > 0)
	{
		int temp = vect[0];
		vect[0] = vect[nbMin];
		vect[nbMin] = temp;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned long _stdcall CalcRiverClsThread(void *ptr)
{
/*	tpThreadAtr *atr = (tpThreadAtr *)ptr;
	clClasters <int> &cls = atr->_cls;
	if (cls.CnClasters()<CN_CLS_DIV)
		DivideCls(cls, atr->_dist, atr->_r, DistRiverHand);
	else
	{
		clClasters <int> clsRes;
		while (cls.CnClasters() > 0)
		{
			if (cls.CnClasters() < CN_CLS_DIV)
			{
				DivideCls(cls, atr->_dist, atr->_r, DistRiverHand);
				clsRes += cls;
				break;
			}
			else
			{
				clClasters <int> clsT;
				clsT.Resize(CN_CLS_DIV);
				int nb0 = cls.CnClasters() - CN_CLS_DIV;
				for (int i = 0; i < CN_CLS_DIV; i++)
					clsT[i] = cls[nb0 + i];
				cls.Resize(nb0);
				DivideCls(clsT, atr->_dist, atr->_r, DistRiverHand);
				clsRes += clsT;
			}
		}
		cls = clsRes;
	}
	int cnCls = cls.CnClasters();
	for (int i = 0; i < cnCls; i++)
		FindCenterRiverCls(cls[i]);
	atr->_stop = true;*/
	return 0;
}
void	CalcRiverHandClsThread(clClasters <int> &cls, int cnClsMax, double dist)
{
	/*	int handle0 = _open(FILE_CLASTERS_TURN, O_RDWR | O_BINARY | O_CREAT);
	vector <int> clsTurn;
	clsTurn.resize(CN_NABOR_HAND_TURN);
	int cnAll = 0;
	for (int i = 0; i < cls.CnClasters(); i++)
	{
	int dim0 = cls[i].size();
	cnAll += dim0;
	for (int j = 0; j < dim0; j++)
	clsTurn[cls[i][j]] = i;
	}
	if (cnAll == CN_NABOR_HAND_TURN)
	WriteVectFile<int>(handle0, clsTurn);
	else
	glTrasser.WriteMessage((clAnsiString)"Ошибка Кластеров");
	return;*/

	double r = dist;
	int cnCls = 0;
	int dim = cls.CnClasters();
	tpThreadAtr atr[CN_THREAD];
	int cn0 = 0;
	for (int i = 0; i < CN_THREAD; i++)
	{
		int nbF = cn0;
		cn0 += (dim - cn0) / (CN_THREAD - i);
		for (int j = nbF; j < cn0; j++)
			atr[i]._cls.AddCheck(&cls[j]);
	}

	while (cnCls < cnClsMax)
	{
		dist /= 1.5;
		time_t t0 = time(NULL);
		for (int i = 0; i < CN_THREAD; i++)
		{
			atr[i]._stop = false;
			atr[i]._dist = dist;
			atr[i]._r = r;
			CreateThread(NULL, 0, &CalcRiverClsThread, &atr[i], 0, NULL);
		}
		while (true)
		{
			int j = 0;
			for (; j < CN_THREAD; j++)
				if (!atr[j]._stop)
					break;
			if (j == CN_THREAD)
				break;
			Sleep(100);
		}
		cnCls = 0;
		for (int i = 0; i < CN_THREAD; i++)
			cnCls += atr[i]._cls.CnClasters();
		int handle = _open(FILE_CLASTERS_TEMP_RIVER, O_RDWR | O_BINARY | O_CREAT);
		if (handle != -1)
		{
			cls = atr[0]._cls;
			for (int i = 1; i < CN_THREAD; i++)
			{
				int dim0 = atr[i]._cls.CnClasters();
				for (int j = 0; j < dim0;j++)
					cls.AddCheck(&atr[i]._cls[j]);
			}
			cls.WriteFileConstSize(handle);
			_close(handle);
		}
		else
			glTrasser.WriteMessage((clAnsiString)"Не могу открыть файл");
		r = dist;
		t0 = time(NULL) - t0;
		clAnsiString as = (clAnsiString)"dist=" + (clAnsiString)dist + (clAnsiString)"; t=" + (clAnsiString)t0 + (clAnsiString)"сек; cnCls=" + (clAnsiString)cnCls;
		glTrasser.WriteMessage(as);

		int h0 = _open(FILE_CLASTERS_RIVER, O_RDWR | O_BINARY | O_CREAT);
		if (h0 != -1)
		{
			_lseeki64(h0, 0, SEEK_END);
			vector <int> refCls;
			refCls.resize(glVectRiverEV.size());
			int cnAll = 0;
			for (int i0 = 0; i0 < CN_THREAD; i0++)
			{
				clClasters <int> &clsA = atr[i0]._cls;
				for (int i = 0; i < clsA.CnClasters(); i++)
				{
					int dim0 = clsA[i].size();
					cnAll += dim0;
					for (int j = 0; j < dim0; j++)
						refCls[clsA[i][j]] = i;
				}
			}
			if (cnAll == CN_NABOR_HAND_RIVER)
				WriteVectFile<int>(h0, refCls);
			else
				glTrasser.WriteMessage((clAnsiString)"Ошибка Кластеров");
			_close(h0);
		}
		else
			glTrasser.WriteMessage((clAnsiString)"Не могу открыть файл");
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
