// RecvSendSocket.cpp : implementation file
//

#include "stdafx.h"
#include "Talk2LingSister.h"
#include "RecvSendSocket.h"
#include "ChatUnit.h"

/////////////////////CSockTeam�ĳ�Ա����///////////////////
CSockTeam::CSockTeam(){
	//����null
	recvFile=NULL;
	arIn=arOut=NULL;
}

CSockTeam::~CSockTeam(){
	//��null������delete
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
/////////////////////////// CRecvSendSocket//////////////////////��Ա����

CRecvSendSocket::CRecvSendSocket()
{
	m_pFatherChatUnit=NULL;
}

CRecvSendSocket::~CRecvSendSocket()
{
	m_pFatherChatUnit=NULL;//�������ڴ�����
}


// CRecvSendSocket member functions
void CRecvSendSocket::RichSockTeam(){//ʹsockTeam��Ч
	m_socketTeam.Create(this);//�����������
}
void CRecvSendSocket::OnReceive(int nErrorCode){
	CSocket::OnReceive(nErrorCode);

	if(!m_socketTeam.arIn->IsLoading()) return;
	CString cStr;
	*m_socketTeam.arIn>>cStr;
	if(cStr.IsEmpty()) return;
	if(((CChatUnit*)m_pFatherChatUnit)->m_chatSts!=CChatUnit::Chating)//��Ϊ���죬�ͱ�Ϊ����״̬
		((CChatUnit*)m_pFatherChatUnit)->m_chatSts=CChatUnit::Calling;//����״̬
	((CChatUnit*)m_pFatherChatUnit)->Add2MessagePool(cStr,CChatUnit::CLIENT);//���ӵ���Ϣ��¼��ȥ��Ĭ��Ϊ
}

void CRecvSendSocket::MyReceive(CString & cStr){//����
	if(!m_socketTeam.arIn->IsLoading()) return;
	*m_socketTeam.arIn>>cStr;	
	if(cStr.IsEmpty()) return;
	((CChatUnit*)m_pFatherChatUnit)->Add2MessagePool(cStr,CChatUnit::CLIENT);//���ӵ���Ϣ��¼��ȥ��Ĭ��Ϊ
}

BOOL CRecvSendSocket::MySend(CString & cStr){
	(*m_socketTeam.arOut)<<cStr;
	(*m_socketTeam.arOut).Flush();
	return TRUE;
}

void CRecvSendSocket::OnClose(int nErrorCode){	
	m_socketTeam.arOut->Close();
	m_socketTeam.arIn->Close();
	m_socketTeam.recvFile->Close();//�ر���

	CSocket::OnClose(nErrorCode);//��ܿ췴Ӧ�գ�����
	((CChatUnit*)m_pFatherChatUnit)->CloseChat();//�رոöԻ���

}