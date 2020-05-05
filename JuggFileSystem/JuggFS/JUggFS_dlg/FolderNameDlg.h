#pragma once


// CFolderNameDlg dialog

class CFolderNameDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFolderNameDlg)

public:
	CFolderNameDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFolderNameDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_INPUTFN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//	CString m_FoldName;
	CString m_FolderName;
};
