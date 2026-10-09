
// EWMView.h : interface of the CEWMView class
//

#pragma once


class CEWMView : public CEditView
{
protected: // create from serialization only
	CEWMView();
	DECLARE_DYNCREATE(CEWMView)

// Attributes
public:
	CEWMDoc* GetDocument() const;

// Operations
public:

	void MessageDisplay( const char* text );

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CEWMView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
//	afx_msg void OnFileSaveAs();
	afx_msg void OnFileNew();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	BOOL PreTranslateKeyDownMessage(WPARAM wParam);
public:
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // debug version in EWMView.cpp
inline CEWMDoc* CEWMView::GetDocument() const
   { return reinterpret_cast<CEWMDoc*>(m_pDocument); }
#endif

