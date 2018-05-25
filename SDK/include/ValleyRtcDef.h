#ifndef _VALLEYRTCDEF_H
#define _VALLEYRTCDEF_H
 
#pragma once

namespace Valley
{    
	#define  __RTCAPI// extern "C"

	#ifndef NULL
	#define NULL 0
	#endif

	#define ERR_SUCCEED 0         // �����ɹ�
	#define ERR_NOT_LOGINED  -1   // δ��¼�ɹ�       
	#define ERR_ALREADY_RUN  -2   // �Ѿ�������   
	#define ERR_USER_NOTFOUND -3  // δ�ҵ��û�   
	#define ERR_EXCUTING -4		  // ����ִ���� 
	#define ERR_NOT_INITIALIZE -5 // δ��ʼ��
	#define ERR_UNSUPPORT -6      // ���ܲ�֧��
	#define ERR_ARGUMENT  -7	  // ��������
    #define ERR_INVOKE -8         // ����˳�����


	#define ERR_CHANNEL_EXPIRED 1        // Ƶ���Ѿ�ʧЧ
	#define ERR_CONNECT_SERVER_FAILED 2  // ���ӷ�����ʧ��
	#define ERR_REQUEST_TIMEOUT 3  // ����ʱ
	#define ERR_CONFIG 4  // ������Ϣ���� 
	#define ERR_NET_POOL 5  // ���粻��
	#define ERR_VERSION_UNSUPPORTED 6  // �汾��֧��
	#define ERR_AUTHO_FAILED 7  // ��Ȩʧ�� 
	#define ERR_NOT_ENOUGH_CHANNEL 8  // Ƶ����Դ����
	#define ERR_SERVER_ERROR 9  // ����������
	#define ERR_OPEN_RECORD 10  // �򿪲ɼ�ʧ��
	#define ERR_OPEN_PLAYOUT 11  // �򿪲���ʧ��
	#define ERR_RECORD_PERMISSION 12  // û��¼��Ȩ��

	#define ERR_UNDEFINED 100  // δ�������

	enum
	{
		STATUS_NONE = 0,
		STATUS_LOGINING = 1,
		STATUS_LOGINED  = 2,
	};

	class object_base
	{ 
	public:
		virtual ~object_base(){}
		virtual void Release(){};
	};

	class IExtInterface
	{
	public:
		virtual ~IExtInterface(){}
		virtual void* inner()=0;
	};

	template<class T>
	class AutoPtr
	{
	public:
		typedef T value_type;
		typedef T* pointer_type;

		AutoPtr(pointer_type p = nullptr){_pointer = p;}
		~AutoPtr(){reset();}
		operator bool() const { return _pointer != nullptr; }
		const value_type& operator*() const{ return *_pointer; }
		const pointer_type operator->() const{ return _pointer; }
		pointer_type get() const{ return _pointer; }
		void reset(pointer_type ptr = nullptr){
			if (ptr != _pointer && _pointer)
				_pointer->Release();
			_pointer = ptr;
		}
		void detach(){ _pointer = nullptr; }
	private:
		AutoPtr(const AutoPtr&);
		AutoPtr& operator=(const AutoPtr&);
	private:
		pointer_type _pointer;
	};


	struct object_string : public object_base
	{
		virtual const char*  c_str() const = 0;
	}; 
	 
	struct object_error : public object_base
	{
		virtual int  getErrorCode() const = 0;
	};


	struct object_login : public object_base
	{
		virtual const char*  getUserID() const = 0;
		virtual const char*  getChannelID() const = 0;
		virtual const char*  getUserInfo() const = 0;
	};

	struct object_userid : public object_base
	{
		virtual const char*  getUserID() const = 0;      
	};
	  
	struct object_user : public object_base
	{
		virtual const char*  getUserID() const = 0;         // userid
		virtual const char*  getUserInfo() const = 0;       // userinf
		virtual bool         getDisableSpeak() const = 0; // is disable speak
		virtual bool         getBlocked() const = 0;        // is block
		virtual const char*  getAttr(const char* name) const = 0; 
	};
	 
	struct object_user_sheet : public object_base
	{
		virtual object_user*  item(int idx) = 0;
		virtual int size() const = 0; 
	};

	struct object_disable_speak : public object_base
	{
		virtual const char*  getUserID() const = 0;       
		virtual bool         getDisabled() const = 0;
	};

	struct object_block_speak : public object_base
	{
		virtual const char*  getUserID() const = 0;
		virtual bool         getBlocked() const = 0;
	};

	struct object_user_speaking : public object_base
	{
		virtual const char*  getUserID() const = 0;
		virtual int          getVolume() const = 0;
	};

	struct object_user_attr : public object_base
	{
		virtual const char*  getUserID() const = 0;
		virtual const char*  getAttrName() const = 0;
		virtual const char*  getAttrValue() const = 0;
	};

	struct object_channel_attr : public object_base
	{ 
		virtual const char*  getAttrName() const = 0;
		virtual const char*  getAttrValue() const = 0;
	};

	struct object_string_sheet : public object_base
	{
		virtual const char*  item(int idx) = 0;
		virtual int size() const = 0;
	};
	 
	typedef AutoPtr<object_string> string_ptr;
	typedef AutoPtr<object_user> user_ptr;
	typedef AutoPtr<object_user_sheet> user_sheet_ptr;
	typedef AutoPtr<object_string_sheet> string_sheet_ptr; 
};
 
#endif//_VALLEYRTCDEF_H
