
// media_client_test_dlgDlg.cpp : �����n
//

#include "stdafx.h"
#include "afxdialogex.h" 
#include "DemoApp.h"
#include "DemoDLG.h"
#include "InputDLG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#if (defined _WIN64)||(defined WIN64) 
#pragma comment(lib, "../win64/libs/ValleyRtcAPI.lib")
#else
#pragma comment(lib, "../win32/libs/ValleyRtcAPI.lib")
#endif

// CDemoDLG ��Ԓ���K 

CDemoDLG::CDemoDLG(CWnd* pParent /*=NULL*/) : CDialogEx(CDemoDLG::IDD, pParent)
{
 	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	char appfolder[512];
	DWORD dwEnd = GetModuleFileNameA(NULL, appfolder, 512);
	appfolder[dwEnd] = 0;
	*(strrchr(appfolder, '\\')) = 0;
	strcat(appfolder, "\\wokfolder");

 
	//��ʼ������SDK, �������̸ú���ֻ��Ҫ����һ�Σ� ������SDK�����ˣ��������CleanSDK����ڴ���Դ���չ���
	//appfolder ����·��������Ҫ����ж�дȨ�ޣ����ڴ洢��־��������Ϣ�ļ�����ʱ�ļ��ȵĹ���·�� 
	InitSDK(appfolder);
	 

	//�����������ʵ����������InitSDK����ã������ʧ�ܣ�����������ʵ��ֻ�����ڵ�¼һ�����䣬���Ҫ��¼������䣬���Դ������ʵ�������
	m_pRoom = CreateChannel();


	//IRtcUsers::IID �����Ҫ�û����롢�˳����û��б����ع��ܣ�ͨ�����µ����������û�ģ�����ع���   
	//IRtcAudio::IID �����Ҫʵʱ�������ܣ�����ͨ�����µ���������ʵʱ����ģ�����ع���   
	//IRtcAudioSystem::IID �����Ҫ��ϵͳ�������ƣ�ͨ�����µ�������ع���  
	//IRtcDeviceControler::IID �����ҪƵ��֧������ģʽ��ͨ�����µ�������ع��� 
	m_pRoom->EnableInterface(IRtcUsers::IID | IRtcAudio::IID | IRtcAudioSystem::IID | IRtcDeviceControler::IID);


	 

	//ע��֪ͨ�ص������� usePoll ��������֪ͨ����ͬ���������߳����ģ����Ϊfalse,��ô����RespondXXX��NotifyXXX�ĺ�����ͨ��SDK�����߳�֪ͨ����,
	//���Ϊtrue,��ô�û������ڹ����̶߳�ʱ����m_pRoom->Poll()���������������߳�������RespondXXX��NotifyXXX�������������Poll()������ʱ�����Ҫ����200ms
	//�����OnTimer����
	m_pRoom->RegisterRtcSink(this, true); 

	m_audioDemo  = (Plugin::IAudioPlayerDemo*)CreatePlugin(Plugin::typeAudioDemo); 
	m_roomCtrler = (Plugin::IRoomControler*)CreatePlugin(Plugin::typeRoomControler);
}

CDemoDLG::~CDemoDLG()
{
	//�ͷŷ������ָ��
	//����Ϊtrue,��ô��Դ�����̻��գ�һ�����Ҫ�˳��ˣ������Ҫ�˳����̣�һ����������ȷ���˳�����ǰ��Դ�õ�����
	//����Ϊfalse,sdk�첽���գ�һ������˳���������̲��˳�����ָ�벻���ˣ���������ȷ��Release���ٷ���

	DWORD fromtime = GetTickCount();

	m_roomCtrler->Release();
	m_audioDemo->Release();
	m_pRoom->Release(true);

	TRACE("1111111111111  %d\n", GetTickCount()-fromtime);

	//��������SDK�ˣ�ͨ������CleanAudioSDK�ͷ� InitAudioSDK�������Դ,��������ΪSDK�����һ����������
	CleanSDK();

	TRACE("222222  %d\n", GetTickCount()-fromtime);
}

void CDemoDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDemoDLG, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOGIN, &CDemoDLG::OnBnClickedLogin)
	ON_BN_CLICKED(IDC_LOGOUT, &CDemoDLG::OnBnClickedLogout) 
	ON_BN_CLICKED(IDC_ENABLE_SPEAK, &CDemoDLG::OnBnClickedEnableSpeak)
	ON_BN_CLICKED(IDC_ENABLE_PLAYOUT, &CDemoDLG::OnBnClickedEnablePlayout)  
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SPK_VOLUME, &CDemoDLG::OnNMCustomdrawSpkVolume)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_BGM_VOLUME, &CDemoDLG::OnNMCustomdrawBGMVolume) 
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_BGM_TRACK, &CDemoDLG::OnNMCustomdrawBGMTrack)
 	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_BGM_TRACK, &CDemoDLG::OnNMCustomdrawBGMTrack)
	ON_WM_TIMER()  
	ON_NOTIFY(NM_RCLICK, IDC_USER_LIST, &CDemoDLG::OnNMRbClickUserList)
	ON_BN_CLICKED(IDC_ENABLE_MUSIC, &CDemoDLG::OnBnClickedEnablemusic) 
	ON_BN_CLICKED(IDC_BGM, &CDemoDLG::OnBnClickedBgm)
	ON_BN_CLICKED(IDC_ENABLE_RTMP, &CDemoDLG::OnBnClickedEnableRtmp)
	ON_COMMAND_RANGE(ID_MENU_SOUND, ID_MENU_SOUND+100, &CDemoDLG::OnClickedMenuItem)
END_MESSAGE_MAP()
 
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//������demo���������һЩ����

//��ʼ���Ի���
BOOL CDemoDLG::OnInitDialog()
{
	CDialogEx::OnInitDialog(); 
	 
	SetIcon(m_hIcon, TRUE);		 
	SetIcon(m_hIcon, FALSE);	

	CMenu mu;
	mu.LoadMenuA(IDR_MENU1);

	::SetMenu(m_hWnd, mu.GetSafeHmenu());
	mu.Detach();
	 
  
	CString szAuthoKey = AfxGetApp()->GetProfileString(_T("ui"), _T("authokey"), "");
	CString szRoomKey  = AfxGetApp()->GetProfileString(_T("ui"), _T("roomkey"), "8");
	CString szUserID   = AfxGetApp()->GetProfileString(_T("ui"), _T("userid"), "guest");

	SetDlgItemText(IDC_ROOMID, szRoomKey);
	SetDlgItemText(IDC_USERID, szUserID);
	SetDlgItemText(IDC_AUTHO_KEY, szAuthoKey); 

	CheckLoginStatus(false, false);
	//������������
	((CSliderCtrl*)GetDlgItem(IDC_SPK_VOLUME))->SetRange(0, 100);
	((CSliderCtrl*)GetDlgItem(IDC_SPK_VOLUME))->SetPos(50);
	 
	((CSliderCtrl*)GetDlgItem(IDC_BGM_VOLUME))->SetRange(0, 100);
	((CSliderCtrl*)GetDlgItem(IDC_BGM_VOLUME))->SetPos((int)(0.1*100));

	((CSliderCtrl*)GetDlgItem(IDC_BGM_TRACK))->SetRange(0, 100);
	((CSliderCtrl*)GetDlgItem(IDC_BGM_TRACK))->SetPos(0); 

	//����TimerID=100�Ķ�ʱ���� ���ڵ���m_pRoom->Poll()
	SetTimer(100, 200, NULL);

	//����TimerID=101�Ķ�ʱ���� ����ˢ�����ڽ������û�״̬
	SetTimer(101, 500, NULL);

	((CButton*)GetDlgItem(IDC_ENABLE_SPEAK))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_ENABLE_PLAYOUT))->SetCheck(TRUE); 

	//�û��б�
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_USER_LIST);
	pList->InsertColumn(0, "User ID", 0, 160);
	pList->InsertColumn(1, "Status", 0, 120);
	pList->InsertColumn(2, "Description", 0, 80);
	pList->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	////��Ϣ�б�
	//pList = (CListCtrl*)GetDlgItem(IDC_MSG_LIST); 
	//pList->InsertColumn(0, "�û�ID", 0, 120);
	//pList->InsertColumn(1, "��Ϣ����", 0, 220);
	//pList->InsertColumn(2, "��Ϣ����", 0, 70);
	//pList->InsertColumn(3, "��Ϣ״̬", 0, 70);
	//pList->SetExtendedStyle(LVS_EX_FULLROWSELECT);
	 
	char caption[64];
	sprintf(caption, "ValleyRtcDemo-%s", GetSDKVersion());
	SetWindowTextA(caption);
	return TRUE;   
} 

void CDemoDLG::OnCancel()
{
	OnBnClickedLogout();
	CDialogEx::OnCancel();
}

void CDemoDLG::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);  

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		 
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		 
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
 
