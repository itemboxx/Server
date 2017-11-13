#pragma once
#include "afxsock.h"
class CFirstServerDlg;
class CMySocket :
	public CAsyncSocket
{
public:
	CMySocket();
	~CMySocket();
	void SetParentDlg(CFirstServerDlg *pDlg, int nbFunc);
	void operator=(CMySocket &nS);
	CFirstServerDlg *m_pDlg;
	int _nbFunc;
	int _nbIdSit;
	bool _isConnected;
	time_t _timeLastActiv;
	virtual void OnAccept(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
};

