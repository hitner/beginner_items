
// JUggFS_dlgDlg.h : header file
//

#pragma once

#include"JDFileListCtrl.h"
#include"FolderNameDlg.h"
// CJUggFS_dlgDlg dialog
class CJUggFS_dlgDlg : public CDialogEx
{
// Construction
public:
	CJUggFS_dlgDlg(CWnd* pParent = NULL);	// standard constructor
	CFolderNameDlg m_fnDlg;
// Dialog Data
	enum { IDD = IDD_JUGGFS_DLG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CJDFileListCtrl m_FileListCtrl;
	afx_msg void OnBnClickedButtonOpendisk();
	afx_msg void OnBnClickedButtonNewdisk();
	afx_msg void OnBnClickedButtonDownload();
	afx_msg void OnBnClickedButtonUpfolder();
	afx_msg void OnBnClickedButtonNewfold();
	afx_msg void OnBnClickedButtonHelp();
	afx_msg void OnBnClickedButtonClosedisk();
	afx_msg void OnBnClickedButtonDelete();
	CString m_CurDirectory;
};
