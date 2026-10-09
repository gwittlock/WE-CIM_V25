// AutoPunchTestDlg.h : header file
//

#if !defined(AFX_AUTOPUNCHTESTDLG_H__B1353026_A83E_11D6_B650_0003B3008E71__INCLUDED_)
#define AFX_AUTOPUNCHTESTDLG_H__B1353026_A83E_11D6_B650_0003B3008E71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAutoPunchTestDlg dialog

class CAutoPunchTestDlg : public CDialog
{
// Construction
public:
	CAutoPunchTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAutoPunchTestDlg)
	enum { IDD = IDD_AUTOPUNCHTEST_DIALOG };
	CComboBox	m_materialCombo;
	CComboBox	m_layerSetupCombo;
	CComboBox	m_toolSetupCombo;
	CComboBox	m_machineCombo;
	BOOL	m_build;
	BOOL	m_folder;
	double	m_mtol;
	CString	m_path;
	double	m_ptol;
	CString	m_cmdbPath;
	CString	m_mm2Path;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoPunchTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAutoPunchTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCloseupMachineCombo();
	virtual void OnOK();
	afx_msg void OnCmdbBrowse();
	afx_msg void OnBrowse();
	afx_msg void OnMm2Browse();
	afx_msg void OnFolder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	void	InitVars();
	void	SaveVars();
	void	InitMachineCombo( BOOL initFromReg );
	void	InitToolSetupCombo( BOOL initFromReg );
	void	InitLayerSetupCombo( BOOL initFromReg );
	void	InitMaterialCombo( BOOL initFromReg );

	void	FilesGet(
				const CString&	path,
				const CString&	filter,
				CStringArray&	files );

private:

	CString	m_filter;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOPUNCHTESTDLG_H__B1353026_A83E_11D6_B650_0003B3008E71__INCLUDED_)
