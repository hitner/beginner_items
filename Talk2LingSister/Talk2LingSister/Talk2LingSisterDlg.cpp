
// Talk2LingSisterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Talk2LingSister.h"
#include "Talk2LingSisterDlg.h"
#include "afxdialogex.h"
#include"RecvSendSocket.h"
#include <mmsystem.h>
#include <windows.h>
#pragma comment(lib, "WINMM.LIB")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTalk2LingSisterDlg dialog




CTalk2LingSisterDlg::CTalk2LingSisterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTalk2LingSisterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_currentChat=NULL;//Ĭ��Ϊû��
}

void CTalk2LingSisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSGEDIT, m_msgEdit);
	DDX_Control(pDX, IDC_MESSAGELIST, m_msgList);
	//DDX_Control(pDX, IDC_HOTKEY1, m_hotKey);
	DDX_Control(pDX, IDC_LIST_SISTORPOOL, m_sistorPool);
}

BEGIN_MESSAGE_MAP(CTalk2LingSisterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT, &CTalk2LingSisterDlg::OnBnClickedConnect)
	ON_MESSAGE(WM_LINGCALLED,OnLingCalled)
	ON_MESSAGE(WM_LINGSAYED,OnLingSayed)
	ON_MESSAGE(WM_HOTKEY,OnHotKey)
	ON_BN_CLICKED(IDC_SENDMSG, &CTalk2LingSisterDlg::OnBnClickedSendmsg)
	ON_BN_CLICKED(IDC_BUTTON_F0, &CTalk2LingSisterDlg::OnBnClickedButtonF0)
	ON_BN_CLICKED(IDC_BUTTON_F1, &CTalk2LingSisterDlg::OnBnClickedButtonF1)
	ON_BN_CLICKED(IDC_BUTTON_F2, &CTalk2LingSisterDlg::OnBnClickedButtonF2)
	ON_BN_CLICKED(IDC_BUTTON_F3, &CTalk2LingSisterDlg::OnBnClickedButtonF3)
	ON_BN_CLICKED(IDC_BUTTON_F4, &CTalk2LingSisterDlg::OnBnClickedButtonF4)
	ON_BN_CLICKED(IDC_BUTTON_F5, &CTalk2LingSisterDlg::OnBnClickedButtonF5)
	ON_BN_CLICKED(IDC_BUTTON_F6, &CTalk2LingSisterDlg::OnBnClickedButtonF6)
	ON_BN_CLICKED(IDC_CLOSECHAT, &CTalk2LingSisterDlg::OnBnClickedClosechat)
	//ON_NOTIFY(NM_OUTOFMEMORY, IDC_HOTKEY1, &CTalk2LingSisterDlg::OnNMOutofmemoryHotkey1)
	ON_BN_CLICKED(IDCANCEL, &CTalk2LingSisterDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_OPTIONS, &CTalk2LingSisterDlg::OnBnClickedButtonOptions)
	ON_BN_CLICKED(IDC_BUTTON_ADDCHAR, &CTalk2LingSisterDlg::OnBnClickedButtonAddchar)
	ON_LBN_SELCHANGE(IDC_LIST_SISTORPOOL, &CTalk2LingSisterDlg::OnSelchangeListSistorpool)
	ON_LBN_DBLCLK(IDC_LIST_SISTORPOOL, &CTalk2LingSisterDlg::OnDblclkListSistorpool)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CTalk2LingSisterDlg::OnBnClickedButtonDelete)
END_MESSAGE_MAP()


// CTalk2LingSisterDlg message handlers

