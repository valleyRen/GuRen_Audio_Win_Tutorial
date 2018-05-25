
// media_client_test_dlgDlg.cpp : 實作檔
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

// CDemoDLG 對話方塊 

CDemoDLG::CDemoDLG(CWnd* pParent /*=NULL*/) : CDialogEx(CDemoDLG::IDD, pParent)
{
 	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	char appfolder[512];
	DWORD dwEnd = GetModuleFileNameA(NULL, appfolder, 512);
	appfolder[dwEnd] = 0;
	*(strrchr(appfolder, '\\')) = 0;
	strcat(appfolder, "\\wokfolder");

 
	//初始化语音SDK, 整个进程该函数只需要调用一次， 当不用SDK功能了，必须调用CleanSDK完成内存资源回收功能
	//appfolder 工作路径，必须要填，具有读写权限，用于存储日志、语音消息文件、临时文件等的工作路径 
	InitSDK(appfolder);
	 

	//创建房间操作实例，必须在InitSDK后调用，否则会失败，创建出来的实例只能用于登录一个房间，如果要登录多个房间，可以创建多个实例来完成
	m_pRoom = CreateChannel();


	//IRtcUsers::IID 如果需要用户进入、退出、用户列表等相关功能，通过以下调用来启用用户模块的相关功能   
	//IRtcAudio::IID 如果需要实时语音功能，可以通过以下调用来启用实时语音模块的相关功能   
	//IRtcAudioSystem::IID 如果需要对系统语音控制，通过以下调启用相关功能  
	//IRtcDeviceControler::IID 如果需要频道支持音乐模式，通过以下调启用相关功能 
	m_pRoom->EnableInterface(IRtcUsers::IID | IRtcAudio::IID | IRtcAudioSystem::IID | IRtcDeviceControler::IID);


	 

	//注册通知回调函数， usePoll 是用来将通知处理同步到工作线程来的，如果为false,那么所有RespondXXX和NotifyXXX的函数将通过SDK的子线程通知上来,
	//如果为true,那么用户必须在工作线程定时调用m_pRoom->Poll()函数以驱动工作线程来调用RespondXXX和NotifyXXX函数，建议调用Poll()函数的时间隔不要超过200ms
	//具体见OnTimer函数
	m_pRoom->RegisterRtcSink(this, true); 

	m_audioDemo  = (Plugin::IAudioPlayerDemo*)CreatePlugin(Plugin::typeAudioDemo); 
	m_roomCtrler = (Plugin::IRoomControler*)CreatePlugin(Plugin::typeRoomControler);
}

CDemoDLG::~CDemoDLG()
{
	//释放房间对象指针
	//参数为true,那么资源被立刻回收，一般进程要退出了，如果需要退出进程，一般这样调用确保退出进程前资源得到回收
	//参数为false,sdk异步回收，一般仅仅退出房间而进程不退出，该指针不用了，这样调用确保Release快速返回

	DWORD fromtime = GetTickCount();

	m_roomCtrler->Release();
	m_audioDemo->Release();
	m_pRoom->Release(true);

	TRACE("1111111111111  %d\n", GetTickCount()-fromtime);

	//不用语音SDK了，通过调用CleanAudioSDK释放 InitAudioSDK分配的资源,必须是作为SDK的最后一个函数调用
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
//以下是demo界面操作的一些函数

//初始化对话框
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
	//播放音量调节
	((CSliderCtrl*)GetDlgItem(IDC_SPK_VOLUME))->SetRange(0, 100);
	((CSliderCtrl*)GetDlgItem(IDC_SPK_VOLUME))->SetPos(50);
	 
	((CSliderCtrl*)GetDlgItem(IDC_BGM_VOLUME))->SetRange(0, 100);
	((CSliderCtrl*)GetDlgItem(IDC_BGM_VOLUME))->SetPos((int)(0.1*100));

	((CSliderCtrl*)GetDlgItem(IDC_BGM_TRACK))->SetRange(0, 100);
	((CSliderCtrl*)GetDlgItem(IDC_BGM_TRACK))->SetPos(0); 

	//启动TimerID=100的定时器， 用于调用m_pRoom->Poll()
	SetTimer(100, 200, NULL);

	//启动TimerID=101的定时器， 用于刷新正在讲话的用户状态
	SetTimer(101, 500, NULL);

	((CButton*)GetDlgItem(IDC_ENABLE_SPEAK))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_ENABLE_PLAYOUT))->SetCheck(TRUE); 

	//用户列表
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_USER_LIST);
	pList->InsertColumn(0, "User ID", 0, 160);
	pList->InsertColumn(1, "Status", 0, 120);
	pList->InsertColumn(2, "Description", 0, 80);
	pList->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	////消息列表
	//pList = (CListCtrl*)GetDlgItem(IDC_MSG_LIST); 
	//pList->InsertColumn(0, "用户ID", 0, 120);
	//pList->InsertColumn(1, "消息内容", 0, 220);
	//pList->InsertColumn(2, "消息类型", 0, 70);
	//pList->InsertColumn(3, "消息状态", 0, 70);
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

