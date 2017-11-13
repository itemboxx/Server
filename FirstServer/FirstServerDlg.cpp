
// FirstServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FirstServer.h"
#include "FirstServerDlg.h"
#include "afxdialogex.h"
#include "..\include\Util\FileRec.h"
#include "..\include\holdem\Common\GlobalsForCalc.h"
#include "..\include\holdem\Strategy\\Source\StrategyPlay.h"
#include "..\include\holdem\Strategy\Source\StrategyPlayEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFirstServerDlg dialog

struct tpSitAndDis
{
	
	clSitHoldem _sit;
	tpDis _dis;
	tpArrResBranch8 _arrRes;
	time_t _time;
	void WriteFile(int handle)
	{
		_write(handle, &_sit, sizeof(clSitHoldem));
		_write(handle, &_dis, sizeof(tpDis));
		_write(handle, &_arrRes, sizeof(tpArrResBranch8));
		_write(handle, &_time, sizeof(time_t));
	}
	int ReadFile(int handle)
	{
		_read(handle, &_sit, sizeof(clSitHoldem));
		_read(handle, &_dis, sizeof(tpDis));
		_read(handle, &_arrRes, sizeof(tpArrResBranch8));
		_read(handle, &_time, sizeof(time_t));
		return (sizeof(clSitHoldem)+ sizeof(tpDis) + sizeof(tpArrResBranch8) + sizeof(time_t));
	}
};

clStrategyPlayEX _glStrategy;
clFileConstRec glSitHoldemRec, glSitHoldemDisRec, glSocketBlockRec;
clFileRecT <tpSitAndDis> _file;
CFirstServerDlg::CFirstServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFirstServerDlg::IDD, pParent)
	, EditUndeff(_T(""))
	, EditError(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	time_t  time1= time(NULL);
	tm *t = localtime(&time1);
	clAnsiString str = (clAnsiString)"C:\\data\\sit&dis.s";
	if (!_file.OpenFileA(str.c_str()))
		_file.Create(str.c_str(), 0, 10000);
	/*
	if (!glSitHoldemRec.OpenFile("c:\\home\\bin\\sitHoldemLogs.bin"))
		glSitHoldemRec.Create("c:\\home\\bin\\sitHoldemLogs.bin", sizeof(clSitHoldem));
	if (!glSitHoldemDisRec.OpenFile("c:\\home\\bin\\sitHoldemDisLogs.bin"))
		glSitHoldemDisRec.Create("c:\\home\\bin\\sitHoldemDisLogs.bin", sizeof(clSitHoldem) + sizeof(tpDis));
	if (!glSocketBlockRec.OpenFile("c:\\home\\bin\\SocketBlockLogs.bin"))
		glSocketBlockRec.Create("c:\\home\\bin\\SocketBlockLogs.bin", CSC_SIZE_BUFFER);
	*/
}

CFirstServerDlg::~CFirstServerDlg()
{
	_file.CloseFile();
	CDialogEx::~CDialogEx();
}
void CFirstServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, EditUndeff);
	//DDX_Text(pDX, IDC_EDIT2, EditError);
	DDX_Text(pDX, IDC_EDIT2, EditError);
}

BEGIN_MESSAGE_MAP(CFirstServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CFirstServerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CFirstServerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CFirstServerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CFirstServerDlg::OnBnClickedButton3)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CFirstServerDlg message handlers

