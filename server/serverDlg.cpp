
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
#include <chrono>
#include <atlconv.h>//T2A
#include <string>
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
std::vector<CString>meg;//��Ϣ��̬����
std::vector<CString>name;//�û���̬����
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
			SOCKET client = INVALID_SOCKET;					//����һ����ʱ�׽��������Ҫת���Ŀͻ����׽���
			if (meg[i].GetLength() == 0)
				continue;
			meg[i] = name[i] +":"+ meg[i];
			//meg[i] = name[i] + meg[i];
			for (size_t j = 0; j < user.size(); j++)
			{
				//ע�⣺����n��len��ֵ��С��ͬ,n�ǰ��ַ�����ģ�len�ǰ��ֽڼ����
				//int n = meg[i].GetLength(); // n = 14, len = 18
				//��ȡ���ֽ��ַ��Ĵ�С����С�ǰ��ֽڼ����
				//int len = WideCharToMultiByte(CP_ACP, 0, meg[i], meg[i].GetLength(), NULL, 0, NULL, NULL);
				//Ϊ���ֽ��ַ���������ռ䣬�����СΪ���ֽڼ���Ŀ��ֽ��ֽڴ�С
				//int clen = len + 512;
				//pFileName = new char[clen]; //���ֽ�Ϊ��λ
				//���ֽڱ���ת���ɶ��ֽڱ���
				//WideCharToMultiByte(CP_ACP, 0, meg[i], meg[i].GetLength(), pFileName, len, NULL, NULL);
				//pFileName[len] = '\0'; //���ֽ��ַ���'\0'����
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
//��������
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
	char temp[512] = {};  //��ʱ���ݻ�����
	std::string message;
	//mux.try_lock();
	while (1)
	{
		memset(temp, 0, sizeof(temp));
		int ret = recv(client, temp, sizeof(temp), 0); //��������
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
		//����Ҳ�����ǵ���CPUʹ����������ԭ��
	}
	//mux.unlock();
	return;
}
//��������
void ThreadAccept(SOCKET ms,std::mutex& mux) {
	while (1)
	{
		SOCKET newsocket = INVALID_SOCKET;
		//����пͻ����������Ӿͽ�������
		SOCKADDR_IN ClientAddr;
		int ClientAddrLen = sizeof(ClientAddr);
		newsocket = accept(ms, (SOCKADDR*)&ClientAddr, &ClientAddrLen);
		if (newsocket == INVALID_SOCKET)
		{
			continue;
		}
		Client newclient;
		newclient.sClient = newsocket;
		recv(newclient.sClient, newclient.userName, sizeof(newclient.userName), 0); //�����û���
		FILE *fp = fopen("ThreadRecv.txt", "a+");
		fprintf(fp, "\n");
		fprintf(fp, "Successfuuly got a connection from IP:%s ,Port: %d,UerName: %s\n",
		inet_ntoa(ClientAddr.sin_addr), htons(ClientAddr.sin_port),newclient.userName);
		fclose(fp);
		memcpy(newclient.IP, inet_ntoa(ClientAddr.sin_addr), sizeof(newclient.IP)); //��¼�ͻ���IP
		newclient.flag = newclient.sClient; //��ͬ��socket�в�ͬUINT_PTR���͵���������ʶ
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
    msaddr.sin_port = htons(18000);
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
	std::thread threadm(ThreadManager, ms);
	threadm.detach();//detach����ִ��ֱ������
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

