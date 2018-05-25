#pragma once
#include <stdint.h>
#define __VALLEYAPI extern "C"

#define ERR_SUCCEED 0         // �����ɹ�
#define ERR_NOT_LOGINED  -1   // δ��¼�ɹ�       
#define ERR_ALREADY_RUN  -2   // �Ѿ�������   
#define ERR_USER_NOTFOUND -3  // δ�ҵ��û�   
#define ERR_EXCUTING -4		  // ����ִ���� 
#define ERR_NOT_INITIALIZE -5 // δ��ʼ��
#define ERR_UNSUPPORT -6      // ���ܲ�֧��
#define ERR_ARGUMENT  -7	  // ��������

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

//��½���
#define RTC_CALLBACK_RESPONED_LOGIN          1
//����Ƶ�����Խ��
#define RTC_CALLBACK_RESPONED_CHANNEL_ATTR   2
//���˽��
#define RTC_CALLBACK_RESPONED_KICK_OFF       3
//�����û����Խ��
#define RTC_CALLBACK_RESPONED_USER_ATTR      4
//�����û����
#define RTC_CALLBACK_RESPONED_BLOCK_USER     5
//�����û����
#define RTC_CALLBACK_RESPONED_DISABLE_SPEAK  6

//�ظ���½���߳�Ƶ��֪ͨ
#define RTC_CALLBACK_NOTIFY_DUPLICATE_LOGINED         101
//Ƶ���ر�֪ͨ
#define RTC_CALLBACK_NOTIFY_CHANNEL_CLOSE             102
//���粻ͨ
#define RTC_CALLBACK_NOTIFY_CONNECTION_LOST           103
//������������
#define RTC_CALLBACK_NOTIFY_RECONNECTED               104
//�û����Ա仯֪ͨ
#define RTC_CALLBACK_NOTIFY_CHANNEL_ATTR              105
//���û�����Ƶ��֪ͨ
#define RTC_CALLBACK_NOTIFY_USER_ENTER_CHANNEL        106
//�û��뿪Ƶ��֪ͨ
#define RTC_CALLBACK_NOTIFY_USER_LEAVE_CHANNEL        107
//���û����߳�Ƶ��
#define RTC_CALLBACK_NOTIFY_KICK_OFF                  108
//�û����Ա仯֪ͨ
#define RTC_CALLBACK_NOTIFY_USER_ATTR                 109
//�û�������֪ͨ
#define RTC_CALLBACK_NOTIFY_USER_DISABLE_SPEAK        110
//�û����ڽ���֪ͨ
#define RTC_CALLBACK_NOTIFY_USER_SPEAKING             111

//�ṹ��ĳ�Աȫ��ʹ��ȷ����С������,��ֹ��ͬƽ̨��ͬ�����в�ͬ�Ľ��͡�
#pragma pack(1)

struct RtcObjectBase
{
#define RTC_OBJECT_BASE 0
    int32_t type;       //��������������ͣ���ΪC���Բ�֧�ּ̳У������ʶ�¡��ϲ㲻��Ҫ����
};

// ��½���ؽ��
struct RtcObjectLogin
{
#define RTC_OBJECT_LOGIN  1
    RtcObjectBase base;
    const char* channelid;
    const char* userid;
    const char* userinfo;
};

//Ƶ������
struct RtcObjectChannelAttr
{
#define RTC_OBJECT_CHANNEL_ATTR 2
    RtcObjectBase base;
    const char* name;
    const char* value;
};

//�û�id
struct RtcObjectUserID
{
#define RTC_OBJECT_USER_ID 3
    RtcObjectBase base;
    const char* userid;
};

//�û����ԣ�attrֵ��Ҫ����Valley_GettUserAttr��ȡ
struct RtcObjectUserAttr
{
#define RTC_OBJECT_USER_ATTR 4
    RtcObjectBase base;
    const char* userid;
    const char* name;
    const char* value;
};

// ����ĳ�û��Ľ������֪ͨ
struct RtcObjectBlockUser
{
#define RTC_OBJECT_BLOCK_USER 5
    RtcObjectBase base;
    const char*   userid;
    int32_t       isblock;
};

//����ĳ�˷��Խ������֪ͨ
struct RtcObjectDisableSpeak
{
#define RTC_OBJECT_DISABLE_SPEAK 6
    RtcObjectBase base;
    const char*   userid;
    int32_t       disable;
};

//������
struct RtcObjectError
{
#define RTC_OBJECT_ERROR 7
    RtcObjectBase base;
    int32_t error_code;
};

//�û�����
struct RtcObjectUser
{
#define RTC_OBJECT_USER 8
    RtcObjectBase base;
    const char* userid;
    const char* userinfo;
    int32_t     isblock;
    int32_t     disablespeak;
};

//�û������б�
struct RtcObjectUserList
{
#define RTC_OBJECT_USER_LIST 9
    RtcObjectBase base;
    RtcObjectUser*user_item;
    int32_t       user_count;
};

