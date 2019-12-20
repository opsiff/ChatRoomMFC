
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
void ThreadSend(SOCKET ms) {
	int ret = 0;
	int flag = 0;
	int g_iStatus = 0;
	SOCKET client = INVALID_SOCKET;					//创建一个临时套接字来存放要转发的客户端套接字
	char temp[128] = { 0 };							//创建一个临时的数据缓冲区，用来存放接收到的数据
	memcpy(temp, user[!flag].buf, sizeof(temp));
	sprintf(user[flag].buf, "%s: %s", user[!flag].userName, temp);//添加一个用户名头

	if (strlen(temp) != 0 && g_iStatus == 0) //如果数据不为空且还没转发则转发
		ret = send(user[flag].sClient, user[flag].buf, sizeof(user[flag].buf), 0);
	if (ret == SOCKET_ERROR)
		return ;
	g_iStatus = 1;   //转发成功后设置状态为已转发
	return;
}
void ThreadAccept(SOCKET ms) {
	return;
}
void ThreadManager(SOCKET ms) {
	std::thread thread(ThreadAccept, ms);
	thread.join();
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
    msaddr.sin_port = htons(6000);
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
	std::thread thread(ThreadManager, ms);
	thread.detach();//detach自主执行直至结束
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
	fp = fopen("message.txt", "a+");
	while (fscanf(fp, "%s", buff)!=EOF) {
		info =buff;
		info += L"\r\n";
		adaptername.Append(info);
	}
	fclose(fp);
	UpdateData(false);
}