//状态条上显示信息
void CDemoDLG::ShowLog(const CString& szlog)
{ 
	SetDlgItemText(IDC_STATUS, szlog);
}

//状态条上显示错误
void CDemoDLG::ShowStatus(const char* op, int ec)
{
	CString msg;
	msg.Format("%s, ec=%d (%s)", op, ec, GetErrDesc(ec));
	SetDlgItemText(IDC_STATUS, msg);
}

//状态条上显示信息
void CDemoDLG::ShowStatus(const char* msg)
{
	SetDlgItemText(IDC_STATUS, msg);
}
 

void CDemoDLG::OnTimer(UINT_PTR timerID)
{
	if (100 == timerID)
	{
		// 调用m_pRoom->Poll()函数来驱动RespondXXX和NotifyXXX函数的调用，语音SDK本身不会消耗Poll()函数的处理时间，处理时间 
		// 由RespondXXX和NotifyXXX上层处理逻辑决定
		
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

			

		////刷新讲话用户列表，因为讲话中的用户会每隔500毫秒通知上来，所以超过500毫秒未通知上来，可以认为已经停止发言
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
					pUserList->SetItemText(nPos, 1, "讲话中...");
					it++;
				}
			}
		} 
	}

	CDialogEx::OnTimer(timerID);
}

//UI控件状态 (登录中，已经登录)
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
 

 
//播放音量调节控件(范围 0 ~ 100 小于50，声音缩小，大于50声音放大)
void CDemoDLG::OnNMCustomdrawSpkVolume(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here

	CSliderCtrl*  pSlider = (CSliderCtrl*)GetDlgItem(IDC_SPK_VOLUME);

	if (GetAudioSystem())
		GetAudioSystem()->SetPlayoutVolume(pSlider->GetPos());

	*pResult = 0;
}
 
//用户列表UI
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
		menu.AppendMenu(MF_STRING, 1, "将用户踢出房间");
		menu.AppendMenu(MF_STRING, 2, "拒听操作");
		menu.AppendMenu(MF_STRING, 3, "禁言操作");

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

//刷新用户列表UI
void CDemoDLG::UpdateUserList()
{
	user_sheet_ptr sheet; 
	int ec = GetUserModule()->GetUserList(sheet);
	if (ERR_SUCCEED != ec)
	{
		ShowStatus("更新用户列表失败", ec);
		OutputDebugString("更新用户列表失败\n");
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

//显示一个用户到UI
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

//移除一个用户UI
void CDemoDLG::RemoveUser(const char* userid)
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_USER_LIST);

	int nIndex = FindUser(userid);
	if (-1 != nIndex)
		pList->DeleteItem(nIndex);
}

