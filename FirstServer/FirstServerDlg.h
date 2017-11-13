
// FirstServerDlg.h : header file
//

#pragma once
#define MAIN_MAX_FORM	20

#include "resource.h"
#include "MySocket.h"
#include "tpScanTable.h"
#include "clValidateServer.h"
#include "afxwin.h"
// CFirstServerDlg dialog
class CFirstServerDlg : public CDialogEx
{
// Construction
public:
	CFirstServerDlg(CWnd* pParent = NULL);	// standard constructor
	~CFirstServerDlg();

// Dialog Data
	enum { IDD = IDD_FIRSTSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	void OnAccept(CMySocket *socket);
	void OnReceive(CMySocket *socket);
	void OnReceive2();
	void S_Close(CMySocket *socket);
	int glCnSocket;
	int cnUndeff, cnError;
	std::vector<tpDis *> glDisBuf;
	//clValidateServer glVSServer;
	HANDLE glSemaphore;
	CMySocket m_pListenSocket; //Слушающий сокет
	CMySocket** m_pConnectSocket; //Передающий сокет
	void CheckOpenSocket();
	CMySocket m_pConnectToMainServerSocket; //Передающий сокет
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnClose();
	CString EditUndeff;
//	CEdit EditError;
	CString EditError;
};