HCURSOR CDemoDLG::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//״̬������ʾ��Ϣ
void CDemoDLG::ShowLog(const CString& szlog)
{ 
	SetDlgItemText(IDC_STATUS, szlog);
}

//״̬������ʾ����
void CDemoDLG::ShowStatus(const char* op, int ec)
{
	CString msg;
	msg.Format("%s, ec=%d (%s)", op, ec, GetErrDesc(ec));
	SetDlgItemText(IDC_STATUS, msg);
}

//״̬������ʾ��Ϣ
void CDemoDLG::ShowStatus(const char* msg)
{
	SetDlgItemText(IDC_STATUS, msg);
}
 

void CDemoDLG::OnTimer(UINT_PTR timerID)
{
	if (100 == timerID)
	{
		// ����m_pRoom->Poll()����������RespondXXX��NotifyXXX�����ĵ��ã�����SDK����������Poll()�����Ĵ���ʱ�䣬����ʱ�� 
		// ��RespondXXX��NotifyXXX�ϲ㴦���߼�����
		
		m_pRoom->Poll();
	} 
	else if (101 == timerID)
	{
		if (!m_bTrackGraging)
		{
			unsigned long filetime = 0;
			m_audioDemo->GetFileTime(0, filetime);
			if (0 != filetime)
			{
				unsigned long playedtime = 0;
				m_audioDemo->GetPlayedTime(0, playedtime);
				int pos = playedtime * 100 / filetime;
				((CSliderCtrl*)GetDlgItem(IDC_BGM_TRACK))->SetPos(pos);
			}
		}

			

		////ˢ�½����û��б���Ϊ�����е��û���ÿ��500����֪ͨ���������Գ���500����δ֪ͨ������������Ϊ�Ѿ�ֹͣ����
		if (m_speakers.empty())
			return;

		CListCtrl* pUserList = (CListCtrl*)GetDlgItem(IDC_USER_LIST);

		uint32_t current = GetTickCount();

		auto it = m_speakers.begin();
		while (it != m_speakers.end())
		{
			if (current - it->second > 1000)
			{
				int nPos = FindUser(it->first.c_str());
				it = m_speakers.erase(it);

				if (-1 != nPos)
					pUserList->SetItemText(nPos, 1, "");

			}
			else
			{
				int nPos = FindUser(it->first.c_str());
				if (-1 == nPos)
				{
					it = m_speakers.erase(it);
				}
				else
				{
					pUserList->SetItemText(nPos, 1, "������...");
					it++;
				}
			}
		} 
	}

	CDialogEx::OnTimer(timerID);
}

//UI�ؼ�״̬ (��¼�У��Ѿ���¼)
void CDemoDLG::CheckLoginStatus(bool loginingflag, bool bLoginedflag)
{ 
	bool loginflag = bLoginedflag || loginingflag;

	GetDlgItem(IDC_ENABLE_SPEAK)->EnableWindow(bLoginedflag);
	GetDlgItem(IDC_ENABLE_PLAYOUT)->EnableWindow(bLoginedflag);
	//GetDlgItem(IDC_IMPUT_TEXT)->EnableWindow(bLoginedflag);
	//GetDlgItem(IDC_SEND_MESSAGE)->EnableWindow(bLoginedflag);
	//GetDlgItem(IDC_START_RECORD)->EnableWindow(bLoginedflag);
	//GetDlgItem(IDC_STOP_RECORD)->EnableWindow(false);
	GetDlgItem(IDC_SPK_VOLUME)->EnableWindow(bLoginedflag);
	GetDlgItem(IDC_LOGIN)->EnableWindow(!loginflag);
	GetDlgItem(IDC_LOGOUT)->EnableWindow(loginflag);

	GetDlgItem(IDC_ROOMID)->EnableWindow(!loginflag);
	GetDlgItem(IDC_USERID)->EnableWindow(!loginflag);
	GetDlgItem(IDC_AUTHO_KEY)->EnableWindow(!loginflag);

	GetDlgItem(IDC_BGM)->EnableWindow(bLoginedflag);
	GetDlgItem(IDC_ENABLE_RTMP)->EnableWindow(bLoginedflag);

	GetDlgItem(IDC_ENABLE_MUSIC)->EnableWindow(!bLoginedflag || nullptr != GetDeviceControler()); 
}
 

 
//�����������ڿؼ�(��Χ 0 ~ 100 С��50��������С������50�����Ŵ�)
void CDemoDLG::OnNMCustomdrawSpkVolume(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here

	CSliderCtrl*  pSlider = (CSliderCtrl*)GetDlgItem(IDC_SPK_VOLUME);

	if (GetAudioSystem())
		GetAudioSystem()->SetPlayoutVolume(pSlider->GetPos());

	*pResult = 0;
}
 
