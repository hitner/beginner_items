
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
	afx_msg LRESULT OnLingCalled(WPARAM wParam,LPARAM lParam);//一个请求
	afx_msg LRESULT OnLingSayed(WPARAM wParam,LPARAM lParam);//说话更新
	afx_msg LRESULT OnHotKey(WPARAM wParam,LPARAM lParam);//快捷键，重载之t

private:
	int nPort;
	CList<SistorCharacter> m_sistorCharList;
	BOOL m_sistorCharModFlag;//属性列表修改标志
	BOOL InitSistorCharFromFile();//从文件中读取并初始化会话对象列表
	BOOL SaveSistorChar2File();//保存数据
	BOOL DisplaySistorCharInList();//显示

	CList<CChatUnit*> m_chatUnitList;//指针方式，会自动释放吗？应该不会
	CChatUnit*m_currentChat;//当前会话
	int m_curBtn;//对应当前按钮，-1表示无效(取值为0-6)是数组的序号
	const static int INTERFACE_CHAT_NUM =7;
	int m_serialCoBtn[INTERFACE_CHAT_NUM];//七个btn里面的值，-1表示无效，注意初始化

	void SelectChat(int btnNum);//点击按钮选择特定会话

	void SetSerialCoBtn(int start=0);
	CButton* m_pSrlBtn[INTERFACE_CHAT_NUM];
	void AddOneChat();//新建了一个连接，增加到界面中去
	void DeleteOneChat(int pos);//删除该位置的chat
	CChatUnit::ChatState GetChatState(int index)const ;//获得对话框的状态
	CTalkSrvrSocket mSrvrListen;//只提供监听服务

	BOOL StartNetServer();//开始网络服务
	BOOL InitTalkerSys();//初始化应用对话应用程序，在OnInitDialog()中调用。内容包括设置网络服务，初始化所有数据等
public:
	afx_msg void OnBnClickedSendmsg();
	CEdit m_msgEdit;
	CString m_hostName;
	CString m_nextPeerName;
	void SendCurrentMsg(CString &sendStr);//开始发送消息
	CEdit m_msgList;
	void RefleshMsgList();//更新list,用currentChat这个指针
	void RefleshBtnState();//更新按钮
	int GetChatCount()const;//获得chat个数
	CString& GetCurPeerIP()const;//获得当前peer的ip地址值
	BOOL IsIPConnected(const CString & rIP)const;//查询该ip是否已经连接了

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