//�û�����˵��֪ͨ
struct RtcObjectUserSpeaking
{
#define RTC_OBJECT_USER_SPEAKING 10
    RtcObjectBase base;
    const char*   userid;
    int32_t       volume;
};

// ͨ���ַ���
struct RtcObjectString
{
#define RTC_OBJECT_STRING   11
    RtcObjectBase base;
    const char* string;
};
#pragma pack()

//�첽�¼��ص��ӿ�
typedef void( *LPRTCEVENTCALLBACK )( int type, int ec, struct RtcObjectBase *base, void* userdata );

//ȫ�ֺ���

/**
* @brief     ��׿ƽ̨���ã���ʼ����׿
* @return    __VALLEYAPI void
* @param     void * AppilcationCtx: ��׿��application context ����
*/
__VALLEYAPI void					Valley_InitAndroid( void* AppilcationCtx );

/**
* @brief     ��ʼ��SDK��ֻ��Ҫ����һ��
* @return    __VALLEYAPI void
* @param     const char * appfolder���ɶ�д���ļ�·��
*/
__VALLEYAPI void					Valley_InitSDK( const char* appfolder );
/**
* @brief     ����SDK��Դ����Valley_InitSDK�ɶԳ��֡�
* @return    __VALLEYAPI void
*/
__VALLEYAPI void					Valley_CleanSDK();
/**
* @brief     ������Ȩ��
* @return    __VALLEYAPI void
* @param     const char * authokey
*/
__VALLEYAPI void					Valley_SetAuthoKey( const char* authokey );
/**
* @brief     ����������
* @return    __VALLEYAPI const char*
* @param     int ec
*/
__VALLEYAPI const char*			    Valley_GetErrDesc( int ec );
/**
* @brief     SDK�汾
* @return    __VALLEYAPI const char*
*/
__VALLEYAPI const char*			    Valley_GetSDKVersion();

//Ƶ�������߼�
/**
* @brief     ����һ��Ƶ��
* @return    __VALLEYAPI void*
*/
__VALLEYAPI void*                  Valley_CreateChannel();
/**
* @brief     ע���첽�¼��ص�����
* @return    __VALLEYAPI void
* @param     void * state Valley_CreateChannel ����ֵ����ͬ��
* @param     LPRTCEVENTCALLBACK cb �ص�����������Ϊ��
* @param     bool use_poll �Ƿ����ϲ������¼���������ȡ��Ϊtrue����Ҫ�ϲ㲻�ϵ���Poll�������Ƽ���true��
* @param     void * userdata �ϲ��Զ�������
*/
__VALLEYAPI void                   Valley_RegisterRtcSink( void* state, LPRTCEVENTCALLBACK cb, bool use_poll,void* userdata );
/**
* @brief     ����ģ��
* @return    __VALLEYAPI int �μ������롣
* @param     void * state
* @param     int id 1��ʾ�û���صĽӿڣ�2��ʾʵʱ����������صĽӿ�
*/
__VALLEYAPI int                    Valley_EnableInterface( void* state, int id );
/**
* @brief     ��½Ƶ�����첽������
* @return    __VALLEYAPI int
* @param     void * state
* @param     const char * channelid������ֵ������Ϊ�ա�
* @param     const char * userid������ֵ������Ϊ�ա�
* @param     const char * userinfo���Զ���
*/
__VALLEYAPI int                    Valley_Login( void* state, const char* channelid, const char* userid, const char* userinfo );
/**
* @brief     �˳�Ƶ��
* @return    __VALLEYAPI void
* @param     void * state
*/
__VALLEYAPI void                   Valley_Logout( void* state );

/**
* @brief     ��ȡ��½״̬
* @return    __VALLEYAPI int
             STATUS_NONE = 0, //δ��¼
             STATUS_LOGINING = 1,//���ڵ�¼
             STATUS_LOGINED  = 2,//�Ѿ���½
* @param     void * state
*/
__VALLEYAPI int                    Valley_GetLoginStatus( void* state );
/**
* @brief     �ϲ��Զ�������Ƶ�����ԣ���ֵ�ԡ�
* @return    __VALLEYAPI int
* @param     void * state
* @param     const char * name ����ֵ������Ϊ�ա�
* @param     const char * value ����ֵ��Ϊ��������ü�ֵ�ԡ�
*/
__VALLEYAPI int                    Valley_SetChannelAttr( void* state, const char* name, const char* value );
/**
* @brief     ��ȡƵ�����ԣ���ֵ��
* @return    __VALLEYAPI int
* @param     void * state
* @param     const char * name ����ֵ
* @param     RtcObjectString * value ������Ҫ����Valley_ReleaseRtcObject�ͷ��ڴ�
*/
__VALLEYAPI int                    Valley_GetChannelAttr( void* state, const char* name, RtcObjectString* value );
/**
* @brief     ɾ��Ƶ��
* @return    __VALLEYAPI void
* @param     void * state
* @param     bool sync Ϊtrue����ͬ���ȴ�Ƶ���ͷ���ϣ������������أ��ں�̨�߳��ͷš��Ƽ���true��
*/
__VALLEYAPI void                   Valley_Release( void* state, bool sync );
/**
* @brief     ��ʱ��ѯ�첽�¼����Ƽ�������200ms����һ�Ρ��������������ע�ắ����use_poll=true��ϡ�
* @return    __VALLEYAPI void
* @param     void * state
*/
__VALLEYAPI void                   Valley_Poll( void* state );