//�û��б�UI
void CDemoDLG::OnNMRbClickUserList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
 
	CListCtrl* pUserList = (CListCtrl*)GetDlgItem(IDC_USER_LIST);
	POSITION pos = pUserList->GetFirstSelectedItemPosition();
	if (NULL != pos)
	{
		int nSelIndex = pUserList->GetNextSelectedItem(pos);
		if (nSelIndex < 0)
		{
			*pResult = 0;
			return;
		}

		CString userid = pUserList->GetItemText(nSelIndex, 0);

		CString selfuserid;
		GetDlgItemText(IDC_USERID, selfuserid);
		if (0 == selfuserid.Compare(userid))
			return;
			

		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu(MF_STRING, 1, "���û��߳�����");
		menu.AppendMenu(MF_STRING, 2, "��������");
		menu.AppendMenu(MF_STRING, 3, "���Բ���");

		POINT pt;
		GetCursorPos(&pt);

		int cmd = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, pt.x, pt.y, this);

		if (1 == cmd)
		{
			GetUserModule()->KickOff(userid);
		} 
		else if (2 == cmd)
		{
			user_ptr user;
			if (0 == GetUserModule()->GetUser(userid, user))
			{
				GetAudioModule()->BlockUser(userid, !user->getBlocked());
			} 
		}

		else if (3 == cmd)
		{
			user_ptr user;
			if (0 == GetUserModule()->GetUser(userid, user))
			{
				GetAudioModule()->DisableUserSpeak(userid, !user->getDisableSpeak());
			} 
		}
	}

	*pResult = 0;
}

//ˢ���û��б�UI
void CDemoDLG::UpdateUserList()
{
	user_sheet_ptr sheet; 
	int ec = GetUserModule()->GetUserList(sheet);
	if (ERR_SUCCEED != ec)
	{
		ShowStatus("�����û��б�ʧ��", ec);
		OutputDebugString("�����û��б�ʧ��\n");
		return;
	}

	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_USER_LIST);
	pList->DeleteAllItems();

	int size = sheet->size();
	for (int index = 0; index < size; index++)
	{ 
		const object_user* user = sheet->item(index);
		AddUser(user);
	}
}

//��ʾһ���û���UI
void CDemoDLG::AddUser(const object_user* user)
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_USER_LIST); 
	int count = pList->GetItemCount();
	pList->InsertItem(count, user->getUserID());

	CString str;
	str.Format("%s EnableSpeak: %s\n", user->getUserID(), user->getAttr("EnableSpeak")); 
	OutputDebugString(str);
	ShowLog(str); 
}

//�Ƴ�һ���û�UI
void CDemoDLG::RemoveUser(const char* userid)
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_USER_LIST);

	int nIndex = FindUser(userid);
	if (-1 != nIndex)
		pList->DeleteItem(nIndex);
}

