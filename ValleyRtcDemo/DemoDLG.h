
// media_client_test_dlgDlg.h : 標頭檔
//

#pragma once 

#include <stdint.h>
#include <map>
#include <string>
#include "Resource.h"
#include <ValleyRtcAPI.h> 
#include <ValleyRtcExtInterface.h> 
#include <ValleyPlugin.h>

using namespace Valley;
 
class CDemoDLG : public CDialogEx, IRtcSinkEx 
{
 
	typedef std::map<std::string, uint32_t> str2int_t;
public:
	CDemoDLG(CWnd* pParent = NULL);	// 標準建構函式
	virtual ~CDemoDLG();
// 對話方塊資料
	enum { IDD = IDD_MEDIA_CLIENT_TEST_DLG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog(); 
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLogin();
	afx_msg void OnBnClickedLogout();
	afx_msg long OnShowLog(WPARAM w, LPARAM l);

protected:
	void ShowLog(const CString& szlog);  
	void ShowStatus(const char* op, int ec);
	void ShowStatus(const char* msg);
	void UpdateUserList();
	void AddUser(const object_user* pUser);
	void RemoveUser(const char* userid);
	int  FindUser(const char* userid); 
	void AddMessage(int msgtype, const char* content, const char* userid, bool toTail);
	int  FindAudioMessage(const char* url);
	virtual void CheckLoginStatus(bool loginingflag, bool bLoginedflag);
	void OnCancel();

 
public:
	afx_msg void OnBnClickedEnableSpeak(); 
	afx_msg void OnBnClickedEnablePlayout(); 
	afx_msg void OnBnClickedEnablemusic();
	afx_msg void OnNMCustomdrawSpkVolume(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawBGMVolume(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawBGMTrack(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR timerID);
	afx_msg void OnNMRbClickUserList(NMHDR *pNMHDR, LRESULT *pResult);  
	afx_msg void OnClickedMenuItem(UINT nID);

private:
	//下面是SDK支持的功能模块，需要在登录前调用RegisterEventHandler函数来注册回调函数 
	IRtcUsers*  GetUserModule(){ return (IRtcUsers*)m_pRoom->GetInterface(IRtcUsers::IID); };
	IRtcAudio*  GetAudioModule(){ return (IRtcAudio*)m_pRoom->GetInterface(IRtcAudio::IID); };
	IRtcAudioSystem*  GetAudioSystem(){ return (IRtcAudioSystem*)m_pRoom->GetInterface(IRtcAudioSystem::IID); };
	IRtcDeviceControler*  GetDeviceControler(){ return (IRtcDeviceControler*)m_pRoom->GetInterface(IRtcDeviceControler::IID); };
	 
protected:
	// Respond操作结果的处理函数
	void Respond(int type, int ec, object_base* ob, void* userdata) override;

	// Notify是被动收到通知的处理函数
	void Notify(int type, object_base* ob, void* userdata) override;

	void OnTransAudioStream(eStemType stype, void* pcm, int len, int channel, int samplerate, const void* userdata);
	 
protected:  
	IRtcChannel*				 m_pRoom;               //房间控制指针
	str2int_t					 m_speakers;            //记录实时语音正在讲话的用户  
	Plugin::IAudioPlayerDemo*	 m_audioDemo; 
	bool                         m_bTrackGraging = false; 
	Plugin::IRoomControler*      m_roomCtrler; 
public:
	afx_msg void OnBnClickedBgm();
	afx_msg void OnBnClickedEnableRtmp();
};
