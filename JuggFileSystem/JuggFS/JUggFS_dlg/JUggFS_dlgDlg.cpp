
// JUggFS_dlgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JUggFS_dlg.h"
#include "JUggFS_dlgDlg.h"
#include "afxdialogex.h"

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


// CJUggFS_dlgDlg dialog




CJUggFS_dlgDlg::CJUggFS_dlgDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CJUggFS_dlgDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_CurDirectory = _T("");
}

void CJUggFS_dlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_JD_FILELIST, m_FileListCtrl);
	DDX_Text(pDX, IDC_EDIT_CURDIRE, m_CurDirectory);
}

BEGIN_MESSAGE_MAP(CJUggFS_dlgDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_NOTIFY(LVN_ITEMCHANGED, IDC_JD_FILELIST, &CJUggFS_dlgDlg::OnLvnItemchangedJdFilelist)
ON_BN_CLICKED(IDC_BUTTON_OPENDISK, &CJUggFS_dlgDlg::OnBnClickedButtonOpendisk)
ON_BN_CLICKED(IDC_BUTTON_NEWDISK, &CJUggFS_dlgDlg::OnBnClickedButtonNewdisk)
ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &CJUggFS_dlgDlg::OnBnClickedButtonDownload)
ON_BN_CLICKED(IDC_BUTTON_UPFOLDER, &CJUggFS_dlgDlg::OnBnClickedButtonUpfolder)
ON_BN_CLICKED(IDC_BUTTON_NEWFOLD, &CJUggFS_dlgDlg::OnBnClickedButtonNewfold)
//ON_NOTIFY(LVN_ITEMCHANGED, IDC_JD_FILELIST, &CJUggFS_dlgDlg::OnLvnItemchangedJdFilelist)
ON_BN_CLICKED(IDC_BUTTON_HELP, &CJUggFS_dlgDlg::OnBnClickedButtonHelp)
ON_BN_CLICKED(IDC_BUTTON_CLOSEDISK, &CJUggFS_dlgDlg::OnBnClickedButtonClosedisk)
ON_BN_CLICKED(IDC_BUTTON_DELETE, &CJUggFS_dlgDlg::OnBnClickedButtonDelete)
END_MESSAGE_MAP()


// CJUggFS_dlgDlg message handlers

BOOL CJUggFS_dlgDlg::OnInitDialog()
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
	(CButton*)GetDlgItem(IDC_BUTTON_CLOSEDISK)->EnableWindow(0);
	m_FileListCtrl.SetExtendedStyle(LVS_EX_TWOCLICKACTIVATE|LVS_EX_FULLROWSELECT );
	m_FileListCtrl.SetColumns(_T("文件名,200;创建日期,130;修改日期,130;大小,80"));
	m_FileListCtrl.m_pCurDirec=&m_CurDirectory;
	//m_FileListCtrl.InsertColumn(0,_T("文件名"));
	//::RegisterHotKey(GetSafeHwnd(),6666,MOD_CONTROL,VK_DELETE );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CJUggFS_dlgDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CJUggFS_dlgDlg::OnPaint()
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
HCURSOR CJUggFS_dlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//void CJUggFS_dlgDlg::OnLvnItemchangedJdFilelist(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}