//����һ���û�UI
int CDemoDLG::FindUser(const char* userid)
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_USER_LIST);
	int        itemcount = pList->GetItemCount();
	for (int index = 0; index < itemcount; index++)
	{
		CString uid = pList->GetItemText(index, 0);
		if (0 == uid.Compare(userid))
			return index;
	}

	return -1; 
}
  
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/************************************************************************ 
 * ִ�н��뷿�����
 * ������Ҫ��Ҫ��Ҫ�������º���
 * SetAuthoKey ������øú���������Ȩ�룬��ô��SDK�ܵ�¼���Է��䣬���Է���roomkey ֻ���� 1 ~ 99
 * EnablePullUserList �û��б��Ƿ���Ҫ�·����������Ϊ false, ��ô��¼�ɹ������յ��û��б��û���������Ҳ�����յ�֪ͨ
 * EnableRealAudio �Ƿ���Ҫ����ʵʱ��������
 * EnableAudioMessage �Ƿ���Ҫ����������Ϣ���� 
 * Login ��¼����¼�ɹ���ʧ���ɺ���RespondLogin�յ�
************************************************************************/
void CDemoDLG::OnBnClickedLogin()
{

	CString szRoomKey;
	CString szUserID;
	CString szAuthoKey;

	GetDlgItemText(IDC_ROOMID, szRoomKey);
	GetDlgItemText(IDC_USERID, szUserID);
	GetDlgItemText(IDC_AUTHO_KEY, szAuthoKey);
	AfxGetApp()->WriteProfileString(_T("ui"), _T("roomkey"), szRoomKey);
	AfxGetApp()->WriteProfileString(_T("ui"), _T("userid"), szUserID);
	AfxGetApp()->WriteProfileString(_T("ui"), _T("authokey"), szAuthoKey);

	//szAuthoKey = "5a00e500d503f7f6ollnTdu";

	SetAuthoKey(szAuthoKey);
	if (szAuthoKey.IsEmpty() && (0 == szRoomKey.Compare(_T("7")) || 0 == szRoomKey.Compare(_T("8")) || 0 == szRoomKey.Compare(_T("9"))))
	{  
	 	GetDeviceControler()->Enable(IRtcDeviceControler::typeMusicMode, true); 
		GetDeviceControler()->Enable(IRtcDeviceControler::typeBackgroundMusic, true); 

		bool bCheck = ((CButton*)GetDlgItem(IDC_ENABLE_MUSIC))->GetCheck(); 
		
		// | IRtcDeviceControler::typeAgc
		uint32_t nKey = IRtcDeviceControler::typeAec | IRtcDeviceControler::typeNs | IRtcDeviceControler::typeVad;
		if (bCheck) //���� 
			GetDeviceControler()->Enable(nKey, false);
		else //��ͨ�û� 
			GetDeviceControler()->Enable(nKey, true);
	}
	else
	{
	 	GetDeviceControler()->Enable(IRtcDeviceControler::typeMusicMode, false); 
		GetDeviceControler()->Enable(IRtcDeviceControler::typeBackgroundMusic, true);
	} 
	  
	bool bCheck = ((CButton*)GetDlgItem(IDC_ENABLE_SPEAK))->GetCheck();
	GetAudioModule()->EnableSpeak(bCheck); 

	//GetDeviceControler()->EnalbleStreamTransCallback(true);

	int ec = m_pRoom->Login(szRoomKey, szUserID, nullptr);
	if (ERR_SUCCEED == ec)
	{
		ShowStatus("���뷿�� ...");
	 	CheckLoginStatus(true, false);
	}
	else
	{
		ShowStatus("���뷿��ʧ��", ec);
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/************************************************************************
* ִ���˳�������� 
* Logout �ǳ����䣬ֻҪ�ڷ������棬��������˳�ʧ�ܵ����
************************************************************************/
void CDemoDLG::OnBnClickedLogout()
{ 
	m_audioDemo->Stop(-1);
	m_roomCtrler->StopRtmp(); 
	m_roomCtrler->StopRecord();
	((CButton*)GetDlgItem(IDC_BGM))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_ENABLE_RTMP))->SetCheck(0); 

	m_pRoom->Logout();
	CheckLoginStatus(false, false);
	ShowStatus("�Ѿ��˳�����");

	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_USER_LIST);
	pList->DeleteAllItems();

	//pList = (CListCtrl*)GetDlgItem(IDC_MSG_LIST);
	//pList->DeleteAllItems();
}


/************************************************************************
* ִ��ʵʱ������������Musicģʽ 
************************************************************************/
void CDemoDLG::OnBnClickedEnablemusic()
{ 
	if (GetDeviceControler() && GetDeviceControler()->IsEnabled(IRtcDeviceControler::typeMusicMode))
	{
		bool     bCheck = ((CButton*)GetDlgItem(IDC_ENABLE_MUSIC))->GetCheck();

		//GetDeviceControler()->Enable(IRtcDeviceControler::typeCtrlByHeadset, false);

		//bool bE = GetDeviceControler()->IsEnabled(IRtcDeviceControler::typeCtrlByHeadset);

		//if (bCheck) //��������
		//{
		//	GetDeviceControler()->Enable(IRtcDeviceControler::typeCtrlByHeadset, true);
		//}
		//else
		//{
		//	GetDeviceControler()->Enable(IRtcDeviceControler::typeCtrlByHeadset, false);

			uint32_t nKey = IRtcDeviceControler::typeAec | IRtcDeviceControler::typeNs | IRtcDeviceControler::typeVad;
			if (bCheck) //���� 
				GetDeviceControler()->Enable(nKey, false);
			else //��ͨ�û� 
				GetDeviceControler()->Enable(nKey, true);
		//} 
	} 
}
 


