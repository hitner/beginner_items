// JDFileListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "JUggFS_dlg.h"
#include "JDFileListCtrl.h"



class CItemData
{
public:
	CItemData(){
		m_ColumnTexts = NULL;
		m_isFolder = 0;
	}
	LPTSTR* m_ColumnTexts;	//��¼��ǰ���������ı�
	DWORD	m_isFolder;		//��ͼ������
	const JuggNode* m_pJuggNode;       //��ӦĿ¼�ڵ�

private:
	CItemData( const CItemData& );
	//��ֹ����
	CItemData& operator=( const CItemData& );
};
// CJDFileListCtrl

IMPLEMENT_DYNAMIC(CJDFileListCtrl, CListCtrl)

CJDFileListCtrl::CJDFileListCtrl()
{
	m_DiskOpenedFlag=0;//gg
	m_nNumColumns=0;
	m_pCurFolder=NULL;

	m_nSortColumn=0;				//������
	m_bAscend=TRUE;

	m_ItmIndex=-1;
	m_pDrgImg=NULL;
	m_Drag=FALSE;
}

CJDFileListCtrl::~CJDFileListCtrl()
{
	//DeleteAllItems();
}


BEGIN_MESSAGE_MAP(CJDFileListCtrl, CListCtrl)
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CJDFileListCtrl::OnNMDblclk)
	ON_WM_DROPFILES()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &CJDFileListCtrl::OnLvnColumnclick)
	ON_WM_CONTEXTMENU()
	ON_WM_HOTKEY()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, &CJDFileListCtrl::OnLvnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
//	ON_NOTIFY(HDN_BEGINDRAG, 0, &CJDFileListCtrl::OnHdnBegindrag)
END_MESSAGE_MAP()



// CJDFileListCtrl message handlers




// Common control functions
BOOL CJDFileListCtrl::SetColumns(const CString &strHeadings)
{
	::RegisterHotKey(GetSafeHwnd(),m_HotKeyID,MOD_NOREPEAT,VK_DELETE );
	//strHeadings.
	int nStart = 0;
	for( ;; )
	{
		int nComma = strHeadings.Find( _T(','), nStart );	
		if( nComma == -1 )
			break;
		//��ȡ�ı�
		CString strHeading = strHeadings.Mid( nStart, nComma - nStart );
		nStart = nComma + 1;	//�ӹ�","
		int nSemiColon = strHeadings.Find( _T(';'), nStart );
		if( nSemiColon == -1 )
			nSemiColon = strHeadings.GetLength();
		int nWidth = _ttoi( strHeadings.Mid( nStart, nSemiColon - nStart ) );	//��ȡ���
		nStart = nSemiColon + 1;			//ָ����һ����Ϣ
		if( InsertColumn( m_nNumColumns++, strHeading, LVCFMT_LEFT, nWidth ) == -1 )
			return FALSE;					//������
	}
	return TRUE;
}
BOOL CJDFileListCtrl::OpenDisk(const CString & diskFilePath){
	CFileException pEr;
	if(!m_DiskFile.Open(diskFilePath,CFile::modeReadWrite,&pEr)){
		   TCHAR   szCause[255];
		   CString strFormatted;
		   pEr.GetErrorMessage(szCause, 255);

		   strFormatted = _T("The data file could not be opened because of this error: ");
		   strFormatted += szCause;

		   AfxMessageBox(strFormatted);
		   return TRUE;//����Ҫ����ʾ!����
	}
	if(!m_JuggEng.PreHandle(m_DiskFile)){
		m_DiskFile.Close();
		return FALSE;
	}
	if(!m_JuggEng.OpenDisk()){
		m_DiskFile.Close();
		return FALSE;
	}
	m_DiskOpenedFlag=1;//ָʾ�Ѿ����ļ�ϵͳ
	m_pCurFolder=m_JuggEng.GetRootNode();
	DisplayFolder();
	//int nIndex = InsertItem( GetItemCount(), _T("WHERE") );
	return TRUE;

}

