#pragma once

 
#include <ValleyRtcExtInterface.h>  

using namespace Valley;

class CSoundSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CSoundSetting)

public:
	CSoundSetting(IRtcAudioSystem* pSystem, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSoundSetting();
	 

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

public:
	IRtcAudioSystem* m_pSystem;
};
