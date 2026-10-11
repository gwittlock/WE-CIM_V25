// SmartCamConvertorDlg.h : header file
//

#if !defined(AFX_SMARTCAMCONVERTORDLG_H__BAEC0F86_118A_11D6_919E_0040335A7818__INCLUDED_)
#define AFX_SMARTCAMCONVERTORDLG_H__BAEC0F86_118A_11D6_919E_0040335A7818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const int BAD_USAGE		= -1;
const int PM4JOF_TO_MM2	=  0;
const int JOF_TO_MDB	=  1;
const int DEBUG_DUMP	=  2;
const int INTERACTIVE	=  3;

class CPm4JofConvertor;



/////////////////////////////////////////////////////////////////////////////
// CSmartCamConvertorDlg dialog

class CSmartCamConvertorDlg : public CDialog
{
// Construction
public:
	
	CSmartCamConvertorDlg(CWnd* pParent = NULL);	// standard constructor

	void Debug( bool debug );
	void Interactive( bool interactive );

// Dialog Data
	//{{AFX_DATA(CSmartCamConvertorDlg)
	enum { IDD = IDD_SMARTCAMCONVERTOR_DIALOG };
	CComboBox	m_toolsetupCombo;
	CComboBox	m_actionCombo;
	CComboBox	m_machineCombo;
	CComboBox	m_materialCombo;
	CString	m_cmdbPath;
	CString	m_pm4Path;
	CString	m_jofPath;
	CString	m_mm2Path;
	BOOL	m_commonJof;
	BOOL	m_part_outline;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmartCamConvertorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSmartCamConvertorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCmdbBrowse();
	afx_msg void OnPm4Browse();
	afx_msg void OnJofBrowse();
	afx_msg void OnMm2Browse();
	afx_msg void OnSelchangeAction();
	virtual void OnOK();
	afx_msg void OnSelchangeMachineCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	void	InitVars();
	void	SaveVars();
	void	InitMachineCombo();
	void	InitToolSetupCombo();
	void	InitMaterialCombo();

private:

	int		m_action;
	bool	m_debug;
	bool	m_interactive;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMARTCAMCONVERTORDLG_H__BAEC0F86_118A_11D6_919E_0040335A7818__INCLUDED_)
