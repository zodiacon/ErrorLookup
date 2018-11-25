
// ErrorLookupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ErrorLookup.h"
#include "ErrorLookupDlg.h"
#include "afxdialogex.h"
#include <string>
#include <array>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CErrorLookupDlg dialog



CErrorLookupDlg::CErrorLookupDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_ERRORLOOKUP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CErrorLookupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CErrorLookupDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOOKUP, &CErrorLookupDlg::OnBnClickedLookup)
	ON_BN_CLICKED(IDC_COPY, &CErrorLookupDlg::OnBnClickedCopy)
END_MESSAGE_MAP()


// CErrorLookupDlg message handlers

BOOL CErrorLookupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

void CErrorLookupDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CErrorLookupDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CErrorLookupDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CErrorLookupDlg::OnBnClickedLookup() {
	CString text;
	GetDlgItemText(IDC_VALUE, text);
	if (text.IsEmpty())
		return;

	long long value = 0;
	bool hex = false;
	if (text.Left(2).CompareNoCase(L"0x") == 0) {
		hex = true;
		text = text.Mid(2);
	}
	if (text.FindOneOf(L"abcdefABCDEF") >= 0)
		hex = true;
	value = std::stoll(std::wstring(text), nullptr, hex ? 16 : 10);

	std::array<WCHAR, 1024> buffer;
	if (::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE, 
		::GetModuleHandle(L"ntdll"), (int)value,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer.data(), buffer.size(), nullptr)) {
		text.Format(L"%d (0x%X): %s", (int)value, (int)value, buffer.data());
		SetDlgItemText(IDC_MSG, text);
	}
	else {
		AfxMessageBox(L"Error number not found");
	}
	auto edit = (CEdit*)GetDlgItem(IDC_VALUE);
	edit->SetSel(0, -1);
	edit->SetFocus();
}


void CErrorLookupDlg::OnBnClickedCopy() {
	if (::OpenClipboard(GetSafeHwnd())) {
		CString text;
		GetDlgItemText(IDC_MSG, text);
		::EmptyClipboard();
		auto h = ::GlobalAlloc(GMEM_MOVEABLE, text.GetLength() * 2 + 2);
		auto p = ::GlobalLock(h);
		::wcscpy_s((PWSTR)p, text.GetLength() + 1, text);
		::GlobalUnlock(h);
		::SetClipboardData(CF_UNICODETEXT, h);
		::CloseClipboard();
	}
}
