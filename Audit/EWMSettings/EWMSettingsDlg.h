// EWMSettingsDlg.h : header file
//

#if !defined(AFX_EWMSETTINGSDLG_H__EDAF2AC4_7A5B_48FB_A6AA_6CEC3A0D60FA__INCLUDED_)
#define AFX_EWMSETTINGSDLG_H__EDAF2AC4_7A5B_48FB_A6AA_6CEC3A0D60FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CEWMSettingsDlg dialog

class CEWMSettingsDlg : public CDialog
{
// Construction
public:
	CEWMSettingsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEWMSettingsDlg)
	enum { IDD = IDD_EWMSETTINGS_DIALOG };
	BOOL	m_check_dao;
	BOOL	m_check_diagnostics;
	BOOL	m_check_enable;
	BOOL	m_check_file;
	BOOL	m_check_filter;
	BOOL	m_check_nesting;
	BOOL	m_check_portal;
	BOOL	m_check_seeds;
	BOOL	m_check_ui;
	BOOL	m_check_user;
	BOOL	m_check_warnings;
	BOOL	m_check_internal;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEWMSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CEWMSettingsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonUpdate();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EWMSETTINGSDLG_H__EDAF2AC4_7A5B_48FB_A6AA_6CEC3A0D60FA__INCLUDED_)
