
// SudokuSolverDlg.h : header file
//

#pragma once

#include "Solver.h"

// CSudokuSolverDlg dialog
class CSudokuSolverDlg : public CDialogEx
{
	// Construction
public:
	CSudokuSolverDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SUDOKUSOLVER_DIALOG };
#endif

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
	afx_msg void OnBnClicked2Dimensions();
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClicked3Dimensions();
private:
	cSolver Solver;
	void PositionCellsForTab(void);
	void SetUp2DTestSudoku(unsigned int IndexFrom1);
	void SetUp3DTestSudoku(unsigned int IndexFrom1);
	void DisplayHelpAboutDlg(void);
	void OnEnChangeCellAcrossForward(int Across, int Forward);
public:
	afx_msg void OnTests2d1();
	afx_msg void OnTests2d2();
	afx_msg void OnTests2d3();
	afx_msg void OnTests2d4();
	afx_msg void OnTests2d5();
	afx_msg void OnTests2d6();
	afx_msg void OnTests3d1();
	afx_msg void OnHelpAbout();
	afx_msg void OnBnClickedSolveButton();
	afx_msg void OnEnChangeCell00();
	afx_msg void OnEnChangeCell01();
	afx_msg void OnEnChangeCell02();
	afx_msg void OnEnChangeCell03();
	afx_msg void OnEnChangeCell04();
	afx_msg void OnEnChangeCell05();
	afx_msg void OnEnChangeCell06();
	afx_msg void OnEnChangeCell07();
	afx_msg void OnEnChangeCell08();
	afx_msg void OnEnChangeCell10();
	afx_msg void OnEnChangeCell11();
	afx_msg void OnEnChangeCell12();
	afx_msg void OnEnChangeCell13();
	afx_msg void OnEnChangeCell14();
	afx_msg void OnEnChangeCell15();
	afx_msg void OnEnChangeCell16();
	afx_msg void OnEnChangeCell17();
	afx_msg void OnEnChangeCell18();
	afx_msg void OnEnChangeCell20();
	afx_msg void OnEnChangeCell21();
	afx_msg void OnEnChangeCell22();
	afx_msg void OnEnChangeCell23();
	afx_msg void OnEnChangeCell24();
	afx_msg void OnEnChangeCell25();
	afx_msg void OnEnChangeCell26();
	afx_msg void OnEnChangeCell27();
	afx_msg void OnEnChangeCell28();
	afx_msg void OnEnChangeCell30();
	afx_msg void OnEnChangeCell31();
	afx_msg void OnEnChangeCell32();
	afx_msg void OnEnChangeCell33();
	afx_msg void OnEnChangeCell34();
	afx_msg void OnEnChangeCell35();
	afx_msg void OnEnChangeCell36();
	afx_msg void OnEnChangeCell37();
	afx_msg void OnEnChangeCell38();
	afx_msg void OnEnChangeCell40();
	afx_msg void OnEnChangeCell41();
	afx_msg void OnEnChangeCell42();
	afx_msg void OnEnChangeCell43();
	afx_msg void OnEnChangeCell44();
	afx_msg void OnEnChangeCell45();
	afx_msg void OnEnChangeCell46();
	afx_msg void OnEnChangeCell47();
	afx_msg void OnEnChangeCell48();
	afx_msg void OnEnChangeCell50();
	afx_msg void OnEnChangeCell51();
	afx_msg void OnEnChangeCell52();
	afx_msg void OnEnChangeCell53();
	afx_msg void OnEnChangeCell54();
	afx_msg void OnEnChangeCell55();
	afx_msg void OnEnChangeCell56();
	afx_msg void OnEnChangeCell57();
	afx_msg void OnEnChangeCell58();
	afx_msg void OnEnChangeCell60();
	afx_msg void OnEnChangeCell61();
	afx_msg void OnEnChangeCell62();
	afx_msg void OnEnChangeCell63();
	afx_msg void OnEnChangeCell64();
	afx_msg void OnEnChangeCell65();
	afx_msg void OnEnChangeCell66();
	afx_msg void OnEnChangeCell67();
	afx_msg void OnEnChangeCell68();
	afx_msg void OnEnChangeCell70();
	afx_msg void OnEnChangeCell71();
	afx_msg void OnEnChangeCell72();
	afx_msg void OnEnChangeCell73();
	afx_msg void OnEnChangeCell74();
	afx_msg void OnEnChangeCell75();
	afx_msg void OnEnChangeCell76();
	afx_msg void OnEnChangeCell77();
	afx_msg void OnEnChangeCell78();
	afx_msg void OnEnChangeCell80();
	afx_msg void OnEnChangeCell81();
	afx_msg void OnEnChangeCell82();
	afx_msg void OnEnChangeCell83();
	afx_msg void OnEnChangeCell84();
	afx_msg void OnEnChangeCell85();
	afx_msg void OnEnChangeCell86();
	afx_msg void OnEnChangeCell87();
	afx_msg void OnEnChangeCell88();
	afx_msg void OnBnClickedResetButton();
	afx_msg void OnBnClickedCheckDiagonals();
};