////�û��б���ƽӿ�
/**
* @brief     ��ȡƵ���û���
* @return    __VALLEYAPI int ����
* @param     void * state
*/
__VALLEYAPI int  Valley_GetUserCount( void* state );
/**
* @brief     ��ȡƵ���û��б�
* @return    __VALLEYAPI int ������
* @param     void * state
* @param     RtcObjectUserList * user_list �û��б�ʹ����������Valley_ReleaseRtcObject �ͷ��ڴ�
*/
__VALLEYAPI int  Valley_GetUserList( void* state, RtcObjectUserList* user_list );
/**
* @brief     ��ȡ�û�
* @return    __VALLEYAPI int
* @param     void * state
* @param     const char * uid �û�id
* @param     RtcObjectUser * user ʹ����������Valley_ReleaseRtcObject �ͷ��ڴ�
*/
__VALLEYAPI int  Valley_GetUser( void* state, const char* uid, RtcObjectUser* user );
/**
* @brief     ���û��߳�����
* @return    __VALLEYAPI int
* @param     void * state
* @param     const char * uid �û�id
*/
__VALLEYAPI int  Valley_KickOff( void* state, const char* uid );
/**
* @brief     �����û����ԣ���ֵ��
* @return    __VALLEYAPI int
* @param     void * state
* @param     const char * uid �û�id
* @param     const char * name ��������ֵ������Ϊ��
* @param     const char * value ֵ������ֵ�����Ϊ�գ������������ֵ��
*/
__VALLEYAPI int  Valley_SetUserAttr( void* state,const char* uid, const char* name, const char* value );
/**
* @brief     ��ȡ�û����ԣ���ֵ��
* @return    __VALLEYAPI int
* @param     void * state
* @param     const char * uid �û�id
* @param     const char * name �� ����ֵ������Ϊ��
* @param     RtcObjectString * value ֵ��ʹ����������Valley_ReleaseRtcObject �ͷ��ڴ�
*/
__VALLEYAPI int  Valley_GettUserAttr( void* state, const char* uid, const char* name, RtcObjectString* value );


////ʵʱ���������߼��ӿ�
/**
* @brief     �����û�����
* @return    __VALLEYAPI int �μ�������
* @param     void * state
* @param     const char * uid �û�id
* @param     bool block �Ƿ����
*/
__VALLEYAPI int   Valley_BlockUser( void* state,const char* uid, bool block );
/**
* @brief     ��ֹ�û�����
* @return    __VALLEYAPI int ������
* @param     void * state
* @param     const char * uid �û�id
* @param     bool disspeak ����
*/
__VALLEYAPI int   Valley_DisableUserSpeak( void* state, const char* uid, bool disspeak );
/**
* @brief     �Ƿ�Ҫ����
* @return    __VALLEYAPI void
* @param     void * state
* @param     bool enable
*/
__VALLEYAPI int  Valley_EnableSpeak( void* state, bool enable );
/**
* @brief     ��ѯ�Ƿ�򿪽���
* @return    __VALLEYAPI bool
* @param     void * state
*/
__VALLEYAPI bool  Valley_GetSpeakEnabled( void* state );
/**
* @brief     ����
* @return    __VALLEYAPI void
* @param     void * state
* @param     bool enable
*/
__VALLEYAPI void  Valley_EnablePlayout( void* state, bool enable );

/**
* @brief     �Ƿ�򿪲���
* @return    __VALLEYAPI bool
* @param     void * state
*/
__VALLEYAPI bool  Valley_GetPlayoutEnabled( void* state );
/**
* @brief     ���ò�������
* @return    __VALLEYAPI int
* @param     void * state
* @param     int volume ��Чֵ[0-100]
*/
__VALLEYAPI int   Valley_SetPlayoutVolume( void* state, int volume );
/**
* @brief     �򿪶���ģʽ
* @return    __VALLEYAPI void
* @param     void * state
* @param     bool enable
*/
__VALLEYAPI void  Valley_SetSpeakerphoneOn( void* state, bool enable );
/**
* @brief     �Ƿ�򿪶���
* @return    __VALLEYAPI bool
* @param     void * state
*/
__VALLEYAPI bool  Valley_GetSpeakerphoneOn( void* state );

/**
* @brief     �ͷ��ڴ�
* @return    __VALLEYAPI void
* @param     RtcObjectBase * obj
*/
__VALLEYAPI void  Valley_ReleaseRtcObject( RtcObjectBase* obj );