BOOL CJDFileListCtrl::CreateDisk(const CString & diskFilePath){
	CFileException pEr;
	if(!m_DiskFile.Open(diskFilePath,CFile::modeReadWrite|CFile::modeCreate,&pEr)){
		   TCHAR   szCause[255];
		   CString strFormatted;
		   pEr.GetErrorMessage(szCause, 255);

		   strFormatted = _T("The data file could not be opened because of this error: ");
		   strFormatted += szCause;

		   AfxMessageBox(strFormatted);
		   return TRUE;//����Ҫ����ʾ!����
	}
	if(!m_JuggEng.PreHandle(m_DiskFile)){
		m_DiskFile.Close();
		return FALSE;
	}
	m_JuggEng.CreateDisk(4096,2);
	m_DiskOpenedFlag=1;//ָʾ�Ѿ����ļ�ϵͳ

	//int nIndex = InsertItem( GetItemCount(), _T("WHERE") );
	//DisplayFolder(m_JuggEng.GetRootNode());//��ʾ���ļ���
	m_pCurFolder=m_JuggEng.GetRootNode();
	return TRUE;
}

BOOL CJDFileListCtrl::CloseDisk(){//�رյ�ǰdisk--�������ú�exit�Ի�����ã�
	if(m_DiskOpenedFlag==1){
		m_JuggEng.CloseDisk();
		//DeleteAllItems();�Զ�ɾ���ģ�����ֻҪfree ���Ե��Ǹ������˰�--���ⲿ���� exitʱ��Ondestroy����
		m_pCurFolder=NULL;
		m_DiskFile.Close();
		m_DiskOpenedFlag=0;
	}
	return TRUE;

}
void CJDFileListCtrl::DisplayFolder(const JuggNode*pJNode){
	if(pJNode==NULL){
		pJNode=m_pCurFolder;
	}
	DeleteAllItems();//��ɾ��

	ASSERT(pJNode!=NULL);
	//��������ʾ
	const JuggNode*pJN;
	pJN=pJNode->m_pLChild;
	while(pJN){
		AddItem(pJN);
		pJN=pJN->m_pRChild;//�ұߵ��ֵ�
	}
	Sort();
	
}
void CJDFileListCtrl::AddItem(const JuggNode*pJNode){
	if(pJNode==NULL){
		return;
	}
	CItemData* pItemData = new CItemData();
	pItemData->m_pJuggNode=pJNode;
	if(pJNode->IsFolder()){
		pItemData->m_isFolder=1;
	}
	pItemData->m_ColumnTexts=new LPTSTR[m_nNumColumns];//��¼�����ı�
	LPTSTR *pColTexts=pItemData->m_ColumnTexts;//����

	pColTexts[0]=new TCHAR[lstrlen(pJNode->jn_name)+1];//��������
	lstrcpy(pColTexts[0],pJNode->jn_name);

	pColTexts[1]=new TCHAR[Date_String_Len];//����ʱ��
	CTime j_tm(pJNode->jn_ctime);
	CString cs_date=j_tm.Format("%Y-%m-%d %H:%M:%S");
	lstrcpy(pColTexts[1],cs_date);
		
	pColTexts[2]=new TCHAR[Date_String_Len];//�޸�ʱ��
	j_tm=pJNode->jn_mtime;
	cs_date=j_tm.Format("%Y-%m-%d %H:%M:%S");
	lstrcpy(pColTexts[2],cs_date);

	pColTexts[3]=new TCHAR[Size_String_Len];//�ļ���С
	if(pItemData->m_isFolder==1){
		lstrcpy(pColTexts[3],_T(""));
	}
	else{
		Int64 lFileLen=pJNode->jn_size;
		CString csFileSize;
		double fGB = lFileLen /(double)(1024*1024*1024);
		if (fGB < 1){
			double fMB = lFileLen / (double)(1024*1024);
			if (fMB < 1){
				double fBK = lFileLen / (double)(1024);
				if (fBK >1){
					csFileSize.Format(_T("%2.2f KB"),fBK);
				}
				else{
					csFileSize.Format(_T("%i B"),lFileLen);
				}
			}
			else{
				csFileSize.Format(_T("%2.2f MB"),fMB);
			}
		}
		else{
			csFileSize.Format(_T("%2.2f GB"),fGB);
		}
		lstrcpy(pColTexts[3],csFileSize);//�ȹ̶���--ggW
	}


	//���ñ��ֵ�������
	int nIndex=InsertItem(GetItemCount(),pColTexts[0]);
	
	for(int nCol=1;nCol<m_nNumColumns;nCol++){
		CListCtrl::SetItem(nIndex,nCol,LVIF_TEXT,pColTexts[nCol],0,0,0,0);
	}

	SHFILEINFO shInfo;
	int nIcon = 0;
	if(pItemData->m_isFolder){
		SHGetFileInfo(_T("C:\\Program Files"),0,&shInfo,sizeof(shInfo),SHGFI_ICON | SHGFI_SMALLICON );
	}
	else{
		SHGetFileInfo(pColTexts[0],FILE_ATTRIBUTE_NORMAL,&shInfo,sizeof(shInfo),SHGFI_ICON | SHGFI_SMALLICON|SHGFI_USEFILEATTRIBUTES );
	}
	DestroyIcon(shInfo.hIcon);
	nIcon = shInfo.iIcon;
	//��ʾ�ļ�����ͼ��
	CListCtrl::SetItem(nIndex,0,LVIF_IMAGE,_T(""),nIcon,0,0,0);

	//�洢�������������
	CListCtrl::SetItemData(nIndex,(DWORD) pItemData);



}

