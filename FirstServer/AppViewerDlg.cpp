// AppViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FirstServer.h"
#include "AppViewerDlg.h"
#include "afxdialogex.h"

#include "clValidateServer.h"

#include "..\include\Util\\errMessage.h"
#include "..\include\Util\FileRec.h"
//#include "C:\home\holdem\Strategy\Source\CalcSitEV.h"
//#include "C:\home\holdem\Common\GlobalsForCalc.h"

#define REG_VIEW		0
#define REG_PLAY		1

// CAppViewerDlg dialog

IMPLEMENT_DYNAMIC(CAppViewerDlg, CDialogEx)

CAppViewerDlg::CAppViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAppViewerDlg::IDD, pParent)
{
	_tag = -1;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CAppViewerDlg::~CAppViewerDlg()
{
}

void CAppViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAppViewerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_FILE_LOAD, &CAppViewerDlg::OnFileLoad)
	ON_COMMAND(ID_FILE_SAVE32772, &CAppViewerDlg::OnFileSave32772)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON2, &CAppViewerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CAppViewerDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON3, &CAppViewerDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON5, &CAppViewerDlg::OnBnClickedButton5)
	ON_COMMAND(ID_VIEWCARDS_PLAYER0, &CAppViewerDlg::OnViewcardsPlayer0)
	ON_COMMAND(ID_VIEWCARDS_PLAYER1, &CAppViewerDlg::OnViewcardsPlayer1)
	ON_COMMAND(ID_VIEWCARDS_PLAYER2, &CAppViewerDlg::OnViewcardsPlayer2)
	ON_COMMAND(ID_VIEWCARDS_PLAYER3, &CAppViewerDlg::OnViewcardsPlayer3)
	ON_COMMAND(ID_VIEWCARDS_PLAYER4, &CAppViewerDlg::OnViewcardsPlayer4)
	ON_COMMAND(ID_VIEWCARDS_PLAYER5, &CAppViewerDlg::OnViewcardsPlayer5)
	ON_BN_CLICKED(IDOK, &CAppViewerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAppViewerDlg::OnBnClickedCancel)
	ON_COMMAND(ID_REGIM_VIEW, &CAppViewerDlg::OnRegimView)
	ON_COMMAND(ID_REGIM_PLAY, &CAppViewerDlg::OnRegimPlay)
	ON_BN_CLICKED(IDC_BTN_FOLD, &CAppViewerDlg::OnBnClickedBtnFold)
	ON_BN_CLICKED(IDC_BTN_CALL, &CAppViewerDlg::OnBnClickedBtnCall)
	ON_BN_CLICKED(IDC_BTN_ALLIN, &CAppViewerDlg::OnBnClickedBtnAllin)
	ON_BN_CLICKED(IDC_BTN_RAISE, &CAppViewerDlg::OnBnClickedBtnRaise)
	ON_COMMAND(ID_FILE_LOADSTRAT, &CAppViewerDlg::OnFileLoadstrat)
	ON_BN_CLICKED(IDC_BUTTON1, &CAppViewerDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CAppViewerDlg message handlers
#define	RADIUS					300
#define     R_POT               20
#define M_PI       3.14159265358979323846
#define     PLAYER_INFO_WIDTH	100
#define     PLAYER_INFO_HEIGHT	100
#define     CLOSE_CARD_X		10
#define     CLOSE_CARD_Y		35
#define     SHIFT_BET_R			150
#define     SHIFT_BUTTON_R		180

const CPoint	CENTER = CPoint(RADIUS, RADIUS);
// CAppViewerDlg message handlers


void CAppViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void	CAppViewerDlg::Clear()
{
	_tag = -1;
}
void	CAppViewerDlg::BuildAndDraw(tpScanTable table)
{
	ScanTableBuildHistory(&_list, &table);
	if (_list.CnGame() > 0)
	{
		_sit.Clear();
		_sit.Build(_list.LastGameHist(), _list.LastGameHist()->CnAct());
		if (_sit.Indicator() != -1 && _sit.ViewPlayer() == _sit.Indicator()) // for calc
		{

		}
//		enNbError nbErr=glVSServer.Add(_sit);
		Invalidate();
	}



}
bool	CAppViewerDlg::NeedToSaveSit()
{
	return (_sit.Indicator() != -1 && _sit.ViewPlayer() == _sit.Indicator()); // for calc

}
void CAppViewerDlg::OnPaint()
{
	CPaintDC dc(this);
	CBitmap *old = this->_dcMainBuffer->GetCurrentBitmap();
	DrawSit(this->_dcMainBuffer);
	dc.BitBlt(0, 0, 1000, 1000, _dcMainBuffer, 0, 0, SRCCOPY);
	CBitmap *reciveBmp = this->_dcMainBuffer->SelectObject(old);
	reciveBmp->DeleteObject();
	delete reciveBmp;
	ViewButtonTrade();
	/*	if (IsIconic())
	{
	CPaintDC dc(this); // device context for painting

	SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

	// Center icon in client rectangle
	int cxIcon = GetSystemMetrics(SM_CXICON);
	int cyIcon = GetSystemMetrics(SM_CYICON);
	CRect rect;
	GetClientRect(&rect);
	int x = (rect.Width() - cxIcon + 1) / 2;
	int y = (rect.Height() - cyIcon + 1) / 2;

	// Draw the icon
	dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
	CDialogEx::OnPaint();
	}*/
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAppViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnFileLoad()
{
	CFileDialog fileD(true, NULL, (LPCTSTR) "*.bin");
	INT_PTR res = fileD.DoModal();
	if (res == IDOK)
	{
		CStringA ss;
		ss = fileD.GetPathName();
		clFileConstRec file;
		if (file.OpenFile(ss.GetBuffer()))
		{
			_vectH.resize(file.CnRecord());
			for (int i = 0; i < _vectH.size(); i++)
				file.ReadRecord(i, &_vectH[i]);
		}
		else
		{
			ErrMessage(ss.GetBuffer(), "Не открывается файл");
			return;
		}
		_reg = REG_VIEW;
		DisabledButtonTrade();
		clAnsiString as = (clAnsiString)_vectH.size() + (clAnsiString)" сдач";
		SetDlgItemText(IDC_STATIC1, as.c_str());
		as = (clAnsiString)" Текущая ";
		SetDlgItemText(IDC_STATIC3, as.c_str());
		_nbCurHand = 0; _nbCurAct = 0;
		BuildSit();
		as = _nbCurHand;
		SetDlgItemText(IDC_EDIT1, as.c_str());
		Invalidate();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnFileSave32772()
{
/*	CFileDialog fileD(false, NULL, (LPCTSTR) "*.strat");
	INT_PTR res = fileD.DoModal();
	if (res == IDOK)
	{
		CStringA ss;
		ss = fileD.GetPathName();
		clFileConstRec file;
		if (file.Create(ss.GetBuffer(), sizeof(clGameHistory)))
		{
			for (int i = 0; i < _vectH.size(); i++)
				file.AddRecord(&_vectH[i]);
		}
		else
		{
			ErrMessage(ss.GetBuffer(), "Не открывается файл");
			return;
		}
	}
*/
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int CAppViewerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

//	GlobalsForCalcLoad();

	CDC *dc = this->GetDC();
	this->_dcMainBuffer = new CDC;
	this->_dcMainBuffer->CreateCompatibleDC(dc);
	this->ReleaseDC(dc);

	CString strPrefix = "c:\\work\\Bin\\Images\\";
	CString path;
	path = strPrefix + "smallcard1.bmp";

	HBITMAP hBitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	this->_bmpUndefCard = new CBitmap();
	this->_bmpUndefCard->Attach(hBitmap);
	hBitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), "c:\\work\\Bin\\Images\\Button27.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	this->_bmpButton = new CBitmap();
	this->_bmpButton->Attach(hBitmap);

	for (int i = 0; i < CN_CARD_COL; i++)
	{
		char name[4];
		clCard card = CardNB(i);
		card.Name(name);
		name[2] = '\0';
		path = strPrefix + name + ".bmp";
		hBitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		this->_bmpCards[i] = new CBitmap();
		this->_bmpCards[i]->Detach();
		this->_bmpCards[i]->Attach(hBitmap);
	}
	_sit.Clear();
	_viewCards[0] = _viewCards[1] = _viewCards[2] = _viewCards[3] = _viewCards[4] = _viewCards[5] = false;

	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
CPoint CoordinateCreation(CPoint center, double angle, int R)
{
	CPoint point;
	point.x = -(int)(R * cos(angle)) + center.x;
	point.y = -(int)(R * sin(angle)) + center.y;
	return point;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CAppViewerDlg::DrawSit(CDC *dc)
{
	CBitmap *newBmp = new CBitmap();
	int edge = 2 * RADIUS;
	CRect winRect, clientRect;
	this->GetWindowRect(&winRect);
	this->GetClientRect(&clientRect);
	int xBorder = winRect.Width() - clientRect.Width();
	int yBorder = winRect.Height() - clientRect.Height();
	//	this->SetWindowPos(&wndTop, 0, 0, edge + xBorder, edge + yBorder, SWP_NOMOVE);
	newBmp->CreateBitmap(edge, edge, 1, 32, 0);
	dc->SelectObject(newBmp);
	COLORREF	bgColor = RGB(215, 218, 208);
	dc->FillSolidRect(0, 0, edge, edge, bgColor);

	if (_sit.CnPlayer() == 0)
		return;

	CRgn potRgn;
	CPoint pPot = CENTER;
	pPot.y -= R_POT;
	potRgn.CreateEllipticRgn(pPot.x - R_POT, pPot.y - R_POT, pPot.x + R_POT, pPot.y + R_POT);
	CBrush potBrush;
	COLORREF  colr = RGB(255, 251, 240);
	potBrush.CreateSolidBrush(colr);
	dc->FillRgn(&potRgn, &potBrush);
	clAnsiString strPot = _sit.TotalPot();
	CSize potSize = dc->GetTextExtent(strPot.c_str());
	pPot.x -= potSize.cx / 2;
	pPot.y -= potSize.cy / 2;
	dc->SetBkMode(TRANSPARENT);
	dc->TextOutA(pPot.x, pPot.y, strPot.c_str()); // надпись на поте

	int maxCn = _sit.MaxCnSeat();
	CDC tmpDC;
	tmpDC.CreateCompatibleDC(dc);
	CBitmap *curBmp = new CBitmap();
	curBmp->CreateBitmap(PLAYER_INFO_WIDTH, PLAYER_INFO_HEIGHT, 1, 32, 0);
	tmpDC.SelectObject(curBmp);
	for (int i0 = 0; i0 < _sit.CnPlayer(); i0++)
		if (_sit.IsPlayer(i0))
		{
			int nb = _sit.PositionToNbTable(i0);
			double angle = 2 * M_PI * nb / maxCn;

			DrawPlayer(&tmpDC, i0);
			CPoint point = CoordinateCreation(CENTER, angle, RADIUS - PLAYER_INFO_WIDTH*.8);
			dc->BitBlt(point.x - PLAYER_INFO_WIDTH / 2, point.y - PLAYER_INFO_HEIGHT / 2, PLAYER_INFO_WIDTH, PLAYER_INFO_HEIGHT, &tmpDC, 0, 0, SRCCOPY);

			if (_sit.PlayerBank(i0) != 0)
			{
				point = CoordinateCreation(CENTER, angle, RADIUS - SHIFT_BET_R);
				char buf[24];
				_itoa(_sit.PlayerBank(i0), buf, 10);
				CSize potSize = dc->GetTextExtent(buf);
				point.x -= potSize.cx / 2;
				point.y -= potSize.cy / 2;
				dc->TextOutA(point.x, point.y, buf);
			}
			if (_sit.NbButton() == nb)
			{
				point = CoordinateCreation(CENTER, angle, RADIUS - SHIFT_BUTTON_R);
				BITMAP bitmap;
				_bmpButton->GetBitmap(&bitmap);
				point.x -= bitmap.bmWidth / 2;
				point.y -= bitmap.bmHeight / 2;
				//				point.x = point.y = 0;
				dc->DrawState(point, CSize(bitmap.bmWidth, bitmap.bmHeight), this->_bmpButton, DST_BITMAP);
			}
		}
	if (_sit.CnCardTable() > 0)
	{
		CPoint pC = CENTER;
		BITMAP bitmap;
		_bmpCards[0]->GetBitmap(&bitmap);
		pC.x -= 2.5*bitmap.bmWidth + 10;
		for (int i = 0; i < _sit.CnCardTable(); i++)
		{
			int nbC = _sit.CardsTable()[i].Number();
			dc->DrawState(pC, CSize(bitmap.bmWidth, bitmap.bmHeight), _bmpCards[nbC], DST_BITMAP);
			pC.x += bitmap.bmWidth + 5;
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CAppViewerDlg::DrawPlayer(CDC *dc, int nb)
{
	CRect rect;
	rect.SetRect(0, 0, PLAYER_INFO_WIDTH, PLAYER_INFO_HEIGHT);
	CBrush potBrush;
	COLORREF  colr = (_sit.Indicator() == nb) ? RGB(255, 100, 100) : RGB(255, 255, 255);
	potBrush.CreateSolidBrush(colr);
	dc->FillRect(&rect, &potBrush);
	dc->SetBkMode(TRANSPARENT);
	dc->TextOutA(10, 1, _sit.PlayerName(nb));
	char buf[16];
	dc->TextOutA(10, 15, _itoa(_sit.PlayerMoney(nb), buf, 10));
	if (_sit.PlayerIsCard(nb))
	{
		CDC tmpDC;
		tmpDC.CreateCompatibleDC(dc);
		CBitmap *old = tmpDC.GetCurrentBitmap();
		if (_sit.PlayerViewCard(nb))
		{
			BITMAP temp;
			_bmpCards[0]->GetBitmap(&temp);
			int nbC = _sit.PlayerCards(nb)[0].Number();
			tmpDC.SelectObject(_bmpCards[nbC]);
			dc->BitBlt(CLOSE_CARD_X, CLOSE_CARD_Y, temp.bmWidth, temp.bmHeight, &tmpDC, 0, 0, SRCCOPY);
			nbC = _sit.PlayerCards(nb)[1].Number();
			tmpDC.SelectObject(_bmpCards[nbC]);
			dc->BitBlt(CLOSE_CARD_X + 15, CLOSE_CARD_Y + 5, temp.bmWidth, temp.bmHeight, &tmpDC, 0, 0, SRCCOPY);
		}
		else
		{
			BITMAP temp;
			_bmpUndefCard->GetBitmap(&temp);
			tmpDC.SelectObject(_bmpUndefCard);
			dc->BitBlt(CLOSE_CARD_X, CLOSE_CARD_Y, temp.bmWidth, temp.bmHeight, &tmpDC, 0, 0, SRCCOPY);
			dc->BitBlt(CLOSE_CARD_X + 15, CLOSE_CARD_Y + 5, temp.bmWidth, temp.bmHeight, &tmpDC, 0, 0, SRCCOPY);
		}
		//		CBitmap *reciveBmp = this->_dcMainBuffer->SelectObject(old);
		//		reciveBmp->DeleteObject();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
int NextPlayerSeat(int nb, tpSeat *seat)
{
	for (int i = 0; i < CN_PLAYER; i++)
	{
		if (++nb >= CN_PLAYER)
			nb = 0;
		if (seat[nb]._inPlay)
			return nb;
	}
	return nb;
}
int PrevPlayerSeat(int nb, tpSeat *seat)
{
	for (int i = 0; i < CN_PLAYER; i++)
	{
		if (--nb <0)
			nb = CN_PLAYER - 1;
		if (seat[nb]._inPlay)
			return nb;
	}
	return nb;
}
void	CreateNewGame(clSitHoldem &sit, clCol &col, tpSeat *seat, int bb, int nbBut)
{
	int cnPl = 0;
	for (int i = 0; i < CN_PLAYER; i++)
		if (seat[i]._inPlay)
			cnPl++;
	int nbCur = NextPlayerSeat(nbBut, seat);
	if (cnPl>2)
		nbCur = NextPlayerSeat(nbCur, seat);
	clStacks st; st = 0; st._bb = bb; st._ante = 0;
	int nbBtn = nbCur;
	st[0] = seat[nbCur]._money;
	for (int i = 1; i < cnPl; i++)
	{
		nbCur = PrevPlayerSeat(nbCur, seat);
		st[i] = seat[nbCur]._money;
	}
	sit.Init(st);

	nbCur = nbBtn;
	strcpy(sit._seat[0]._name, seat[nbCur]._name);
	sit.SetNbTable(0, nbCur);
	for (int i = 1; i < cnPl; i++)
	{
		nbCur = PrevPlayerSeat(nbCur, seat);
		strcpy(sit._seat[i]._name, seat[nbCur]._name);
		sit.SetNbTable(i, nbCur);
	}
	sit.DealCards(col);
	for (int i = 0; i < CN_PLAYER; i++)
		if (seat[i]._inPlay && seat[i]._busy)
			sit.PlayerAtr(sit.NbToPosition(i))->_viewCards = true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnBnClickedButton2()
{
	if (_reg == REG_VIEW)
	{
		if (_nbCurHand + 1 < _vectH.size())
		{
			_nbCurHand++;
			_nbCurAct = 0;
			BuildSit();
			clAnsiString as = _nbCurHand;
			SetDlgItemText(IDC_EDIT1, as.c_str());
			Invalidate();
		}
	}
	else
	{
		_allRes += _curRes;
		clAnsiString as = (clAnsiString)"AllRes=" + (clAnsiString)_allRes[0];
		SetDlgItemText(IDC_STATIC, as.c_str());
		_col.New();
		_col.Shufle();
		_col.Shufle();
		int nb = NextPlayerSeat(_sit.NbButton(), _seat);
		CreateNewGame(_sit, _col, _seat, 100, nb);
		_curRes = 0;

		int ind = _sit.Indicator();
		if (!_sit.PlayerViewCard(ind))
			OnBnClickedButton1();

		ViewButtonTrade();
		Invalidate();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnBnClickedButton4()
{
	if (_nbCurHand>0)
	{
		_nbCurHand--;
		_nbCurAct = 0;
		BuildSit();
		clAnsiString as = _nbCurHand;
		SetDlgItemText(IDC_EDIT1, as.c_str());
		Invalidate();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnBnClickedButton3()
{
	if (_nbCurAct<_vectH[_nbCurHand].CnAct())
	{
		_nbCurAct++;
		BuildSit();
		Invalidate();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnBnClickedButton5()
{
	if (_nbCurAct >0)
	{
		_nbCurAct--;
		BuildSit();
		Invalidate();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnViewcardsPlayer0()
{
	_viewCards[0] = !_viewCards[0];
	CMenu* cm = GetMenu();
	cm->CheckMenuItem(ID_VIEWCARDS_PLAYER0, (_viewCards[0]) ? MF_CHECKED : MF_UNCHECKED);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnViewcardsPlayer1()
{
	_viewCards[1] = !_viewCards[1];
	CMenu* cm = GetMenu();
	cm->CheckMenuItem(ID_VIEWCARDS_PLAYER1, (_viewCards[1]) ? MF_CHECKED : MF_UNCHECKED);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnViewcardsPlayer2()
{
	_viewCards[2] = !_viewCards[2];
	CMenu* cm = GetMenu();
	cm->CheckMenuItem(ID_VIEWCARDS_PLAYER2, (_viewCards[2]) ? MF_CHECKED : MF_UNCHECKED);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnViewcardsPlayer3()
{
	_viewCards[3] = !_viewCards[3];
	CMenu* cm = GetMenu();
	cm->CheckMenuItem(ID_VIEWCARDS_PLAYER3, (_viewCards[3]) ? MF_CHECKED : MF_UNCHECKED);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnViewcardsPlayer4()
{
	_viewCards[4] = !_viewCards[4];
	CMenu* cm = GetMenu();
	cm->CheckMenuItem(ID_VIEWCARDS_PLAYER4, (_viewCards[4]) ? MF_CHECKED : MF_UNCHECKED);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnViewcardsPlayer5()
{
	_viewCards[5] = !_viewCards[5];
	CMenu* cm = GetMenu();
	cm->CheckMenuItem(ID_VIEWCARDS_PLAYER5, (_viewCards[5]) ? MF_CHECKED : MF_UNCHECKED);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*clGamePoker PokerWin(clGamePoker *gamePoker)
{
clGamePoker game = gamePoker[0];
for (int i = 1; i<CN_PLAYER; i++)
{
if (game<gamePoker[i])
game = gamePoker[i];
}
return(game);

}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void DistribPot(clSitHoldem *table, clGamePoker *gamePoker)
{
bool win[CN_PLAYER];
int colViewCards = table->CNPlayerIsCard();
int totalBetPl[CN_PLAYER];
//    int rake=0;
for (int i = 0; i<CN_PLAYER; i++)
{
win[i] = false;
totalBetPl[i] = table->TotalBetPl(i);
}
clGamePoker minGame = GamePokerMinGame();
while (colViewCards>0)
{
int n = 0, bet = -1;
clGamePoker comb;
comb = PokerWin(gamePoker);
for (int i = 0; i<CN_PLAYER; i++)
{
if (table->PlayerIsCard(i) && comb == gamePoker[i])
{
win[i] = true;
n++;
if (bet == -1)
{
bet = totalBetPl[i];
}
else if (bet>totalBetPl[i])
{
bet = totalBetPl[i];
}
}
}
int totalB = 0;
for (int i = 0; i<CN_PLAYER; i++)
{
if (totalBetPl[i] <= bet)
{
totalB += totalBetPl[i];
totalBetPl[i] = 0;
}
else
{
totalB += bet;
totalBetPl[i] -= bet;
}
}
int ost = 0;

if (n == 0)
{
ErrMessage("Деление на 0", "Ошибка");
n = 1;
}

ost = totalB%n;
for (int i = 0; i<CN_PLAYER; i++)
if (win[i])
{
table->ExecAct(HistAction(i, ACT_WIN, totalB / n + ost));
ost = 0;
}
for (int i = 0; i<CN_PLAYER; i++)
{
if (totalBetPl[i] == 0)
{
if (!(gamePoker[i] == minGame))
{
win[i] = false;
colViewCards--;
gamePoker[i] = minGame;
}
}
}
}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP CalcShowDown(clSitHoldem &sit)
{
clDoubleCP money;
for (int i = 0; i < CN_PLAYER; i++)
money[i] = (i<sit.CnPlayer()) ? sit.PlMoney(i) : 0;
clGamePoker gamePoker[CN_PLAYER];
clGamePoker minGame = GamePokerMinGame();
for (int i = 0; i<CN_PLAYER; i++)
{
if (sit.PlayerIsCard(i))
gamePoker[i] = sit.GamePlayerHoldem(i);
else
gamePoker[i] = minGame;
}
DistribPot(&sit, gamePoker);
for (int i = 0; i < sit.CnPlayer(); i++)
money[i] = sit.PlayerMoney(i) - money[i];
return money;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
clDoubleCP CalcSitDistribWin(clSitHoldem &sit)
{
clDoubleCP res;
if (sit.CNPlayerIsCard() == 1)
{
res = 0;
for (int i = 0; i < sit.CnPlayer(); i++)
res[i] = sit.PlMoney(i);
sit.CloseBank();
for (int i = 0; i < sit.CnPlayer(); i++)
{
if (sit.PlayerIsCard(i))
sit.PlayerAtr(i)->_money += sit.TotalPot();
res[i] = sit.PlayerMoney(i) - res[i];
}
}
else
{
res = CalcShowDown(sit);
//		for (int i = 0; i < sit.CnPlayer(); i++)
//			sit.PlayerAtr(i)->_money += res[i];
}
sit.TotalPot() = 0;
return res;
}*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CAppViewerDlg::BuildSit()
{
	_sit.ExecHist(&_vectH[_nbCurHand], _nbCurAct);
	if (_sit.EndedSit())
	{
//		CalcSitDistribWin(_sit);
		for (int i = 0; i < _sit.CnPlayer(); i++)
			_sit.PlayerAtr(i)->_viewCards = true;
	}
	else
		for (int i = 0; i < _sit.CnPlayer(); i++)
		{
			int nb = _sit.PositionToNbTable(i);
			_sit.PlayerAtr(i)->_viewCards = _viewCards[nb];
		}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnBnClickedOk()
{
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnRegimView()
{
	_reg = REG_VIEW;
	DisabledButtonTrade();
	Invalidate();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnRegimPlay()
{
	srand(time(NULL));
	_reg = REG_PLAY;
	StartPlay();
	Invalidate();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CAppViewerDlg::ViewButtonTrade()
{
	if (_reg == REG_PLAY)
	{
		EnabledButtonView(false);
		if (_sit.EndedSit() || _sit.Indicator() == -1)
			DisabledButtonTrade();
		else
		{
			tpDis disC = _sit.CreateDisCallCheck();
			tpDis dis = _sit.CreateDis(HistCreateDis(ACT_FOLD));
			GetDlgItem(IDC_BTN_FOLD)->EnableWindow(!(dis == disC));
			GetDlgItem(IDC_BTN_CALL)->EnableWindow(true);
			dis = _sit.CreateDis(HistCreateDis(ACT_ALL_IN));
			if (dis == disC)
			{
				GetDlgItem(IDC_BTN_RAISE)->EnableWindow(false);
				GetDlgItem(IDC_BTN_ALLIN)->EnableWindow(false);
			}
			else
			{
				tpDis disR = _sit.MinRaise();
				GetDlgItem(IDC_BTN_ALLIN)->EnableWindow(true);
				GetDlgItem(IDC_BTN_RAISE)->EnableWindow(!(disR == dis));
				char buf[16];
				_itoa(disR._money, buf, 10);
				SetDlgItemText(IDC_EDIT_RAISE, buf);
			}
		}
	}
	else
	{
		EnabledButtonView(true);
		DisabledButtonTrade();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CAppViewerDlg::DisabledButtonTrade()
{
	GetDlgItem(IDC_BTN_FOLD)->EnableWindow(false);
	GetDlgItem(IDC_BTN_CALL)->EnableWindow(false);
	GetDlgItem(IDC_BTN_RAISE)->EnableWindow(false);
	GetDlgItem(IDC_BTN_ALLIN)->EnableWindow(false);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CAppViewerDlg::EnabledButtonView(bool val)
{
	//	GetDlgItem(IDC_BUTTON2)->EnableWindow(val);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(val);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(val);
	GetDlgItem(IDC_BUTTON5)->EnableWindow(val);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnBnClickedBtnFold()
{
	ExecDecide(HistCreateDis(ACT_FOLD));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CAppViewerDlg::ExecDecide(tpDis dis)
{
/*	try{
		dis = _sit.CreateDis(dis);
		_sit.ExecDis(dis);
		while (_sit.Indicator() == -1 && !_sit.EndedSit())
		{
			switch (_sit.CnCardTable())
			{
				case 0: _sit.ExecDis(HistCreateDis(ACT_DELFLOP)); break;
				case 3: _sit.ExecDis(HistCreateDis(ACT_DELTURN)); break;
				case 4: _sit.ExecDis(HistCreateDis(ACT_DELRIVER)); break;
			}
		}
		if (_sit.EndedSit())
		{
			_curRes = CalcSitDistribWin(_sit);
			for (int i = 0; i < _sit.CnPlayer(); i++)
				if (_sit.PlayerAtr(i)->_isCard)
					_sit.PlayerAtr(i)->_viewCards = true;
			_curRes = _sit.RestraightResPosToNb(_curRes);
			//		clAnsiString as = _curRes[0];
			//		SetDlgItemText(IDC_STATIC1, as.c_str());
			_vectH.push_back(*_sit.PtrHistory());
		}

		int ind = _sit.Indicator();
		if (!_sit.PlayerViewCard(ind))
			OnBnClickedButton1();

		Invalidate();
	}
	catch (...)
	{
		ErrMessage("ExecDecide", "");
	}
	*/
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnBnClickedBtnCall()
{
	ExecDecide(_sit.CreateDisCallCheck());
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnBnClickedBtnAllin()
{
	ExecDecide(_sit.CreateDis(HistCreateDis(ACT_ALL_IN)));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnBnClickedBtnRaise()
{
	char buf[16];
	CWnd *edit = GetDlgItem(IDC_EDIT_RAISE);
	edit->GetWindowTextA(buf, 16);
	int bet = atoi(buf);
	tpDis dis = _sit.CreateDis(HistCreateDis(ACT_RAISE, bet));
	ExecDecide(dis);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void	CAppViewerDlg::StartPlay()
{
	_col.New();
	_col.Shufle();
	_col.Shufle();
	for (int i = 0; i < CN_PLAYER; i++)
		_seat[i].Clear();
	strcpy(_seat[0]._name, "Player0");
	strcpy(_seat[1]._name, "Player1");
	_seat[0]._money = 2500;
	_seat[3]._money = 2500;
	_seat[0]._inPlay = true;
	_seat[3]._inPlay = true;
	_seat[3]._busy = true; // Card Visible
	CreateNewGame(_sit, _col, _seat, 100, 0);

	_curRes = 0;
	_allRes = 0;
	Invalidate();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnFileLoadstrat()
{
	CFileDialog fileD(true, NULL, (LPCTSTR) "*.aaa");
	INT_PTR res = fileD.DoModal();
	if (res == IDOK)
	{
		CStringA ss;
		ss = fileD.GetPathName();
		//_strat.LoadFromFile(ss.GetBuffer());
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void CAppViewerDlg::OnBnClickedButton1()
{
	StartPlay();
	/*
	try
	{
		clTreeOnFile *node = _strat._tree.FindSitNode(&_sit);
		if (node == NULL || node->CnBranch() == 0)
		{
			ErrMessage("Думайте сами", "");
			return;
		}
		clAtrStratHoldem *atr = _strat.LockNode(node->AtrTree()->_nbRec, _sit.NameTrade());
		//	tpCardsGame cards;
		//	cards.GetHandsFromSit(_sit);
		//	cards.CalcAllIndex();
		tpFloat *weight = atr->GetStratWeight(node->CnBranch(), _sit);
		if (weight == NULL)
		{
			bool b0 = false;
			if (b0)
				OnBnClickedButton1();
			ErrMessage("Думайте сами", "");
			return;
		}
#ifdef ANTISTRATEGY_DEC
		int nbA = node->CnBranch() - 1;
		double wA = 0;
		double delta;
		clTreeStrat *node1 = HeroDecide(cards, node, 1000, NULL, delta);
		if (delta > 100)
		{
			for (int i = 0; i < node->CnBranch(); i++)
				if (node1 == node->Branch(i))
					nbA = i;
			wA = (delta > 200) ? 1 : 0.5;
			weight[nbA] += wA;
			wA++;
			for (int i = 0; i < node->CnBranch(); i++)
				weight[i] /= wA;
		}
#endif
		tpFloat val = (tpFloat)rand() / RAND_MAX;
		int nbB = node->CnBranch() - 1;
		for (int i = 0; i < node->CnBranch(); i++)
			if (weight[i] > val)
			{
				nbB = i;
				break;
			}
			else
				val -= weight[i];
		clAnsiString as;
		for (int i = 0; i < node->CnBranch(); i++)
		{
			//			as += HistDisToString(node->Branch(i)->GetSit().GetLastAct()->_dis) + (clAnsiString)" - " + WSDoubleToAS(weight[i],4) + (clAnsiString)";";
		}
		SetDlgItemText(IDC_STATIC3, as.c_str());
		ExecDecide(node->Branch(nbB)->AtrTree()->_dis);
		_strat.UnLockNode(node->AtrTree()->_nbRec);
	}
	catch (...)
	{
		ErrMessage("Сломались", "");
	}
	*/
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------


BOOL CAppViewerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
