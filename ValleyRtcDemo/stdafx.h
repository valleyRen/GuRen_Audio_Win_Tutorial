
// stdafx.h : ���ڴ˘��^�n�а����˜ʵ�ϵ�y Include �n��
// ���ǽ���ʹ�Ås����׃����
// �������� Include �n��

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ���^�ų�����ʹ�õĳɆT
#endif

#include <string>

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ���_���x���ֵ� CString ������ʽ

// �P�] MFC �[��һЩ��Ҋ��ɺ��Ծ���ӍϢ�Ĺ���
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �����c�˜�Ԫ��
#include <afxext.h>         // MFC �U�书��


#include <afxdisp.h>        // MFC Automation e



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC ֧Ԯ�� Internet Explorer 4 ͨ�ÿ����
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC ֧Ԯ�� Windows ͨ�ÿ����
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // ���܅^�Ϳ����е� MFC ֧Ԯ









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


