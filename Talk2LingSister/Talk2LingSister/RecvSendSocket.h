#pragma once

//#include"ChatUnit.h"
//sockTeam��
class CSockTeam{
public:
	CSockTeam();
	~CSockTeam();
	
	void Create(CSocket*pSrvrRecv);
	CSocketFile* recvFile;
	CArchive* arIn;
	CArchive*arOut;
};


// CRecvSendSocket command target

class CRecvSendSocket : public CSocket
{
public:
	CRecvSendSocket();
	virtual ~CRecvSendSocket();
	void RichSockTeam();//ʹsockTeam��Ч
	void MyReceive(CString & cStr);
	BOOL MySend(CString & cStr);
	void *m_pFatherChatUnit;//�����ĸ�CChatUnit���Ա��ڰ������ϴ�
	CSockTeam m_socketTeam;//����Ҫָ�룬��δ��ʵ�ʿ���
protected:
	
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//virtual void OnSend(
	
};