BOOL CFirstServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	glSemaphore = CreateSemaphore(NULL,1,1,NULL);
	//for (int i = 0; i < MAIN_MAX_FORM; i++)
	//	m_pConnectSocket[i].SetParentDlg(this, 1);
	cnUndeff = 0;
	cnError = 0;
	m_pListenSocket.SetParentDlg(this, 1);
	m_pListenSocket.Create(2016); //Создаем сокет на 2016 порту
	m_pListenSocket.Listen(); //Сокет начинает слушать
	GlobalsForCalcLoad(1);
	_glStrategy.Load();//"D:\\Home\\Save\\ForPlay\\NL2_4.aaa"
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFirstServerDlg::OnPaint()
{
	if (IsIconic())
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
	}
}
void CFirstServerDlg::CheckOpenSocket()
{
	for (int i = 0; i <glCnSocket ; i++)
		if(m_pConnectSocket[i] != NULL)
			if (m_pConnectSocket[i]->_isConnected == false)
			{
				delete(m_pConnectSocket[i]);
				m_pConnectSocket[i] = NULL;
			}
}
void CFirstServerDlg::OnAccept(CMySocket *socket)
{
	/*for (int i = 0; i < MAIN_MAX_FORM; i++)
		if (!m_pConnectSocket[i]._isConnected)
		{
			m_pConnectSocket[i].Close();
			socket->Accept(m_pConnectSocket[i]);
			m_pConnectSocket[i]._isConnected = true;
			return;
		}*/
	if (glCnSocket % 20 == 0)
		CheckOpenSocket();
	glCnSocket++;
	if (glCnSocket == 1)
		m_pConnectSocket = (CMySocket **)malloc(sizeof(CMySocket*));
	else
		m_pConnectSocket = (CMySocket **)realloc(m_pConnectSocket , glCnSocket * sizeof(CMySocket*));
	CMySocket *newSocket = new CMySocket;
	newSocket->SetParentDlg(this, 1);
	newSocket->_isConnected = true;
	m_pConnectSocket[glCnSocket - 1] = newSocket;
	socket->Accept(*newSocket);

}
bool glLocalTest = false;
void CFirstServerDlg::OnReceive2()
{
	char buf[CSC_SIZE_BUFFER];
	m_pConnectToMainServerSocket.Receive(buf, CSC_SIZE_BUFFER);
	if (buf[0] == '1')
	{
		clSitHoldem sit;
		memcpy(&sit, buf + 1, sizeof(clSitHoldem));
	}
}

void CFirstServerDlg::S_Close(CMySocket *socket)
{
	if(socket->_nbIdSit != -1)
		if(glDisBuf[socket->_nbIdSit] != NULL)
			delete glDisBuf[socket->_nbIdSit];
	glDisBuf[socket->_nbIdSit] = NULL;
}

