
// sssDlg.cpp : ʵ���ļ�
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

// ���ڽ��տͻ�����Ϣ����
class Client
{
public:
	SOCKET sClient;      //�ͻ����׽���
	char buf[128];		 //���ݻ�����
	char userName[16];   //�ͻ����û���
	char IP[20];		 //�ͻ���IP
	UINT_PTR flag;       //��ǿͻ��ˣ��������ֲ�ͬ�Ŀͻ���
};
std::vector<Client>user;//�ͻ��˶�̬����
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CsssDlg �Ի���




CsssDlg::CsssDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CsssDlg::IDD, pParent)
	, hostname(_T("������δ����"))
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


// CsssDlg ��Ϣ�������

BOOL CsssDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
    
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CsssDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CsssDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//ת���߳�
void ThreadSend(SOCKET ms) {
	int ret = 0;
	int flag = 0;
	int g_iStatus = 0;
	SOCKET client = INVALID_SOCKET;					//����һ����ʱ�׽��������Ҫת���Ŀͻ����׽���
	char temp[128] = { 0 };							//����һ����ʱ�����ݻ�������������Ž��յ�������
	memcpy(temp, user[!flag].buf, sizeof(temp));
	sprintf(user[flag].buf, "%s: %s", user[!flag].userName, temp);//���һ���û���ͷ

	if (strlen(temp) != 0 && g_iStatus == 0) //������ݲ�Ϊ���һ�ûת����ת��
		ret = send(user[flag].sClient, user[flag].buf, sizeof(user[flag].buf), 0);
	if (ret == SOCKET_ERROR)
		return ;
	g_iStatus = 1;   //ת���ɹ�������״̬Ϊ��ת��
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
    WSAData data;//����׽�����Ϣ�Ľṹ
    if (0 != WSAStartup(wver, &data))//��ʼ���׽���
    {
        error.Format(_T("�����汾ʧ�ܣ�������룺%d!"), WSAGetLastError());
        MessageBox(error);
		return;
    }
	if (LOBYTE(data.wVersion) != 2 || HIBYTE(data.wVersion) != 2)//�жϰ汾
	{
		error.Format(_T("�����汾����2.2"));
		MessageBox(error);
		return;
	}
    SOCKET ms = socket(2, SOCK_STREAM, 0);//����TCP�׽���
    if (ms == INVALID_SOCKET)
    {
        error.Format(_T("�����׽���ʧ�ܣ�������룺%d!"), WSAGetLastError());
        MessageBox(error);
		return;
    }
	//���÷�������ַ
    SOCKADDR_IN msaddr;
    msaddr.sin_family = AF_INET;
    msaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    msaddr.sin_port = htons(6000);
    int len = sizeof(msaddr);
	//�󶨷�����
    if (SOCKET_ERROR == bind(ms, (SOCKADDR*)&msaddr, len))
    {
        error.Format(_T("���׽���ʧ�ܣ�������룺%d!"), WSAGetLastError());
        MessageBox(error);
		return;
    }
	//���ü����ͻ���������
	if (SOCKET_ERROR == listen(ms, 255))
	{
		error.Format(_T("�ͻ������������࣬������룺%d!"), WSAGetLastError());
		MessageBox(error);
		closesocket(ms);
		WSACleanup();
		return;
	}
	std::thread thread(ThreadManager, ms);
	thread.detach();//detach����ִ��ֱ������
	CString message;
    message.Format(_T("�ɹ����������������룺%d!"), 1000);
    MessageBox(message);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	char host[256];
	bool hp;
	hp=gethostname(host,100);
	if(hp==0)
		hostname=host;
	else
		hostname="δ��ȡ��������";
	UpdateData(false);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	

}


void CsssDlg::OnEnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CsssDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}


void CsssDlg::OnEnChangeEdit2()
{
    // TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
    // ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
    // ���������� CRichEditCtrl().SetEventMask()��
    // ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

    // TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CAboutDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
}


void CsssDlg::OnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