/************************************************************************
* ִ��ʵʱ������������
* Ĭ������£����������ǹرյģ�������Ҫ����ʱ����EnableSpeak �򿪽������� 
************************************************************************/
void CDemoDLG::OnBnClickedEnableSpeak()
{
	bool bCheck = ((CButton*)GetDlgItem(IDC_ENABLE_SPEAK))->GetCheck();
	GetAudioModule()->EnableSpeak(bCheck);

	CString szUserID;
	GetDlgItemText(IDC_USERID, szUserID);

	GetUserModule()->SetUserAttr(szUserID, "EnableSpeak", bCheck?"1":nullptr);
}


/************************************************************************
* ִ��ʵʱ�������Ź���
* Ĭ������£����Ź����ǿ��ŵģ����Ե��ô˽ӿڹرղ��Ź���
************************************************************************/
void CDemoDLG::OnBnClickedEnablePlayout()
{
	bool bCheck = ((CButton*)GetDlgItem(IDC_ENABLE_PLAYOUT))->GetCheck();
	GetAudioModule()->EnablePlayout(bCheck);
}
  


//�����������ڿؼ�(��Χ 0 ~ 100)
void CDemoDLG::OnNMCustomdrawBGMVolume(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here

	CSliderCtrl*  pSlider = (CSliderCtrl*)GetDlgItem(IDC_BGM_VOLUME);

	float bgmVolumeScale = (float)pSlider->GetPos() / 100;
	m_audioDemo->SetVolume(TRACKINDEX_ALL, bgmVolumeScale);

	*pResult = 0;
}

//�������Ž�����(��Χ 0 ~ 100)
void CDemoDLG::OnNMCustomdrawBGMTrack(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (NM_RELEASEDCAPTURE == pNMHDR->code)
	{ 
		m_bTrackGraging = false;
		unsigned long filetime = 0;
		m_audioDemo->GetFileTime(0, filetime);
		if (0 != filetime)
		{
			int nPos = ((CSliderCtrl*)GetDlgItem(IDC_BGM_TRACK))->GetPos();
			unsigned long  playtime = filetime * nPos / 100;
			m_audioDemo->SetPlayTime(0, playtime);
		}
	}
	else if (NM_CUSTOMDRAW == pNMHDR->code)
	{
		LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
		if (0 != pNMCD->uItemState)
		{
			CWnd* pWnd = GetCapture();
			if (pWnd == GetDlgItem(IDC_BGM_TRACK))
			   m_bTrackGraging = true;
			else
			   m_bTrackGraging = false;
		}
			
	} 
 }
 

 
 

/************************************************************************
* �򿪱�������
************************************************************************/
void CDemoDLG::OnBnClickedBgm()
{ 
	bool bCheck = ((CButton*)GetDlgItem(IDC_BGM))->GetCheck();
	if (bCheck)
	{
		CFileDialog fd(TRUE, "*.mp3", 0, 0, "*.mp3|*.mp3||");
		if (IDOK == fd.DoModal())
		{
			CString filePath = fd.GetPathName();
			const char* pfilename = filePath;
			((CSliderCtrl*)GetDlgItem(IDC_BGM_VOLUME))->SetPos((int)(0.1 * 100));
			m_audioDemo->Play(0, GetDeviceControler(), pfilename, true, 0.1, true, true);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_BGM))->SetCheck(0);
		}
	}
	else
	{
		m_audioDemo->Stop(0);
	}

}


void CDemoDLG::OnBnClickedEnableRtmp()
{
	bool bCheck = ((CButton*)GetDlgItem(IDC_ENABLE_RTMP))->GetCheck();
	if (bCheck)
	{ 
		CInputDLG dlg;
		dlg.m_inputText = AfxGetApp()->GetProfileString(_T("ui"), _T("rtmurl"), _T("rtmp://127.0.0.1:1335/test/roomid"));
		
		if (IDOK == dlg.DoModal())
		{  
			AfxGetApp()->WriteProfileString(_T("ui"), _T("rtmurl"), dlg.m_inputText);
			m_roomCtrler->Init(m_pRoom);
			 
			m_roomCtrler->StartRtmp(dlg.m_inputText);// rtmp://push.paoba1.qingcdn.com/live"); 
		} 
	}
	else
	{
		m_roomCtrler->StopRtmp();	
	}
}


