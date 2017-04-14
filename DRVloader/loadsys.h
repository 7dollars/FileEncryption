// loadsys.h : main header file for the LOADSYS application
//

#if !defined(AFX_LOADSYS_H__3B7DA2A7_F356_463D_B520_C216C0F2D812__INCLUDED_)
#define AFX_LOADSYS_H__3B7DA2A7_F356_463D_B520_C216C0F2D812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLoadsysApp:
// See loadsys.cpp for the implementation of this class
//

class CLoadsysApp : public CWinApp
{
public:
	CLoadsysApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadsysApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLoadsysApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
// Downloads By http://www.veryhuo.com

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADSYS_H__3B7DA2A7_F356_463D_B520_C216C0F2D812__INCLUDED_)