BOOL CJDFileListCtrl::DeleteAllItems(){
	int ItemTotal=GetItemCount();
	for(int iItem = 0; iItem < ItemTotal; iItem ++ )
		FreeData(iItem);
	return CListCtrl::DeleteAllItems();
}
void CJDFileListCtrl::FreeData(const int nItem)
{
	CItemData* pItemData = ( CItemData* )CListCtrl::GetItemData( nItem  );
	LPTSTR* pcsTexts = pItemData->m_ColumnTexts;

	for( int i = 0; i < m_nNumColumns; i++ ){
		if(pcsTexts[ i ]){
			delete[] pcsTexts[ i ];
		}
	}

	delete[] pcsTexts;
	delete pItemData;

	VERIFY( CListCtrl::SetItemData( nItem , NULL ) );//ÿ����Ŀ����һ��ָ�룬Ϊ�β�����JuggNode��
}

void CJDFileListCtrl::OnDestroy() 
{	
	::UnregisterHotKey(GetSafeHwnd(),m_HotKeyID);
	for(int nItem=0; nItem<GetItemCount(); nItem++)
	{
		FreeData(nItem);
	}
	CListCtrl::OnDestroy();
}

void CJDFileListCtrl::LoadSysFileIcon()
{
	SHFILEINFO shInfo;
	memset(&shInfo,0,sizeof(SHFILEINFO));
	HIMAGELIST hImage = (HIMAGELIST)SHGetFileInfo(_T("C:\\"),0,&shInfo, sizeof( SHFILEINFO ),
		SHGFI_SYSICONINDEX | SHGFI_SMALLICON );
	m_ImageList.Attach(hImage); 
	SetImageList(&m_ImageList, LVSIL_SMALL);
	ASSERT(GetImageList(LVSIL_SMALL) == &m_ImageList);

}


//����Ŀ¼
void CJDFileListCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	const CItemData*pItem;
	int nItem = GetSelectionMark();
	if (nItem != -1){
		pItem = (CItemData*)GetItemData(nItem);
		if (pItem->m_isFolder){	//�ж��Ƿ�ΪĿ¼	
			//��ȡ������Ŀ¼��Ϣ
			m_pCurFolder=const_cast<JuggNode*>(pItem->m_pJuggNode);
			
			//*m_pCurDirec+=m_pCurFolder->jn_name;
			//*m_pCurDirec+=_T("/");
			//CListCtrl::GetParentOwner()->UpdateWindow();

			DisplayFolder();
		}
	}
	*pResult = 0;
}
//��һ��Ŀ¼
void CJDFileListCtrl::UpFolder(){
	//��ȡ������Ŀ¼��Ϣ
	JuggNode* pFirstLParent=NULL;
	pFirstLParent=m_pCurFolder->GetUpFolderNode();
	if(pFirstLParent==NULL){
		AfxMessageBox(_T("�Ѿ��Ǹ�Ŀ¼��"));
		return;
	}
	else{
		m_pCurFolder=pFirstLParent;
	}
	DisplayFolder();

}