//////////////////////////////////////////////////////////////////////////
//�������յ���������Ĵ�����
void CDemoDLG::Respond(int type, int ec, object_base* ob, void* userdata)
{
	switch (type)
	{
		/************************************************************************
		* �յ���¼�ɹ���ʧ�ܵĴ�����
		* ��¼�ɹ���һ����Ҫ�����²���
		* �����û��б�
		* ����UI�ϵ���Ϣ�б����յ���Ϣ�б��ˢ����ʷ��Ϣ
		************************************************************************/
		case IRtcChannel::RespondLogin:
		{
			object_login* resob = (object_login*)ob;

			//��¼�ɹ�
			if (ERR_SUCCEED == ec)
			{
			 	CheckLoginStatus(false, true);
				ShowStatus("���뷿��ɹ�");
				UpdateUserList();

				//CListCtrl* pMsgList = (CListCtrl*)GetDlgItem(IDC_MSG_LIST);
				//pMsgList->DeleteAllItems();

			}
			//��¼ʧ�� 
			else
			{
				CheckLoginStatus(false, false);
				 
				ShowStatus("���뷿��ʧ��", ec);
			}
		}
		break;

		/************************************************************************
		* �յ����û��߳�����ɹ���ʧ�ܵĴ����� 
		************************************************************************/
		case IRtcUsers::RespondKickOff:
		{
			object_userid* resob = (object_userid*)ob;

			if (ERR_SUCCEED == ec)
			{
				RemoveUser(resob->getUserID());

				CString str;
				str.Format("%s �Ѿ������߳�����", resob->getUserID());
				ShowLog(str);
			}
			else
			{
				ShowStatus("�߳��û�ʧ��", ec);
			}
		}
		break;


		/************************************************************************
		* �յ���ֹĳ�˷��Գɹ���ʧ�ܵĴ�����
		************************************************************************/
		case IRtcAudio::RespondDisableUserSpeak:
		{
			if (ERR_SUCCEED == ec)
			{
				object_disable_speak* resob = (object_disable_speak*)ob;

				CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_USER_LIST);
				int nIdex = FindUser(resob->getUserID());

				if (-1 != nIdex)
					pList->SetItemText(nIdex, 2, resob->getDisabled() ? "������" : "");
			}
			else
			{
				ShowStatus("���Բ���ʧ��", ec);
			}
		}
		break;


		/************************************************************************
		* �յ�����ĳ�������ɹ���ʧ�ܵĴ���
		************************************************************************/
		case IRtcAudio::RespondBlockUser:
		{
			if (ERR_SUCCEED == ec)
			{
				object_block_speak* resob = (object_block_speak*)ob;

				CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_USER_LIST);
				int nIdex = FindUser(resob->getUserID());
				if (-1 != nIdex)
					pList->SetItemText(nIdex, 2, resob->getBlocked() ? "������" : "");
			}
			else
			{
				ShowStatus("��������ʧ��", ec);
			}
		}
		break;

		/************************************************************************
		*  �յ��޸��û����ɹ���ʧ�ܵĴ���
		************************************************************************/
		case IRtcUsers::RespondUserAttr:
		{
			object_user_attr* ntfv = (object_user_attr*)ob;
		}
		break;

		/************************************************************************
		* �յ��޸ķ������Գɹ���ʧ�ܵĴ���
		************************************************************************/
		case IRtcChannel::RespondChannelAttr:
		{
			object_channel_attr* ntfv = (object_channel_attr*)ob;
		}
		break;
		 
		default:
			break;
	}
}

void CDemoDLG::OnTransAudioStream(eStemType stype, void* pcm, int len, int channel, int samplerate, const void* userdata)
{

}

