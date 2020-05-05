// FolderNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JUggFS_dlg.h"
#include "FolderNameDlg.h"
#include "afxdialogex.h"


// CFolderNameDlg dialog

IMPLEMENT_DYNAMIC(CFolderNameDlg, CDialogEx)

CFolderNameDlg::CFolderNameDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFolderNameDlg::IDD, pParent)
{

	//  m_FoldName = _T("");
	m_FolderName = _T("");
}

CFolderNameDlg::~CFolderNameDlg()
{
}

void CFolderNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Text(pDX, IDC_EDIT_FOLDNAME, m_FoldName);
	DDX_Text(pDX, IDC_EDIT_FOLDNAME, m_FolderName);
	DDV_MaxChars(pDX, m_FolderName, 80);
}


BEGIN_MESSAGE_MAP(CFolderNameDlg, CDialogEx)
END_MESSAGE_MAP()


// CFolderNameDlg message handlers
