// SudokuDlg.cpp : implementation file
//


/***********************************************************************************
* Created by Drakon
* drakon@dubki.ru
* 2007
***********************************************************************************/


#include "stdafx.h"
#include "Sudoku.h"
#include "SudokuDlg.h"
#include "sudoku_solve.h"
#include "fstream"
#include "classes.h"
using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

vector<CArr> solutions;//storage of solutions

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSudokuDlg dialog




CSudokuDlg::CSudokuDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSudokuDlg::IDD, pParent)
	, m_flag(FALSE)
	, m_watch_number(0)
	, m_number(0)
{
	for (int i = 0; i < 9; i++)
	{
		m_SudokuData[i] = new BYTE[9];
		m_LastSudokuData[i] = new BYTE[9];
	}
	

	for (int i = 0; i < 9; i++)
		for(int j = 0; j < 9; j++)
		{
			m_SudokuData[i][j] = 0;
		}

	m_ActiveField.x = 10;
	m_ActiveField.y = 10;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
}

void CSudokuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SUDOKU_VIEW, m_View);
	DDX_Check(pDX, IDC_CHECK1, m_flag);
	DDX_Text(pDX, IDC_EDIT2, m_watch_number);
	DDX_Text(pDX, IDC_EDIT1, m_number);
}

BEGIN_MESSAGE_MAP(CSudokuDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_START_BUTTON, &CSudokuDlg::OnBnClickedStartButton)
	ON_BN_CLICKED(IDC_CLEAR_BUTTON, &CSudokuDlg::OnBnClickedClearButton)
	ON_STN_CLICKED(IDC_SUDOKU_VIEW, &CSudokuDlg::OnStnClickedSudokuView)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON1, &CSudokuDlg::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT1, &CSudokuDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_CHECK1, &CSudokuDlg::OnBnClickedCheck1)
	//ON_CBN_SELCHANGE(IDC_COMBO1, &CSudokuDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_CHECK2, &CSudokuDlg::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_BUTTON2, &CSudokuDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CSudokuDlg message handlers

BOOL CSudokuDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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

void CSudokuDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

/***********************************************************************************************************************/
void CreateSudokuFont(CFont* pFont, CDC* pDC)
{
	ASSERT(pFont);

	int iFontSize = 15;
	WCHAR fontName[] = _T("MS Sans Serif");

	LOGFONT		log;
	
	log.lfHeight = (-MulDiv(iFontSize, GetDeviceCaps(*pDC, LOGPIXELSY), 72)); 
	log.lfWidth = 0;
	log.lfEscapement = 0;   
	log.lfOrientation = 0;   
	log.lfWeight = FW_REGULAR;   
	log.lfItalic =  false;
	log.lfUnderline = false;   
	log.lfStrikeOut = 0;   
	log.lfCharSet = ANSI_CHARSET;
	log.lfOutPrecision = OUT_DEFAULT_PRECIS;   
	log.lfClipPrecision = CLIP_DEFAULT_PRECIS;   
	log.lfQuality = ANTIALIASED_QUALITY;
	log.lfPitchAndFamily = DEFAULT_PITCH || FF_ROMAN;  
	
	wcscpy (log.lfFaceName, fontName);

	pFont->CreateFontIndirect (&log);
}
/***********************************************************************************************************************/

void CSudokuDlg::OnPaint()
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

	/**************************************************************************************************************/
	CDC* pDC = m_View.GetDC();

	CRect rcClient;
	m_View.GetClientRect(rcClient);

	pDC->SetBkColor(RGB(255,255,255));
	CBrush bkBrush(RGB(255,255,255));
	pDC->FillRect(rcClient,&bkBrush);

	DWORD dwWidth = rcClient.Width();
	DWORD dwHeight = rcClient.Height();

	DWORD dwFieldSizeX = dwWidth / 9;
	DWORD dwFieldSizeY = dwHeight / 9;

	CPen pen(PS_SOLID, 3, RGB(0,0,0));
	CPen penThin(PS_SOLID, 1, RGB(0,0,0));

	CPen* pOldPen = pDC->SelectObject(&pen);

	int iX, iY;
	int i, j;

	for (i = 0; i < 9; i++)
		for (j = 0; j < 9; j++)
			if (m_ActiveField.x == i && m_ActiveField.y == j)
			{
				CBrush bkBrush(RGB(227,231,232));
				CPoint rectStart(i * dwFieldSizeX + 1, j * dwFieldSizeY + 1);
				SIZE rectSize;
				rectSize.cx = dwFieldSizeX - 1;
				rectSize.cy = dwFieldSizeY - 1;
				CRect r(rectStart, rectSize);
				pDC->FillRect(r,&bkBrush);
			}

	pDC->MoveTo(0, 0);
	pDC->LineTo(dwWidth, 0);
	pDC->LineTo(dwWidth, dwHeight);
	pDC->LineTo(0, dwHeight);
	pDC->LineTo(0, 0);

	for (iX = 1; iX <= 8; iX++)
	{
		pDC->SelectObject((iX % 3 == 0) ? (&pen) : (&penThin));
		pDC->MoveTo(dwFieldSizeX * iX, 0);
		pDC->LineTo(dwFieldSizeX * iX, dwHeight);
	}

	for (iY = 1; iY <= 8; iY++)
	{
		pDC->SelectObject((iY % 3 == 0) ? (&pen) : (&penThin));
		pDC->MoveTo(0, dwFieldSizeY * iY);
		pDC->LineTo(dwWidth, dwFieldSizeY * iY);
	}

	CFont* pFont = new CFont();
	CreateSudokuFont(pFont, pDC);
	CGdiObject* pOldObject = pDC->SelectObject(pFont);

	WCHAR buf[32];

	pDC->SetBkMode(TRANSPARENT);

	for (i = 0; i < 9; i++)
		for (j = 0; j < 9; j++)
		{
			if (m_SudokuData[j][i] == 0) continue;

			SIZE txtSize;
			_itow(m_SudokuData[j][i], buf, 10);
			CString strText(buf);
			GetTextExtentPoint32(pDC->GetSafeHdc(), strText, strText.GetLength(), &txtSize);

			pDC->TextOut
			(
				i * dwFieldSizeX + ((dwFieldSizeX - txtSize.cx) / 2), 
				j * dwFieldSizeY + ((dwFieldSizeY - txtSize.cy) / 2) + 1, 
				strText, 
				strText.GetLength()
			);
		}
	
	pDC->SelectObject(pOldObject);

	pDC->SelectObject(pOldPen);
	/**************************************************************************************************************/
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSudokuDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


