//�Ự��Ԫ��֧�ֶ�Ự���ܵ�ǰ��

#pragma once

#include"RecvSendSocket.h"


class CChatUnit{
public:
	CChatUnit();
	~CChatUnit();
	CChatUnit& operator=(const CChatUnit&);//ָ����벻���ݣ���ֹ�������
	BOOL Create(CRecvSendSocket *a,CWnd*m_pFatherWnd);//����������������Ƚ϶࣬���һЩ״̬

	enum ChatState{NotUsing,Staticing,Calling,Chating};//��ǰ״̬
	ChatState m_chatSts;//

	const static int HOST=1;
	const static int CLIENT=2;
	void Add2MessagePool(CString&cont,int hostFlag=HOST);//�������
	void CloseChat();
	void SendMsg(CString &sendStr);//����������Ϣ
	CList<CString> m_MsgPoolList;

	void Delete();//�رղ���
	//��create�����г�ʼ��
	CString m_rPeerAddress;//�Է�IP
	UINT m_nPeerPort;//�Է�port
	CString m_peerName;
protected:
	CRecvSendSocket *m_pRecvSendSock;//ע���������������⣡����
	CWnd*m_pFatherWnd;//����Ӻζ�������srvr���ݣ�����
	
	//CWordsPool m_wordsPool;//��Ϣ��¼

private:
	
};