//查找一个用户UI
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
 * 执行进入房间操作
 * 这里主要需要需要调用以下函数
 * SetAuthoKey 如果不用该函数设置授权码，那么用SDK能登录测试房间，测试房间roomkey 只能是 1 ~ 99
 * EnablePullUserList 用户列表是否需要下发，如果参数为 false, 那么登录成功不会收到用户列表，用户上线下线也不会收到通知
 * EnableRealAudio 是否需要开启实时语音功能
 * EnableAudioMessage 是否需要开启语音消息功能 
 * Login 登录，登录成功或失败由函数RespondLogin收到
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
		if (bCheck) //主播 
			GetDeviceControler()->Enable(nKey, false);
		else //普通用户 
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
		ShowStatus("进入房间 ...");
	 	CheckLoginStatus(true, false);
	}
	else
	{
		ShowStatus("进入房间失败", ec);
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/************************************************************************
* 执行退出房间操作 
* Logout 登出房间，只要在房间里面，不会出现退出失败的情况
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
	ShowStatus("已经退出房间");

	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_USER_LIST);
	pList->DeleteAllItems();

	//pList = (CListCtrl*)GetDlgItem(IDC_MSG_LIST);
	//pList->DeleteAllItems();
}


/************************************************************************
* 执行实时语音讲话功带Music模式 
************************************************************************/
void CDemoDLG::OnBnClickedEnablemusic()
{ 
	if (GetDeviceControler() && GetDeviceControler()->IsEnabled(IRtcDeviceControler::typeMusicMode))
	{
		bool     bCheck = ((CButton*)GetDlgItem(IDC_ENABLE_MUSIC))->GetCheck();

		//GetDeviceControler()->Enable(IRtcDeviceControler::typeCtrlByHeadset, false);

		//bool bE = GetDeviceControler()->IsEnabled(IRtcDeviceControler::typeCtrlByHeadset);

		//if (bCheck) //主播连麦
		//{
		//	GetDeviceControler()->Enable(IRtcDeviceControler::typeCtrlByHeadset, true);
		//}
		//else
		//{
		//	GetDeviceControler()->Enable(IRtcDeviceControler::typeCtrlByHeadset, false);

			uint32_t nKey = IRtcDeviceControler::typeAec | IRtcDeviceControler::typeNs | IRtcDeviceControler::typeVad;
			if (bCheck) //主播 
				GetDeviceControler()->Enable(nKey, false);
			else //普通用户 
				GetDeviceControler()->Enable(nKey, true);
		//} 
	} 
}
 


/************************************************************************
* 执行实时语音讲话功能
* 默认情况下，讲话功能是关闭的，所以需要发言时调用EnableSpeak 打开讲话功能 
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
* 执行实时语音播放功能
* 默认情况下，播放功能是开着的，可以调用此接口关闭播放功能
************************************************************************/
void CDemoDLG::OnBnClickedEnablePlayout()
{
	bool bCheck = ((CButton*)GetDlgItem(IDC_ENABLE_PLAYOUT))->GetCheck();
	GetAudioModule()->EnablePlayout(bCheck);
}
  


//背景音量调节控件(范围 0 ~ 100)
void CDemoDLG::OnNMCustomdrawBGMVolume(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here

	CSliderCtrl*  pSlider = (CSliderCtrl*)GetDlgItem(IDC_BGM_VOLUME);

	float bgmVolumeScale = (float)pSlider->GetPos() / 100;
	m_audioDemo->SetVolume(TRACKINDEX_ALL, bgmVolumeScale);

	*pResult = 0;
}

//背景播放进度条(范围 0 ~ 100)
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
* 打开背景音乐
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
//以下是收到操作结果的处理函数
void CDemoDLG::Respond(int type, int ec, object_base* ob, void* userdata)
{
	switch (type)
	{
		/************************************************************************
		* 收到登录成功或失败的处理函数
		* 登录成功，一般需要做以下操作
		* 更新用户列表
		* 清理UI上的消息列表，等收到消息列表后刷新历史消息
		************************************************************************/
		case IRtcChannel::RespondLogin:
		{
			object_login* resob = (object_login*)ob;

			//登录成功
			if (ERR_SUCCEED == ec)
			{
			 	CheckLoginStatus(false, true);
				ShowStatus("进入房间成功");
				UpdateUserList();

				//CListCtrl* pMsgList = (CListCtrl*)GetDlgItem(IDC_MSG_LIST);
				//pMsgList->DeleteAllItems();

			}
			//登录失败 
			else
			{
				CheckLoginStatus(false, false);
				 
				ShowStatus("进入房间失败", ec);
			}
		}
		break;

		/************************************************************************
		* 收到将用户踢出房间成功或失败的处理函数 
		************************************************************************/
		case IRtcUsers::RespondKickOff:
		{
			object_userid* resob = (object_userid*)ob;

			if (ERR_SUCCEED == ec)
			{
				RemoveUser(resob->getUserID());

				CString str;
				str.Format("%s 已经被你踢出房间", resob->getUserID());
				ShowLog(str);
			}
			else
			{
				ShowStatus("踢出用户失败", ec);
			}
		}
		break;


		/************************************************************************
		* 收到禁止某人发言成功或失败的处理函数
		************************************************************************/
		case IRtcAudio::RespondDisableUserSpeak:
		{
			if (ERR_SUCCEED == ec)
			{
				object_disable_speak* resob = (object_disable_speak*)ob;

				CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_USER_LIST);
				int nIdex = FindUser(resob->getUserID());

				if (-1 != nIdex)
					pList->SetItemText(nIdex, 2, resob->getDisabled() ? "被禁言" : "");
			}
			else
			{
				ShowStatus("禁言操作失败", ec);
			}
		}
		break;


		/************************************************************************
		* 收到拒听某人声音成功或失败的处理
		************************************************************************/
		case IRtcAudio::RespondBlockUser:
		{
			if (ERR_SUCCEED == ec)
			{
				object_block_speak* resob = (object_block_speak*)ob;

				CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_USER_LIST);
				int nIdex = FindUser(resob->getUserID());
				if (-1 != nIdex)
					pList->SetItemText(nIdex, 2, resob->getBlocked() ? "拒听中" : "");
			}
			else
			{
				ShowStatus("拒听操作失败", ec);
			}
		}
		break;

		/************************************************************************
		*  收到修改用户属成功或失败的处理
		************************************************************************/
		case IRtcUsers::RespondUserAttr:
		{
			object_user_attr* ntfv = (object_user_attr*)ob;
		}
		break;

		/************************************************************************
		* 收到修改房间属性成功或失败的处理
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
//以下是收到操作结果或者通知的处理函数
void CDemoDLG::Notify(int type, object_base* ob, void* userdata)
{
	switch (type)
	{
		/************************************************************************
		* 收到网络断开的通知
		* SDK会自动重连，该事件一般只需要在UI上体现当前网络不通
		***********************************************************************/
		case IRtcChannel::NotifyConnectionLost:
		{
			ShowStatus("网络断开，重连中 ...");
			CheckLoginStatus(true, false);
		}
		break;

		/************************************************************************
		* 收到SDK重连成功的通知
		* 重连成功，跟登录成功一样，用户列表和消息列表需要重置
		************************************************************************/
		case IRtcChannel::NotifyReConnected:
		{
			ShowStatus("重连成功");
			UpdateUserList();

			CListCtrl* pMsgList = (CListCtrl*)GetDlgItem(IDC_MSG_LIST);
			if (nullptr != pMsgList)
				pMsgList->DeleteAllItems();

			CheckLoginStatus(false, true);
		}
		break;

		/************************************************************************
		* 收到该帐号在异地登录的通知 
		************************************************************************/
		case IRtcChannel::NotifyDuplicateLogined:
		{
			AfxMessageBox("该帐号已经在异地登录");
		}
		break;

		/************************************************************************
		* 收到用房间被关闭的通知处理
		************************************************************************/
		case IRtcChannel::NotifyChannelClose:
		{
			object_error* ntfv = (object_error*)ob;
			ShowStatus("房间已经关闭", ntfv->getErrorCode());
			AfxMessageBox("房间已经关闭");
		}
		break;

		/************************************************************************
		* 收到新用户进入房间的通知处理
		************************************************************************/
		case IRtcUsers::NotifyUserEnterChannel:
		{
			object_user* ntfv = (object_user*)ob; 
			AddUser(ntfv);
		}
		break;

		/************************************************************************
		* 收到用户离开房间的通知处理
		************************************************************************/
		case IRtcUsers::NotifyUserLeaveChannel:
		{
			object_userid* ntfv = (object_userid*)ob;
			RemoveUser(ntfv->getUserID());
		}
		break;

		/************************************************************************
		* 收到用户发言的通知处理
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
		* 收到用户被踢出房间通知的处理函数
		************************************************************************/
		case IRtcUsers::NotifyKickOff:
		{
			object_userid* ntfv = (object_userid*)ob;
			CString selfuserid;
			GetDlgItemText(IDC_USERID, selfuserid);

			if (0 == selfuserid.Compare(ntfv->getUserID()))
			{
				AfxMessageBox("你已经被踢出房间\n");

				((CListCtrl*)GetDlgItem(IDC_USER_LIST))->DeleteAllItems();
				//((CListCtrl*)GetDlgItem(IDC_MSG_LIST))->DeleteAllItems();
			}
			else
			{
				RemoveUser(ntfv->getUserID());

				CString str;
				str.Format("%s 已经被踢出房间", ntfv->getUserID());
				ShowLog(str);
			}
		}
		break;

		/************************************************************************
		* 收到某人被禁止发言通知的处理函数
		************************************************************************/
		case IRtcAudio::NotifyDisableUserSpeak:
		{
			object_disable_speak* ntfv = (object_disable_speak*)ob;
			CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_USER_LIST);
			int nIdex = FindUser(ntfv->getUserID());

			if (-1 != nIdex)
				pList->SetItemText(nIdex, 2, ntfv->getDisabled() ? "被禁言" : "");
		}
		break;

		/************************************************************************
		* 收到用户属性被修改的通知
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
		* 收到房间属性被修改的通知
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
 