bool IsBadPath(DWORD * pStat, DWORD deep)
{
	if (deep < 10 || pStat[deep-1] < 10) return false;
	if (deep > 512) return true;

	DWORD sum = 0;
	DWORD sum2 = 0;

	for (int i = deep - 5; i < deep + 5; i++)
		sum += pStat[i];

	for (int i = 0; i < deep - 5; i++)
	{
		sum2 += pStat[i];
	}

	if (sum2 * 2 < sum) return true;

	return false;
}


int CSudokuDlg::SudokuSolveRecursive(BYTE ** pData)
{
	m_RecursDeep++;

	if (m_RecursDeep >= 900) 
	{
		MessageBox(_T("Переполнение!"));
		return -1;
	}

	m_RecurseDeepStat[m_RecursDeep]++;

	if (IsBadPath(m_RecurseDeepStat, m_RecursDeep)) 
	{
		m_RecursDeep--;
		return -1;
	}

	SetDlgItemInt(IDC_TMP_STATIC, m_RecursDeep);
	CPointSudoku pt;
	int iRet;

	// ищем клетку с наименьшим числом кандидатов:
	iRet = SudokuFindMinPoint(pt, pData); 

	if (iRet == 0) 
	{
		m_RecursDeep--;
		SetDlgItemInt(IDC_TMP_STATIC, m_RecursDeep);
		return 1; // решение найдено
	}
	if (iRet == -1) 
	{
		m_RecursDeep--;
		SetDlgItemInt(IDC_TMP_STATIC, m_RecursDeep);
		return -1; // тупиковая комбинация
	}

	BYTE ** pSaveData = SudokuCreateTempArray(pData);
	SudokuCopyArray(pData, pSaveData); // сохраняем то, что есть

	for (BYTE variance = 1; variance <= 9; variance++) // пытаемся ставить на найденную клетку цифры от 1 до 9
	{
		if (! pt.variances[variance]) continue;
		pData[pt.i][pt.j] = variance;
		
		// if (! CheckSudoku(pData)) continue; // такую цифру поставить нельзя. берём следующую.

		// OnPaint();
		Sleep(10);
	
		iRet = SudokuSolveRecursive(pData); // запускаем функцию рекурсивно

		if (iRet == -1) // тупиковая комбинация
		{
			SudokuCopyArray(pSaveData, pData); // восстанавливаем массив и берём следующую цифру на то же место
			continue;
		}

		if (iRet == 1) // решение найдено
		{
			SudokuDeleteTempArray(pSaveData);
			m_RecursDeep--;
			SetDlgItemInt(IDC_TMP_STATIC, m_RecursDeep);
			return 1;
		}
	}

	SudokuDeleteTempArray(pSaveData);
	m_RecursDeep--;
	SetDlgItemInt(IDC_TMP_STATIC, m_RecursDeep);

	return -1;
}


