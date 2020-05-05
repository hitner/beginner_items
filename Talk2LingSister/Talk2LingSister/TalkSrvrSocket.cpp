// TalkSrvrSocket.cpp : implementation file
//

#include "stdafx.h"
#include "Talk2LingSister.h"
#include "TalkSrvrSocket.h"
#include"RecvSendSocket.h"






// CTalkSrvrSocket

CTalkSrvrSocket::CTalkSrvrSocket()
{
	m_pFatherChatList=NULL;
	m_pFatherWnd=NULL;
}

CTalkSrvrSocket::~CTalkSrvrSocket()
{
}

// CTalkSrvrSocket member functions

void CTalkSrvrSocket::SetFatherPoint(CWnd*pWnd,CList<CChatUnit*> *pChatList){
	m_pFatherWnd=pWnd;
	m_pFatherChatList=pChatList;
}


void CTalkSrvrSocket::OnAccept(int nErrorCode){
	CSocket::OnAccept(nErrorCode);

	//1,��ͼ����
	CRecvSendSocket *pRecvSendSocket=new CRecvSendSocket();
	if(0==Accept(*pRecvSendSocket)){//��������
		delete pRecvSendSocket;
		pRecvSendSocket=NULL;
		//MessageBox(
		ASSERT(1);
		return;
	}
	pRecvSendSocket->RichSockTeam();//�ú�������file��CArchive

	//2����ӵ�CChatUnit
	CChatUnit* pTempChatUnit=new CChatUnit();
	
	if(TRUE==pTempChatUnit->Create(pRecvSendSocket,m_pFatherWnd)){
		pRecvSendSocket->m_pFatherChatUnit=pTempChatUnit;
		m_pFatherChatList->AddTail(pTempChatUnit);
	}
	else {
		delete pRecvSendSocket;
		delete pTempChatUnit;
	}
	//pSrvrRecvSocket->m_socketTeam->
}
