// ZzhPassDlg.cpp : implementation file
//
#include "StdAfx.h"
#include "GetVIP.h"
#include "GetVIPDlg.h"
#include <iostream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZzhPassDlg dialog


#include "WINSOCK.H"

#define Request_DOWN			0x9
#define Request_UP			    0x23
#define File_Buffer				0x10
#define File_Buffer_Finish		0x11
#define Request_EnOK            0x7
#define Request_Encode          0x51
#define Request_Decode          0x52
typedef struct
{
	BYTE	Buffer1[128];
	BYTE	Buffer2[128];

}codedata, *LPcodedata;
typedef struct
{
	BYTE	Flags;
	DWORD	Buffer_Size;
	BYTE	Buffer[1024];
	BYTE	Info[256];
	DWORD   File_Size;
	BYTE    Fname[256];
}NET_DATA, *LPNET_DATA;
typedef struct
{
    int     request;
	int     name;
}NameData,*LPNameData;


SOCKET m_hSocket;
HANDLE hLinstenThread = NULL;
CZzhPassDlg::CZzhPassDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZzhPassDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CZzhPassDlg)
	m_port = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_port = 3108;

}

void CZzhPassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZzhPassDlg)
	DDX_Text(pDX, IDC_EDIT1, m_port);
	DDV_MinMaxInt(pDX, m_port, 1, 65535);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CZzhPassDlg, CDialog)
	//{{AFX_MSG_MAP(CZzhPassDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZzhPassDlg message handlers

BOOL CZzhPassDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CZzhPassDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CZzhPassDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CZzhPassDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void GetFile(SOCKET sockInt,char filename[])
{
	char TmpPath1[MAX_PATH];
	DWORD recvsize = 0;
	char tgtIP[30] = {0};
	struct hostent *hp = NULL;
	NET_DATA MyData;
	DWORD dwBytes;
	BYTE request[256] = {0};
	DWORD size=0;

	GetCurrentDirectory(MAX_PATH,TmpPath1);
	lstrcatA( TmpPath1, "\\Data\\" );
	lstrcatA( TmpPath1, filename );
	
	if ( GetFileAttributes(TmpPath1) != -1 )
	{
		WSACleanup();
		return ;
	}
	
 
	recv( sockInt, (char*)&size, sizeof(size), NULL );
	
	BYTE *file=new BYTE[size+1];
	memset(file,0,sizeof(file)+1);
	
	HANDLE hFile = CreateFile( TmpPath1, GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL );
	while (1)
	{
		memset( &MyData, 0, sizeof(NET_DATA) );
		recv( sockInt, (char*)&MyData, sizeof(MyData), NULL );
		
		if ( MyData.Flags != File_Buffer_Finish && MyData.Flags != File_Buffer ) break;
		memcpy(file+recvsize,MyData.Buffer,MyData.Buffer_Size);
		
		recvsize += MyData.Buffer_Size;
		if ( MyData.Flags == File_Buffer_Finish ) break;
	}
	WriteFile(hFile, file, size, &dwBytes, NULL);
    CloseHandle(hFile);
	closesocket(sockInt);
	return ;
}

void SendFile( SOCKET sock,char filename[] )
{
	char Buffer[128] = {0};
	DWORD size=0;
	char TmpPath1[MAX_PATH];

	GetCurrentDirectory(MAX_PATH,TmpPath1);
	lstrcatA(TmpPath1,"\\Data\\");
	lstrcatA( TmpPath1, filename );
	HANDLE hFile = CreateFile( TmpPath1, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox("文件不存在!");
		return;
	}

    size=GetFileSize(hFile,NULL);
	if(size!=0)

	send( sock, (char*)&size, sizeof(size), NULL );

	NET_DATA MyData;
	while(1)
	{	
		Sleep(1);
		memset( &MyData, 0, sizeof(NET_DATA) );
		MyData.Flags = File_Buffer;
		ReadFile(hFile, MyData.Buffer, 1024, &MyData.Buffer_Size, NULL); // 读取数据, 后面发送

		if ( MyData.Buffer_Size == 0 )//读取完了，
		{
			CloseHandle(hFile);
		
			memset( &MyData, 0, sizeof(NET_DATA) );
			MyData.Flags = File_Buffer_Finish;

			MyData.Buffer_Size = 1;
			
			if ( send( sock, (char*)&MyData, sizeof(MyData), NULL ) == SOCKET_ERROR ) break;
			break;
		}
		if (send( sock, (char*)&MyData, sizeof(MyData), NULL ) == SOCKET_ERROR ) break; // 发送上面读取的数据
	}

	CloseHandle(hFile);
    closesocket(sock);
	return;
}
/////////////////////////////////////////////////////////////////
void SendData( SOCKET sock,int d)
{
	
	 unsigned char data[45] = {
		0x8B, 0x44, 0x24, 0x08, 0x56, 0x33, 0xC9, 0x8D, 0x70, 0xFF, 0x85, 0xF6, 0x7E, 0x1B, 0x8B, 0x54, 
			0x24, 0x08, 0x8A, 0x04, 0x11, 0x3C, 0xFF, 0x75, 0x06, 0xC6, 0x04, 0x11, 0x00, 0xEB, 0x05, 0xFE, 
			0xC0, 0x88, 0x04, 0x11, 0x41, 0x3B, 0xCE, 0x7C, 0xE9, 0x33, 0xC0, 0x5E, 0xC3
	};
	
	 unsigned char undata[45] = {
		0x8B, 0x44, 0x24, 0x08, 0x56, 0x33, 0xC9, 0x8D, 0x70, 0xFF, 0x85, 0xF6, 0x7E, 0x1B, 0x8B, 0x54, 
			0x24, 0x08, 0x8A, 0x04, 0x11, 0x84, 0xC0, 0x75, 0x06, 0xC6, 0x04, 0x11, 0xFF, 0xEB, 0x05, 0xFE, 
			0xC8, 0x88, 0x04, 0x11, 0x41, 0x3B, 0xCE, 0x7C, 0xE9, 0x33, 0xC0, 0x5E, 0xC3
	};

	    codedata mydata;
		memset(&mydata,0,sizeof(codedata));
		memcpy(mydata.Buffer1,data,sizeof(data));
		memcpy(mydata.Buffer2,undata,sizeof(undata));
		if (send( sock, (char*)&mydata, sizeof(mydata), NULL ) == SOCKET_ERROR )
			MessageBox(0,"发送模块失败",0,0);

    closesocket(sock);
	return;
}
//////////////////////////////////////////////////////////////


