// FileDWGDlg.h : header file
//

#if !defined(AFX_FILEDWGDLG_H__B846D626_7A9A_11D5_919D_0040335A7818__INCLUDED_)
#define AFX_FILEDWGDLG_H__B846D626_7A9A_11D5_919D_0040335A7818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDwgThing;


/////////////////////////////////////////////////////////////////////////////
// CFileDWGDlg dialog

class CFileDWGDlg : public CDialog
{
// Construction
public:

	CFileDWGDlg(CWnd* pParent = NULL);	// standard constructor

	void ConvertorSet( CDwgThing& dwgConvertor )	{ m_dwgConvertor = &dwgConvertor; }

// Dialog Data
	//{{AFX_DATA(CFileDWGDlg)
	enum { IDD = IDD_FILEDWG_DIALOG };
	CComboBox	m_toolSetupCombo;
	CComboBox	m_materialCombo;
	CComboBox	m_layerSetupCombo;
	BOOL	m_analyze;
	BOOL	m_convert;
	CString	m_anlPath;
	CString	m_cmdbPath;
	CString	m_dwgPath;
	CString	m_mm2Path;
	int		m_toolSetupID;
	int		m_layerSetupID;
	int		m_materialID;
	BOOL	m_messages;
	BOOL	m_stripLayers;
	double	m_rotation;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileDWGDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFileDWGDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAnlBrowse();
	afx_msg void OnCmdbBrowse();
	afx_msg void OnDwgBrowse();
	afx_msg void OnMm2Browse();
	afx_msg void OnDumpsBrowse();
	afx_msg void OnAnalyze();
	afx_msg void OnConvert();
	virtual void OnOK();
	afx_msg void OnDumps();
	afx_msg void OnScan();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	void DimmingUpdate();

	void InitVars();
	void SaveVars();

	void InitCMDBvars( const CString& cmdbPath );
	void IDsInit();

private:

	CDwgThing*	m_dwgConvertor;

	CString		m_lastToolSetup;
	CString		m_lastLayerSetup;
	CString		m_lastMaterial;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEDWGDLG_H__B846D626_7A9A_11D5_919D_0040335A7818__INCLUDED_)