void CFirstServerDlg::OnReceive(CMySocket *socket)
{
	char buf[2 * sizeof(int) + sizeof(tpArrResBranch8) + sizeof(tpDis) + sizeof(clSitHoldem)];
	int *buf0 = (int *)buf;
	int size = 2 * sizeof(int) + sizeof(clSitHoldem);
	socket->Receive(buf, size);
	if (buf0[1] == 1)
	{
		clSitHoldem sit;
		tpSitAndDis siD;
		memcpy(&sit, buf + 2 * sizeof(int), sizeof(clSitHoldem));
		tpArrResBranch8 arrRes;
		tpDis dis;
		buf0[0] = 2 * sizeof(int) + sizeof(tpArrResBranch8) + sizeof(tpDis) + sizeof(clSitHoldem);
		buf0[1] = 1;
		glTrasser.WriteMessage((clAnsiString)"Ждем семафор");
		DWORD dwWaitResult = WaitForSingleObject(glSemaphore, 1);
		while (dwWaitResult != WAIT_OBJECT_0)
			dwWaitResult = WaitForSingleObject(glSemaphore, 1);
		glTrasser.WriteMessage((clAnsiString)"Дождались семафор");
		enNbError nbEr = S_Check(sit);
		int handle = _open("C:\\data\\lastSit.bin", _O_BINARY | _O_CREAT | _O_WRONLY, _S_IWRITE);
		if (handle != -1)
		{
			_write(handle, &sit, sizeof(clSitHoldem));
			_close(handle);
		}
		else
			glTrasser.WriteMessage(handle);
		if (nbEr == AllIsGood)
		{
				glTrasser.WriteMessage((clAnsiString)"Запускаем DecideRemote");
				dis = _glStrategy.DecideRemote(sit, arrRes, 5000);
		}
		else
		{
			glTrasser.WriteMessage((clAnsiString)"Ситуация битая " + (clAnsiString)nbEr);
			dis._act = ACT_UNDEF;
			cnError++;
		}
		int i0;
		for (i0 = 0; i0 < arrRes._cn; i0++)
			if (arrRes._weight[i0] - 1. / arrRes._cn > DOUBLE_0)
				break;
		if(i0 == arrRes._cn && arrRes._cn != 1)
			dis = HistCreateDis(ACT_UNDEF);
		if (dis._act == ACT_UNDEF)
			cnUndeff++;
		if ((dis._act == ACT_RAISE || dis._act == ACT_ALL_IN || ACT_CALL) && dis._money > sit.TotalPot())
			for (int i = 0; i < arrRes._cn; i++)
				if (dis == arrRes._dis[i] && arrRes._weight[i] < 0.1)
					dis = arrRes._dis[0];
		clAnsiString ss = (clAnsiString)cnUndeff;
		SetDlgItemTextA(IDC_EDIT1, ss.c_str());
		ss = (clAnsiString)cnError;
		SetDlgItemTextA(IDC_EDIT2, ss.c_str());
		UpdateData();
		glTrasser.WriteMessage((clAnsiString)"Вывели кол-во UNDEFF");
		siD._time = time(NULL);
		siD._sit = sit;
		siD._dis = dis;
		memcpy(&siD._arrRes, &arrRes, sizeof(tpArrResBranch8));
		_file.AddRecord(&siD);
		glTrasser.WriteMessage((clAnsiString)"Записали решение и ситуацию");
		ReleaseSemaphore(glSemaphore, 1, NULL);
		memcpy(buf + 2 * sizeof(int), &arrRes, sizeof(tpArrResBranch8));
		memcpy(buf + 2 * sizeof(int) + sizeof(tpArrResBranch8), &dis, sizeof(dis));
		memcpy(buf + 2 * sizeof(int) + sizeof(tpArrResBranch8) + sizeof(tpDis), &sit, sizeof(clSitHoldem));
		socket->Send(buf, 2 * sizeof(int) + sizeof(tpArrResBranch8) + sizeof(tpDis) + sizeof(clSitHoldem));
	}

	socket->_timeLastActiv = time(NULL);

}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFirstServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFirstServerDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


void CFirstServerDlg::OnBnClickedButton1()
{
	int nbErr = 0;
	for (int i = 0; i < glCnSocket; i++)
		if (m_pConnectSocket[i] != NULL)
		{
			m_pConnectSocket[i]->OnClose(nbErr);
			delete(m_pConnectSocket[i]);
			m_pConnectSocket[i] = NULL;
		}
}


void CFirstServerDlg::OnBnClickedButton2()
{
	CMySocket **mSocket;
	mSocket = (CMySocket **)malloc(sizeof(CMySocket*) * glCnSocket);
	int cn0 = glCnSocket, j = 0;
	for (int i = 0; i < glCnSocket; i++)
		if (m_pConnectSocket[i] == NULL)
			cn0--;
		else
		{
			mSocket[j] = m_pConnectSocket[i];
			j++;
		}
	glCnSocket = cn0;
	mSocket = (CMySocket **)realloc(mSocket,sizeof(CMySocket*) * glCnSocket);
	free(m_pConnectSocket);
	m_pConnectSocket = mSocket;
}


