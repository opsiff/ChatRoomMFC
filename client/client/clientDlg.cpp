
// clientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "client.h"
#include "clientDlg.h"
#include "afxdialogex.h"
#include <thread>
#include <atlconv.h>//T2A
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SOCKET ClientSocket = INVALID_SOCKET;//客户端套接字

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CclientDlg 对话框



CclientDlg::CclientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLIENT_DIALOG, pParent)
	, mesgsend(_T(""))
	, mesgrecv(_T(""))
	, serveraddr(_T("127.0.0.1"))
	, serverport(_T("18000"))
	, username(_T("关文涛"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, mesgsend);
	DDV_MaxChars(pDX, mesgsend, 250);
	DDX_Text(pDX, IDC_EDIT4, mesgrecv);
	DDX_Text(pDX, IDC_EDIT1, serveraddr);
	DDX_Text(pDX, IDC_EDIT2, serverport);
	DDV_MaxChars(pDX, serverport, 20);
	DDX_Text(pDX, IDC_EDIT5, username);
}

BEGIN_MESSAGE_MAP(CclientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &CclientDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CclientDlg::OnBnClickedButton1)
	ON_MESSAGE(WM_DISPLAY_CHANGE, OnDisplayChange)
END_MESSAGE_MAP()


// CclientDlg 消息处理程序

BOOL CclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CclientDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (ClientSocket == INVALID_SOCKET)
		return;
	UpdateData(true);
	USES_CONVERSION;
	const char* buf = T2A(mesgsend.GetString());
	char truebuf[256];
	memset(truebuf, 0, sizeof(truebuf));
	if (strlen(buf) == 0)
		return;
	else
		memcpy(truebuf, buf, strlen(buf));
	//char* buf = "Hello World!";
	int ret=0;
	ret = send(ClientSocket, truebuf, sizeof(truebuf), 0);
	if (ret == SOCKET_ERROR) {
		mesgrecv.Append(L"发送消息失败\r\n");
		return ;
	}
	mesgrecv.Append( L"发送消息成功\r\n");
	mesgsend = "";
	UpdateData(false);
}

LRESULT CclientDlg::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{

	//更新控件
	SetDlgItemText(IDC_EDIT4, mesgrecv);//用于更新界面中的控件
	return 0;
}
UINT CclientDlg::ThreadRecv(LPVOID pParam)
{
	CclientDlg* pDlg = (CclientDlg*)pParam;
	char buf[512] = {};
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		int ret = recv(ClientSocket, buf, sizeof(buf), 0);
		if (ret == SOCKET_ERROR)
		{
			Sleep(500);
			continue;
		}
		if (strlen(buf) != 0)
		{
			//UpdateData(true);
			pDlg->mesgrecv.Append(CA2T(buf));
			pDlg->mesgrecv.Append(_T("\r\n"));
			FILE* p = fopen("client.log", "a+");
			fprintf(p, buf);
			fclose(p);
			::PostMessage(pDlg->m_hWnd, WM_DISPLAY_CHANGE, 0, 0);//线程向对话框发送自定义消息
			//UpdateData(false);
			//printf("recv %d BYTES message\n", strlen(buf));
			//printf("%s\n", buf);
			//iStatus = RECV_OVER;
		}
		else
			Sleep(100);
		Sleep(100);

	}/*
	while (true)
	{
		

	}*/
	return 1;
}
void CclientDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//处理多次点击清理上次的socket
	if (ClientSocket != INVALID_SOCKET)
	{
		closesocket(ClientSocket);
		WSACleanup();
		ClientSocket = INVALID_SOCKET;
	}
	CString info;
	WSADATA wsaData = { 0 };//存放套接字信息
	SOCKADDR_IN ServerAddr = { 0 };//服务端地址
	UpdateData(true);
	USHORT uPort = atoi(CT2A(serverport.GetBuffer()));//服务端端口
						 //初始化套接字
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		info.Format(_T("WSAStartup failed with error code: %d!"), WSAGetLastError());
		MessageBox(info);
		return;
	}
	//判断套接字版本
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		info.Format(_T("wVersion was not 2.2 with error code: %d!"), WSAGetLastError());
		MessageBox(info);
		return;
	}
	//创建套接字
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET)
	{
		info.Format(_T("socket failed with error code: %d!"), WSAGetLastError());
		MessageBox(info);
		return;
	}
	//输入服务器IP
	printf("Please input server IP:");
	char IP[32] = { 0 };
	//gets_s(IP);
	UpdateData(true);
	int serveraddrsize = strlen(CT2A(serveraddr.GetBuffer()));
	if (serveraddrsize > 32)
		return;
	memcpy_s(IP, sizeof(IP), CT2A(serveraddr.GetBuffer()),serveraddrsize);
	UpdateData(false);
	//设置服务器地址
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(uPort);//服务器端口
	//ServerAddr.sin_addr.S_un.S_addr = inet_addr(IP);//服务器地址
	inet_pton(AF_INET, IP, &ServerAddr.sin_addr);
	UpdateData(true);
	mesgrecv = L"连接服务器中。。。\r\n";
	UpdateData(false);
	//printf("connecting......\n");
	//连接服务器
	if (SOCKET_ERROR == connect(ClientSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr)))
	{
		info.Format(_T("connect failed with error code: %d!"), WSAGetLastError());
		MessageBox(info);
		closesocket(ClientSocket);
		WSACleanup();
		return;
	}
	UpdateData(true);
	USES_CONVERSION;
	const char *name = T2A(username.GetString());
	char truename[50];
	memset(truename, 0, sizeof(truename));
	if (strlen(name) == 0)
		return;
	else
	memcpy(truename, name, strlen(name));
	send(ClientSocket, truename, sizeof(truename), 0);
	mesgrecv = L"连接服务器成功\r\n";
	UpdateData(false);
	CWinThread* hThread = AfxBeginThread(ThreadRecv, (LPVOID)this);
	return;
}
