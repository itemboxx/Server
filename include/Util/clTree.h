//---------------------------------------------------------------------------

#ifndef clTreeH
#define clTreeH
//---------------------------------------------------------------------------
typedef void * (*tpPtrFuncTree)(void *ptr);
typedef void  (*tpPtrFuncCopyTree)(void *dest,void *src);

#include <io.h>
#include "c:\home\Util\WorkStr.h"

#define MAX_CN_BRANCH   8

struct stShortPoint
{
    short _x,_y;
};
//---------------------------------------------------------------------------
template<class T> class clTree
{
public:
clTree()
{
    _weight=0;
    _parent=NULL;
    _cnB=0;
    _val=new T;
}
clTree(const clTree<T> &tree)
{
	_weight = 0;
	_parent = tree._parent;
	_cnB = 0;
	_val = new T;
	*this = tree;
} 
~clTree()
{
    ClearAllBranch();
    delete(_val);
}
    clTree  *Parent()           {return _parent;}
    int CnBranch() const				{return _cnB;}
    clTree<T>  *Branch(int nb) const    {return _arr[nb];}
clTree  *Root()
{ 
	clTree  *node = this;
	while (node->_parent != NULL)
		node = node->_parent;
	return node;
}
clTree<T>  *AddBranch()
{
    _arr[_cnB]=new clTree();
    _arr[_cnB]->_parent=this;
    _cnB++;
    return _arr[_cnB-1];
}
    void    AddBranch(clTree *tree);
clTree *AddBranchCopy(clTree *tree)
{
    clTree *ptr=AddBranch();
    *ptr=*tree;
    return ptr;
}
void MoveTree(clTree *tree)
{
	_weight = tree->_weight;
	_parent = NULL;
	_cnB = tree->_cnB;
	for (int i = 0; i < _cnB; i++)
		_arr[i] = tree->_arr[i];
	_val = tree->_val;

	tree->_weight = 0;
	tree->_cnB = 0;
	tree->_val = new T;
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
void    DelBranch(clTree *ptr)
{
    for(int i=0;i<CnBranch();i++)
        if(ptr==_arr[i])
        {
            DelBranch(i);
            return;
        }
}
    T       *AtrTree()           {return _val;}
clTree &operator = (const clTree &tree)
{
    ClearAllBranch();
    for(int i=0;i<tree.CnBranch();i++)
    {
        _arr[_cnB]=new clTree();
        _arr[_cnB]->_parent=this;
        *_arr[_cnB]=*tree.Branch(i);
        _cnB++;
    }
    _weight=tree._weight;
    _name=tree._name;
    *_val=*tree._val;
    return *this;
}
int     CnNode()
{
	int res = 1;
	for (int i = 0; i<CnBranch(); i++)
		res += _arr[i]->CnNode();
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
int     CnNode(int (*f)(T *))
{
	int res = f(_val);
	for (int i = 0; i<CnBranch(); i++)
		res += _arr[i]->CnNode(f);
	return res;
}
int ReadFile(int handle)
{
    ClearAllBranch();
	_val->Clear();
    int val;
    _name.ReadFile(handle);
    _read(handle,&_weight,sizeof(_weight));
    _val->ReadFile(handle);
    _read(handle,&val,sizeof(_cnB));
    for(int i=0;i<val;i++)
    {
        clTree<T> *ptr=AddBranch();
        ptr->ReadFile(handle);
    }
    return 1;
}
int WriteFile(int handle)
{
	_name.WriteFile(handle);
	_write(handle, &_weight, sizeof(_weight));
	_val->WriteFile(handle);
	_write(handle, &_cnB, sizeof(_cnB));
	for (int i = 0; i<_cnB; i++)
		Branch(i)->WriteFile(handle);
	return 1;
}
size_t SizeInFile()
{
	size_t size = _name.SizeInFile();
	size+=sizeof(_weight);
	size += _val->SizeInFile();
	size+=sizeof(_cnB);
	for (int i = 0; i<_cnB; i++)
		size += Branch(i)->SizeInFile();
	return size;
}

    clAnsiString _name; 
    double _weight;
    int _Tag;
//private:
void    ClearAllBranch()
{
	for (int i = 0; i<CnBranch(); i++)
		delete Branch(i);
	_cnB = 0;
}
void    ClearAllBranchExclude(clTree<T>  *node)
{
	for (int i = 0; i<CnBranch(); i++)
		if(Branch(i) != node)
			delete Branch(i);
	_cnB = 1;
	_arr[0] = node;
}
void    NormWeight(double w0 = 1)
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
void ClearWeight()
{
	_weight = 0;
	for (int i = 0; i<CnBranch(); i++)
		Branch(i)->ClearWeight();
}

    clTree<T> *_arr[MAX_CN_BRANCH],*_parent;
    int _cnB;
    T *_val;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template<class T> class clBinTree
{
public:
	clBinTree()
	{
		_parent = _left = _right = NULL;
		_val = new T;
	}
	clBinTree(clBinTree &tree)
	{
		_parent = _left = _right = NULL;
		_val = new T;
		*this = tree;
	}
	~clBinTree()
	{
		ClearAllBranch();
		delete(_val);
	}
	void	Clear()						{ ClearAllBranch(); _val->Clear(); }
	clBinTree<T>	*Parent()           { return _parent; }
	clBinTree<T>	*Left() const    { return _left; }
	clBinTree<T>	*Right() const    { return _right; }
	clBinTree<T>	*AddLeftBranch()
	{
		_left = new clBinTree < T > ;
		_left->_parent = this;
		return _left;
	}
	clBinTree<T>	*AddRightBranch()
	{
		_right = new clBinTree < T >;
		_right->_parent = this;
		return _right;
	}
	void	ClearAllBranch()
	{
		if (_left != NULL)
			delete _left;
		if (_right != NULL)
			delete _right;
	}
	clBinTree &operator = (const clBinTree &tree)
	{
		ClearAllBranch();
		if (tree._left != NULL)
		{
			_left = new clBinTree < T > ;
			_left->_parent = this;
			*_left = *tree._left;
		}
		if (tree._right != NULL)
		{
			_right = new clBinTree < T > ;
			_right->_parent = this;
			*_right = *tree._right;
		}
		*_val = *tree._val;
		return *this;
	}

	int ReadFile(int handle)
	{
		ClearAllBranch();
		_val->Clear();
		_val->ReadFile(handle);
		byte c;
		_read(handle, &c, 1);
		if (c)
		{
			AddLeftBranch();
			_left->ReadFile(handle);
		}
		_read(handle, &c, 1);
		if (c)
		{
			AddRightBranch();
			_right->ReadFile(handle);
		}
		return 0;
	}
	int WriteFile(int handle)
	{
		_val->WriteFile(handle);
		byte c;
		if (_left != NULL)
		{
			c = 1;
			_write(handle, &c, 1);
			_left->WriteFile(handle);
		}
		else
		{
			c = 0;
			_write(handle, &c, 1);
		}
		if (_right != NULL)
		{
			c = 1;
			_write(handle, &c, 1);
			_right->WriteFile(handle);
		}
		else
		{
			c = 0;
			_write(handle, &c, 1);
		}
		return 0;
	}
	bool	EndedNode() { return _left == NULL && _right == NULL; }
	clBinTree<T> *_left, *_right, *_parent;
	T *_val;
};
//---------------------------------------------------------------------------
#endif
