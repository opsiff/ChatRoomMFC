
// sssDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "sss.h"
#include "sssDlg.h"
#include "afxdialogex.h"
#include <winsock.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <thread>
#include <condition_variable>
#include <mutex> 
#include <vector>
#include <chrono>
#include <atlconv.h>//T2A
#include <string>
#pragma comment(lib,"Iphlpapi.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于接收客户端消息的类
class Client
{
public:
	SOCKET sClient;      //客户端套接字
	char buf[128];		 //数据缓冲区
	char userName[16];   //客户端用户名
	char IP[20];		 //客户端IP
	UINT_PTR flag;       //标记客户端，用来区分不同的客户端
};
std::vector<Client>user;//客户端动态数组
std::vector<CString>meg;//消息动态数组
std::vector<CString>name;//用户动态数组
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &CAboutDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CsssDlg 对话框




CsssDlg::CsssDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CsssDlg::IDD, pParent)
	, hostname(_T("服务器未启动"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CsssDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, hostname);
	DDX_Text(pDX, IDC_EDIT2, adaptername);
	DDX_Control(pDX, IDC_EDIT2, ListBoxEdit);
	DDX_Control(pDX, IDC_BUTTON1, Startserverbutton);
}

BEGIN_MESSAGE_MAP(CsssDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CsssDlg::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT1, &CsssDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDOK, &CsssDlg::OnBnClickedOk)
    ON_EN_CHANGE(IDC_EDIT2, &CsssDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON2, &CsssDlg::OnClickedButton2)
END_MESSAGE_MAP()


// CsssDlg 消息处理程序