BOOL CTalk2LingSisterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	InitTalkerSys();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTalk2LingSisterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTalk2LingSisterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTalk2LingSisterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//��ʼ��Ӧ�öԻ�Ӧ�ó�����OnInitDialog()�е��á����ݰ�������������񣬳�ʼ���������ݵ�
BOOL CTalk2LingSisterDlg::InitTalkerSys(){

//���ÿ�ݼ�
	::RegisterHotKey(GetSafeHwnd(),m_HotKeyID,MOD_CONTROL,VK_RETURN );
//��ȡ�����ļ�����ʼ�������������
	m_hostName=_T("LZZ");
	CTalk2LingSisterDlg::m_msgEdit.SetLimitText(-1);
	//��ϵ����������
	InitSistorCharFromFile();
	//��ʾ������
	DisplaySistorCharInList();
//��Ӧ
	m_curBtn=-1;
	for(int i=0;i<INTERFACE_CHAT_NUM;i++)
		m_serialCoBtn[i]=-1;

	m_pSrlBtn[0]=(CButton*)CTalk2LingSisterDlg::GetDlgItem(IDC_BUTTON_F0);
	m_pSrlBtn[1]=(CButton*)CTalk2LingSisterDlg::GetDlgItem(IDC_BUTTON_F1);
	m_pSrlBtn[2]=(CButton*)CTalk2LingSisterDlg::GetDlgItem(IDC_BUTTON_F2);
	m_pSrlBtn[3]=(CButton*)CTalk2LingSisterDlg::GetDlgItem(IDC_BUTTON_F3);
	m_pSrlBtn[4]=(CButton*)CTalk2LingSisterDlg::GetDlgItem(IDC_BUTTON_F4);
	m_pSrlBtn[5]=(CButton*)CTalk2LingSisterDlg::GetDlgItem(IDC_BUTTON_F5);
	m_pSrlBtn[6]=(CButton*)CTalk2LingSisterDlg::GetDlgItem(IDC_BUTTON_F6);
//Peer IP ��ַ���ɱ༭�� ���԰ɣ�����;���Բ�������ΰ��Ľ����Ϊ�˼�������ȥ��ͯ�꣬��Ҫ׼����ȥ�Ⱦ� 

//��ʼ�������
	StartNetServer();
	return TRUE;
}


void wirte2newFile(){
	CFile fileW(_T("sistorsChar.bin"),CFile::modeWrite|CFile::modeCreate);
		CArchive arStore(&fileW,CArchive::store);
		int m=0;
		arStore<<m;
		arStore.Flush();
		arStore.Close();
}
//���ļ��ж�ȡ����ʼ���Ự�����б�
BOOL CTalk2LingSisterDlg::InitSistorCharFromFile(){
	/*wirte2newFile();
	CFile file;
	if(!file.Open(_T("sistorsChar.bin"),CFile::modeRead|CFile::modeCreate|CFile::modeNoTruncate))
		MessageBox(_T("damm it"));
	CArchive *ar=new CArchive(&file,CArchive::load);
	int i;
	*ar>>i;
	ar->Flush();
	i++;*/
	//CArchive *ar;
	CFile file;
	if(!file.Open(_T("sistorChar.bin"),CFile::modeRead|CFile::modeCreate|CFile::modeNoTruncate)){
		MessageBox(_T("damm it"));
		return FALSE;
	}

	if(file.GetLength()==0){//�ļ�Ϊ��
		file.Close();
		return FALSE;
	}
	else {
		CArchive ar(&file,CArchive::load);
		//�������
		int total=0;
		if(ar.IsLoading()) {
			ar>>total;
			ar.Flush();
		}
		if(total<=0||total>255) return FALSE; //�������

		SistorCharacter * pSistorChar;
		for(;total>0;total--){
			pSistorChar=new SistorCharacter();
			pSistorChar->Enrich(ar);//���
			m_sistorCharList.AddTail(*pSistorChar);
		}
	
		return TRUE;
	}
}


//��������
BOOL CTalk2LingSisterDlg::SaveSistorChar2File(){
	//��ó���
	INT_PTR sum=m_sistorCharList.GetCount();

	CFile file(_T("sistorChar.bin"),CFile::modeWrite|CFile::modeCreate);
	CArchive ar(&file,CArchive::store);

	if(sum>0) ar<<sum;

	for(int i=0;i<sum;i++){
		(m_sistorCharList.GetAt(m_sistorCharList.FindIndex(i))).Sddtore(ar);
	}
	ar.Close();
	return TRUE;
}


