#include "stdafx.h"
#include "MySocket.h"
#include "FirstServerDlg.h"


CMySocket::CMySocket()
{
	_nbFunc = 1;
	_isConnected = false;
	_timeLastActiv = -1;
	_nbIdSit = -1;
	m_pDlg = NULL;
}


CMySocket::~CMySocket()
{
}
void CMySocket::SetParentDlg(CFirstServerDlg *pDlg, int nbFunc)
{
	m_pDlg = pDlg; //”казатель на диалоговое окно
}

void CMySocket::operator=(CMySocket & nS)
{
	_nbFunc = nS._nbFunc;
	_isConnected = nS._isConnected;
	_timeLastActiv = nS._timeLastActiv;
	m_pDlg = nS.m_pDlg;
	_nbIdSit = nS._nbIdSit;
}

void CMySocket::OnAccept(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	m_pDlg->OnAccept(this);
	CAsyncSocket::OnAccept(nErrorCode);
}


void CMySocket::OnReceive(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	if (_nbFunc == 1)
	{
		m_pDlg->OnReceive(this);
	}
	if (_nbFunc == 2)
		m_pDlg->OnReceive2();
	CAsyncSocket::OnReceive(nErrorCode);
}

void CMySocket::OnClose(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	//m_pDlg->S_Close(this);
	_isConnected = false;
	_timeLastActiv = -1;
	_nbIdSit = -1;
	CAsyncSocket::Close();
	CAsyncSocket::OnClose(nErrorCode);
}

