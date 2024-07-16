
// SudokuSolverDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "SudokuSolver.h"
#include "Solver.h"
#include "SudokuSolverDlg.h"
#include "afxdialogex.h"
#include "TestsAndMisc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSudokuSolverDlg dialog
CSudokuSolverDlg::CSudokuSolverDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SUDOKUSOLVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSudokuSolverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSudokuSolverDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_2_DIMENSIONS, &CSudokuSolverDlg::OnBnClicked2Dimensions)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CSudokuSolverDlg::OnTcnSelchangeTab1)
	ON_BN_CLICKED(IDC_3_DIMENSIONS, &CSudokuSolverDlg::OnBnClicked3Dimensions)
	ON_COMMAND(ID_TESTS_2D_1, &CSudokuSolverDlg::OnTests2d1)
	ON_COMMAND(ID_TESTS_2D_2, &CSudokuSolverDlg::OnTests2d2)
	ON_COMMAND(ID_TESTS_2D_3, &CSudokuSolverDlg::OnTests2d3)
	ON_COMMAND(ID_TESTS_2D_4, &CSudokuSolverDlg::OnTests2d4)
	ON_COMMAND(ID_TESTS_2D_5, &CSudokuSolverDlg::OnTests2d5)
	ON_COMMAND(ID_TESTS_2D_6, &CSudokuSolverDlg::OnTests2d6)
	ON_COMMAND(ID_TESTS_3D_1, &CSudokuSolverDlg::OnTests3d1)
	ON_COMMAND(ID_ABOUT, &CSudokuSolverDlg::OnHelpAbout)
	ON_BN_CLICKED(IDC_SOLVE_BUTTON, &CSudokuSolverDlg::OnBnClickedSolveButton)
	ON_EN_CHANGE(IDC_CELL_0_0, &CSudokuSolverDlg::OnEnChangeCell00)
	ON_EN_CHANGE(IDC_CELL_0_1, &CSudokuSolverDlg::OnEnChangeCell01)
	ON_EN_CHANGE(IDC_CELL_0_2, &CSudokuSolverDlg::OnEnChangeCell02)
	ON_EN_CHANGE(IDC_CELL_0_3, &CSudokuSolverDlg::OnEnChangeCell03)
	ON_EN_CHANGE(IDC_CELL_0_4, &CSudokuSolverDlg::OnEnChangeCell04)
	ON_EN_CHANGE(IDC_CELL_0_5, &CSudokuSolverDlg::OnEnChangeCell05)
	ON_EN_CHANGE(IDC_CELL_0_6, &CSudokuSolverDlg::OnEnChangeCell06)
	ON_EN_CHANGE(IDC_CELL_0_7, &CSudokuSolverDlg::OnEnChangeCell07)
	ON_EN_CHANGE(IDC_CELL_0_8, &CSudokuSolverDlg::OnEnChangeCell08)
	ON_EN_CHANGE(IDC_CELL_1_0, &CSudokuSolverDlg::OnEnChangeCell10)
	ON_EN_CHANGE(IDC_CELL_1_1, &CSudokuSolverDlg::OnEnChangeCell11)
	ON_EN_CHANGE(IDC_CELL_1_2, &CSudokuSolverDlg::OnEnChangeCell12)
	ON_EN_CHANGE(IDC_CELL_1_3, &CSudokuSolverDlg::OnEnChangeCell13)
	ON_EN_CHANGE(IDC_CELL_1_4, &CSudokuSolverDlg::OnEnChangeCell14)
	ON_EN_CHANGE(IDC_CELL_1_5, &CSudokuSolverDlg::OnEnChangeCell15)
	ON_EN_CHANGE(IDC_CELL_1_6, &CSudokuSolverDlg::OnEnChangeCell16)
	ON_EN_CHANGE(IDC_CELL_1_7, &CSudokuSolverDlg::OnEnChangeCell17)
	ON_EN_CHANGE(IDC_CELL_1_8, &CSudokuSolverDlg::OnEnChangeCell18)
	ON_EN_CHANGE(IDC_CELL_2_0, &CSudokuSolverDlg::OnEnChangeCell20)
	ON_EN_CHANGE(IDC_CELL_2_1, &CSudokuSolverDlg::OnEnChangeCell21)
	ON_EN_CHANGE(IDC_CELL_2_2, &CSudokuSolverDlg::OnEnChangeCell22)
	ON_EN_CHANGE(IDC_CELL_2_3, &CSudokuSolverDlg::OnEnChangeCell23)
	ON_EN_CHANGE(IDC_CELL_2_4, &CSudokuSolverDlg::OnEnChangeCell24)
	ON_EN_CHANGE(IDC_CELL_2_5, &CSudokuSolverDlg::OnEnChangeCell25)
	ON_EN_CHANGE(IDC_CELL_2_6, &CSudokuSolverDlg::OnEnChangeCell26)
	ON_EN_CHANGE(IDC_CELL_2_7, &CSudokuSolverDlg::OnEnChangeCell27)
	ON_EN_CHANGE(IDC_CELL_2_8, &CSudokuSolverDlg::OnEnChangeCell28)
	ON_EN_CHANGE(IDC_CELL_3_0, &CSudokuSolverDlg::OnEnChangeCell30)
	ON_EN_CHANGE(IDC_CELL_3_1, &CSudokuSolverDlg::OnEnChangeCell31)
	ON_EN_CHANGE(IDC_CELL_3_2, &CSudokuSolverDlg::OnEnChangeCell32)
	ON_EN_CHANGE(IDC_CELL_3_3, &CSudokuSolverDlg::OnEnChangeCell33)
	ON_EN_CHANGE(IDC_CELL_3_4, &CSudokuSolverDlg::OnEnChangeCell34)
	ON_EN_CHANGE(IDC_CELL_3_5, &CSudokuSolverDlg::OnEnChangeCell35)
	ON_EN_CHANGE(IDC_CELL_3_6, &CSudokuSolverDlg::OnEnChangeCell36)
	ON_EN_CHANGE(IDC_CELL_3_7, &CSudokuSolverDlg::OnEnChangeCell37)
	ON_EN_CHANGE(IDC_CELL_3_8, &CSudokuSolverDlg::OnEnChangeCell38)
	ON_EN_CHANGE(IDC_CELL_4_0, &CSudokuSolverDlg::OnEnChangeCell40)
	ON_EN_CHANGE(IDC_CELL_4_1, &CSudokuSolverDlg::OnEnChangeCell41)
	ON_EN_CHANGE(IDC_CELL_4_2, &CSudokuSolverDlg::OnEnChangeCell42)
	ON_EN_CHANGE(IDC_CELL_4_3, &CSudokuSolverDlg::OnEnChangeCell43)
	ON_EN_CHANGE(IDC_CELL_4_4, &CSudokuSolverDlg::OnEnChangeCell44)
	ON_EN_CHANGE(IDC_CELL_4_5, &CSudokuSolverDlg::OnEnChangeCell45)
	ON_EN_CHANGE(IDC_CELL_4_6, &CSudokuSolverDlg::OnEnChangeCell46)
	ON_EN_CHANGE(IDC_CELL_4_7, &CSudokuSolverDlg::OnEnChangeCell47)
	ON_EN_CHANGE(IDC_CELL_4_8, &CSudokuSolverDlg::OnEnChangeCell48)
	ON_EN_CHANGE(IDC_CELL_5_0, &CSudokuSolverDlg::OnEnChangeCell50)
	ON_EN_CHANGE(IDC_CELL_5_1, &CSudokuSolverDlg::OnEnChangeCell51)
	ON_EN_CHANGE(IDC_CELL_5_2, &CSudokuSolverDlg::OnEnChangeCell52)
	ON_EN_CHANGE(IDC_CELL_5_3, &CSudokuSolverDlg::OnEnChangeCell53)
	ON_EN_CHANGE(IDC_CELL_5_4, &CSudokuSolverDlg::OnEnChangeCell54)
	ON_EN_CHANGE(IDC_CELL_5_5, &CSudokuSolverDlg::OnEnChangeCell55)
	ON_EN_CHANGE(IDC_CELL_5_6, &CSudokuSolverDlg::OnEnChangeCell56)
	ON_EN_CHANGE(IDC_CELL_5_7, &CSudokuSolverDlg::OnEnChangeCell57)
	ON_EN_CHANGE(IDC_CELL_5_8, &CSudokuSolverDlg::OnEnChangeCell58)
	ON_EN_CHANGE(IDC_CELL_6_0, &CSudokuSolverDlg::OnEnChangeCell60)
	ON_EN_CHANGE(IDC_CELL_6_1, &CSudokuSolverDlg::OnEnChangeCell61)
	ON_EN_CHANGE(IDC_CELL_6_2, &CSudokuSolverDlg::OnEnChangeCell62)
	ON_EN_CHANGE(IDC_CELL_6_3, &CSudokuSolverDlg::OnEnChangeCell63)
	ON_EN_CHANGE(IDC_CELL_6_4, &CSudokuSolverDlg::OnEnChangeCell64)
	ON_EN_CHANGE(IDC_CELL_6_5, &CSudokuSolverDlg::OnEnChangeCell65)
	ON_EN_CHANGE(IDC_CELL_6_6, &CSudokuSolverDlg::OnEnChangeCell66)
	ON_EN_CHANGE(IDC_CELL_6_7, &CSudokuSolverDlg::OnEnChangeCell67)
	ON_EN_CHANGE(IDC_CELL_6_8, &CSudokuSolverDlg::OnEnChangeCell68)
	ON_EN_CHANGE(IDC_CELL_7_0, &CSudokuSolverDlg::OnEnChangeCell70)
	ON_EN_CHANGE(IDC_CELL_7_1, &CSudokuSolverDlg::OnEnChangeCell71)
	ON_EN_CHANGE(IDC_CELL_7_2, &CSudokuSolverDlg::OnEnChangeCell72)
	ON_EN_CHANGE(IDC_CELL_7_3, &CSudokuSolverDlg::OnEnChangeCell73)
	ON_EN_CHANGE(IDC_CELL_7_4, &CSudokuSolverDlg::OnEnChangeCell74)
	ON_EN_CHANGE(IDC_CELL_7_5, &CSudokuSolverDlg::OnEnChangeCell75)
	ON_EN_CHANGE(IDC_CELL_7_6, &CSudokuSolverDlg::OnEnChangeCell76)
	ON_EN_CHANGE(IDC_CELL_7_7, &CSudokuSolverDlg::OnEnChangeCell77)
	ON_EN_CHANGE(IDC_CELL_7_8, &CSudokuSolverDlg::OnEnChangeCell78)
	ON_EN_CHANGE(IDC_CELL_8_0, &CSudokuSolverDlg::OnEnChangeCell80)
	ON_EN_CHANGE(IDC_CELL_8_1, &CSudokuSolverDlg::OnEnChangeCell81)
	ON_EN_CHANGE(IDC_CELL_8_2, &CSudokuSolverDlg::OnEnChangeCell82)
	ON_EN_CHANGE(IDC_CELL_8_3, &CSudokuSolverDlg::OnEnChangeCell83)
	ON_EN_CHANGE(IDC_CELL_8_4, &CSudokuSolverDlg::OnEnChangeCell84)
	ON_EN_CHANGE(IDC_CELL_8_5, &CSudokuSolverDlg::OnEnChangeCell85)
	ON_EN_CHANGE(IDC_CELL_8_6, &CSudokuSolverDlg::OnEnChangeCell86)
	ON_EN_CHANGE(IDC_CELL_8_7, &CSudokuSolverDlg::OnEnChangeCell87)
	ON_EN_CHANGE(IDC_CELL_8_8, &CSudokuSolverDlg::OnEnChangeCell88)
	ON_BN_CLICKED(IDC_RESET_BUTTON, &CSudokuSolverDlg::OnBnClickedResetButton)
	ON_BN_CLICKED(IDC_CHECK_DIAGONALS, &CSudokuSolverDlg::OnBnClickedCheckDiagonals)