BOOL CTalk2LingSisterDlg::DisplaySistorCharInList(){
	//��ó���
	INT_PTR sum=m_sistorCharList.GetCount();

	for(int i=0;i<sum;i++){
		CTalk2LingSisterDlg::m_sistorPool.AddString((m_sistorCharList.GetAt(m_sistorCharList.FindIndex(i))).GetDisplayStr());
	}
	return TRUE;
}
BOOL CTalk2LingSisterDlg::StartNetServer(){//��ʼ�������
	//��ʱ�˿ں�
	nPort=7895;
	//
	mSrvrListen.SetFatherPoint(this,&m_chatUnitList);//���ñ�Ҫ�ĸ�����ָ�������list
	//CSocket sockSrvr;
	mSrvrListen.Create(nPort);
	mSrvrListen.Listen();

	//CSocket sockSrvrRecv;
	//mSrvrListen.Accept(sockSrvrRecv);
	//CSocketFile sockSrvrFile(&sockSrvrRecv);
	//CArchive srvrArIn(&sockSrvrFile,CArchive::load);
	//sockSrvrRecv.onr

	return TRUE;
}
void CTalk2LingSisterDlg::OnBnClickedConnect()
{
	// TODO: Add your control notification handler code here
	
	//1,�ж�ip���Ƿ���д������д
	DWORD dwIP=0;
	CIPAddressCtrl*pIPCtrlWnd=(CIPAddressCtrl*)GetDlgItem(IDC_SRVR_IPADDRESS);
	if(4!=pIPCtrlWnd->GetAddress(dwIP)){
		MessageBox(_T("IP not intact"));
		return;
	}
	//2,���IP
	CString ipStr;
	pIPCtrlWnd->GetWindowTextW(ipStr);
	//3,�жϸ�IP�Ƿ��Ѿ�������
	if(TRUE==IsIPConnected(ipStr)){
		MessageBox(_T("Already connected to this IP friend!"));
		return;
	}

	//4,�������Ӵ���
	CRecvSendSocket*pTemp=new CRecvSendSocket();//����socket
	pTemp->Create();
	if(0==pTemp->Connect(LPCTSTR(ipStr),nPort)) {//����û�гɹ�
		delete pTemp;
		pTemp=NULL;
		MessageBox(_T("cann't connect to this IP server!"));
		return;
	}	
	pTemp->RichSockTeam();//�ú�������file��CArchive

	//5 new CChatUnit��ӵ�list
	CChatUnit* pTempChatUnit=new CChatUnit();
	pTempChatUnit->m_peerName=m_nextPeerName;//ֱ�ӻ������
	//pTempChatUnit->Create(pTemp,this); //�ᷢ�������ѳɹ���Ϣ�������ﲻ��addoneChat
	if(TRUE==pTempChatUnit->Create(pTemp,this)){
		pTemp->m_pFatherChatUnit=pTempChatUnit;//pTemp��CChatUnit��˫��ָ��
	}
	m_chatUnitList.AddTail(pTempChatUnit);
}

BOOL CTalk2LingSisterDlg::IsIPConnected(const CString & rIP)const{//��ѯ��ip�Ƿ��Ѿ�������
	//int totalChat=GetChatCount()
	//for(int i=0;i<GetChatCount();i++){

	POSITION pos=CTalk2LingSisterDlg::m_chatUnitList.GetHeadPosition();
	if(pos==NULL) return FALSE;

	if(0==m_chatUnitList.GetAt(pos)->m_rPeerAddress.Compare(rIP)) return TRUE;
	//��Ԫ��
	CString tmp=m_chatUnitList.GetNext(pos)->m_rPeerAddress;

	for(;pos!=NULL;tmp=m_chatUnitList.GetNext(pos)->m_rPeerAddress){
		if(tmp.Compare(rIP)==0) return TRUE;

	}
	if(tmp.Compare(rIP)==0) return TRUE;//���һ��Ԫ��Ҳ�ñȽ�

	return FALSE;//һֱû��ƥ�䣬��û��

}


LRESULT CTalk2LingSisterDlg::OnLingCalled(WPARAM wParam,LPARAM lParam){
	//�����ѽ���
	AddOneChat();
	PlaySound(MAKEINTRESOURCE(IDR_WAVE_CALL),AfxGetResourceHandle(), SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);
	MessageBox(_T("�����ѽ�����"));
	return NULL;
}



LRESULT CTalk2LingSisterDlg::OnLingSayed(WPARAM wParam,LPARAM lParam){

	//��������ʱ�������������
	if(wParam==0)//��ǰ�Ự����ť�������
		RefleshMsgList();
	else
		//��0��ʾ�����������Ự�е�
		RefleshBtnState();
	
	//��������
	if(CTalk2LingSisterDlg::GetSafeHwnd()!=::GetForegroundWindow())
		PlaySound(MAKEINTRESOURCE(IDR_WAVE1),AfxGetResourceHandle(), SND_ASYNC|SND_RESOURCE|SND_NODEFAULT); 
	//MessageBox(_T("���յ�����"));
	return NULL;
}

