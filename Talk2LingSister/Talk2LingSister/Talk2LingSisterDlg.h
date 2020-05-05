
// Talk2LingSisterDlg.h : header file
//

#pragma once
#include"SistorCharacter.h"
#include"ChatUnit.h"
#include"TalkSrvrSocket.h"
#include "afxwin.h"
#include "afxcmn.h"

// CTalk2LingSisterDlg dialog
class CTalk2LingSisterDlg : public CDialogEx
{
// Construction
public:
	CTalk2LingSisterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TALK2LINGSISTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	

// Implementation
protected:
	HICON m_hIcon;

	const static int m_HotKeyID =8560;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedConnect();
	afx_msg LRESULT OnLingCalled(WPARAM wParam,LPARAM lParam);//һ������
	afx_msg LRESULT OnLingSayed(WPARAM wParam,LPARAM lParam);//˵������
	afx_msg LRESULT OnHotKey(WPARAM wParam,LPARAM lParam);//��ݼ�������֮t

private:
	int nPort;
	CList<SistorCharacter> m_sistorCharList;
	BOOL m_sistorCharModFlag;//�����б��޸ı�־
	BOOL InitSistorCharFromFile();//���ļ��ж�ȡ����ʼ���Ự�����б�
	BOOL SaveSistorChar2File();//��������
	BOOL DisplaySistorCharInList();//��ʾ

	CList<CChatUnit*> m_chatUnitList;//ָ�뷽ʽ�����Զ��ͷ���Ӧ�ò���
	CChatUnit*m_currentChat;//��ǰ�Ự
	int m_curBtn;//��Ӧ��ǰ��ť��-1��ʾ��Ч(ȡֵΪ0-6)����������
	const static int INTERFACE_CHAT_NUM =7;
	int m_serialCoBtn[INTERFACE_CHAT_NUM];//�߸�btn�����ֵ��-1��ʾ��Ч��ע���ʼ��

	void SelectChat(int btnNum);//�����ťѡ���ض��Ự

	void SetSerialCoBtn(int start=0);
	CButton* m_pSrlBtn[INTERFACE_CHAT_NUM];
	void AddOneChat();//�½���һ�����ӣ����ӵ�������ȥ
	void DeleteOneChat(int pos);//ɾ����λ�õ�chat
	CChatUnit::ChatState GetChatState(int index)const ;//��öԻ����״̬
	CTalkSrvrSocket mSrvrListen;//ֻ�ṩ��������

	BOOL StartNetServer();//��ʼ�������
	BOOL InitTalkerSys();//��ʼ��Ӧ�öԻ�Ӧ�ó�����OnInitDialog()�е��á����ݰ�������������񣬳�ʼ���������ݵ�
public:
	afx_msg void OnBnClickedSendmsg();
	CEdit m_msgEdit;
	CString m_hostName;
	CString m_nextPeerName;
	void SendCurrentMsg(CString &sendStr);//��ʼ������Ϣ
	CEdit m_msgList;
	void RefleshMsgList();//����list,��currentChat���ָ��
	void RefleshBtnState();//���°�ť
	int GetChatCount()const;//���chat����
	CString& GetCurPeerIP()const;//��õ�ǰpeer��ip��ֵַ
	BOOL IsIPConnected(const CString & rIP)const;//��ѯ��ip�Ƿ��Ѿ�������

	afx_msg void OnBnClickedButtonF0();
	afx_msg void OnBnClickedButtonF1();
	afx_msg void OnBnClickedButtonF2();
	afx_msg void OnBnClickedButtonF3();
	afx_msg void OnBnClickedButtonF4();
	afx_msg void OnBnClickedButtonF5();
	afx_msg void OnBnClickedButtonF6();
	afx_msg void OnBnClickedClosechat();
	CHotKeyCtrl m_hotKey;
//	afx_msg void OnNMOutofmemoryHotkey1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonOptions();
	afx_msg void OnBnClickedButtonAddchar();
	CListBox m_sistorPool;
	afx_msg void OnSelchangeListSistorpool();
	afx_msg void OnDblclkListSistorpool();
	afx_msg void OnBnClickedButtonDelete();
};
