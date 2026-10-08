// FilePeekDlg.h : header file
//

#if !defined(AFX_FILEPEEKDLG_H__02E2B69C_A638_11D6_B650_0003B3008E71__INCLUDED_)
#define AFX_FILEPEEKDLG_H__02E2B69C_A638_11D6_B650_0003B3008E71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _DWG_FILE_H
#include "dwg_file.h"
#endif

#include "FilePeekMsgs.h"
#include "Readout.h"
#include "OffsetWindow.h"
#include "DataWindow.h"


/////////////////////////////////////////////////////////////////////////////
// CFilePeekDlg dialog

class CFilePeekDlg : public CDialog
{
// Construction
public:
	CFilePeekDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFilePeekDlg)
	enum { IDD = IDD_FILEPEEK_DIALOG };
	COffsetWindow	m_offsetWin;
	CReadout	m_short;
	CReadout	m_long;
	CReadout	m_integer;
	CReadout	m_handle;
	CReadout	m_byte;
	CDataWindow	m_dataWin;
	CSpinButtonCtrl	m_bitSpinner;
	CSpinButtonCtrl	m_byteSpinner;
	CString	m_path;
	int		m_bits;
	long	m_bytes;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilePeekDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFilePeekDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBrowse();
	virtual void OnOK();
	afx_msg void OnDeltaposByteSpinner(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposBitSpinner(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeBytes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	CReturn	DataSeekDisplay( int bytes, int bits );

	void	DataDisplay();

	void	DataWinDisplay();

	void	ByteDisplay();
	void	ShortDisplay();
	void	IntegerDisplay();
	void	LongDisplay();
	void	HandleDisplay();

	void	Seek( int bytes, int bits );

	void	WinDisplay( int widgetID, const CString& sval );

	void	DataHighlight( WPARAM wParam, LPARAM lParam );

private:

	CDWGFile	m_file;

	int			m_base;	// index into file of first displayed byte
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEPEEKDLG_H__02E2B69C_A638_11D6_B650_0003B3008E71__INCLUDED_)
