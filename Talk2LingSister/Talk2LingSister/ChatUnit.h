//会话单元，支持多会话功能的前提

#pragma once

#include"RecvSendSocket.h"


class CChatUnit{
public:
	CChatUnit();
	~CChatUnit();
	CChatUnit& operator=(const CChatUnit&);//指针必须不传递，防止多层析构
	BOOL Create(CRecvSendSocket *a,CWnd*m_pFatherWnd);//创建可能做的事情比较多，获得一些状态

	enum ChatState{NotUsing,Staticing,Calling,Chating};//当前状态
	ChatState m_chatSts;//

	const static int HOST=1;
	const static int CLIENT=2;
	void Add2MessagePool(CString&cont,int hostFlag=HOST);//添加内容
	void CloseChat();
	void SendMsg(CString &sendStr);//主动发送消息
	CList<CString> m_MsgPoolList;

	void Delete();//关闭操作
	//在create过程中初始化
	CString m_rPeerAddress;//对方IP
	UINT m_nPeerPort;//对方port
	CString m_peerName;
protected:
	CRecvSendSocket *m_pRecvSendSock;//注意析构函数的问题！！！
	CWnd*m_pFatherWnd;//这个从何而来，又srvr传递！！！
	
	//CWordsPool m_wordsPool;//消息记录

private:
	
};