void CTalk2LingSisterDlg::OnBnClickedSendmsg()
{
	// TODO: Add your control notification handler code here
	CString sendStr;
	m_msgEdit.GetWindowTextW(sendStr);
	if(sendStr.IsEmpty()) return;

	//���͸��ַ���
	SendCurrentMsg(sendStr);
}


LRESULT CTalk2LingSisterDlg::OnHotKey(WPARAM wParam,LPARAM lParam){
	OnBnClickedSendmsg();
	return NULL;
}

void CTalk2LingSisterDlg::SendCurrentMsg(CString &sendStr){//��ʼ������Ϣ
	if(m_currentChat==NULL){
		MessageBox(_T("no conversation"));
		return;
	}
	if(m_currentChat->m_chatSts==CChatUnit::NotUsing){
		MessageBox(_T("ʧȥ����"));
		return;
	}
	m_currentChat->SendMsg(sendStr);
	//���
	m_msgEdit.SetSel(0,-1);
	m_msgEdit.Clear();

}


void CTalk2LingSisterDlg::RefleshMsgList(){//����list
	 CChatUnit*pChat=m_currentChat;
	//CString cstr(_T("kkk"));
	 CString allStr;
	 m_msgList.Clear();
	 if(pChat!=NULL) {
	for(int i=0;i<pChat->m_MsgPoolList.GetCount();i++){
		(pChat->m_MsgPoolList).GetHead();
		allStr+=((pChat->m_MsgPoolList).GetAt((pChat->m_MsgPoolList).FindIndex(i)));
		allStr+=_T("\r\n");
	}
	 }
	m_msgList.SetWindowTextW(allStr);
	m_msgList.LineScroll(m_msgList.GetLineCount());
		
	//pChat->m_MsgPoolList.GetAt(


}

void CTalk2LingSisterDlg::AddOneChat(){
	//�½���һ�����ӣ����ӵ�������ȥ
	if(m_curBtn==-1){//��δ������Ŀ,������Ҫ��ָ��
		m_curBtn=0;
		m_serialCoBtn[0]=0;
		m_currentChat=m_chatUnitList.GetHead();
		m_currentChat->m_chatSts=CChatUnit::Chating;
		RefleshMsgList();//ˢ������
	}
	else{
	//��������
		if(m_serialCoBtn[INTERFACE_CHAT_NUM-1]==-1){
			//���滹δ����7��
			ASSERT(GetChatCount()!=0);//Ϊ0ʱ�쳣

			m_serialCoBtn[GetChatCount()-1]=GetChatCount()-1;
		}
		else{
			//�ݲ�����

		}

	}
	//ˢ�°�ť����
	RefleshBtnState();
}
void CTalk2LingSisterDlg::DeleteOneChat(int pos){//ɾ����λ�õ�chat��ֻɾ������
	if(pos>=GetChatCount())  MessageBox(_T("error delete wrong!"));

	//
	CChatUnit*delObj=m_chatUnitList.GetAt(m_chatUnitList.FindIndex(pos));
	delObj->Delete();
	delete delObj;//m_chatUnitList.GetAt(m_chatUnitList.FindIndex(pos));
	m_chatUnitList.RemoveAt(m_chatUnitList.FindIndex(pos));
}

int CTalk2LingSisterDlg::GetChatCount()const{
	return m_chatUnitList.GetCount();
}
void CTalk2LingSisterDlg::RefleshBtnState(){
	CString btnText;
	for(int i=0;i<INTERFACE_CHAT_NUM;i++){
		if(m_serialCoBtn[i]==-1) {
			//���أ�һ�����ֲ����ã�˵���Ѿ���ͷ��
			m_pSrlBtn[i]->ShowWindow(SW_HIDE);
			break;
		}
		else{
			//1,����,����
			m_pSrlBtn[i]->EnableWindow(1);
			m_pSrlBtn[i]->ShowWindow(SW_NORMAL);
			//2������Ϣ����ָʾ
			if(GetChatState(m_serialCoBtn[i])==CChatUnit::Calling){
				//�ȷ�˵������ɫ�����ı�
				m_pSrlBtn[i]->GetWindowTextW(btnText);
				if(0>=btnText.Find(_T('.'))){//û�С�������
					btnText+=_T("...");
					m_pSrlBtn[i]->SetWindowTextW(btnText);
				}
			}
			else{//��calling
				m_pSrlBtn[i]->GetWindowTextW(btnText);
				if(0<btnText.Find(_T('.'))){//��
					m_pSrlBtn[i]->SetWindowTextW(btnText.Left(2));
				}
			}
		}
	}
	//�Ƿ�������,���Ҳ�����
	if(m_curBtn!=-1){
		//m_pSrlBtn[m_curBtn]->setclic(1);
		m_pSrlBtn[m_curBtn]->EnableWindow(0);
	}


	//����peer ip��ַ
	CTalk2LingSisterDlg::SetDlgItemTextW(IDC_CURPEER_IPADDRESS,GetCurPeerIP());
}