void CJDFileListCtrl::OnDropFiles(HDROP hDropInfo){
	// TODO: Add your message handler code here and/or call default
	 UINT count;
         TCHAR filePath[256];
try{
	if(m_DiskOpenedFlag==0){
		AfxMessageBox(_T("û���Ѵ򿪵��ļ�ϵͳ"));
		return;
	}
	 if(m_pCurFolder==NULL){
		 AfxMessageBox(_T("FATAL ERROR"));
		 return;
	 }
         count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	 CFileException pEr;
	 for(UINT i=0; i<count; i++) {
		int pathLen = DragQueryFile(hDropInfo, i, filePath, sizeof(filePath));
		if(pathLen>0){
			if(m_UpDownFile.Open(filePath,CFile::modeRead,&pEr)){
				if(JNE_Correct!=m_JuggEng.AddFile(m_pCurFolder,m_UpDownFile)){
					AfxMessageBox(_T("����ļ�ʧ��"));				
				}
				else{//���Item
					AddItem(GetCurAddedNode());
				}
				m_UpDownFile.Close();
			}
			else if(pEr.m_lOsError==5){
				//�����ļ���
				m_JuggEng.CreatFolder(m_pCurFolder,m_UpDownFile.GetFileName());
				//���Item
				AddItem(GetCurAddedNode());
			}
			else{
				pEr.ReportError();
			}			
		}
		Sort();
        }
	DragFinish(hDropInfo);
}
catch(...){
	AfxMessageBox(_T("load file failure"));
}
	CListCtrl::OnDropFiles(hDropInfo);
}
void CJDFileListCtrl::CreateNewFolder(const CString &fnm){
	if(m_DiskOpenedFlag==0){
		AfxMessageBox(_T("û���Ѵ򿪵��ļ�ϵͳ"));
		return;
	}
	 if(m_pCurFolder==NULL){
		 AfxMessageBox(_T("FATAL ERROR"));
		 return;
	 }
	 //�����ļ���
	if(JNE_Correct!=m_JuggEng.CreatFolder(m_pCurFolder,fnm)){
		AfxMessageBox(_T("����ʧ��"));
		return;
	}
	//���Item
	AddItem(GetCurAddedNode());
	Sort();
}
const JuggNode* CJDFileListCtrl::GetCurAddedNode(){
	JuggNode* pLF=m_pCurFolder->m_pLChild;
	if(pLF->m_pRChild){
		return pLF->GetLastRChild();
	}
	else{
		return pLF;
	}
}

void CJDFileListCtrl::DownloadFile(){//�����ļ�
	UINT  uSelectedCount = GetSelectedCount();
	const CItemData* pItem=NULL;
	UINT uItem=-1;
	//ѡ���˵����ļ������Ϊ�Ի���
	if(uSelectedCount==1){
		uItem = GetNextItem(uItem, LVNI_SELECTED);
		ASSERT(uItem != -1);
		pItem=(CItemData*)GetItemData(uItem);
		if(pItem->m_isFolder){
			AfxMessageBox(_T("�ݲ�֧���ļ��в���"));
			return;
		}
		CFileDialog dnFileDlg(FALSE,NULL,pItem->m_ColumnTexts[0],OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("All files(*.*)|*.*||"));
		if(dnFileDlg.DoModal()==IDOK)   { 
			CString filePath=dnFileDlg.GetPathName();////////ȡ���ļ�·��������---
			DownloadSingleFile(uItem,filePath);
		}
		return;
	}
	else if(uSelectedCount>1){
	//ѡ���ļ��жԻ���
		CFolderPickerDialog dnFolderDlg;
		if(dnFolderDlg.DoModal()==IDOK){
			CString filePath=dnFolderDlg.GetFolderPath();
			for (UINT i=0; i < uSelectedCount; i++){
				uItem = GetNextItem(uItem, LVNI_SELECTED);
				ASSERT(uItem != -1);
				//����ÿ���ļ�
				DownloadSingleFile(uItem,filePath,1);//����1��ʾ�Զ���������filePath�Ǹ��ļ���
			}
		}	
	}
	//bad click
	else{
		AfxMessageBox(_T("û��ѡ���ļ�"));
	}
}
void CJDFileListCtrl::DownloadSingleFile(UINT uItem,const CString fpath,int para){
	CFileException pEr;
	const CItemData* pItem=(CItemData*)GetItemData(uItem);
	if(pItem->m_isFolder){//��֧���ļ��в���
		return ;
	}
	CString path(fpath);
	if(para==1){//�Զ�������
		path+=_T("\\jg_");
		path+=CString(pItem->m_ColumnTexts[0]);
	}
	if(m_UpDownFile.Open(path,CFile::modeWrite|CFile::modeCreate,&pEr)){
		m_JuggEng.DownloadNodedFile(m_UpDownFile,pItem->m_pJuggNode);
		m_UpDownFile.Close();
	}
	else{
		pEr.ReportError();
	}
}


