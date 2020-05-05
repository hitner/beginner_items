#pragma once


// CJDFileListCtrl
#include"JuggEngine.h"

#include"FileHeaderCtrl.h"
class CJDFileListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CJDFileListCtrl)

public:
	CJDFileListCtrl();
	virtual ~CJDFileListCtrl();

	void LoadSysFileIcon();

	BOOL SetColumns(const CString &strHeadings);
	BOOL OpenDisk(const CString & diskFilePath);
	BOOL CreateDisk(const CString & diskFilePath);

	BOOL CloseDisk();//�رյ�ǰdisk

	void DisplayFolder(const JuggNode*pJNode=NULL);//Ĭ����ʾ��ǰĿ¼�µ��ļ�
	void AddItem(const JuggNode*pJNode);
	BOOL DeleteAllItems();
	void FreeData( const int nItem );

	void Sort(int iColumn, BOOL bAscending);//����
	void Sort();//����
	static int CALLBACK SortFunction( LPARAM lParam1, LPARAM lParam2, LPARAM lParamData );//��Sort�лص�

	void CreateNewFolder(const CString &fnm);
	const JuggNode* GetCurAddedNode();//��ȡ�ո����ӵĽڵ�

	void DownloadFile();//�����ļ�
	void DownloadSingleFile(UINT uItem,const CString fpath,int para=0);//Ĭ�ϲ���Ҫ�Զ�������

	void DeleteFileFolders();
	void DeleteSingleNode(UINT uItem);
	CString *m_pCurDirec;

	void UpFolder();//��һ��Ŀ¼
	CFileHeaderCtrl m_ctlHeader;				//��ͷ
	int				m_nSortColumn;				//������
	BOOL			m_bAscend;					//�Ƿ���������
	CImageList		m_ImageList;				//����ͼ���б�
	int m_nNumColumns;
	int m_DiskOpenedFlag;//ggK

	//�϶�����
	int			m_ItmIndex;
	CImageList*	m_pDrgImg;
	BOOL		m_Drag;
private:
	
	JuggEngine m_JuggEng;
	JuggNode* m_pCurFolder;
	CFile m_UpDownFile;//�����ϴ���
	//CFile m_DownloadFile;//��������
	CFile m_DiskFile;

	static const int m_HotKeyID=6668;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnDestroy();
protected:
	virtual void PreSubclassWindow();
public:
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnHdnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
};

const int Date_String_Len=20;

const int Size_String_Len=10;


bool IsNumber( LPCTSTR pszText );
int CompareDataAsNumber( LPCTSTR pszParam1, LPCTSTR pszParam2 );
bool IsDate( LPCTSTR pszText );
int CompareDataAsDate( const CString& strDate1, const CString& strDate2 );