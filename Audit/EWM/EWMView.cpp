
// EWMView.cpp : implementation of the CEWMView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "EWM.h"
#endif

#include "EWMDoc.h"
#include "EWMView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "IpcSender.h"


// CEWMView

IMPLEMENT_DYNCREATE(CEWMView, CEditView)

BEGIN_MESSAGE_MAP(CEWMView, CEditView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CEditView::OnFilePrint)
	ON_WM_CREATE()
	// ON_COMMAND(ID_FILE_SAVE_AS, &CEWMView::OnFileSaveAs)
	ON_COMMAND(ID_FILE_NEW, &CEWMView::OnFileNew)
END_MESSAGE_MAP()

// CEWMView construction/destruction

CEWMView::CEWMView()
{
	// TODO: add construction code here

}

CEWMView::~CEWMView()
{
}

BOOL CEWMView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CEditView::PreCreateWindow(cs);
}

// CEWMView drawing

void CEWMView::OnDraw(CDC* /*pDC*/)
{
	CEWMDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CEWMView printing

BOOL CEWMView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return CEditView::OnPreparePrinting(pInfo);
}

void CEWMView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: add extra initialization before printing
	CEditView::OnBeginPrinting(pDC, pInfo);
}

void CEWMView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: add cleanup after printing
	CEditView::OnEndPrinting(pDC, pInfo);
}


// CEWMView diagnostics

#ifdef _DEBUG
void CEWMView::AssertValid() const
{
	CEditView::AssertValid();
}

void CEWMView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CEWMDoc* CEWMView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEWMDoc)));
	return (CEWMDoc*)m_pDocument;
}
#endif //_DEBUG


// CEWMView message handlers

void CEWMView::MessageDisplay( const char* text )
{
	// ShowWindow( SW_SHOW );
	BringWindowToTop();

	CEdit& edit = this->GetEditCtrl();

	CString buf;
	edit.GetWindowText( buf );

	buf += text;
	buf += "\r\n";

	edit.SetWindowText( buf );

	// Scroll to bottom.
	edit.LineScroll( edit.GetLineCount() );
}

#if 0
void CEWMView::OnFileSaveAs()
{
	CDocument * pDoc = GetDocument();
	
	char szFilters[]= "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||";

	CFileDialog fileDlg (FALSE, "txt", NULL, OFN_FILEMUSTEXIST| OFN_OVERWRITEPROMPT, szFilters, this);
   
	if( fileDlg.DoModal()==IDOK )
	{
		CFile theFile;
		theFile.Open( fileDlg.GetFileName(), CFile::modeWrite );
		CArchive archive( &theFile, CArchive::store );

		this->SerializeRaw( archive );

		archive.Close();
		theFile.Close();
	}
}
#endif

void CEWMView::OnFileNew()
{
	CEdit& edit = this->GetEditCtrl();

	// Select everything and then delete it.
	edit.SetSel(0, -1);
	edit.Clear();
}


BOOL CEWMView::PreTranslateMessage(MSG* pMsg)
{
	// http://www.codeproject.com/Articles/265869/Improved-CEdit-control
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		if (PreTranslateKeyDownMessage( pMsg->wParam) == TRUE)
			return TRUE;
		break;
	}

	return CEditView::PreTranslateMessage(pMsg);
}

BOOL CEWMView::PreTranslateKeyDownMessage(WPARAM wParam)
{
    switch (wParam)
	{
    case _T('A'):
        // Ctrl + 'A'
        if ((GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			CEdit& edit = this->GetEditCtrl();
            edit.SetSel(0, -1);
            return TRUE;
        }
        break;
	}

	return FALSE;
}


void CEWMView::OnInitialUpdate()
{
	CEditView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CEdit &curEditor = GetEditCtrl();
	CFont *SimpleFont = new CFont;

	// SimpleFont->CreatePointFont(120, "Times New Roman");  // the original font
	// SimpleFont->CreatePointFont(120, "Lucida Sans");
	SimpleFont->CreatePointFont(120, "Courier");
	curEditor.SetFont(SimpleFont);
}
