#pragma once

#include "resource.h"
#include "tpScanTable.h"
// CAppViewerDlg dialog
#include "..\include\holdem\Common\SitHoldem.h"
#include <vector>

class CAppViewerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAppViewerDlg)

public:
	CAppViewerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAppViewerDlg();

// Dialog Data
	enum { IDD = IDD_APPVIEWER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnFileLoad();
	afx_msg void OnFileSave32772();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnViewcardsPlayer0();
	afx_msg void OnViewcardsPlayer1();
	afx_msg void OnViewcardsPlayer2();
	afx_msg void OnViewcardsPlayer3();
	afx_msg void OnViewcardsPlayer4();
	afx_msg void OnViewcardsPlayer5();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnRegimView();
	afx_msg void OnRegimPlay();
	afx_msg void OnBnClickedBtnFold();
	afx_msg void OnBnClickedBtnCall();
	afx_msg void OnBnClickedBtnAllin();
	afx_msg void OnBnClickedBtnRaise();
	afx_msg void OnFileLoadstrat();
	afx_msg void OnBnClickedButton1();

	void	DrawSit(CDC *dc);
	void	DrawPlayer(CDC *dc, int nb);
	void	BuildSit();
	void	ViewButtonTrade();
	void	DisabledButtonTrade();
	void	EnabledButtonView(bool val);
	void	ExecDecide(tpDis dis);
	void	StartPlay();
	void	Clear();

	void	BuildAndDraw(tpScanTable table);
	bool	NeedToSaveSit();

	int _tag, _time;
	clCol _col;
	clSitHoldem _sit;
	clListHistory _list;
	CDC		*_dcMainBuffer;
	CBitmap *_bmpCards[CN_CARD_COL];
	CBitmap *_bmpButton, *_bmpUndefCard;
	std::vector <clGameHistory> _vectH;
	int _reg, _nbCurHand, _nbCurAct;
	bool _viewCards[CN_PLAYER];
	clDoubleCP _curRes, _allRes;
	tpSeat _seat[CN_PLAYER];

	virtual BOOL OnInitDialog();
};