void CFirstServerDlg::OnBnClickedButton3()
{

	clSitHoldem sit;
	clHoldemTree treeStats;
	treeStats.AtrTree()->_sitA = sit;
	BuildTreeNLAuto(&treeStats, sit);
	tpDis dis;
	tpArrResBranch8 arrRes;
	tpSitAndDis sD;
	clFileRecT <tpSitAndDis> file;
	clSitHoldem **vectSit;
	int handle1 = _open("C:\\data\\sit\\lastSit.bin", _O_BINARY, _S_IREAD);
	_read(handle1, &sit, sizeof(clSitHoldem));
	int handle = _open("C:\\data\\sit\\AllCorrectSit0.bin", _O_BINARY, _S_IREAD);
	int cnVect = 0;
	size_t *cn;
	if (handle != -1)
	{
		_read(handle, &cnVect, sizeof(int));
		cn = (size_t *)malloc(sizeof(size_t)* cnVect);
		vectSit = (clSitHoldem **)malloc(sizeof(clSitHoldem *) * cnVect);
		for (int i = 0; i < cnVect; i++)
		{
			_read(handle, &cn[i], sizeof(size_t));
			vectSit[i] = (clSitHoldem *)malloc(sizeof(clSitHoldem) * cn[i]);
			ReadBigBlock(handle, vectSit[i], sizeof(clSitHoldem) * cn[i]);
		}
		_close(handle);
	}
	handle = _open("C:\\Work\\Util\\Trasser\\TrassError.txt", _O_TEXT | _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IWRITE);
	file.OpenFileA("C:\\data\\Sit\\sit&dis.s");
	vector <long long> vectNbGame;
	vector <tpSitAndDis> vectSd;
	clGameHistory hist;
	for (int i = file.CnRecord() - 1; i >= 0 ; i--)
	{
		file.ReadRecord(i, &sD);
		
		if (sD._dis._act == ACT_UNDEF)
		{
			std::tm *ptm = localtime(&sD._time);
			int a = 0;
		}
		else
			continue;
		/*
		if (sD._sit.CnAct() == 0)
			continue;
		int cn0, cn1;
		for (cn0 = 0; cn0 < cnVect; cn0++)
			if (sD._sit.NbTrny() == vectSit[cn0][0].NbTrny())
				break;
		
		if (cn0 >= cnVect)
			continue;
		for (cn1 = 0; cn1 < cn[cn0]; cn1++)
			if (sD._sit.NbGame() == vectSit[cn0][cn1].NbGame())
				break;
		if (cn1 >= cn[cn0])
			continue;
		if (sD._dis._act != ACT_UNDEF && !AccuratDis(sD._dis, vectSit[cn0][cn1].GetAct(sD._sit.CurAct())->_dis))
		{
			vectNbGame.push_back(sD._sit.NbGame());
			clAnsiString ss = (clAnsiString)"Ошибка :" +
				HistDisToString(vectSit[cn0][cn1].GetAct(sD._sit.CurAct())->_dis) +
				(clAnsiString)" вместо " +
				HistDisToString(sD._dis);
			char c = '\n';
			_write(handle, &c, sizeof(char));
			char *str = ss.c_str();
			_write(handle, str, strlen(str));
		}
		else if (sD._dis._act == ACT_UNDEF)
			vectNbGame.push_back(sD._sit.NbGame());
			*/
	}
	_close(handle);
	//for (int i = 0; i < cnVect; i++)
	//	free(vectSit[i]);
	//free(vectSit);
	int handleNbTrn = _open("C:\\data\\Sit\\nbTrn.bin", _O_BINARY | _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IWRITE);
	int size = vectNbGame.size();
	_write(handleNbTrn, &size, sizeof(int));
	for(int i =0; i < size; i++)
		_write(handleNbTrn, &vectNbGame[i], sizeof(long long));
	_close(handleNbTrn);
	
}


void CFirstServerDlg::OnClose()
{

	_file.CloseFile();
	glTrasser.WriteMessage((clAnsiString)"кол-во ошибок =" + (clAnsiString)cnError);
	glTrasser.WriteMessage((clAnsiString)"кол-во UNDEFF =" + (clAnsiString)cnUndeff);
	CDialogEx::OnClose();
}

