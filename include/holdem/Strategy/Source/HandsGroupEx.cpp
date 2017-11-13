#include <vector>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>

#include "HandsGroupEX.h"
#include "..\..\..\Util\Sort.h"

//vector <vector <tpBoardHandProperty>> glFlopIndex;
//vector <int> glFlopCount;
//vector <vector <tpK_HandBoard>> glHandToFlopK;

/*bool	LoadFlopIndex();
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	tpParamBoardGames::Include(clBoard &board, clHandValue &hv)
{
	if (board._pairB != _pairB) return false;
	if (board._flushB != _flushB) return false;
	if (board._straightB._straightB != _straightB) return false;

	if (hv.FirstParam() != _firstP) return false;
	if (hv.SecondParam() != _secondP) return false;
	if (hv.FlushDraw() != _param1) return false;
	if (hv.StraightDraw() != _param2) return false;
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*tpBoardHandProperty *GetFirstPropForFlop(int nbFlop)
{ 
	return &glFlopIndex[nbFlop][0]; 
}
//---------------------------------------------------------------------------
int		CnUnitsForFlopNb(int nbFlop)						
{ 
	return glFlopIndex[nbFlop].size();
}*/
//---------------------------------------------------------------------------
void	clHandsGroupExHand::operator = (clHandsGroupEx &gh)
{
	int dim = gh._vect.size();
	if (dim == ALL_CN_HAND)
	{
		_vect.resize(dim);
		for (int i = 0; i < dim; i++)
		{
			_vect[i]._hand.SetHandAll(i);
			_vect[i]._weight = gh[i];
		}
	}
}
//---------------------------------------------------------------------------
void	clHandsGroupEx::FullPreFlop()
{
//	_trade = TRADE_PREFLOP;
	_vect.resize(CN_HAND);
	for (int i = 0; i < CN_HAND; i++)
		_vect[i] = HandWeight(i);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHandsGroupEx::FullPreFlopAllHands()
{
	//_trade = TRADE_PREFLOP;
	_vect.resize(ALL_CN_HAND);
	float val = 1.f / ALL_CN_HAND;
	for (int i = 0; i < ALL_CN_HAND; i++)
		_vect[i] = val;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHandsGroupEx::FullPreFlopAllHands(clCol &deck)
{
	//_trade = TRADE_PREFLOP;
	_vect.resize(ALL_CN_HAND);
	float val = 1.f / ALL_CN_HAND;
	ZeroMemory(&_vect[0], ALL_CN_HAND*sizeof(float));
	for (int i = 0; i < deck.CnCards(); i++)
		for (int j = i + 1; j < deck.CnCards(); j++)
		{
			int nb0 = deck[i].Number();
			int nb1 = deck[j].Number();
			nb0 = (nb0<nb1) ? nb1*(nb1 - 1) / 2 + nb0 : nb0*(nb0 - 1) / 2 + nb1;
			_vect[nb0] = val;
		}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHandsGroupEx::FullPreFlopAllHands(tpBooleanDeck &deck)
{
	//_trade = TRADE_PREFLOP;
	_vect.resize(ALL_CN_HAND);
	int cnAll = deck.CnCard();
	cnAll = cnAll*(cnAll - 1) / 2;
	float val = 1.f / cnAll;
//	ZeroMemory(&_vect[0], ALL_CN_HAND*sizeof(float));
	for (int i = 0; i < CN_CARD_COL; i++)
		for (int j = i + 1; j < CN_CARD_COL; j++)
		{
			int nbH = j*(j - 1)/2 + i;
			_vect[nbH] = (deck._card[i] && deck._card[j]) ? val : 0;
		}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHandsGroupEx::FullPreFlopAllHands(clDeck deck)
{
	deck.ShiftCardToBegin();
	//_trade = TRADE_PREFLOP;
	_vect.resize(ALL_CN_HAND);
	int cnAll = deck.CnCards();
	cnAll = cnAll*(cnAll - 1) / 2;
	float val = 1.f / cnAll;
	ZeroMemory(&_vect[0], ALL_CN_HAND*sizeof(float));
	for (int i = 0; i < deck.CnCards(); i++)
		for (int j = i + 1; j < deck.CnCards(); j++)
		{
			int nb0 = deck[i];
			int nb1 = deck[j];
			nb0 = (nb0<nb1) ? nb1*(nb1 - 1) / 2 + nb0 : nb0*(nb0 - 1) / 2 + nb1;
			_vect[nb0] = val;
		}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
float	clHandsGroupEx::Sum()
{
	float w0 = 0;
	for (std::vector<float>::iterator it = _vect.begin(); it < _vect.end(); it++)
		w0 += *it;
	return w0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
float	clHandsGroupEx::Norm(float w)
{
	float w0 = 0;
	for (std::vector<float>::iterator it = _vect.begin(); it < _vect.end(); it++)
		w0 += *it;
	if (w0 > DOUBLE_0)
	{
		w /= w0;
		for (std::vector<float>::iterator it = _vect.begin(); it < _vect.end(); it++)
			*it*=w;
	}
	return w0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*int		clHandsGroupEx::GetNbHandUr(float ur)
{
	for (std::vector<float>::iterator it = _vect.begin(); it < _vect.end(); it++)
		if (ur < *it)
			return it - _vect.begin();
		else
			ur -= *it;
	for (std::vector<float>::iterator it = _vect.end() - 1; it >= _vect.begin(); it--)
		if (*it>DOUBLE_0)
			return it - _vect.begin();
	return 0;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHandsGroupEx::SkipPreFlopAllHands(tpCard c0)
{
	for (int i = 0; i < CN_CARD_COL;i++)
		if (i != c0)
		{
			int nb = (i>c0) ? i*(i - 1) / 2 + c0 : c0*(c0 - 1) / 2 + i;
			_vect[nb] = 0;
		}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHandsGroupEx::SkipCard(tpCard c0, float val)
{
	for (int i = 0; i < CN_CARD_COL; i++)
		if (i != c0)
		{
			int nb = (i>c0) ? i*(i - 1) / 2 + c0 : c0*(c0 - 1) / 2 + i;
			_vect[nb] = val;
		}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHandsGroupEx::CopyAllHandsFromGH169(vector <float> &gh)
{
	_vect.resize(ALL_CN_HAND);
	for (int i = 0; i < ALL_CN_HAND; i++)
	{
		int nb = HandAllGetNb169(i);
		_vect[i] = gh[nb] / HandCnHand169(nb);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	clHandsGroupEx::Copy169FromAllHands(clHandsGroupEx &gh)
{
	SetDim(CN_HAND);
	ClearDat();
	for (int i = 0; i < ALL_CN_HAND; i++)
	{
		int nb = HandAllGetNb169(i);
		_vect[nb] += gh[i];
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int		clHandsGroupEx::NbMaxWeight()
{
	int dim = _vect.size();
	int nbMax = 0;
	for (int i = 1; i < dim; i++)
		if (_vect[i] > _vect[nbMax])
			nbMax = i;
	return nbMax;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*clAnsiString	IdHandBoardToStr(tpIdHandBoard id)
{
//	char str[16];
//	return clHand::NameHand(id._nbHand, str);
	return "";
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	SaveFileHandsBoard(int handle, vector <vector <tpBoardHandProperty>> &vect)
{
	int dim = vect.size();
	_write(handle, &dim, sizeof(int));
	for (int i = 0; i < dim; i++)
	{
		int dimV = vect[i].size();
		_write(handle, &dimV, sizeof(int));
		if (dimV>0)
			_write(handle, &vect[i][0], dimV*sizeof(tpBoardHandProperty));
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	SaveFileHandsBoard(int handle, vector <vector <tpK_HandBoard>> &vect)
{
	int dim = vect.size();
	_write(handle, &dim, sizeof(int));
	for (int i = 0; i < dim; i++)
	{
		int dimV = vect[i].size();
		_write(handle, &dimV, sizeof(int));
		if (dimV>0)
			_write(handle, &vect[i][0], dimV*sizeof(tpK_HandBoard));
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*bool	LoadFlopIndex()
{
	int handle = _open(FILE_FLOP_ID, O_RDWR | O_BINARY);
	if (handle == -1) return false;

	int dim;
	_read(handle, &dim, sizeof(int));
	glFlopIndex.resize(dim);
	glFlopCount.resize(dim);
	int cn0 = 0;
	for (int i = 0; i < dim; i++)
	{
		int dimV;
		_read(handle, &dimV, sizeof(int));
		glFlopIndex[i].resize(dimV);
		glFlopCount[i] = cn0;
		cn0 += dimV;
		if (dimV > 0)
			_read(handle, &glFlopIndex[i][0], dimV*sizeof(tpBoardHandProperty));
	}
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*bool	LoadHandsToFlop()
{
	int handle = _open(FILE_HAND_FLOP, O_RDWR | O_BINARY);
	if (handle == -1) return false;

	int dim;
	_read(handle, &dim, sizeof(int));
	glHandToFlopK.resize(dim);
	for (int i = 0; i < dim; i++)
	{
		int dimV;
		_read(handle, &dimV, sizeof(int));
		glHandToFlopK[i].resize(dimV);
		if (dimV > 0)
			_read(handle, &glHandToFlopK[i][0], dimV*sizeof(tpK_HandBoard));
	}
	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
bool	LoadDataHandsBoard()
{
	if (!LoadFlopIndex())
		return false;
	if (!LoadHandsToFlop())
		return false;
	return true;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*clAnsiString NaborisheToString(int nb)
{
	clAnsiString as;
	stNBSHProperty &pr = glNBSHVect[nb];
	for (int i = 0; i < pr._cnCard; i++)
		as += pr._cards[i].Name();
	return as;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