CString& CTalk2LingSisterDlg::GetCurPeerIP()const{//��õ�ǰpeer��ip��ֵַ
	if(m_currentChat==NULL) return CString();

	return m_currentChat->m_rPeerAddress;

}

//���ָ��λ�öԻ����״̬
CChatUnit::ChatState CTalk2LingSisterDlg::GetChatState(int index)const {
	return m_chatUnitList.GetAt(m_chatUnitList.FindIndex(index))->m_chatSts;
}

void CTalk2LingSisterDlg::OnBnClickedButtonF0()
{
	// TODO: Add your control notification handler code here
	SelectChat(0);
}


void CTalk2LingSisterDlg::OnBnClickedButtonF1()
{
	// TODO: Add your control notification handler code here
	SelectChat(1);

}


void CTalk2LingSisterDlg::OnBnClickedButtonF2()
{
	// TODO: Add your control notification handler code here
	SelectChat(2);
}


void CTalk2LingSisterDlg::OnBnClickedButtonF3()
{
	// TODO: Add your control notification handler code here
	SelectChat(3);

}


void CTalk2LingSisterDlg::OnBnClickedButtonF4()
{
	// TODO: Add your control notification handler code here
	SelectChat(4);
}


void CTalk2LingSisterDlg::OnBnClickedButtonF5()
{
	// TODO: Add your control notification handler code here
	SelectChat(5);
}


void CTalk2LingSisterDlg::OnBnClickedButtonF6()
{
	// TODO: Add your control notification handler code here
	SelectChat(6);
}

void CTalk2LingSisterDlg::SelectChat(int btnNum){
	//ѡ����Ч��---���ع��У�
	ASSERT(m_curBtn!=btnNum);	
	if(m_serialCoBtn[btnNum]==-1) ASSERT(0);
	//1,����ָ��
	m_chatUnitList.GetAt(m_chatUnitList.FindIndex(m_serialCoBtn[m_curBtn]))->m_chatSts=CChatUnit::Staticing;//ԭָ���Ϊ��̬

	m_curBtn=btnNum;
	m_currentChat=m_chatUnitList.GetAt(m_chatUnitList.FindIndex(m_serialCoBtn[m_curBtn]));
	m_currentChat->m_chatSts=CChatUnit::Chating;
	//2��ˢ����ʾ
	RefleshBtnState();
	RefleshMsgList();
}
void CTalk2LingSisterDlg::OnBnClickedClosechat()
{
	// TODO: Add your control notification handler code here
	if(m_curBtn!=-1){
		int deleteObj=m_serialCoBtn[m_curBtn];
		//�رղ���
		DeleteOneChat(deleteObj );//ɾ��CChatUnit
		//�����ˢ��
		//1��������m_serialBoBtn
		SetSerialCoBtn(m_serialCoBtn[0]);//�ӵ�һ����ſ�ʼ

		m_curBtn=-1;
		//2,����µĶԻ�Ȩ
		//�׸�call
		if(GetChatCount()>0){
		for(int i=m_serialCoBtn[0];i<GetChatCount();i++){
			if(m_chatUnitList.GetAt(m_chatUnitList.FindIndex(i))->m_chatSts==CChatUnit::Calling) {
				m_curBtn=i-m_serialCoBtn[0];
				break;
			}
			
		}//�׸�static
		if(m_curBtn==-1){
			for(int i=m_serialCoBtn[0];i<GetChatCount();i++)
				if(m_chatUnitList.GetAt(m_chatUnitList.FindIndex(i))->m_chatSts==CChatUnit::Staticing){
					m_curBtn=i-m_serialCoBtn[0];
					break;
				}
		}
		//�׸�notusing
		if(m_curBtn==-1){
			for(int i=m_serialCoBtn[0];i<GetChatCount();i++)
				if(m_chatUnitList.GetAt(m_chatUnitList.FindIndex(i))->m_chatSts==CChatUnit::NotUsing){
					m_curBtn=i-m_serialCoBtn[0];
					break;
				}
		}
		}
		//3,����ָ��
		if(m_curBtn==-1) m_currentChat=NULL;
		else m_currentChat=m_chatUnitList.GetAt(m_chatUnitList.FindIndex(m_serialCoBtn[m_curBtn]));
		RefleshBtnState();
		RefleshMsgList();
		//if(deleteObj GetChatCount()
		//�ҵ���һ��calling�ı��

	}
}
void CTalk2LingSisterDlg::SetSerialCoBtn(int start){//��Ĭ�ϲ���Ϊ0
	int total=GetChatCount();
	if(start==0||start+INTERFACE_CHAT_NUM<=GetChatCount()){//����װ����
		for(int i=0;i<INTERFACE_CHAT_NUM;i++){
			if(start+i<total) m_serialCoBtn[i]=start+i;
			else m_serialCoBtn[i]=-1;
		}
	}
	else{
		//���ܴ�start��ʼ,�����ٿ�ʼ
		int temp=GetChatCount()-INTERFACE_CHAT_NUM;
		start=temp>=0?temp:0;//����Ϊ��
		for(int i=0;i<INTERFACE_CHAT_NUM;i++){
			if(start+i<total) m_serialCoBtn[i]=start+i;
			else m_serialCoBtn[i]=-1;
		}
	}
}