void CJDFileListCtrl::PreSubclassWindow() 
{
	ASSERT( GetStyle() & LVS_REPORT );
	CListCtrl::PreSubclassWindow();
	VERIFY( m_ctlHeader.SubclassWindow( GetHeaderCtrl()->GetSafeHwnd() ) );	
	LoadSysFileIcon();
}
void CJDFileListCtrl::Sort(int iColumn, BOOL bAscending)
{
	m_nSortColumn = iColumn;
	m_bAscend = bAscending;
	m_ctlHeader.SetSortColomn(m_nSortColumn, m_bAscend);
	SortItems(SortFunction, (DWORD)this);
}
void CJDFileListCtrl::Sort(){//��������
	m_ctlHeader.SetSortColomn(m_nSortColumn, m_bAscend);
	SortItems(SortFunction, (DWORD)this);
}
int CALLBACK CJDFileListCtrl::SortFunction( LPARAM lParam1, LPARAM lParam2, LPARAM lParamData )
{
	CJDFileListCtrl* pListCtrl = (CJDFileListCtrl*)(lParamData);
	CItemData* pParam1 = (CItemData*)(lParam1);
	CItemData* pParam2 = (CItemData*)(lParam2); 
	//��ȡ�����е��ı�
	LPCTSTR pszText1 = pParam1->m_ColumnTexts[pListCtrl->m_nSortColumn];
	LPCTSTR pszText2 = pParam2->m_ColumnTexts[pListCtrl->m_nSortColumn];

	if(IsNumber(pszText1))	//����ֵ�Ƚ�
		return pListCtrl->m_bAscend ? CompareDataAsNumber(pszText1, pszText2)
			: CompareDataAsNumber(pszText2, pszText1);
	else if(IsDate(pszText1))	//�����ڱȽ�
		return pListCtrl->m_bAscend ? CompareDataAsDate(pszText1, pszText2)
			: CompareDataAsDate(pszText2, pszText1);
	else						//���ı��Ƚ�
		return pListCtrl->m_bAscend ? lstrcmp(pszText1, pszText2)
			: lstrcmp(pszText2, pszText1);
}

void CJDFileListCtrl::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	//NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nColumn = pNMLV->iSubItem;
	Sort(nColumn, nColumn == m_nSortColumn ? !m_bAscend : TRUE);
	*pResult = 0;
}
void CJDFileListCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: Add your message handler code here
}


void CJDFileListCtrl::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: Add your message handler code here and/or call default
	DeleteFileFolders();
	CListCtrl::OnHotKey(nHotKeyId, nKey1, nKey2);
}

void CJDFileListCtrl::DeleteFileFolders(){
	//
	UINT  uSelectedCount = GetSelectedCount();
	
	UINT uItem=-1;
	for (UINT i=0; i < uSelectedCount; i++){
		uItem = GetNextItem(-1, LVNI_SELECTED);//���Ǵ�ͷ����
		ASSERT(uItem != -1);
		//����ÿ���ļ�
		DeleteSingleNode(uItem);//����1��ʾ�Զ���������filePath�Ǹ��ļ���
	}
}

void CJDFileListCtrl::DeleteSingleNode(UINT uItem){
	CItemData* pItem=NULL;
	pItem=(CItemData*)GetItemData(uItem);
	ASSERT(pItem->m_pJuggNode!=NULL);
	m_JuggEng.JN_DeleteFile(const_cast<JuggNode*>(pItem->m_pJuggNode));
	FreeData(uItem);
	DeleteItem(uItem);//ɾ���ýڵ�
}


//�ж��ı��Ƿ�Ϊ����
bool IsNumber( LPCTSTR pszText )
{
	for( int i = 0; i < lstrlen( pszText ); i++ )
		if( !_istdigit( pszText[ i ] ) )
			return false;
	return true;
}


int CompareDataAsNumber( LPCTSTR pszParam1, LPCTSTR pszParam2 )
{
	int nNumber1 = _wtoi( pszParam1 );
	int nNumber2 = _wtoi( pszParam2 );
	return nNumber1 - nNumber2;
}