BOOL CsssDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CsssDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CsssDlg::OnPaint()
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
HCURSOR CsssDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//转发线程
void ThreadSend(std::mutex &mux) {
	char buff[10240];
	memset(buff, 0, sizeof(buff));
	FILE* fp = NULL;
	fp = fopen("ThreadSend.txt", "w");
	fprintf(fp,"Begin ThreadSend");
	fclose(fp);
	while (true)
	{
		bool exitflag = 0;
		//mux.try_lock();
		for (size_t i = 0; i < meg.size(); i++)
		{
			int ret = 0;
			int g_iStatus = 0;
			SOCKET client = INVALID_SOCKET;					//创建一个临时套接字来存放要转发的客户端套接字
			if (meg[i].GetLength() == 0)
				continue;
			meg[i] = name[i] +":"+ meg[i];
			//meg[i] = name[i] + meg[i];
			for (size_t j = 0; j < user.size(); j++)
			{
				//注意：以下n和len的值大小不同,n是按字符计算的，len是按字节计算的
				//int n = meg[i].GetLength(); // n = 14, len = 18
				//获取宽字节字符的大小，大小是按字节计算的
				//int len = WideCharToMultiByte(CP_ACP, 0, meg[i], meg[i].GetLength(), NULL, 0, NULL, NULL);
				//为多字节字符数组申请空间，数组大小为按字节计算的宽字节字节大小
				//int clen = len + 512;
				//pFileName = new char[clen]; //以字节为单位
				//宽字节编码转换成多字节编码
				//WideCharToMultiByte(CP_ACP, 0, meg[i], meg[i].GetLength(), pFileName, len, NULL, NULL);
				//pFileName[len] = '\0'; //多字节字符以'\0'结束
				USES_CONVERSION;
				char* pFileName = T2A(meg[i]);
				ret = send(user[j].sClient, pFileName, strlen(pFileName), 0);
				fp = fopen("message.txt", "a+");
				fprintf(fp, "\n%sSIZE:%d",pFileName, strlen(pFileName));
				fclose(fp);
				if (ret == SOCKET_ERROR)
					exitflag = 1;
			}
		}
		meg.clear();
		name.clear();
		//mux.unlock();
		if (exitflag)
			break;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	
	return;
}
//接收数据
void ThreadRecv(SOCKET ms, std::mutex& mux) {
	FILE* fp = NULL;
	fp = fopen("ThreadRecv.txt", "a+");
	fprintf(fp, "\nBegin ThreadRecv");
	fclose(fp);
	SOCKET client = ms;
	int flag = -1;
	for (int i = 0; i < user.size(); i++) {
		if (user[i].sClient == ms) {
			flag = i;
			break;
		}
	}
	if (flag == -1) {
		return;
	}
	char temp[512] = {};  //临时数据缓冲区
	std::string message;
	//mux.try_lock();
	while (1)
	{
		memset(temp, 0, sizeof(temp));
		int ret = recv(client, temp, sizeof(temp), 0); //接收数据
		if (ret == SOCKET_ERROR)
			continue;
		if (ret == 0)
			break;
		fp = fopen("message.txt", "a+");
		//fprintf(fp, "\r\n");
		//fprintf(fp, temp);
		fclose(fp);
		message = temp;
		USES_CONVERSION;
		CString sa = A2T(message.c_str());
		meg.push_back(sa);
		message = user[flag].userName;
		sa = A2T(message.c_str());
		name.push_back(sa);
		//这里也可能是导致CPU使用率上升的原因。
	}
	//mux.unlock();
	return;
}
//接收连接
void ThreadAccept(SOCKET ms,std::mutex& mux) {
	while (1)
	{
		SOCKET newsocket = INVALID_SOCKET;
		//如果有客户端申请连接就接受连接
		SOCKADDR_IN ClientAddr;
		int ClientAddrLen = sizeof(ClientAddr);
		newsocket = accept(ms, (SOCKADDR*)&ClientAddr, &ClientAddrLen);
		if (newsocket == INVALID_SOCKET)
		{
			continue;
		}
		Client newclient;
		newclient.sClient = newsocket;
		recv(newclient.sClient, newclient.userName, sizeof(newclient.userName), 0); //接收用户名
		FILE *fp = fopen("ThreadRecv.txt", "a+");
		fprintf(fp, "\n");
		fprintf(fp, "Successfuuly got a connection from IP:%s ,Port: %d,UerName: %s\n",
		inet_ntoa(ClientAddr.sin_addr), htons(ClientAddr.sin_port),newclient.userName);
		fclose(fp);
		memcpy(newclient.IP, inet_ntoa(ClientAddr.sin_addr), sizeof(newclient.IP)); //记录客户端IP
		newclient.flag = newclient.sClient; //不同的socket有不同UINT_PTR类型的数字来标识
		user.push_back(newclient);
		std::thread threadsRecv(ThreadRecv, newclient.sClient,std::ref(mux));
		threadsRecv.detach();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	return;
}
void ThreadManager(SOCKET ms) {
	std::mutex mux;
	int sendcontrol = 0;
	std::thread threadAC(ThreadAccept, ms, std::ref(mux));
	threadAC.detach();
	std::thread threads(ThreadSend,std::ref(mux));
	threads.detach();

	return;
}
void CsssDlg::OnBnClickedButton1()
{
    CString error;
    WORD wver;
    wver = MAKEWORD(2, 2);
    WSAData data;//存放套接字信息的结构
    if (0 != WSAStartup(wver, &data))//初始化套接字
    {
        error.Format(_T("声明版本失败，错误代码：%d!"), WSAGetLastError());
        MessageBox(error);
		return;
    }
	if (LOBYTE(data.wVersion) != 2 || HIBYTE(data.wVersion) != 2)//判断版本
	{
		error.Format(_T("声明版本不是2.2"));
		MessageBox(error);
		return;
	}
    SOCKET ms = socket(2, SOCK_STREAM, 0);//创建TCP套接字
    if (ms == INVALID_SOCKET)
    {
        error.Format(_T("创建套接字失败，错误代码：%d!"), WSAGetLastError());
        MessageBox(error);
		return;
    }
	//设置服务器地址
    SOCKADDR_IN msaddr;
    msaddr.sin_family = AF_INET;
    msaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    msaddr.sin_port = htons(18000);
    int len = sizeof(msaddr);
	//绑定服务器
    if (SOCKET_ERROR == bind(ms, (SOCKADDR*)&msaddr, len))
    {
        error.Format(_T("绑定套接字失败，错误代码：%d!"), WSAGetLastError());
        MessageBox(error);
		return;
    }
	//设置监听客户端连接数
	if (SOCKET_ERROR == listen(ms, 255))
	{
		error.Format(_T("客户端连接数过多，错误代码：%d!"), WSAGetLastError());
		MessageBox(error);
		closesocket(ms);
		WSACleanup();
		return;
	}
	std::thread threadm(ThreadManager, ms);
	threadm.detach();//detach自主执行直至结束
	CString message;
    message.Format(_T("成功创建服务器，代码：%d!"), 1000);
    MessageBox(message);
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	char host[256];
	bool hp;
	hp=gethostname(host,100);
	if(hp==0)
		hostname=host;
	else
		hostname="未获取到主机名";
	UpdateData(false);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	

}


void CsssDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CsssDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CsssDlg::OnEnChangeEdit2()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CDialogEx::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
}


void CAboutDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	
}


void CsssDlg::OnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	CString info;
	FILE* fp = NULL;
	char buff[10240];
	memset(buff, 0, sizeof(buff));
	fp = fopen("message.txt", "r");
	adaptername = "";
	while (fscanf(fp, "%s",buff)!=EOF) {
		USES_ATL_SAFE_ALLOCA;
		USES_CONVERSION;
		info =A2T(buff);
		info += L"\r\n";
		adaptername.Append(info);
	}
	fclose(fp);
	UpdateData(false);
}