void CSudokuDlg::OnBnClickedStartButton()
{
	// Цель игры судоку - заполнить пустые клетки цифрами от 1 до 9 таким образом, 
	// чтобы по всем строкам и столбцам, а также внутри каждого квадрата 3*3 на 
	// игровом поле судоку цифры не повторялись.
/*
	BYTE ** pData = new BYTE*[9];

	for (int i = 0; i < 9; i++)
		pData[i] = new BYTE[9];

	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			pData[i][j] = m_SudokuData[i][j];
*/
////////////////////////////////////////////////////////

	if (!CheckSudoku(/*pData*/m_SudokuData))
	{
		MessageBox(_T("Неверное условие!"));
		return;
	}
	
	ifstream ifs("input.txt", ios::in);
	ofstream ofs("output.txt", ios::out);
	int num;
	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
		{
			ifs >> num;
			m_SudokuData[i][j] = (BYTE)num;
		}

	AddOneInPos = (ptr1)GetProcAddress(hLibrary, "AddOneInPos");
	RemoveOneFromPos = (ptr1)GetProcAddress(hLibrary, "RemoveOneFromPos");
	NumOf1 = (ptr2)GetProcAddress(hLibrary, "NumOf1");
	VerifyIsOne = (ptr1)GetProcAddress(hLibrary, "VerifyIsOne");

	if (!UpdateData(true))
		return;
	
	for (int i = 0; i < 9; ++i)
	{
		for (int j = 0; j < 9; ++j)
			m_LastSudokuData [i][j] = m_SudokuData[i][j];
	}
				

	//int N = atoi((const char*)m_num_of_sol.GetString());
	int N = m_number;
	solutions.reserve(N);
	CField MyField;
	MyField.ReadData(m_SudokuData);
	MyField.CalcFirstField();
	MyField.VerifQueue(MyField);
	MyField.Columns();
	MyField.Blocks();
	MyField.Lines();
	MyField.VerifQueue(MyField);
	
	MyField.Search(&solutions, N, m_flag);
	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
			m_SudokuData[i][j] = (BYTE)solutions[0].arr[i][j];
	//MyField.OutputData((int**)m_SudokuData);

	///////////////////////////////////////////////////
	m_RecursDeep = 0;
	memset(m_RecurseDeepStat, 0, sizeof(DWORD)*1024);

	//int iRet = SudokuSolveRecursive(/*pData*/m_SudokuData);
	

	//if (iRet == -1)
	//{
	//	MessageBox(_T("Решение не найдено."));
	//}
/*
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			m_SudokuData[i][j] = pData[i][j];

	for (int i = 0; i < 9; i++)
		delete [] pData[i];

	delete [] pData;
*/
	OnPaint();
}
/**************************************************************************************************************/

void CSudokuDlg::OnBnClickedClearButton()
{
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			m_SudokuData[i][j] = 0;

	m_ActiveField.x = 10;
	m_ActiveField.y = 10;

	OnPaint();
}

void CSudokuDlg::OnStnClickedSudokuView()
{
}

void CSudokuDlg::OnLButtonDown(UINT uFlags, CPoint p)
{
	CRect r;
	m_View.GetClientRect(r);

	CPoint ptClicked(p.x - 14, p.y - 14);

	if (!(r.top <= ptClicked.y && r.bottom >= ptClicked.y && 
		r.left <= ptClicked.x && r.right >= ptClicked.x)) return;

	DWORD dwFieldSizeX = r.Width() / 9;
	DWORD dwFieldSizeY = r.Height() / 9;
	
	m_ActiveField.x = ptClicked.x / dwFieldSizeX;
	m_ActiveField.y = ptClicked.y / dwFieldSizeY;

	OnPaint();

	return;
}

BOOL CSudokuDlg::PreTranslateMessage(MSG* pMsg)
{
	bool bNum = false, bNumPad = false;

	if 
	(
		pMsg->message == WM_KEYDOWN 
		&& 
		(
			(bNum = (pMsg->wParam >= 0x30 && pMsg->wParam <= 0x39)) // 0,1,2,3,4,5,6,7,8,9
			||
			(bNumPad = (pMsg->wParam >= VK_NUMPAD0 && pMsg->wParam <= VK_NUMPAD9)) // numpad0, numpad1, ..., numpad9
		)
		&&
		m_ActiveField.y < 10
		&&
		m_ActiveField.x < 10
	)
	{
		m_SudokuData[m_ActiveField.y][m_ActiveField.x] = 
			((BYTE)(pMsg->wParam)) - (bNum ? 0x30 : (VK_NUMPAD0));
		OnPaint();
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CSudokuDlg::OnBnClickedButton1()
{
	if (!UpdateData(true))
		return;
	int p = m_watch_number;
	if (p <= 0 || p > m_number)
	{
		MessageBox(_T("Несуществующий номер решения!"));
		return;
	}
	//m_out.ResetContent();
	//m_out.AddString(m_num_of_sol);
	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
			m_SudokuData[i][j] = (BYTE)solutions[m_watch_number - 1].arr[i][j];
	m_RecursDeep = 0;
	memset(m_RecurseDeepStat, 0, sizeof(DWORD)*1024);
	OnPaint();
	UpdateData(FALSE);
}

void CSudokuDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CSudokuDlg::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
}

void CSudokuDlg::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
}

void CSudokuDlg::OnBnClickedCheck2()
{
	// TODO: Add your control notification handler code here
}

void CSudokuDlg::OnBnClickedButton2()
{
		
	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
			m_SudokuData [i][j] = m_LastSudokuData[i][j];
	OnPaint();
}
