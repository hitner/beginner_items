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


CChatUnit& CChatUnit::operator=(const CChatUnit&org){//ָ����벻���ݣ���ֹ�������
	if(m_pRecvSendSock) {
		delete m_pRecvSendSock;
		m_pRecvSendSock=NULL;
	}
	//������һ�¸�
	m_pFatherWnd=org.m_pFatherWnd;
	//MassageBox(
	ASSERT(1);
	return *this;
}


BOOL CChatUnit::Create(CRecvSendSocket * p,CWnd*pFatherWnd){
	//����������������Ƚ϶࣬���һЩ״̬
	m_pRecvSendSock=p;//�ײ�sock����
	m_pFatherWnd=pFatherWnd;
	//��ȡIP�������Ĺ���
	m_chatSts=Staticing;

	m_pRecvSendSock->GetPeerName(m_rPeerAddress,m_nPeerPort);
	//p->m_pFatherChatUnit=this;//����
	//m_peerName=peerName;//�������
	//���������ӵ���Ϣ����ʾΪ������Ϣ��
	m_pFatherWnd->PostMessageW(WM_LINGCALLED);//ע����Ϣ������
	return TRUE;
}

//������Ϣ����
void CChatUnit::Add2MessagePool(CString&cont,int hostFlag){//�������
	if(cont.IsEmpty()) return;

	//�������
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

	//���͸�����Ϣ�������в������԰�
	WPARAM wParam=0U;
	if(m_chatSts!=Chating) wParam=1;
	m_pFatherWnd->PostMessageW(WM_LINGSAYED,wParam);//ע����Ϣ������
}

//������Ϣ�����Ƕ�����һ��һ�����´���
void CChatUnit::SendMsg(CString &sendStr){
	m_pRecvSendSock->MySend(sendStr);
	Add2MessagePool(sendStr);

}
void CChatUnit::CloseChat(){
	delete m_pRecvSendSock;
	m_pRecvSendSock=NULL;
	m_rPeerAddress.Empty();//�Է�IPҲΪ��
	m_nPeerPort=0;
	m_chatSts=NotUsing;//״̬Ϊʧȥ����(���֪ͨ���棬��ʱ���ܣ�

}

void CChatUnit::Delete(){//�رղ���
	if(m_pRecvSendSock==NULL) return;//�Ѿ�ɾ������
	m_pRecvSendSock->m_socketTeam.arIn->Close();
	m_pRecvSendSock->m_socketTeam.arOut->Close();
	m_pRecvSendSock->m_socketTeam.recvFile->Close();
	m_pRecvSendSock->Close();
	delete m_pRecvSendSock;
	m_pRecvSendSock=NULL;
}