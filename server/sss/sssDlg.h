
// sssDlg.h : ͷ�ļ�
//

#pragma once


// CsssDlg �Ի���
class CsssDlg : public CDialogEx
{
// ����
public:
	CsssDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SSS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedOk();
	CString hostname;
	CString adaptername;
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnClickedButton2();
	CEdit ListBoxEdit;
	CButton Startserverbutton;
};