//////////////////////////////////////////////////////////////////////////
//�������յ������������֪ͨ�Ĵ�����
void CDemoDLG::Notify(int type, object_base* ob, void* userdata)
{
	switch (type)
	{
		/************************************************************************
		* �յ�����Ͽ���֪ͨ
		* SDK���Զ����������¼�һ��ֻ��Ҫ��UI�����ֵ�ǰ���粻ͨ
		***********************************************************************/
		case IRtcChannel::NotifyConnectionLost:
		{
			ShowStatus("����Ͽ��������� ...");
			CheckLoginStatus(true, false);
		}
		break;

		/************************************************************************
		* �յ�SDK�����ɹ���֪ͨ
		* �����ɹ�������¼�ɹ�һ�����û��б����Ϣ�б���Ҫ����
		************************************************************************/
		case IRtcChannel::NotifyReConnected:
		{
			ShowStatus("�����ɹ�");
			UpdateUserList();

			CListCtrl* pMsgList = (CListCtrl*)GetDlgItem(IDC_MSG_LIST);
			if (nullptr != pMsgList)
				pMsgList->DeleteAllItems();

			CheckLoginStatus(false, true);
		}
		break;

		/************************************************************************
		* �յ����ʺ�����ص�¼��֪ͨ 
		************************************************************************/
		case IRtcChannel::NotifyDuplicateLogined:
		{
			AfxMessageBox("���ʺ��Ѿ�����ص�¼");
		}
		break;

		/************************************************************************
		* �յ��÷��䱻�رյ�֪ͨ����
		************************************************************************/
		case IRtcChannel::NotifyChannelClose:
		{
			object_error* ntfv = (object_error*)ob;
			ShowStatus("�����Ѿ��ر�", ntfv->getErrorCode());
			AfxMessageBox("�����Ѿ��ر�");
		}
		break;

		/************************************************************************
		* �յ����û����뷿���֪ͨ����
		************************************************************************/
		case IRtcUsers::NotifyUserEnterChannel:
		{
			object_user* ntfv = (object_user*)ob; 
			AddUser(ntfv);
		}
		break;

		/************************************************************************
		* �յ��û��뿪�����֪ͨ����
		************************************************************************/
		case IRtcUsers::NotifyUserLeaveChannel:
		{
			object_userid* ntfv = (object_userid*)ob;
			RemoveUser(ntfv->getUserID());
		}
		break;

		/************************************************************************
		* �յ��û����Ե�֪ͨ����
		************************************************************************/
		case IRtcAudio::NotifyUserSpeaking:
		{
		 	object_user_speaking* ntfv = (object_user_speaking*)ob;
			auto it = m_speakers.find(ntfv->getUserID());
			if (it == m_speakers.end()) 
				m_speakers[std::string(ntfv->getUserID())] = GetTickCount();
			else
				it->second = GetTickCount();

		}
		break;

		/************************************************************************
		* �յ��û����߳�����֪ͨ�Ĵ�����
		************************************************************************/
		case IRtcUsers::NotifyKickOff:
		{
			object_userid* ntfv = (object_userid*)ob;
			CString selfuserid;
			GetDlgItemText(IDC_USERID, selfuserid);

			if (0 == selfuserid.Compare(ntfv->getUserID()))
			{
				AfxMessageBox("���Ѿ����߳�����\n");

				((CListCtrl*)GetDlgItem(IDC_USER_LIST))->DeleteAllItems();
				//((CListCtrl*)GetDlgItem(IDC_MSG_LIST))->DeleteAllItems();
			}
			else
			{
				RemoveUser(ntfv->getUserID());

				CString str;
				str.Format("%s �Ѿ����߳�����", ntfv->getUserID());
				ShowLog(str);
			}
		}
		break;

		/************************************************************************
		* �յ�ĳ�˱���ֹ����֪ͨ�Ĵ�����
		************************************************************************/
		case IRtcAudio::NotifyDisableUserSpeak:
		{
			object_disable_speak* ntfv = (object_disable_speak*)ob;
			CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_USER_LIST);
			int nIdex = FindUser(ntfv->getUserID());

			if (-1 != nIdex)
				pList->SetItemText(nIdex, 2, ntfv->getDisabled() ? "������" : "");
		}
		break;

		/************************************************************************
		* �յ��û����Ա��޸ĵ�֪ͨ
		************************************************************************/
		case IRtcUsers::NotifyUserAttr:
		{
			object_user_attr* ntfv = (object_user_attr*)ob;

			const char* attrval = ntfv->getAttrValue();

			CString str;
			str.Format("user (%s) attr (%s) changed, val(%s)", ntfv->getUserID(), ntfv->getAttrName(), attrval ? attrval: "");
			ShowLog(str); 
		}
		break;

		/************************************************************************
		* �յ��������Ա��޸ĵ�֪ͨ
		************************************************************************/
		case IRtcChannel::NotifyChannelAttr:
		{
			object_channel_attr* ntfv = (object_channel_attr*)ob;

			const char* attrval = ntfv->getAttrValue();

			CString str;
			str.Format("channel attr (%s) changed, val(%s)", ntfv->getAttrName(), attrval ? attrval : "");
			ShowLog(str);
		}
		break;

		 
		default:
			break;
	}
} 


#include "SoundSetting.h"
void CDemoDLG::OnClickedMenuItem(UINT nID)
{
	switch (nID)
	{
	case ID_MENU_SOUND:
	{
		CSoundSetting dlg(GetAudioSystem());
		dlg.DoModal();
	}
	break;
	default:
		break;
	}
}
 



