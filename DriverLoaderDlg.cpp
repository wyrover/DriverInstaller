
// DriverLoaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DriverLoader.h"
#include "DriverLoaderDlg.h"
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


// CDriverLoaderDlg dialog



CDriverLoaderDlg::CDriverLoaderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDriverLoaderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDriverLoaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDriverLoaderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CDriverLoaderDlg::OnBnClickedButton1)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON2, &CDriverLoaderDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDriverLoaderDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDriverLoaderDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CDriverLoaderDlg message handlers

BOOL CDriverLoaderDlg::OnInitDialog()
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

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDriverLoaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDriverLoaderDlg::OnPaint()
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
HCURSOR CDriverLoaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


static CString GetLastErrorMessage() {
	DWORD dwError = ::GetLastError();
	LPTSTR lpMsg = NULL;
	::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)(&lpMsg),
		0,
		NULL);
	CString msg;
	msg.Format(_T("0x%X, %s"), dwError, lpMsg);
	::LocalFree(lpMsg);
	return msg;
}

static CString FormatErrorMessage(DWORD dwError) {
	LPTSTR lpMsg = NULL;
	::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)(&lpMsg),
		0,
		NULL);
	CString msg;
	msg.Format(_T("0x%X, %s"), dwError, lpMsg);
	::LocalFree(lpMsg);
	return msg;
}

enum Operation {
	OP_INSTALL,
	OP_START,
	OP_STOP,
	OP_UNINSTALL,
};

DWORD foo(Operation op, const TCHAR* filePath, const TCHAR* serviceName) {
	SC_HANDLE hService = NULL;
	SC_HANDLE hSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCManager) return ::GetLastError();
	DWORD dwRet = 0;
	SERVICE_STATUS_PROCESS status;

	switch (op) {
	case OP_INSTALL:
		hService = ::CreateService(
			hSCManager,
			serviceName,
			serviceName,
			SERVICE_ALL_ACCESS,
			SERVICE_KERNEL_DRIVER,
			SERVICE_DEMAND_START,
			SERVICE_ERROR_IGNORE,
			filePath,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL);
		if (NULL == hService)
			dwRet = ::GetLastError();
		break;
	case OP_START:
		hService = ::OpenService(hSCManager, serviceName, SC_MANAGER_ALL_ACCESS);
		if (NULL == hService) {
			dwRet = ::GetLastError();
			break;
		}
		if (!::StartService(hService, 0, NULL)) {
			dwRet = ::GetLastError();
			break;
		}
		break;
	case OP_STOP:
		hService = ::OpenService(hSCManager, serviceName, SC_MANAGER_ALL_ACCESS);
		if (NULL == hService) {
			dwRet = ::GetLastError();
			break;
		}
		if (!::ControlService(hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&status)) {
			dwRet = ::GetLastError();
			break;
		}
		while (status.dwCurrentState != SERVICE_STOPPED) {
			::Sleep(status.dwWaitHint);
			DWORD dwByteNeeded = 0;
			if (!::QueryServiceStatusEx(
				hService,
				SC_STATUS_PROCESS_INFO,
				(LPBYTE)&status,
				sizeof(SERVICE_STATUS_PROCESS),
				&dwByteNeeded)) {
				dwRet = ::GetLastError();
				break;
			}
			if (status.dwCurrentState == SERVICE_STOPPED) {
				break;
			}
		}
		break;
	case OP_UNINSTALL:
		hService = ::OpenService(hSCManager, serviceName, SC_MANAGER_ALL_ACCESS);
		if (NULL == hService) {
			dwRet = ::GetLastError();
			break;
		}
		if (!::DeleteService(hService)) {
			dwRet = ::GetLastError();
			break;
		}
		break;
	}

	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCManager);
	return dwRet;
}


void CDriverLoaderDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	CString filePath;
	CString serviceName;
	GetDlgItemText(IDC_EDIT2, filePath);
	GetDlgItemText(IDC_EDIT1, serviceName);	

	DWORD dwError = foo(OP_INSTALL, filePath.GetString(), serviceName.GetString());
	if (dwError != 0) {
		MessageBox(FormatErrorMessage(dwError));
	}
}


void CDriverLoaderDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	int DropCount = DragQueryFile(hDropInfo, -1, NULL, 0);
	if (1 == DropCount) {
		TCHAR str[MAX_PATH];
		DragQueryFile(hDropInfo, 0, str, MAX_PATH);
		SetDlgItemText(IDC_EDIT2, str);
		LPTSTR fileName = ::PathFindFileName(str);
		::PathRemoveExtension(fileName);
		SetDlgItemText(IDC_EDIT1, fileName);
	}
	CDialogEx::OnDropFiles(hDropInfo);
}


void CDriverLoaderDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	CString filePath;
	CString serviceName;
	GetDlgItemText(IDC_EDIT2, filePath);
	GetDlgItemText(IDC_EDIT1, serviceName);

	DWORD dwError = foo(OP_START, filePath.GetString(), serviceName.GetString());
	if (dwError != 0) {
		MessageBox(FormatErrorMessage(dwError));
	}
}


void CDriverLoaderDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	CString filePath;
	CString serviceName;
	GetDlgItemText(IDC_EDIT2, filePath);
	GetDlgItemText(IDC_EDIT1, serviceName);

	DWORD dwError = foo(OP_STOP, filePath.GetString(), serviceName.GetString());
	if (dwError != 0) {
		MessageBox(FormatErrorMessage(dwError));
	}
}


void CDriverLoaderDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	CString filePath;
	CString serviceName;
	GetDlgItemText(IDC_EDIT2, filePath);
	GetDlgItemText(IDC_EDIT1, serviceName);

	DWORD dwError = foo(OP_UNINSTALL, filePath.GetString(), serviceName.GetString());
	if (dwError != 0) {
		MessageBox(FormatErrorMessage(dwError));
	}
}
