
// media_client_test_dlg.h : PROJECT_NAME ���ó�ʽ����Ҫ���^�n
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �����˙n��ǰ�Ȱ��� 'stdafx.h'"
#endif

#include "resource.h"		// ��Ҫ��̖


// CMainApp: 
// Ո��醌�����e�� media_client_test_dlg.cpp
//

class CDemoApp : public CWinApp
{
public:
	CDemoApp();

// ����
public:
	virtual BOOL InitInstance();

// ��ʽ�a����

	DECLARE_MESSAGE_MAP()
};

extern CDemoApp theApp;