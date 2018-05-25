
// media_client_test_dlgDlg.h : ���^�n
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
	CDemoDLG(CWnd* pParent = NULL);	// �˜ʽ�����ʽ
	virtual ~CDemoDLG();
// ��Ԓ���K�Y��
	enum { IDD = IDD_MEDIA_CLIENT_TEST_DLG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧Ԯ


// ��ʽ�a����
protected:
	HICON m_hIcon;

	// �a����ӍϢ������ʽ
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
	//������SDK֧�ֵĹ���ģ�飬��Ҫ�ڵ�¼ǰ����RegisterEventHandler������ע��ص����� 
	IRtcUsers*  GetUserModule(){ return (IRtcUsers*)m_pRoom->GetInterface(IRtcUsers::IID); };
	IRtcAudio*  GetAudioModule(){ return (IRtcAudio*)m_pRoom->GetInterface(IRtcAudio::IID); };
	IRtcAudioSystem*  GetAudioSystem(){ return (IRtcAudioSystem*)m_pRoom->GetInterface(IRtcAudioSystem::IID); };
	IRtcDeviceControler*  GetDeviceControler(){ return (IRtcDeviceControler*)m_pRoom->GetInterface(IRtcDeviceControler::IID); };
	 
protected:
	// Respond��������Ĵ�����
	void Respond(int type, int ec, object_base* ob, void* userdata) override;

	// Notify�Ǳ����յ�֪ͨ�Ĵ�����
	void Notify(int type, object_base* ob, void* userdata) override;

	void OnTransAudioStream(eStemType stype, void* pcm, int len, int channel, int samplerate, const void* userdata);
	 
protected:  
	IRtcChannel*				 m_pRoom;               //�������ָ��
	str2int_t					 m_speakers;            //��¼ʵʱ�������ڽ������û�  
	Plugin::IAudioPlayerDemo*	 m_audioDemo; 
	bool                         m_bTrackGraging = false; 
	Plugin::IRoomControler*      m_roomCtrler; 
public:
	afx_msg void OnBnClickedBgm();
	afx_msg void OnBnClickedEnableRtmp();
};