//�ж��Ƿ�Ϊ����
bool IsDate( LPCTSTR pszText )
{
	//��ʽΪ0000-00-00
	if( lstrlen( pszText ) < 10 )
		return false;
	return _istdigit( pszText[ 0 ] )
		&& _istdigit( pszText[ 1 ] )
		&& _istdigit( pszText[ 2 ] )
		&& _istdigit( pszText[ 3 ] )
		&& pszText[ 4 ] == _T('-')
		&& _istdigit( pszText[ 5 ] )
		&& _istdigit( pszText[ 6 ] )
		&& pszText[ 7 ] == _T('-')
		&& _istdigit( pszText[ 8 ] )
		&& _istdigit( pszText[ 9 ] );
}

//�����ڱȽ�����
int CompareDataAsDate( const CString& strDate1, const CString& strDate2 )
{
	//�ȱȽ���
	int nYear1 = _wtoi( strDate1.Mid(0, 4 ) );
	int nYear2 = _wtoi( strDate2.Mid(0, 4 ) );
	if (nYear1 != nYear2)
	{
		return nYear1 - nYear2;
	}
	//�Ƚ���
	int nMonth1 = _wtoi( strDate1.Mid( 5, 2 ) );
	int nMonth2 = _wtoi( strDate2.Mid( 5, 2 ) );
	if( nMonth1 != nMonth2 )
	{
		return nMonth1 = nMonth2;	
	}
	//�Ƚ���
	int nDay1 = _wtoi( strDate1.Mid( 8, 2 ) );
	int nDay2 = _wtoi( strDate2.Mid( 8, 2 ) );
	
	if( nDay1 !=  nDay2 )
	{
		return nDay1 - nDay2;
	}
	return 0;		//������ȣ�����0
}




void CJDFileListCtrl::OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	POINT pt;
	m_ItmIndex = ((NM_LISTVIEW *)pNMHDR)->iItem;
	m_pDrgImg  = CreateDragImage(m_ItmIndex,&pt);
	m_pDrgImg->BeginDrag(0,pt);
	ClientToScreen(&pt);						//ת���ͻ����굽��Ļ����
	m_pDrgImg->DragEnter(this,pt);
	m_Drag = TRUE;


	*pResult = 0;
}


void CJDFileListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(m_Drag){
		/*
		int iItem = HitTest( point );//iItem������굱ǰָ�����Ŀ��
		if( iItem!=-1 ) {
			SetItemState( iItem, LVIS_SELECTED, LVIS_SELECTED );
		}
		else{
			if(
		}*/
		CPoint pt;
		pt.x = point.x;
		pt.y = point.y + (m_ItmIndex + 1) * 15;
		m_pDrgImg->DragMove(pt);

		//SetItemState( iItem, LVIS_SELECTED, LVIS_SELECTED );
	}
	CListCtrl::OnMouseMove(nFlags, point);
}


void CJDFileListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(m_Drag){
		m_pDrgImg->EndDrag();
		m_Drag = FALSE;
		delete m_pDrgImg;
		m_pDrgImg=NULL;

		int iItem = HitTest( point );
		if(iItem==-1||iItem==m_ItmIndex){
			return;
		}
		CItemData*pItem=(CItemData*)GetItemData(iItem);
		if(!pItem->m_isFolder){
			return;
		}
		CItemData*pItemOrg=(CItemData*)GetItemData(m_ItmIndex);
		JN_Error jnerror=m_JuggEng.MoveNode(const_cast<JuggNode*>(pItem->m_pJuggNode),
			const_cast<JuggNode*>(pItemOrg->m_pJuggNode));
		if(jnerror!=JNE_Correct){
			CString err;
			m_JuggEng.GetJNErrorString(err,jnerror);
			AfxMessageBox(err);
		}
		else{
			//
			FreeData(m_ItmIndex);
			DeleteItem(m_ItmIndex);//ɾ���ýڵ�
		}
		/*
		TCHAR name[256];
		LV_ITEM lvi;
		CString subitem[3];
		for(int i=2;i>=0;i--)
		{
			ZeroMemory(&lvi,sizeof(LV_ITEM));
			lvi.iItem		= m_ItmIndex;
			lvi.iSubItem	= i;
			lvi.mask		= LVIF_IMAGE | LVIF_TEXT;
			lvi.pszText		= name;
			lvi.cchTextMax	= 255;
			GetItem(&lvi);
			subitem[i].Format(_T("%s"),name);
		}
		InsertItem(&lvi);
		SetItemText(m_ItmIndex,1,subitem[1]);
		SetItemText(m_ItmIndex,2,subitem[2]);*/
	}

	CListCtrl::OnLButtonUp(nFlags, point);
}

/*
void CJDFileListCtrl::OnHdnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}*/
