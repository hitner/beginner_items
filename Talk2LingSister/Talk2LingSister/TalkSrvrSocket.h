#pragma once
#include"ChatUnit.h"



// CTalkSrvrSocket command target

class CTalkSrvrSocket : public CSocket
{
public:
	CTalkSrvrSocket();
	//CTalkSrvrSocket(CTalkSrvrSocket &);
	virtual ~CTalkSrvrSocket();
	//���ص�OnAccept
	virtual void OnAccept(int nErrorCode);
	//CSockTeam m_socketTeam;//����Ҫָ�룬��δ��ʵ�ʿ���
	
	void SetFatherPoint(CWnd*,CList<CChatUnit*> *);
protected:
	CWnd*m_pFatherWnd;//�Լ����ã����ݸ�CChatUni
	CList<CChatUnit*> *m_pFatherChatList;//listernר��,���ﲻ��������
	//CChatUnit *m_pFatherChatUnit;//recvר��
	//int mChatID;//�ỰID�����ֻ����Ϊlisten������Ϊ-1����ʼΪ0������Ҳ������Ϊһ�����֣�
};



