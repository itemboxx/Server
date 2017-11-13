//---------------------------------------------------------------------------

#ifndef clTreeH
#define clTreeH
//---------------------------------------------------------------------------
typedef void * (*tpPtrFuncTree)(void *ptr);
typedef void  (*tpPtrFuncCopyTree)(void *dest,void *src);
//#include <system.hpp>
#include "c:\home\Util\WorkStr.h"
#include <vector>

#define TV_MAX_CN_BRANCH   6

struct stShortPoint
{
    short _x,_y;
};
//---------------------------------------------------------------------------
template<class T> class clTreeVect
{
public:
	clTreeVect()
	{
		_weight=0;
		_parent=NULL;
		_vect.clear();
		_val=new T;
	}
	~clTreeVect()
	{
		ClearAllBranch();
		delete(_val);
	}
	clTreeVect  *Parent()           { return _parent; }
	int CnBranch() const				{ return _vect.size(); }
	clTreeVect<T>  *Branch(int nb) const    { return _vect[nb]; }
	T       *AtrTree()           { return _val; }
	clTreeVect<T>  *AddBranch()
	{
		int nb = _vect.size();
		_vect.push_back(new clTreeVect);
		
		_vect[nb]->_parent = this;
		return _vect[nb];
	}
	void    AddBranch(clTreeVect *tree);
	clTreeVect *AddBranchCopy(clTreeVect *tree)
	{
		clTree *ptr=AddBranch();
		*ptr=*tree;
		return ptr;
	}
	void    DelBranch(int nb)
	{
		if(nb>=0 && nb<CnBranch())
		{
			delete _arr[nb];
			for(int i=nb+1;i<CnBranch();i++)
				_arr[i-1]=_arr[i];
			_cnB--;
		}
	}
	void    DelBranch(clTreeVect *ptr)
	{
		for(int i=0;i<CnBranch();i++)
			if(ptr==_arr[i])
			{
				DelBranch(i);
				return;
			}
	}
	clTreeVect &operator = (const clTreeVect &tree)
	{
		ClearAllBranch();
		for(int i=0;i<tree.CnBranch();i++)
		{
			int nb = _vect.size();
			_vect.push_back(new clTreeVect);

			_vect[nb]->_parent = this;
			*_vect[nb] = *tree.Branch(i);
		}
		_weight=tree._weight;
		_name=tree._name;
		*_val=*tree._val;
		return *this;
	}
	int     CnNode()
	{
		int res=1;
		for(int i=0;i<CnBranch();i++)
			res+=_arr[i]->CnNode();
		return res;
	}
	int     CnLastNode()
	{
		if(CnBranch()==0) return 1;
		int res=0;
		for(int i=0;i<CnBranch();i++)
			res+=_arr[i]->CnLastNode();
		return res;
	}
	int ReadFile(int handle)
	{
		ClearAllBranch();
		_name.ReadFile(handle);
		_read(handle,&_weight,sizeof(_weight));
		_val->LoadFromFile(handle);
//		_val->ReadFile(handle);
		int cnB;
		_read(handle, &cnB, sizeof(int));
		for (int i = 0; i<cnB; i++)
		{
			clTreeVect<T> *ptr = AddBranch();
			ptr->ReadFile(handle);
		}
		return 1;
	}
	int WriteFile(int handle)
	{
		_name.WriteFile(handle);
		_write(handle, &_weight, sizeof(_weight));
		_val->SaveToFile(handle);
//		_val-> WriteFile(handle);
		int cnB = _vect.size();
		_write(handle, &cnB, sizeof(int));
		for (int i = 0; i<cnB; i++)
			Branch(i)->WriteFile(handle);
		return 1;
	}
	int SizeInFile()
	{
		int size = _name.SizeInFile();
		size+=sizeof(_weight);
		size += _val->SizeInFile();
		int cnB = _vect.size();
		size += sizeof(cnB);
		for (int i = 0; i<cnB; i++)
			size += Branch(i)->SizeInFile();
		return size;
	}

	//private:
	void    ClearAllBranch()
	{
		for(int i=0;i<CnBranch();i++)
			delete Branch(i);
		_vect.clear();
	}
	void    NormWeight(double w0=1)
	{
		if(_cnB>0)
		{
			double w=0;
			for(int i=0;i<CnBranch();i++)
				w+=Branch(i)->_weight;
			if(w>1.E-10)
				for(int i=0;i<CnBranch();i++)
					Branch(i)->_weight*=w0/w;
			for(int i=0;i<CnBranch();i++)
				Branch(i)->NormWeight(w0);
		}
	}
	double	WeightFromRoot()
	{
		return (_parent == NULL) ? 1 : _weight*_parent->WeightFromRoot();
	}

	std::vector <clTreeVect<T> *> _vect;
	clTreeVect<T> *_parent;
	T *_val;
	clAnsiString _name;
	double _weight;
	int _Tag;
};

//---------------------------------------------------------------------------
#endif
