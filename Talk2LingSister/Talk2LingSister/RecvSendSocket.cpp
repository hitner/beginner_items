// RecvSendSocket.cpp : implementation file
//

#include "stdafx.h"
#include "Talk2LingSister.h"
#include "RecvSendSocket.h"
#include "ChatUnit.h"

/////////////////////CSockTeam的成员函数///////////////////
CSockTeam::CSockTeam(){
	//都赋null
	recvFile=NULL;
	arIn=arOut=NULL;
}

CSockTeam::~CSockTeam(){
	//非null，都得delete
	if(recvFile) delete recvFile;
	if(arIn) delete arIn;
	if(arOut) delete arOut;
}

void CSockTeam::Create(CSocket*pSrvrRecv){
	recvFile=new CSocketFile(pSrvrRecv);
	//CSockTeam::recvFileIn=new CSocketFile(pSrvrRecv);
	arIn=new CArchive(recvFile ,CArchive::load);
	arOut=new CArchive(recvFile,CArchive::store);

	//char helo[6]="hello";
	//int a=0x11223344;
	//*arOut<<a;
}
/////////////////////////// CRecvSendSocket//////////////////////成员函数

CRecvSendSocket::CRecvSendSocket()
{
	m_pFatherChatUnit=NULL;
}

CRecvSendSocket::~CRecvSendSocket()
{
	m_pFatherChatUnit=NULL;//父辈不在此销毁
}


// CRecvSendSocket member functions
void CRecvSendSocket::RichSockTeam(){//使sockTeam有效
	m_socketTeam.Create(this);//三个类的生成
}
void CRecvSendSocket::OnReceive(int nErrorCode){
	CSocket::OnReceive(nErrorCode);

	if(!m_socketTeam.arIn->IsLoading()) return;
	CString cStr;
	*m_socketTeam.arIn>>cStr;
	if(cStr.IsEmpty()) return;
	if(((CChatUnit*)m_pFatherChatUnit)->m_chatSts!=CChatUnit::Chating)//不为聊天，就变为呼叫状态
		((CChatUnit*)m_pFatherChatUnit)->m_chatSts=CChatUnit::Calling;//呼叫状态
	((CChatUnit*)m_pFatherChatUnit)->Add2MessagePool(cStr,CChatUnit::CLIENT);//增加到消息记录中去，默认为
}

void CRecvSendSocket::MyReceive(CString & cStr){//废弃
	if(!m_socketTeam.arIn->IsLoading()) return;
	*m_socketTeam.arIn>>cStr;	
	if(cStr.IsEmpty()) return;
	((CChatUnit*)m_pFatherChatUnit)->Add2MessagePool(cStr,CChatUnit::CLIENT);//增加到消息记录中去，默认为
}

BOOL CRecvSendSocket::MySend(CString & cStr){
	(*m_socketTeam.arOut)<<cStr;
	(*m_socketTeam.arOut).Flush();
	return TRUE;
}

void CRecvSendSocket::OnClose(int nErrorCode){	
	m_socketTeam.arOut->Close();
	m_socketTeam.arIn->Close();
	m_socketTeam.recvFile->Close();//关闭先

	CSocket::OnClose(nErrorCode);//会很快反应勒！！！
	((CChatUnit*)m_pFatherChatUnit)->CloseChat();//关闭该对话框

}