//void CTalk2LingSisterDlg::OnNMOutofmemoryHotkey1(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}


void CTalk2LingSisterDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	//��������
	SaveSistorChar2File();

	::UnregisterHotKey(GetSafeHwnd(),m_HotKeyID);
	CDialogEx::OnCancel();
}


void CTalk2LingSisterDlg::OnBnClickedButtonOptions()
{
	// TODO: Add your control notification handler code here
	//PlaySound(MAKEINTRESOURCE(IDR_WAVE1),AfxGetResourceHandle(), SND_ASYNC|SND_RESOURCE|SND_NODEFAULT); 


}


void CTalk2LingSisterDlg::OnBnClickedButtonAddchar()
{
	// TODO: Add your control notification handler code here
	//1,�ж�ip���Ƿ���д������д
	DWORD dwIP=0;
	CIPAddressCtrl*pIPCtrlWnd=(CIPAddressCtrl*)GetDlgItem(IDC_CURPEER_IPADDRESS);
	if(4!=pIPCtrlWnd->GetAddress(dwIP)){
		MessageBox(_T("IP not intact"));
		return;
	}
	//2,���IP
	CString ipStr;
	pIPCtrlWnd->GetWindowTextW(ipStr);
	//3,����ҲҪ��
	CString charName;
	GetDlgItemTextW(IDC_EDIT_CHARNAME,charName);
	if(charName.IsEmpty()){
		MessageBox(_T("sign a name ,please"));
		return;
	}
	//4��ӵ�CList����ȥ
	SistorCharacter*pChar=new SistorCharacter();
	pChar->Enrich(charName,ipStr);
	m_sistorCharList.AddTail(*pChar);
	//4��ӵ�list��ȥ��@����
	charName+=(_T("@")+ipStr);
	m_sistorPool.AddString(charName);
}


void CTalk2LingSisterDlg::OnSelchangeListSistorpool()
{
	// TODO: Add your control notification handler code here
	//���ѡ�е���Ŀ
	//int listSize=m_sistorPool.GetCount();
	//if(listSize<1) return;
	//int *selIndex=new int [listSize];
	int selNum=m_sistorPool.GetCurSel();
	if(selNum==LB_ERR) return;

	CString peerName,peerIP;

	m_sistorCharList.GetAt(m_sistorCharList.FindIndex(selNum)).GetNameIP(peerName,peerIP);
	//delete selIndex[];
	SetDlgItemTextW(IDC_SRVR_IPADDRESS,peerIP);
	m_nextPeerName=peerName;
}

//˫�����ӣ�ȫ����
void CTalk2LingSisterDlg::OnDblclkListSistorpool()
{
	// TODO: Add your control notification handler code here
	OnSelchangeListSistorpool();
	OnBnClickedConnect();
}


void CTalk2LingSisterDlg::OnBnClickedButtonDelete()
{
	// TODO: Add your control notification handler code here
	int selNum=m_sistorPool.GetCurSel();
	if(selNum==LB_ERR) return;

	m_sistorPool.DeleteString(selNum);
	m_sistorCharList.RemoveAt(m_sistorCharList.FindIndex(selNum));
}