DWORD WINAPI AnswerThread(LPVOID lparam)
{
	SOCKET ClientSocket = (SOCKET)lparam;
	char Buffer[1024] = {0};
	int Ret = 0;

	NameData namedata;


	while(1)
	{
		memset( &namedata, 0, sizeof(NameData) );
		Ret = recv( ClientSocket, (char*)&namedata, sizeof(NameData), NULL );
		if ( Ret == 0 || Ret == SOCKET_ERROR )
		{
			break;
		}


		if ( namedata.request == Request_DOWN )
		{
			char a[255];
			itoa(namedata.name,a,10);
			GetFile( ClientSocket,a);//发送文件，然后断开
			break;
		}
		else if ( namedata.request == Request_UP )
		{
			char a[255];
			itoa(namedata.name,a,10);
			SendFile(ClientSocket,a);
			break;
		}
		else if(namedata.request==Request_Encode)
		{
			SendData(ClientSocket,1);
			break;
		}
		else break;
	}
	closesocket(ClientSocket);
	return 0;
}
DWORD WINAPI ListenThread(LPVOID lparam)
{
	SOCKET AcceptSocket;
	DWORD dwThreadId;
	while (1)
	{
		AcceptSocket = SOCKET_ERROR;
		
		while(AcceptSocket == SOCKET_ERROR)
		{
			AcceptSocket = accept( m_hSocket, NULL, NULL );
		
		}
		CreateThread( NULL, NULL, AnswerThread, (LPVOID)AcceptSocket,0,&dwThreadId);
	}
	
	WSACleanup();
	return 0;
}

void CZzhPassDlg::OnStart() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if ( m_port <= 0 || m_port >65535 )
	{
		return;
	}

	WSADATA wsaData;
	SOCKADDR_IN m_addr;
	int iRet = 0;
	DWORD ThreadId;

	if ( m_hSocket != NULL )
	{
		closesocket(m_hSocket);
	}

	WSACleanup();
	iRet = WSAStartup(MAKEWORD(2,2),&wsaData);
	if( iRet != NO_ERROR ) AfxMessageBox("Error at WSAStartup()");

	memset( &m_addr, 0, sizeof(SOCKADDR) );
	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	m_addr.sin_port = htons(m_port);

	m_hSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if ( bind(m_hSocket,(LPSOCKADDR)&m_addr,sizeof(m_addr)) == SOCKET_ERROR )
	{
		AfxMessageBox("Bind失败");
		WSACleanup();
		return;
	}
	if ( listen( m_hSocket, 10000 ) ==  SOCKET_ERROR )
	{
		AfxMessageBox("Listen失败!");
		WSACleanup();
		return;
	}
	hLinstenThread = CreateThread( NULL, NULL, ListenThread, NULL, 0, &ThreadId);
	((CEdit*)GetDlgItem(IDC_EDIT1))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_START))->EnableWindow(FALSE);	//禁用按钮
	return;
}

void CZzhPassDlg::OnExit()
{
	// TODO: Add your control notification handler code here
	if ( hLinstenThread != NULL )
	{
		CloseHandle(hLinstenThread);
	}
	if ( m_hSocket != NULL )
	{
		closesocket(m_hSocket);
	}
	WSACleanup();
	ExitProcess(0);
}
