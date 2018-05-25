
// media_client_test_dlg.h : PROJECT_NAME 應用程式的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CMainApp: 
// 請參閱實作此類別的 media_client_test_dlg.cpp
//

class CDemoApp : public CWinApp
{
public:
	CDemoApp();

// 覆寫
public:
	virtual BOOL InitInstance();

// 程式碼實作

	DECLARE_MESSAGE_MAP()
};

extern CDemoApp theApp;