void CJUggFS_dlgDlg::OnBnClickedButtonOpendisk()
{
	// TODO: Add your control notification handler code here
/*
	OPENFILENAME fopt; 
	memset(&fopt, 0, sizeof(fopt));
	fopt.lStructSize = sizeof(OPENFILENAME);
	fopt.Flags       = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
 	fopt.hwndOwner   = GetSafeHwnd(); 
	//fopt.lpstrFilter =_T("All Files(*.*)"); 
	GetOpenFileName(&fopt);
	*/
	CString filePath;
	CFileDialog ldFileDlg(TRUE,_T(".jugg"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("Virtual disks(*.jugg)|*.jugg|All files(*.*)|*.*||"));
	if(ldFileDlg.DoModal()==IDOK)   
        { 
                  filePath=ldFileDlg.GetPathName();////////取出文件路径 
                  UpdateData(FALSE); 
	}
	if(!filePath.IsEmpty()){
		if(!m_FileListCtrl.OpenDisk(filePath)){
			MessageBox(_T("初始化Jugg文件系统失败!"));
		}
		//让新建和打开失效
		(CButton*)GetDlgItem(IDC_BUTTON_NEWDISK)->EnableWindow(0);
		(CButton*)GetDlgItem(IDC_BUTTON_OPENDISK)->EnableWindow(0);
		(CButton*)GetDlgItem(IDC_BUTTON_CLOSEDISK)->EnableWindow();
	}
}


void CJUggFS_dlgDlg::OnBnClickedButtonNewdisk()
{
	// TODO: Add your control notification handler code here
	//暂时不设参数，设置为4G
	CString filePath;
	CFileDialog ldFileDlg(FALSE,_T(".jugg"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("Virtual disks(*.jugg)|*.jugg|All files(*.*)|*.*||"));
	if(ldFileDlg.DoModal()==IDOK)   
        { 
                  filePath=ldFileDlg.GetPathName();////////取出文件路径 
                  UpdateData(FALSE); 
	}
	if(!filePath.IsEmpty()){
		if(!m_FileListCtrl.CreateDisk(filePath)){
			MessageBox(_T("创建Jugg文件系统失败!"));
		}
		//让新建和打开失效
		(CButton*)GetDlgItem(IDC_BUTTON_NEWDISK)->EnableWindow(0);
		(CButton*)GetDlgItem(IDC_BUTTON_OPENDISK)->EnableWindow(0);
		(CButton*)GetDlgItem(IDC_BUTTON_CLOSEDISK)->EnableWindow();
	}

}


void CJUggFS_dlgDlg::OnBnClickedButtonDownload()
{
	// TODO: Add your control notification handler code here
	m_FileListCtrl.DownloadFile();
	//获得是否有选定项目
}


void CJUggFS_dlgDlg::OnBnClickedButtonUpfolder()
{
	// TODO: Add your control notification handler code here
	m_FileListCtrl.UpFolder();
}


void CJUggFS_dlgDlg::OnBnClickedButtonNewfold()
{
	// TODO: Add your control notification handler code here
	//CString fold_name;
	//m_fnDlg.
	//CDialog new_fn(IDD_DIALOG_INPUTFN);
	if(m_FileListCtrl.m_DiskOpenedFlag==0){
		AfxMessageBox(_T("没有已打开的文件系统"));
		return;
	}
	m_fnDlg.m_FolderName=_T("");
	if(m_fnDlg.DoModal()==IDOK){
			//CString fold_name;
		m_FileListCtrl.CreateNewFolder(m_fnDlg.m_FolderName);
	}

	//m_FileListCtrl.CreateNewFolder(fold_name);
}


//void CJUggFS_dlgDlg::OnLvnItemchangedJdFilelist(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}


void CJUggFS_dlgDlg::OnBnClickedButtonHelp()
{
	// TODO: Add your control notification handler code here
	AfxMessageBox(
		_T("使用说明\n1,第一次使用需点击NewDisk新建一个\"文件系统\"\n2,可以将外部文件拖拽进列表框实现上传\n3,删除文件或文件夹可以使用快捷键Delete"),
		MB_OK|MB_ICONINFORMATION );
}


void CJUggFS_dlgDlg::OnBnClickedButtonClosedisk()
{
	// TODO: Add your control notification handler code here
	m_FileListCtrl.CloseDisk();
	m_FileListCtrl.DeleteAllItems();
	(CButton*)GetDlgItem(IDC_BUTTON_NEWDISK)->EnableWindow();
	(CButton*)GetDlgItem(IDC_BUTTON_OPENDISK)->EnableWindow();
	(CButton*)GetDlgItem(IDC_BUTTON_CLOSEDISK)->EnableWindow(0);
}


void CJUggFS_dlgDlg::OnBnClickedButtonDelete()
{
	// TODO: Add your control notification handler code here
	m_FileListCtrl.DeleteFileFolders();
}
