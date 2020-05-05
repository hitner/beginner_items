#include"stdafx.h"
#include"ChatUnit.h"
//#include"Talk2LingSisterDlg.h"
/*
class CChatUnit{
public:
	CChatUnit();
	~CChatUnit();
	
	CTalkSrvrSocket *m_srvrRecvSock;

};


*/

CChatUnit::CChatUnit(){
	m_pRecvSendSock=NULL;
	m_pFatherWnd=NULL;
	m_peerName=_T("She/He");
}
CChatUnit::~CChatUnit(){
	if(m_pRecvSendSock) delete m_pRecvSendSock;
	m_pRecvSendSock=NULL;
	m_pFatherWnd=NULL;
}


CChatUnit& CChatUnit::operator=(const CChatUnit&org){//指针必须不传递，防止多层析构
	if(m_pRecvSendSock) {
		delete m_pRecvSendSock;
		m_pRecvSendSock=NULL;
	}
	//其他的一致赋
	m_pFatherWnd=org.m_pFatherWnd;
	//MassageBox(
	ASSERT(1);
	return *this;
}


BOOL CChatUnit::Create(CRecvSendSocket * p,CWnd*pFatherWnd){
	//创建可能做的事情比较多，获得一些状态
	m_pRecvSendSock=p;//底层sock有了
	m_pFatherWnd=pFatherWnd;
	//获取IP等其他的工作
	m_chatSts=Staticing;

	m_pRecvSendSock->GetPeerName(m_rPeerAddress,m_nPeerPort);
	//p->m_pFatherChatUnit=this;//互联
	//m_peerName=peerName;//获得名称
	//发送已连接的消息（表示为请求消息）
	m_pFatherWnd->PostMessageW(WM_LINGCALLED);//注意消息的设置
	return TRUE;
}

//增加消息内容
void CChatUnit::Add2MessagePool(CString&cont,int hostFlag){//添加内容
	if(cont.IsEmpty()) return;

	//添加内容
	CTime time(CTime::GetCurrentTime());
	CString tmStr=time.Format(_T("%H:%M:%S"));

	CString hostName;
	if(hostFlag==CChatUnit::HOST) hostName=_T("Me");//((CTalk2LingSisterDlg*)m_pFatherWnd)->m_hostName;
	else hostName=m_peerName;

	hostName+=_T("\t");
	hostName+=tmStr;
	
	m_MsgPoolList.AddTail(hostName);
	cont.Replace(_T("\r\n"),_T("\r\n    "));
	m_MsgPoolList.AddTail(_T("    ")+cont);

	//发送更新消息，必须有参数，对吧
	WPARAM wParam=0U;
	if(m_chatSts!=Chating) wParam=1;
	m_pFatherWnd->PostMessageW(WM_LINGSAYED,wParam);//注意消息的设置
}

//发送消息，不是动作，一级一级往下传递
void CChatUnit::SendMsg(CString &sendStr){
	m_pRecvSendSock->MySend(sendStr);
	Add2MessagePool(sendStr);

}
void CChatUnit::CloseChat(){
	delete m_pRecvSendSock;
	m_pRecvSendSock=NULL;
	m_rPeerAddress.Empty();//对方IP也为空
	m_nPeerPort=0;
	m_chatSts=NotUsing;//状态为失去连接(如何通知桌面，暂时不管）

}

void CChatUnit::Delete(){//关闭操作
	if(m_pRecvSendSock==NULL) return;//已经删除过了
	m_pRecvSendSock->m_socketTeam.arIn->Close();
	m_pRecvSendSock->m_socketTeam.arOut->Close();
	m_pRecvSendSock->m_socketTeam.recvFile->Close();
	m_pRecvSendSock->Close();
	delete m_pRecvSendSock;
	m_pRecvSendSock=NULL;
}