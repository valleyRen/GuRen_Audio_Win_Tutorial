// SoundSetting.cpp : implementation file
//

#include "stdafx.h"
#include "SoundSetting.h"
#include "afxdialogex.h"
#include "Resource.h"


// CSoundSetting dialog

IMPLEMENT_DYNAMIC(CSoundSetting, CDialogEx)

CSoundSetting::CSoundSetting(IRtcAudioSystem* pSystem, CWnd* pParent /*=NULL*/)
: CDialogEx(IDD_SOUND_SETTING, pParent)
{
	m_pSystem = pSystem;
}

CSoundSetting::~CSoundSetting()
{
}

void CSoundSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSoundSetting, CDialogEx)
END_MESSAGE_MAP()


// CSoundSetting message handlers


BOOL CSoundSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	{
		string_sheet_ptr sheet;
		m_pSystem->GetDeviceList(true, sheet);

		CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBOX_INPUTS);

		string_ptr dft;
		m_pSystem->GetCurrentDevice(true, dft);

		for (int index = 0; index < sheet->size(); index++)
		{ 
			int nPos = pBox->AddString(sheet->item(index));
			if (dft->c_str() && 0 == stricmp(dft->c_str(), sheet->item(index)))
				pBox->SetCurSel(nPos);
		}
			
	}

	{
		string_sheet_ptr sheet;
		m_pSystem->GetDeviceList(false, sheet);

		CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBOX_OUTPUTS);

		string_ptr dft;
		m_pSystem->GetCurrentDevice(false, dft);

		for (int index = 0; index < sheet->size(); index++)
		{
			int nPos = pBox->AddString(sheet->item(index));
			if (dft->c_str() && 0 == stricmp(dft->c_str(), sheet->item(index)))
				pBox->SetCurSel(nPos);
		} 
	} 

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CSoundSetting::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBOX_INPUTS);
	int nSel = pBox->GetCurSel();
	if (LB_ERR != nSel)
	{
		CString strText;
		pBox->GetLBText(nSel, strText);
		m_pSystem->SetCurrentDevice(true, strText);
	}

	pBox = (CComboBox*)GetDlgItem(IDC_COMBOX_OUTPUTS);
	nSel = pBox->GetCurSel();
	if (LB_ERR != nSel)
	{
		CString strText;
		pBox->GetLBText(nSel, strText);
		m_pSystem->SetCurrentDevice(false, strText);
	}

	CDialogEx::OnOK();
}
