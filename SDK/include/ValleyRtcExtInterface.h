#ifndef _VALLEYRTCAPIEXT_H
#define _VALLEYRTCAPIEXT_H

#include "ValleyRtcDef.h"
#include "ValleyRtcAPI.h"

namespace Valley
{    
	class IRtcSinkEx : public IRtcSink
	{
	public:
		enum eStemType
		{ 
			steam_before_send = 0x02,
			steam_playout = 0x04,
		};
	public:
		virtual void OnTransAudioStream(eStemType stype, void* pcm, int len, int channel, int samplerate, const void* userdata){}
		virtual void OnTransUserAudioStream(void* pcm, int len, int channel, int samplerate, const char* uid, const void* userdata){}
	};

	class IRtcUsers : public IExtInterface
	{
	public:
		enum{ IID = 0x01 };
		enum
		{  
			RespondKickOff        = 101,	   // object_userid 
			RespondUserAttr       = 102,	   // object_user_attr
		};

		enum
		{ 
			NotifyUserEnterChannel = 101,      // object_user
			NotifyUserLeaveChannel = 102,      // object_userid  
			NotifyKickOff          = 103,      // object_userid 
			NotifyUserAttr         = 104,      // object_user_attr
		};

	public:
		virtual int  GetUserCount() = 0;
		virtual int  GetUserList(user_sheet_ptr& userlist) = 0;
		virtual int  GetUser(const char* uid, user_ptr& user) = 0;
		virtual int  KickOff(const char* uid) = 0;
		virtual int  SetUserAttr(const char* uid, const char* name, const char* value) = 0;
		virtual int  GettUserAttr(const char* uid, const char* name, string_ptr& value) = 0;
	};
 
	class IRtcAudio : public IExtInterface
	{ 
	public:
		enum{ IID = 0x02 };
		enum
		{ 
			RespondDisableUserSpeak    = 201, // object_disable_speak
			RespondBlockUser           = 202, // object_block_speak
		};
		enum
		{ 
			NotifyDisableUserSpeak     = 201,   // object_disable_speak  
			NotifyUserSpeaking         = 202,   // object_user_speaking 
		}; 
	public:  
		virtual int   BlockUser(const char* uid, bool block) = 0;
		virtual int   DisableUserSpeak(const char* uid, bool disspeak) = 0;
		virtual int   EnableSpeak(bool enable) = 0;
		virtual bool  GetSpeakEnabled() = 0;
		virtual void  EnablePlayout(bool enable) = 0;
		virtual bool  GetPlayoutEnabled() = 0;
	}; 

	class IRtcAudioSystem : public IExtInterface
	{
	public:
		enum{ IID = 0x04 }; 
		virtual int   SetPlayoutVolume(int volume) = 0;
		virtual int   GetPlayoutVolume(int& volume) = 0;
		virtual void  SetSpeakerphoneOn(bool bOn) = 0;
		virtual bool  GetSpeakerphoneOn() = 0;

		/*
		 * GetDeviceList ö��ϵͳ��Ƶ�ɼ��򲥷��豸 
		 * �ɼ��豸�� Ĭ��ΪAUTO ��ʾϵͳ���Զ���������豸����һ����⵽�����źŵ���Ϊ�ɼ��豸
		 * �����豸�� Ĭ��ΪDEFAULT��ʾϵͳĬ���豸
		 */
		virtual int  GetDeviceList(bool bRecord, string_sheet_ptr& arr) = 0;   
		virtual int  GetCurrentDevice(bool bRecord, string_ptr& deviceName) = 0; //��ȡ��ǰ�����ɼ��򲥷ŵ��豸�� AUTOΪ����Ӧ
		virtual int  SetCurrentDevice(bool bRecord, const char* deviceName) = 0; //ָ����ǰ�����ɼ��򲥷ŵ��豸��DEFAULTΪϵͳĬ��
	}; 


	class IRtcDeviceControler : public IExtInterface
	{  
	public: 
		enum{ IID = 0x08 };  
		enum{ //typeCtrlByHeadset = 0x01,  //�ܶ�����ο���(���϶������޻�������ô�����ʣ����򽲻�ģʽ)
			  typeAec = 0x04,    //�������ƿ���
			  typeNs = 0x08,     //�������ƿ���
			  typeAgc = 0x010,   //�Զ��������
			  typeVad = 0x020,   //���������� 
			  typeEcho = 0x40,   //��������

			  typeMusicMode         = 0x1000, // ���ַ�����ƽӿڣ��������typeMusicMode����Ϊ���ַ������Ч
			  typeHookRecord        = 0x2000, // HookCapture  PushRecordAudioFrame ��Ч
			  typeHookPlayout       = 0x4000, // HookPlayout  GetPlayoutAudioFrame ��Ч
			  typeBackgroundMusic   = 0x8000, // ֧�ֱ�������  PushBackgroudAudioFrame ��Ч 
		};

		virtual int  Enable(int types, bool enable) = 0;
		virtual bool IsEnabled(int type) = 0;  

 		virtual int  GetPlayoutAudioFrame(char* pcm, int len, int& samprate, int& channel) = 0; //��ȡ��ǰ���ص��Ĳ�������
		virtual int  PushRecordAudioFrame(char* pcm, int len, int samprate, int channel) = 0;   //���Ͳɼ��������� hookCaptre=true��Ч
		   
		//���ͱ�������  trackindex �����ʶ������0~500
		virtual int  PushBackgroudAudioFrame(int trackindex, char* pcm, int len, int samprate, int channel, float scale = 1.0f, bool bSendToNet = false, bool bPlayout = false) = 0; 

		virtual int  EnalbleStreamTransCallback(bool bEnable, const void* userdata = nullptr) = 0;  //�˽ӿڱ����ڵ�¼ǰ���ã�OnTransAudioStream �������ݴ��������û�����ʵ�ֱ���¼�����޸������е�
		virtual int  EnalbleUserStreamTransCallback(bool bEnable, const void* userdata = nullptr) = 0; //�˽ӿڱ����ڵ�¼ǰ���ã�OnTransUserAudioStream ���յ���ÿ���˵����������ݴ�����
	}; 
}; 
 

#endif//_VALLEYRTCAPIEXT_H 