END_MESSAGE_MAP()

// CSudokuSolverDlg message handlers

BOOL CSudokuSolverDlg::OnInitDialog()
{
	int down, across;
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Check 2D radio button.
	CButton *pRadioButton2D = (CButton*)this->GetDlgItem(IDC_2_DIMENSIONS);
	pRadioButton2D->SetCheck(true);

	// Create the 9 tabs and draw the 9x9 grid on each one.
	CTabCtrl *pTabControl = (CTabCtrl*)GetDlgItem(IDC_TAB1);
	pTabControl->InsertItem(0, _T("1"));
	pTabControl->InsertItem(1, _T("2"));
	pTabControl->InsertItem(2, _T("3"));
	pTabControl->InsertItem(3, _T("4"));
	pTabControl->InsertItem(4, _T("5"));
	pTabControl->InsertItem(5, _T("6"));
	pTabControl->InsertItem(6, _T("7"));
	pTabControl->InsertItem(7, _T("8"));
	pTabControl->InsertItem(8, _T("9"));
	pTabControl->SetCurFocus(0);
	pTabControl->EnableWindow(0);

	Solver.ClearCellContents();
	PositionCellsForTab();

	for (across = 0; across < 9; across++)
	{
		for (down = 0; down < 9; down++)
		{
			((CEdit*)GetDlgItem(IDC_CELL_0_0 + 9 * down + across))->SetLimitText(1);
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSudokuSolverDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSudokuSolverDlg::OnPaint()
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
HCURSOR CSudokuSolverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSudokuSolverDlg::OnBnClicked2Dimensions()
{
	CButton *pRadioButton2D = (CButton*)this->GetDlgItem(IDC_2_DIMENSIONS);
	if (pRadioButton2D->GetCheck())
	{
		CTabCtrl* pTabControl = (CTabCtrl*)GetDlgItem(IDC_TAB1);
		pTabControl->SetCurFocus(0);
		pTabControl->EnableWindow(0);
	}
	Solver.Dimensions = SUDOKU_2D;
}

void CSudokuSolverDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	PositionCellsForTab();
	*pResult = 0;
}


void CSudokuSolverDlg::OnBnClicked3Dimensions()
{
	CButton* pRadioButton3D = (CButton*)this->GetDlgItem(IDC_3_DIMENSIONS);
	if (pRadioButton3D->GetCheck())
	{
		CTabCtrl* pTabControl = (CTabCtrl*)GetDlgItem(IDC_TAB1);
		pTabControl->SetCurFocus(0);
		pTabControl->EnableWindow(1);
	}
	Solver.Dimensions = SUDOKU_3D;
}

void CSudokuSolverDlg::PositionCellsForTab(void)
{
	CTabCtrl* pTabControl = (CTabCtrl*)GetDlgItem(IDC_TAB1);
	int t = pTabControl->GetCurFocus();
	int down, across;
	unsigned int xInc, yInc;
	CString OneCharStr;
	uint16 Number;

	// Gradually move 9x9 cells to the right as user moves through tabs 1->9.
	for (across = 0; across < 9; across++)
	{
		xInc = 23 * across + (across > 2 ? 3 : 0) + (across > 5 ? 3 : 0);
		for (down = 0; down < 9; down++)
		{
			yInc = 23 * down + (down > 2 ? 3 : 0) + (down > 5 ? 3 : 0);
			CEdit* pCell = (CEdit*)GetDlgItem(IDC_CELL_0_0 + 9 * down + across);
			RECT CellRect, TabCtrlRect;

			CTabCtrl* pTabControl = (CTabCtrl*)GetDlgItem(IDC_TAB1);
			pTabControl->GetWindowRect(&TabCtrlRect);

			CellRect.left = 28 + xInc + 24 * t;
			CellRect.right = CellRect.left + 21;
			CellRect.top = 158 + yInc;
			CellRect.bottom = CellRect.top + 21;
			pCell->SetRect(&CellRect);
			pCell->MoveWindow(&CellRect, 1);

			Number = Solver.GetCellContents(t, down, across);
			if (Number == ALL_POSSIBILITIES)
			{
				OneCharStr = "";
			}
			else
			{
				OneCharStr = " ";
				OneCharStr.SetAt(0, Number + '0');
			}
			pCell->SetWindowText(OneCharStr);
		}
	}
}

void CSudokuSolverDlg::OnTests2d1() { SetUp2DTestSudoku(1); }
void CSudokuSolverDlg::OnTests2d2() { SetUp2DTestSudoku(2); }
void CSudokuSolverDlg::OnTests2d3() { SetUp2DTestSudoku(3); }
void CSudokuSolverDlg::OnTests2d4() { SetUp2DTestSudoku(4); }
void CSudokuSolverDlg::OnTests2d5() { SetUp2DTestSudoku(5); }
void CSudokuSolverDlg::OnTests2d6() { SetUp2DTestSudoku(6); }
void CSudokuSolverDlg::OnTests3d1() { SetUp3DTestSudoku(1); }
void CSudokuSolverDlg::OnHelpAbout() { DisplayHelpAboutDlg(); }

void CSudokuSolverDlg::SetUp2DTestSudoku(unsigned int IndexFrom1)
{
	uchar Across, Forward, StrAcross;
	CButton* pRadioButton;
	CEdit *pCell;
	CString OneCharStr;

	pRadioButton = (CButton*)this->GetDlgItem(IDC_2_DIMENSIONS);
	pRadioButton->SetCheck(true);
	pRadioButton = (CButton*)this->GetDlgItem(IDC_3_DIMENSIONS);
	pRadioButton->SetCheck(false);

	CButton *pCheckBox = (CButton*)this->GetDlgItem(IDC_CHECK_DIAGONALS);
	Solver.Diagonals = Tests_2D[IndexFrom1 - 1].Diagonals;
	pCheckBox->SetCheck(Solver.Diagonals);

	Solver.Dimensions = SUDOKU_2D;

	CTabCtrl* pTabControl = (CTabCtrl*)GetDlgItem(IDC_TAB1);
	pTabControl->SetCurFocus(0);
	pTabControl->EnableWindow(0);

	Solver.ClearCellContents();
	for (Forward = 0; Forward < 9; Forward++)
	{
		for (StrAcross = Across = 0; Across < 9; Across++, StrAcross++)
		{
			// Skip over "|" characters in test puzzle data.
			if (Across == 3 || Across == 6)
				StrAcross += 1;
			pCell = (CEdit*)GetDlgItem(IDC_CELL_0_0 + 9 * Forward + Across);
			OneCharStr = Tests_2D[IndexFrom1 - 1].GridStr[0][Forward][StrAcross];
			if (OneCharStr == " ")
			{
				pCell->SetWindowText(_T(""));
				Solver.SetCellContents(0, Forward, Across, 0);
			}
			else
			{
				pCell->SetWindowText(OneCharStr);
				Solver.SetCellContents(0, Forward, Across, OneCharStr[0] - '0');
			}
		}
	}
}

void CSudokuSolverDlg::DisplayHelpAboutDlg(void)
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CSudokuSolverDlg::SetUp3DTestSudoku(unsigned int IndexFrom1)
{
	uchar Up, Across, Forward, StrAcross;
	CButton* pRadioButton;
	CEdit* pCell;
	CString OneCharStr;

	pRadioButton = (CButton*)this->GetDlgItem(IDC_2_DIMENSIONS);
	pRadioButton->SetCheck(false);
	pRadioButton = (CButton*)this->GetDlgItem(IDC_3_DIMENSIONS);
	pRadioButton->SetCheck(true);

	CButton* pCheckBox = (CButton*)this->GetDlgItem(IDC_CHECK_DIAGONALS);
	Solver.Diagonals = Tests_3D[IndexFrom1 - 1].Diagonals;
	pCheckBox->SetCheck(Solver.Diagonals);

	Solver.Dimensions = SUDOKU_3D;

	CTabCtrl* pTabControl = (CTabCtrl*)GetDlgItem(IDC_TAB1);
	pTabControl->EnableWindow(1);
	pTabControl->SetCurFocus(0);

	Solver.ClearCellContents();
	for (Up = 0; Up < 9; Up++)
	{
		for (Forward = 0; Forward < 9; Forward++)
		{
			for (StrAcross = Across = 0; Across < 9; Across++, StrAcross++)
			{
				// Skip over "|" characters in test puzzle data.
				if (Across == 3 || Across == 6)
					StrAcross += 1;
				OneCharStr = Tests_3D[IndexFrom1 - 1].GridStr[Up][Forward][StrAcross];
				if (OneCharStr == " ")
				{
					if (Up == 0)
					{
						pCell = (CEdit*)GetDlgItem(IDC_CELL_0_0 + 9 * Forward + Across);
						pCell->SetWindowText(_T(""));
					}
					Solver.SetCellContents(Up, Forward, Across, 0);
				}
				else
				{
					if (Up == 0)
					{
						pCell = (CEdit*)GetDlgItem(IDC_CELL_0_0 + 9 * Forward + Across);
						pCell->SetWindowText(OneCharStr);
					}
					Solver.SetCellContents(Up, Forward, Across, OneCharStr[0] - '0');
				}
			}
		}
	}
}

void CSudokuSolverDlg::OnBnClickedSolveButton()
{
	eResult Result = Solver.SolveIt();

	if (Result == SUCCESS)
	{
		uchar Across, Forward;
		CEdit* pCell;
		CString OneCharStr("0");

		for (Forward = 0; Forward < 9; Forward++)
		{
			for (Across = 0; Across < 9; Across++)
			{
				pCell = (CEdit*)GetDlgItem(IDC_CELL_0_0 + 9 * Forward + Across);
				OneCharStr.SetAt(0, Solver.GetCellContents(0, Forward, Across) + '0');
				pCell->SetWindowText(OneCharStr);
			}
		}
		MessageBox(_T("Solved it."), _T("Result"), MB_OK);
	}
	else if (Result == FAILURE)
	{
		MessageBox(_T("Agh! Couldn't solve it."), _T("Result"), MB_OK);
	}
	else // BAD_INITIAL_DATA.
	{
		MessageBox(_T("Hmm. Are you sure the numbers you provided were correct? Please check and try again."), _T("Result"), MB_OK);
	}
}

void CSudokuSolverDlg::OnEnChangeCell00() { OnEnChangeCellAcrossForward( 0 , 0  ); }
void CSudokuSolverDlg::OnEnChangeCell01() { OnEnChangeCellAcrossForward( 1 , 0  ); }
void CSudokuSolverDlg::OnEnChangeCell02() { OnEnChangeCellAcrossForward( 2 , 0  ); }
void CSudokuSolverDlg::OnEnChangeCell03() { OnEnChangeCellAcrossForward( 3 , 0  ); }
void CSudokuSolverDlg::OnEnChangeCell04() { OnEnChangeCellAcrossForward( 4 , 0  ); }
void CSudokuSolverDlg::OnEnChangeCell05() { OnEnChangeCellAcrossForward( 5 , 0  ); }
void CSudokuSolverDlg::OnEnChangeCell06() { OnEnChangeCellAcrossForward( 6 , 0  ); }
void CSudokuSolverDlg::OnEnChangeCell07() { OnEnChangeCellAcrossForward( 7 , 0  ); }
void CSudokuSolverDlg::OnEnChangeCell08() { OnEnChangeCellAcrossForward( 8 , 0  ); }
void CSudokuSolverDlg::OnEnChangeCell10() { OnEnChangeCellAcrossForward( 0 , 1  ); }
void CSudokuSolverDlg::OnEnChangeCell11() { OnEnChangeCellAcrossForward( 1 , 1  ); }
void CSudokuSolverDlg::OnEnChangeCell12() { OnEnChangeCellAcrossForward( 2 , 1  ); }
void CSudokuSolverDlg::OnEnChangeCell13() { OnEnChangeCellAcrossForward( 3 , 1  ); }
void CSudokuSolverDlg::OnEnChangeCell14() { OnEnChangeCellAcrossForward( 4 , 1  ); }
void CSudokuSolverDlg::OnEnChangeCell15() { OnEnChangeCellAcrossForward( 5 , 1  ); }
void CSudokuSolverDlg::OnEnChangeCell16() { OnEnChangeCellAcrossForward( 6 , 1  ); }
void CSudokuSolverDlg::OnEnChangeCell17() { OnEnChangeCellAcrossForward( 7 , 1  ); }
void CSudokuSolverDlg::OnEnChangeCell18() { OnEnChangeCellAcrossForward( 8 , 1  ); }
void CSudokuSolverDlg::OnEnChangeCell20() { OnEnChangeCellAcrossForward( 0 , 2  ); }
void CSudokuSolverDlg::OnEnChangeCell21() { OnEnChangeCellAcrossForward( 1 , 2  ); }
void CSudokuSolverDlg::OnEnChangeCell22() { OnEnChangeCellAcrossForward( 2 , 2  ); }
void CSudokuSolverDlg::OnEnChangeCell23() { OnEnChangeCellAcrossForward( 3 , 2  ); }
void CSudokuSolverDlg::OnEnChangeCell24() { OnEnChangeCellAcrossForward( 4 , 2  ); }
void CSudokuSolverDlg::OnEnChangeCell25() { OnEnChangeCellAcrossForward( 5 , 2  ); }
void CSudokuSolverDlg::OnEnChangeCell26() { OnEnChangeCellAcrossForward( 6 , 2  ); }
void CSudokuSolverDlg::OnEnChangeCell27() { OnEnChangeCellAcrossForward( 7 , 2  ); }
void CSudokuSolverDlg::OnEnChangeCell28() { OnEnChangeCellAcrossForward( 8 , 2  ); }
void CSudokuSolverDlg::OnEnChangeCell30() { OnEnChangeCellAcrossForward( 0 , 3  ); }
void CSudokuSolverDlg::OnEnChangeCell31() { OnEnChangeCellAcrossForward( 1 , 3  ); }
void CSudokuSolverDlg::OnEnChangeCell32() { OnEnChangeCellAcrossForward( 2 , 3  ); }
void CSudokuSolverDlg::OnEnChangeCell33() { OnEnChangeCellAcrossForward( 3 , 3  ); }
void CSudokuSolverDlg::OnEnChangeCell34() { OnEnChangeCellAcrossForward( 4 , 3  ); }
void CSudokuSolverDlg::OnEnChangeCell35() { OnEnChangeCellAcrossForward( 5 , 3  ); }
void CSudokuSolverDlg::OnEnChangeCell36() { OnEnChangeCellAcrossForward( 6 , 3  ); }
void CSudokuSolverDlg::OnEnChangeCell37() { OnEnChangeCellAcrossForward( 7 , 3  ); }
void CSudokuSolverDlg::OnEnChangeCell38() { OnEnChangeCellAcrossForward( 8 , 3  ); }
void CSudokuSolverDlg::OnEnChangeCell40() { OnEnChangeCellAcrossForward( 0 , 4  ); }
void CSudokuSolverDlg::OnEnChangeCell41() { OnEnChangeCellAcrossForward( 1 , 4  ); }
void CSudokuSolverDlg::OnEnChangeCell42() { OnEnChangeCellAcrossForward( 2 , 4  ); }
void CSudokuSolverDlg::OnEnChangeCell43() { OnEnChangeCellAcrossForward( 3 , 4  ); }
void CSudokuSolverDlg::OnEnChangeCell44() { OnEnChangeCellAcrossForward( 4 , 4  ); }
void CSudokuSolverDlg::OnEnChangeCell45() { OnEnChangeCellAcrossForward( 5 , 4  ); }
void CSudokuSolverDlg::OnEnChangeCell46() { OnEnChangeCellAcrossForward( 6 , 4  ); }
void CSudokuSolverDlg::OnEnChangeCell47() { OnEnChangeCellAcrossForward( 7 , 4  ); }
void CSudokuSolverDlg::OnEnChangeCell48() { OnEnChangeCellAcrossForward( 8 , 4  ); }
void CSudokuSolverDlg::OnEnChangeCell50() { OnEnChangeCellAcrossForward( 0 , 5  ); }
void CSudokuSolverDlg::OnEnChangeCell51() { OnEnChangeCellAcrossForward( 1 , 5  ); }
void CSudokuSolverDlg::OnEnChangeCell52() { OnEnChangeCellAcrossForward( 2 , 5  ); }
void CSudokuSolverDlg::OnEnChangeCell53() { OnEnChangeCellAcrossForward( 3 , 5  ); }
void CSudokuSolverDlg::OnEnChangeCell54() { OnEnChangeCellAcrossForward( 4 , 5  ); }
void CSudokuSolverDlg::OnEnChangeCell55() { OnEnChangeCellAcrossForward( 5 , 5  ); }
void CSudokuSolverDlg::OnEnChangeCell56() { OnEnChangeCellAcrossForward( 6 , 5  ); }
void CSudokuSolverDlg::OnEnChangeCell57() { OnEnChangeCellAcrossForward( 7 , 5  ); }
void CSudokuSolverDlg::OnEnChangeCell58() { OnEnChangeCellAcrossForward( 8 , 5  ); }
void CSudokuSolverDlg::OnEnChangeCell60() { OnEnChangeCellAcrossForward( 0 , 6  ); }
void CSudokuSolverDlg::OnEnChangeCell61() { OnEnChangeCellAcrossForward( 1 , 6  ); }
void CSudokuSolverDlg::OnEnChangeCell62() { OnEnChangeCellAcrossForward( 2 , 6  ); }
void CSudokuSolverDlg::OnEnChangeCell63() { OnEnChangeCellAcrossForward( 3 , 6  ); }
void CSudokuSolverDlg::OnEnChangeCell64() { OnEnChangeCellAcrossForward( 4 , 6  ); }
void CSudokuSolverDlg::OnEnChangeCell65() { OnEnChangeCellAcrossForward( 5 , 6  ); }
void CSudokuSolverDlg::OnEnChangeCell66() { OnEnChangeCellAcrossForward( 6 , 6  ); }
void CSudokuSolverDlg::OnEnChangeCell67() { OnEnChangeCellAcrossForward( 7 , 6  ); }
void CSudokuSolverDlg::OnEnChangeCell68() { OnEnChangeCellAcrossForward( 8 , 6  ); }
void CSudokuSolverDlg::OnEnChangeCell70() { OnEnChangeCellAcrossForward( 0 , 7  ); }
void CSudokuSolverDlg::OnEnChangeCell71() { OnEnChangeCellAcrossForward( 1 , 7  ); }
void CSudokuSolverDlg::OnEnChangeCell72() { OnEnChangeCellAcrossForward( 2 , 7  ); }
void CSudokuSolverDlg::OnEnChangeCell73() { OnEnChangeCellAcrossForward( 3 , 7  ); }
void CSudokuSolverDlg::OnEnChangeCell74() { OnEnChangeCellAcrossForward( 4 , 7  ); }
void CSudokuSolverDlg::OnEnChangeCell75() { OnEnChangeCellAcrossForward( 5 , 7  ); }
void CSudokuSolverDlg::OnEnChangeCell76() { OnEnChangeCellAcrossForward( 6 , 7  ); }
void CSudokuSolverDlg::OnEnChangeCell77() { OnEnChangeCellAcrossForward( 7 , 7  ); }
void CSudokuSolverDlg::OnEnChangeCell78() { OnEnChangeCellAcrossForward( 8 , 7  ); }
void CSudokuSolverDlg::OnEnChangeCell80() { OnEnChangeCellAcrossForward( 0 , 8  ); }
void CSudokuSolverDlg::OnEnChangeCell81() { OnEnChangeCellAcrossForward( 1 , 8  ); }
void CSudokuSolverDlg::OnEnChangeCell82() { OnEnChangeCellAcrossForward( 2 , 8  ); }
void CSudokuSolverDlg::OnEnChangeCell83() { OnEnChangeCellAcrossForward( 3 , 8  ); }
void CSudokuSolverDlg::OnEnChangeCell84() { OnEnChangeCellAcrossForward( 4 , 8  ); }
void CSudokuSolverDlg::OnEnChangeCell85() { OnEnChangeCellAcrossForward( 5 , 8  ); }
void CSudokuSolverDlg::OnEnChangeCell86() { OnEnChangeCellAcrossForward( 6 , 8  ); }
void CSudokuSolverDlg::OnEnChangeCell87() { OnEnChangeCellAcrossForward( 7 , 8  ); }
void CSudokuSolverDlg::OnEnChangeCell88() { OnEnChangeCellAcrossForward( 8 , 8  ); }


void CSudokuSolverDlg::OnEnChangeCellAcrossForward(int Across, int Forward)
{
	CEdit* pCell;
	CString OneCharStr;
	int t;

	CTabCtrl* pTabControl = (CTabCtrl*)GetDlgItem(IDC_TAB1);
	t = pTabControl->GetCurFocus();
	
	pCell = (CEdit*)GetDlgItem(IDC_CELL_0_0 + 9 * Forward + Across);
	pCell->GetWindowText(OneCharStr);

	if (OneCharStr.GetAt(0) >= '1' && OneCharStr.GetAt(0) <= '9')
		Solver.SetCellContents(t, Forward, Across, OneCharStr.GetAt(0) - '0');
}


void CSudokuSolverDlg::OnBnClickedResetButton()
{
	uchar Across, Forward;
	CEdit* pCell;

	CTabCtrl* pTabControl = (CTabCtrl*)GetDlgItem(IDC_TAB1);
	pTabControl->SetCurFocus(0);

	Solver.ClearCellContents();
	for (Forward = 0; Forward < 9; Forward++)
	{
		for (Across = 0; Across < 9; Across++)
		{
			pCell = (CEdit*)GetDlgItem(IDC_CELL_0_0 + 9 * Forward + Across);
			pCell->SetWindowText(_T(""));
		}
	}
}


void CSudokuSolverDlg::OnBnClickedCheckDiagonals()
{
	CButton* pCheckBox = (CButton*)this->GetDlgItem(IDC_CHECK_DIAGONALS);
	Solver.Diagonals = pCheckBox->GetCheck();
}
