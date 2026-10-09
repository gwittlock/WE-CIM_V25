
// EWMDoc.cpp : implementation of the CEWMDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "EWM.h"
#endif

#include "MainFrm.h"
#include "EWMDoc.h"
#include "EWMView.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEWMDoc

IMPLEMENT_DYNCREATE(CEWMDoc, CDocument)

BEGIN_MESSAGE_MAP(CEWMDoc, CDocument)
END_MESSAGE_MAP()


// CEWMDoc construction/destruction

CEWMDoc::CEWMDoc()
{
	// TODO: add one-time construction code here

}

CEWMDoc::~CEWMDoc()
{
}

BOOL CEWMDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CEWMDoc serialization

// NOTE: Our implementation of Serialize() is a little unusual because
// the EWM appplication is really just a simple text editor that is
// based on a CEditView which, in turn, is derived from CEdit. As such,
// it is really the CEdit control that acts as our document, hence, the
// efforts to obtain the active view.
void CEWMDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
		CEWMView *pView = (CEWMView*) pFrame->GetActiveView();
	
		// 'cause we want only the text from the view.
		pView->SerializeRaw( ar );
	}
	else
	{
		// Nothing to do 'cause we don't read files.
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CEWMDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CEWMDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CEWMDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CEWMDoc diagnostics

#ifdef _DEBUG
void CEWMDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEWMDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CEWMDoc commands
