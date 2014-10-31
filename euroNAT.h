// euroNAT.h : main header file for the euroNAT DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CeuroNATApp
// See euroNAT.cpp for the implementation of this class
//

class CeuroNATApp : public CWinApp
{
public:
	CeuroNATApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
