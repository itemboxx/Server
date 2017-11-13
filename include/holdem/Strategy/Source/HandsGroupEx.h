//---------------------------------------------------------------------------

#ifndef HandsGroupEX_H
#define HandsGroupEX_H
//---------------------------------------------------------------------------
#include <vector>
#include "..\..\Common\sitHoldem.h"
#include "..\..\..\util\clTree.h"
#include "..\..\Common\BoardGames.h"
#include "..\..\..\card\naborCards.h"
#include "..\..\..\Util\TemplateFileRec.h"
using namespace std;

//#define FILE_FLOP_ID	"c:\\home\\bin\\flopId1.bin"
//#define FILE_HAND_FLOP	"c:\\home\\bin\\HandToFlopK.bin"
//#define CN_NABOR_IN_FLOP		553800

struct tpIdHandBoard
{

	int   _idBoard,_idHandBoard;
};

struct tpHandId
{
	int _idHB;
	clHand _hand;
};

class clHandsGroupEx
{
public:
	clHandsGroupEx() {}
	clHandsGroupEx(int dim) { _vect.resize(dim); }
	size_t	SizeInFile() { return SizeVectFile(_vect); }
	void	WriteFile(int handle)	{ WriteVectFile(handle, _vect); }
	void	ReadFile(int handle)	{ ReadVectFile(handle, _vect); }
	void	SetDim(size_t dim) { _vect.resize(dim); }
	size_t	Dimension() { return _vect.size(); }
	void	FullPreFlop();
	void	FullPreFlopAllHands();
	void	FullPreFlopAllHands(clCol &deck);
	void	FullPreFlopAllHands(tpBooleanDeck &deck);
	void	FullPreFlopAllHands(clDeck deck);
	void	ReadFileAllHand(int handle)					{ _vect.resize(ALL_CN_HAND); _read(handle, &_vect[0], ALL_CN_HAND*sizeof(float)); }
	void	WriteFileAllHand(int handle)				{ _write(handle, &_vect[0], ALL_CN_HAND*sizeof(float)); }
	float	&Weight(int nb)			{ return _vect[nb]; }
	float	Sum();
	float	Norm(float w = 1);
	//void	CreateNbAllHand(clHand hand);
	float &operator [] (int nb)		{ return _vect[nb]; }
	int		GetNbHandUr(tpFloat ur) { return 0; }
	void	SkipPreFlopAllHands(tpCard c0);
	void	SkipPreFlopAllHands(clCard c0)				{ SkipPreFlopAllHands((tpCard)c0); }
	void	SkipCard(tpCard c0) { SkipPreFlopAllHands(c0); }
	void	SkipCard(tpCard c0, float val);
	void	ClearDat() { ZeroMemory(&_vect[0], Dimension() * sizeof(float)); }
	void	CopyAllHandsFromGH169(vector <float> &vect);
	void	Copy169FromAllHands(clHandsGroupEx &gh);
	void	CopyAllHandsFromGH169(clHandsGroupEx &gh) { CopyAllHandsFromGH169(gh._vect); }
	int		NbMaxWeight();

//	enNameTrade _trade;
	std::vector<float>	_vect; 
};

struct stHandWeight
{
	tpHand _hand;
	float _weight;
};
class clHandsGroupExHand
{
public:
	void	operator = (clHandsGroupEx &gh);

	std::vector<stHandWeight>	_vect;
};

/*struct tpParamBoardGames
{
	bool	Include(clBoard &board, clHandValue &hv);
	tpParamBoardGames()
	{
		_firstP = PHV1_UNDEF;
		_secondP = PHV2_UNDEF;
		_param1 = DPHV_UNDEF;
		_param2 = DPHV2_UNDEF;
		_pairB = DGP_NO;
		_flushB = DGF_NO;
		_straightB = DGS_NO;
		_cnUnits = 0;
	}

	tpParamBoardGames(clBoard &board, clHandValue &hv)
	{
		_firstP = hv.FirstParam();
		_secondP = hv.SecondParam();
		_param1 = hv.FlushDraw();
		_param2 = hv.StraightDraw();
		_pairB = board._pairB;
		_flushB = board._flushB;
		_straightB = board._straightB._straightB;
		_cnUnits = 1;
	}

	enParamHV_1 _firstP;
	enParamHV_2 _secondP;
	enFlushDrawParamHV_1 _param1;
	enStraightDrawParamHV_2 _param2;
	enDGPairBoard  _pairB;
	enDGFlushBoard  _flushB;
	enDGStraightBoard _straightB;
	int _cnUnits;
};*/

//extern  vector <vector <tpBoardHandProperty>> glFlopIndex;		// размерность равна количеству флопов, элемент вектор в котором идентификаторы 5-карточных наборов по возрастанию для данного флопа и Hand игрока
//extern  vector <int> glFlopCount;				// вектор в котором записано сумма элементов glFlopIndex, в 0-0, в 1 glFlopIndex[0].size, во 2 glFlopIndex[0].size+glFlopIndex[1].size 

//inline	int	NbFlopFromId(int id)		{ return glNBSHArray[id] - NBSH_CN_WITH_ALL_2; }
//clAnsiString	IdHandBoardToStr(tpIdHandBoard id);
//void	SaveFileHandsBoard(int handle, vector <vector <tpBoardHandProperty>> &vect);
//void	SaveFileHandsBoard(int handle, vector <vector <tpK_HandBoard>> &vect);
//bool	LoadDataHandsBoard();
//clAnsiString NameFlopBoard(int nb);
//clAnsiString NaborisheToString(int nb);
//int		GetFlopIndex(clCard *arr);		// массив из 5 карт
//int		CnFlop();
//int		CnUnitsForFlopNb(int nbFlop);
//tpBoardHandProperty *GetFirstPropForFlop(int nbFlop);
//---------------------------------------------------------------------------
#endif
