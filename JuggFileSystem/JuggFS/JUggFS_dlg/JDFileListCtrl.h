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

	BOOL CloseDisk();//关闭当前disk

	void DisplayFolder(const JuggNode*pJNode=NULL);//默认显示当前目录下的文件
	void AddItem(const JuggNode*pJNode);
	BOOL DeleteAllItems();
	void FreeData( const int nItem );

	void Sort(int iColumn, BOOL bAscending);//排序
	void Sort();//排序
	static int CALLBACK SortFunction( LPARAM lParam1, LPARAM lParam2, LPARAM lParamData );//在Sort中回调

	void CreateNewFolder(const CString &fnm);
	const JuggNode* GetCurAddedNode();//获取刚刚增加的节点

	void DownloadFile();//下载文件
	void DownloadSingleFile(UINT uItem,const CString fpath,int para=0);//默认不需要自动重命名

	void DeleteFileFolders();
	void DeleteSingleNode(UINT uItem);
	CString *m_pCurDirec;

	void UpFolder();//上一层目录
	CFileHeaderCtrl m_ctlHeader;				//列头
	int				m_nSortColumn;				//排序列
	BOOL			m_bAscend;					//是否升序排列
	CImageList		m_ImageList;				//定义图像列表
	int m_nNumColumns;
	int m_DiskOpenedFlag;//ggK

	//拖动处理
	int			m_ItmIndex;
	CImageList*	m_pDrgImg;
	BOOL		m_Drag;
private:
	
	JuggEngine m_JuggEng;
	JuggNode* m_pCurFolder;
	CFile m_UpDownFile;//用来上传的
	//CFile m_DownloadFile;//用来下载
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