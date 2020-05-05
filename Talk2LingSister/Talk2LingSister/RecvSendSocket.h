#pragma once

//#include"ChatUnit.h"
//sockTeam组
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
	void RichSockTeam();//使sockTeam有效
	void MyReceive(CString & cStr);
	BOOL MySend(CString & cStr);
	void *m_pFatherChatUnit;//关联的父CChatUnit，以便于把数据上传
	CSockTeam m_socketTeam;//不需要指针，并未有实际开销
protected:
	
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//virtual void OnSend(
	
};


