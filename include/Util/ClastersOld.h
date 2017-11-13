//---------------------------------------------------------------------------

#ifndef ClastersH
#define ClastersH
//---------------------------------------------------------------------------
//#include "c:\home\util\Clasters.h"
#include "c:\Home\Util\TemplateFileRec.h"
#include "c:\Home\util\clTree.h"

#include <vector>
using namespace std;

//---------------------------------------------------------------------------
template<class T> void	VectAddCheck(vector <T> &vect, T &val)
{
	size_t dim = vect.size();
	if (dim == vect.capacity())
		vect.reserve(2 * dim);
	vect.push_back(val);
}
//---------------------------------------------------------------------------
template<class T> class clClasters
{
public:
void    Clear()                     {_vect.clear();}
vector <T> &operator [] (int nb)    {return _vect[nb];}
int     CnClasters()                {return _vect.size();}
void    Resize(int cn)              {_vect.resize(cn);}
void	DeleteCls(int nb)				{ _vect.erase(_vect.begin() + nb); }

size_t	CnElements()
{
	size_t res = 0;
	size_t dim = _vect.size();
	for (size_t i = 0; i < dim; i++)
		res += _vect[i].size();
	return res;
}
void	operator += (clClasters<T> &cls)
{ 
	int dim = _vect.size();
	int dimCls = cls._vect.size();
	_vect.resize(dim+dimCls);
	for (int i = 0; i < dimCls; i++)
		_vect[dim + i] = cls._vect[i];
}

void    WriteFileConstSize(int handle)
{
    int dim=_vect.size();
    _write(handle,&dim,sizeof(int));
	for (int i = 0; i < dim; i++)
		WriteVectFile<T>(handle, _vect[i]);
}

void    ReadFileConstSize(int handle)
{
    int dim;
    _read(handle,&dim,sizeof(int));
    _vect.resize(dim);
	for (int i = 0; i < dim; i++)
		ReadVectFile<T>(handle, _vect[i]);
}

void    Divide(int cnCls,int cnIter,double dist)
{
    if(_vect[0].size()<(unsigned)cnCls) return;
	vector <T> cls;cls = _vect[0];
    _vect[0].resize(1);
    _vect[0][0]=cls.back();
    cls.pop_back();
    while(_vect.size()<cnCls)
    {
        for(int i=cls.size()-1;i>=0;i--)
        {
            unsigned j=0;
            for(;j<_vect.size();j++)
                if(cls[i].Distance(_vect[j][0])<dist)
                    break;
            if(j==_vect.size())
            {
                _vect.resize(j+1);
                _vect[j].push_back(cls[i]);
                cls.erase(cls.begin()+i);
                if(_vect.size()==cnCls)
                    break;
            }
        }
        dist/=2;
    }
    for(unsigned i=0;i<cls.size();i++)
    {
        int nb=NearestClaster(cls[i]);
        _vect[nb].push_back(cls[i]);
    }
    for(unsigned i=0;i<cnIter;i++)
        if(!ReStraightClasters())
            break;
}
//---------------------------------------------------------------------------
int     NearestClaster(T &sp)
{
    if(_vect.size()==0) return -1;
    double min=_vect[0][0].Distance(sp);
    int minNb=0;
    for(int i=1;i<CnClasters();i++)
    {
        double dist=_vect[i][0].Distance(sp);
        if(dist<min)
        {
            min=dist;
            minNb=i;
        }
    }
    return minNb;
}
//---------------------------------------------------------------------------
bool    ReStraightClasters()
{
    bool b0=false;
    int dim=_vect.size();
    for(unsigned i=0;i<dim;i++)
        if(FindCenterElementAndRemove(i))
            b0=true;
    if(b0)
    {
        vector <T> cls;
        for(unsigned i=0;i<dim;i++)
        {
            cls.reserve(cls.size()+_vect[i].size());
            for(unsigned j=1;j<_vect[i].size();j++)
                cls.push_back(_vect[i][j]);
            _vect[i].resize(1);
        }
        for(unsigned i=0;i<cls.size();i++)
        {
            int nb=NearestClaster(cls[i]);
            _vect[nb].push_back(cls[i]);
        }
    }
    return b0;
}
//---------------------------------------------------------------------------
bool    FindCenterElementAndRemove(int i)
{
	int nb = T::FindNbCenterElement(_vect[i]);
	if (nb == 0) return false;
	T temp = _vect[i][0];
	_vect[i][0] = _vect[i][nb];
	_vect[i][nb] = temp;
	return true;
}
//---------------------------------------------------------------------------
int		DivideQuick(double dist,double r)
{
	clClasters <T> cls;
	vector <T> vB;
	int dim = _vect.size();
	vB.resize(dim);
	for (int i = 0; i < dim; i++)
		vB[i] = _vect[i][0];
	while (dim > 0)
	{
		vector <T> vNew;
		T m = _vect[0][0];
		VectAddCheck<T>(vNew, m);
		int dim0 = _vect[0].size();
		for (int k = dim0-1; k >0; k--)
			if (m.Distance(_vect[0][k]) < dist)
			{
				VectAddCheck<T>(vNew, _vect[0][k]);
				_vect[0].erase(_vect[0].begin()+k);
			}

		for (int i = 1; i < dim; i++)
		{
			if ((vB[0].Distance(vB[i]) - r) > vB[0].Distance(m) + dist) 
				continue;
			dim0 = _vect[i].size();
			for (int k = dim0 - 1; k >= 0; k--)
				if (m.Distance(_vect[i][k]) < dist)
				{
					VectAddCheck<T>(vNew, _vect[i][k]);
					_vect[i].erase(_vect[i].begin()+k);
				}
		}
		_vect[0].erase(_vect[0].begin());
		cls.AddCheck(vNew);
		for (int i = dim - 1; i >= 0; i--)
			if (_vect[i].size() == 0)
			{
				vB.erase(vB.begin() + i);
				_vect.erase(_vect.begin() + i);
			}
		dim = _vect.size();
	}
	*this = cls;
	return CnClasters();
}
//---------------------------------------------------------------------------
void	AddCheck(vector <T> *val)
{
	size_t dim = _vect.size();
	if (dim == _vect.capacity())
		_vect.reserve(2 * dim);
	if (val==NULL)
		_vect.resize(dim+1);
	else
		_vect.push_back(*val);
}
//---------------------------------------------------------------------------
double	BuildBinTree(int level)
{
	_tree.Clear();
	_tree._val->_dimLeft = _vect[0].size();
	_tree._val->_leftVect = (T*)malloc(_tree._val->_dimLeft*sizeof(T));
	memcpy(_tree._val->_leftVect, &_vect[0][0], _tree._val->_dimLeft*sizeof(T));
	for (size_t i = 0; i < _tree._val->_dimLeft; i++)
		if ((int)(_tree._val->_leftVect[i]) != (int)(_vect[0][i]))
			glTrasser.WriteMessage(i);
	_vect.clear();
	return BuildTree(&_tree, level);
}
//---------------------------------------------------------------------------
size_t	DivideBin(double dist)
{
	_vect.clear();
	clBinTree <stBinTreeClasters01> tree = _tree;
	DivideBin(&tree,tree._left, dist);
	DivideBin(&tree, tree._right, dist);
	return CnClasters();
}
//---------------------------------------------------------------------------
void	RecalcCls(double dist)
{
	size_t dim = CnClasters();
	vector <int> temp;
	temp.resize(dim);
	for (size_t i = 0; i < dim; i++)
	{
		size_t dim0 = _vect[i].size();
		if (dim0 < 2) continue;
		int cn = 0;
		for (size_t j = 0; j < dim; j++)
			if (i != j)
			{
				if (_vect[i][0].Distance(_vect[j][0]) < 2 * dist)
					temp[cn++] = j;
			}
		if (cn == 0) continue;
		for (size_t j = dim0 - 1; j >0; j--)
		{
			T &val = _vect[i][j];
			double min = val.Distance(_vect[i][0]);
			if (min < dist / 2) continue;
			int nbMin = i;
			for (int k = 0; k < cn; k++)
			{
				double d = val.Distance(_vect[temp[k]][0]);
				if (d < min)
				{
					d = min;
					nbMin = temp[k];
				}
			}
			if (nbMin != i)
			{
				VectAddCheck<T>(_vect[nbMin], val);
				_vect[i].erase(_vect[i].begin() + j);
			}
		}
	}


	
	/*vector <T> temp;
	size_t cnEl = CnElements()-dim,cur=0;
	temp.resize(cnEl);
	for (size_t i = 0; i < dim; i++)
	{
		size_t dim0 = _vect[i].size() - 1;
		if (dim0 > 0)
		{
			memcpy(&temp[0] + cur, &_vect[i][1], dim0*sizeof(T));
			cur += dim0;
		}
		_vect[i].resize(1);
	}
	for (size_t i = 0; i < cnEl; i++)
	{
		double min = temp[i].Distance(_vect[0][0]);
		int nbMin = 0;
		for (size_t j = 1; j < dim; j++)
		{
			double d = temp[i].Distance(_vect[j][0]);
			if (d < min)
			{
				min = d;
				nbMin = j;
			}
			VectAddCheck<T>(_vect[nbMin], temp[i]);
		}
	}*/
}
//---------------------------------------------------------------------------
struct stBinTreeClasters01
{
	stBinTreeClasters01()			{ _leftVect = NULL; _rightVect = NULL; _dimLeft = _dimRight = 0; }
	~stBinTreeClasters01()			{ Clear(); }
	void	Clear()
	{
		if (_leftVect != NULL)
			delete _leftVect;
		if (_rightVect != NULL)
			delete _rightVect;
		_dimLeft = _dimRight = 0;
	}
	void	operator = (stBinTreeClasters01 &val)
	{
		if (val._leftVect == NULL)
		{
			if (_leftVect != NULL)
			{
				delete _leftVect;
				_leftVect = NULL;
				_dimLeft = 0;
			}
		}
		else
		{
			if (val._dimLeft != _dimLeft)
				_leftVect = (T*)realloc(_leftVect,val._dimLeft * sizeof(T));
			memcpy(_leftVect, val._leftVect, val._dimLeft * sizeof(T));
			_dimLeft = val._dimLeft;
		}
		if (val._rightVect == NULL)
		{
			if (_rightVect != NULL)
			{
				delete _rightVect;
				_rightVect = NULL;
				_dimRight = 0;
			}
		}
		else
		{
			if (val._dimRight != _dimRight)
				_rightVect = (T*)realloc(_rightVect, val._dimRight * sizeof(T));
			memcpy(_rightVect, val._rightVect, val._dimRight * sizeof(T));
			_dimRight = val._dimRight;
		}
		_leftR = val._leftR;
		_rightR = val._rightR;
		_leftC = val._leftC;
		_rightC = val._rightC;
		_level = val._level;
	}

	T *_leftVect, *_rightVect;
	size_t _dimLeft,_dimRight;
	double _leftR, _rightR;
	T _leftC, _rightC;
	int _level;
};
void	DivideBin(clBinTree <stBinTreeClasters01> *root, clBinTree <stBinTreeClasters01> *tree, double dist)
{
	if (tree->_left == NULL)
	{
		if (tree->_val->_dimLeft > 0)
		{
			T *vect = tree->_val->_leftVect;
			size_t dim = tree->_val->_dimLeft;
			for (size_t i = 0; i < dim; i++)
				if (!vect[i].IsEmpty())
				{
					size_t nb = _vect.size();
					AddCheck(NULL);
					vector <T> &vs = _vect[nb];
					VectAddCheck<T>(vs, vect[i]);
					vect[i].SetEmpty();
					CreateClaster(root, vs, dist);
				}
			tree->_val->_leftVect = (T*)realloc(tree->_val->_leftVect,0);
			tree->_val->_dimLeft = 0;
		}
	}
	else
		DivideBin(root, tree->_left, dist);
	if (tree->_right == NULL)
	{
		if (tree->_val->_dimRight > 0)
		{
			T* vect = tree->_val->_rightVect;
			size_t dim = tree->_val->_dimRight;
			for (size_t i = 0; i < dim; i++)
				if (!vect[i].IsEmpty())
				{
					size_t nb = _vect.size();
					AddCheck(NULL);
					vector <T> &vs = _vect[nb];
					VectAddCheck<T>(vs, vect[i]);
					vect[i].SetEmpty();
					CreateClaster(root, vs, dist);
				}
			tree->_val->_rightVect = (T*)realloc(tree->_val->_rightVect, 0);
			tree->_val->_dimRight = 0;
		}
	}
	else
		DivideBin(root, tree->_right, dist);
}
//---------------------------------------------------------------------------
private:
	static	void	CheckClaster(vector <T> &vs, size_t dim,T* vect, double dist)
	{
		for (size_t i = 0; i < dim;i++)
			if (!vect[i].IsEmpty())
			{
				double d = vs[0].Distance(vect[i]);
				if (d < dist)
				{
					VectAddCheck<T>(vs, vect[i]);
					vect[i].SetEmpty();
				}
			}
	}
	static void	CreateClaster(clBinTree <stBinTreeClasters01> *root, vector <T> &vs, double dist)
	{
		double d = vs[0].Distance(root->_val->_leftC);
		if (d < root->_val->_leftR + dist)
			if (root->_left == NULL)
				CheckClaster(vs, root->_val->_dimLeft,root->_val->_leftVect, dist);
			else
				CreateClaster(root->_left, vs, dist);
		
		d = vs[0].Distance(root->_val->_rightC);
		if (d < root->_val->_rightR + dist)
			if (root->_right == NULL)
				CheckClaster(vs, root->_val->_dimRight, root->_val->_rightVect, dist);
			else
				CreateClaster(root->_right, vs, dist);
	}
	double	BuildTree(clBinTree <stBinTreeClasters01> *tree, int level)
	{
		tree->_val->_level = level;
		DivideBinNode(tree);
		if (level == 0) return (tree->_val->_rightR > tree->_val->_leftR) ? tree->_val->_rightR : tree->_val->_leftR;
		double resL = 0,resR=0;
		if (tree->_val->_dimLeft > 1)
		{
			clBinTree <stBinTreeClasters01> *br = tree->AddLeftBranch();
			br->_val->_leftVect = tree->_val->_leftVect;
			br->_val->_dimLeft = tree->_val->_dimLeft;
			tree->_val->_leftVect = NULL;
			tree->_val->_dimLeft = 0;
			resL=BuildTree(br, level - 1);
		}
		if (tree->_val->_dimRight > 1)
		{
			clBinTree <stBinTreeClasters01> *br = tree->AddRightBranch();
			br->_val->_leftVect = tree->_val->_rightVect;
			br->_val->_dimLeft = tree->_val->_dimRight;
			tree->_val->_rightVect = NULL;
			tree->_val->_dimRight = 0;
			resR=BuildTree(br, level - 1);
		}
		return (resL > resR) ? resL : resR;
	}
	static void	Get2Centers(size_t dim,T* vect, int &v1, int &v2)
	{
		unsigned r1 = random32(dim);
		unsigned r2 = random32(dim);
		unsigned r3 = random32(dim);
		double d12 = vect[r1].Distance(vect[r2]);
		double d13 = vect[r1].Distance(vect[r3]);
		double d23 = vect[r2].Distance(vect[r3]);
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
	}
	void	DivideBinNode(clBinTree <stBinTreeClasters01> *tree)
	{
		int c0, c1;
		T *vect0 = tree->_val->_leftVect;
		tree->_val->_rightVect = (T*)malloc(tree->_val->_dimLeft*sizeof(T));
		T *vect1 = tree->_val->_rightVect;
		Get2Centers(tree->_val->_dimLeft, vect0, c0, c1);
		size_t dim = tree->_val->_dimLeft, cn0 = 1,cn1=1;
		T temp = vect0[0];
		vect0[0] = vect0[c0];
		vect0[c0] = temp;
		vect1[0]=vect0[c1];
		vect0[c1] = vect0[1];
		double r0 = 0, r1 = 0;
		for (size_t i = 2; i < dim; i++)
		{
//			if (i==2)
//				glTrasser.WriteMessage(i);
			double val0 = vect0[0].Distance(vect0[i]);
			double val1 = vect1[0].Distance(vect0[i]);
			if (val0 < val1)
			{
				vect0[cn0++] = vect0[i];
				if (val0 > r0) r0 = val0;
			}
			else
			{
				vect1[cn1++]=vect0[i];
				if (val1 > r1) r1 = val1;
			}
		}
		tree->_val->_leftVect = (T*)realloc(tree->_val->_leftVect, cn0*sizeof(T));
		tree->_val->_dimLeft = cn0;
		tree->_val->_rightVect = (T*)realloc(tree->_val->_rightVect, cn1*sizeof(T));
		tree->_val->_dimRight = cn1;
		tree->_val->_leftR = r0;
		tree->_val->_leftC = tree->_val->_leftVect[0];
		tree->_val->_rightR = r1;
		tree->_val->_rightC = tree->_val->_rightVect[0];
	}

	vector <vector <T> > _vect;
	clBinTree <stBinTreeClasters01>_tree;
/*struct tpVectorTName : public vector <T>
{
	char _name[32];
};

private :
	vector <tpVectorTName > _vect;*/
};
//---------------------------------------------------------------------------
#endif
