#pragma once
#include"ChatUnit.h"



// CTalkSrvrSocket command target

class CTalkSrvrSocket : public CSocket
{
public:
	CTalkSrvrSocket();
	//CTalkSrvrSocket(CTalkSrvrSocket &);
	virtual ~CTalkSrvrSocket();
	//重载的OnAccept
	virtual void OnAccept(int nErrorCode);
	//CSockTeam m_socketTeam;//不需要指针，并未有实际开销
	
	void SetFatherPoint(CWnd*,CList<CChatUnit*> *);
protected:
	CWnd*m_pFatherWnd;//自己不用，传递给CChatUni
	CList<CChatUnit*> *m_pFatherChatList;//listern专用,这里不负责销毁
	//CChatUnit *m_pFatherChatUnit;//recv专用
	//int mChatID;//会话ID（如果只是作为listen，该项为-1，初始为0，这样也可以作为一个区分）
};



