// SudokuDlg.h : header file
//


/***********************************************************************************
* Created by Drakon
* drakon@dubki.ru
* 2007
***********************************************************************************/


#pragma once
#include "afxwin.h"


// CSudokuDlg dialog
class CSudokuDlg : public CDialog
{
// Construction
public:
	CSudokuDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SUDOKU_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	void CSudokuDlg::OnBnClickedStartButton();
	void CSudokuDlg::OnBnClickedClearButton();

	BYTE* m_SudokuData[9];
	BYTE* m_LastSudokuData[9];
	CPoint m_ActiveField;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT, CPoint);
	afx_msg HCURSOR OnQueryDragIcon();

	int SudokuSolveRecursive(BYTE ** pData);
	int m_RecursDeep;
	DWORD m_RecurseDeepStat[1024];

	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_View;
public:
	afx_msg void OnStnClickedSudokuView();
	int NUM_OF_SOL;
	CListBox m_out;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeEdit1();
	CString m_num_of_sol;
	afx_msg void OnBnClickedCheck1();
	BOOL m_flag;
	int m_quantity;
	afx_msg void OnCbnSelchangeCombo1();
	int m_watch_number;
	int m_number;
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedButton2();
};
