### 
ValleyRtcDemo 为demo源码
SDK 为SDK包
官网地址：https://doc.valley.ren/
###

### API 调用手册
#### 概述
谷人语音SDK提供了多种编程语言接口，方便第三方开发者接入。目前SDK提供了C/C++,Java,Objective-C,C#接口，支持系统包括 Windows PC xp 以上版本,Android 4.1以上版本，iOS7.0以上版本。本文档是C++版本的调用手册，如果要其他版本语言，请到对应的子目录下查看。

SDK包含了如下头文件：
1.ValleyRtcAPI.h
2.ValleyRtcDef.h
3.ValleyRtcExtInterface.h
4.ValleyPlugin.h
ValleyRtcAPI.h包含了全局函数和频道（或者说房间）类IRtcChannel。
ValleyRtcDef.h包含了一些错误码和其他常量,辅助类定义。
ValleyRtcExtInterface.h包含了频道内操作的具体功能，SDK以模块的形式对外提供具体语音功能，包括发消息，实时语音，音量控制，音乐模式设置等等。
ValleyPlugin.h 提供了额外的功能，比如播放mp3格式的背景音乐。

Windows平台包含dll动态库，安卓包含了so动态库和jar包，iOS平台包含了framework框架库，但是其实是静态库。

下面先介绍SDK的调用流程，然后介绍每个API的使用说明。

### 调用流程
#### 全局函数的设置。
SDK的初始化/清理
对于安卓平台，第一步一定要调用InitAndroid函数，而且只需要调用一次。然后调用 InitSDK，用于SDK的环境初始化。在整个SDK的使用过程，包括同时使用多个频道期间，不需要重复调用。当不再使用SDK，并完成频道的退出工作后，应该调用CleanSDK，用来清理SDK的环境资源。下次再使用需重新调用InitSDK。然后调用SetAuthoKey设置授权码，授权码的获得需要与服务商联系。测试环境下可以传NULL，此时登陆的频道id限制为1-25。GetErrDesc/GetSDKVersion 函数用来查询错误码信息和版本号，可选。

#### 频道的基本操作
CreateChannel用来创建一个新的频道实例。
当通过CreateChannel 成功创建频道返回非空指针之后，
先调用IRtcChannel::RegisterRtcSink 注册回调函数；
再调用IRtcChannel::EnableInterface 启用模块功能。

然后调用IRtcChannel::Login 执行登陆操作，此操作是异步的，当登陆成功或者失败，会通过先前注册的回调函数通知上层。
在执行Login操作后，如果注册回调的时候，第二个参数传true,则应该定时的调用Poll函数轮询，SDK有新的事件会同步地向上层回调。

当收到成功的事件后，可以通过GetInterface取得模块接口，操作频道功能，具体操作参考下面的API使用说明。
当要退出频道，需要调用IRtcChannel::Logout,此函数立即返回。上层可以继续调用Login函数重新登陆，或者调用Release释放频道实例。

以上就是完整的SDK调用流程。
#### 例子
以下是一个简单的例子(伪C++代码)：
```cpp
#include "ValleyRtcAPI.h"
#include "ValleyRtcDef.h"
#include "ValleyRtcExtInterface.h"
using namespace Valley;
//必须实现异步回调事件处理接口，SDK的大部分接口都是异步的。
class EventCallback : public IRtcSink
{
public:
		virtual void Respond(int type, int ec, object_base* ob)
		{
		    //这里处理回调事件
		    if(IRtcChannel::RespondLogin == type)
		    {
		        if(type == 0)
				{
				    //登陆成功
				}
				else
				{
				    // 登陆失败
				}
		    }
		}
		virtual void Notify(int type, object_base* ob) = 0
		{
			//这里处理通知事件
		}
}
EventCallback* sink;
InitSDK("/yourapp/audio/");
#ifdef PLATFORM_ANDOIRD
void* javaApplicationContext;//获取安卓的app context
InitAndroid(javaApplicationContext);
#endif
SetAuthoKey(NULL);

IRtcChannel* channel = CreateChannel();
ASSERT(channel != NULL);

channel->RegisterRtcSink(sink,true);//
channel->EnableInterface(1);//users module
channel->EnableInterface(2);//real audio module
channel->Login("5","test");


// 定时调用Poll函数
channel->Poll();

//登陆成功后
IRtcUsers* iusers = (IRtcUsers*)channel->GetInterface(1);
int count = iusers->GetUserCount();//频道用户总数
user_sheet_ptr userlist；
int ec = iusers->GetUserList(userlist);
ASSERT(ec == 0);
if(userlist)
{
    //将用户列表显示到UI
}

IRtcAudio* realaudio = (IRtcAudio*)channel->GetInterface(2);
realaudio->EnableSpeak(true);//允许说话
realaudio->EnablePlayout(true);//允许播放


//当要退出频道
channel->Logout();
channel->Release(true);
channel = NULL;

//当不再使用SDK
CleanSDK();
```


### API使用说明

初始化
__RTCAPI void    InitAndroid(void* AppilcationCtx);

**功能**  设置安卓系统的上下文对象。整个进程只需要调用一次。只用于安卓平台。
**参数说明**  AppilcationCtx： 即Android MainActivity 的Context。如果是其他平台，可以传NULL。
**调用方式** 同步

__RTCAPI void	InitSDK(const char* appfolder);

**功能** 初始化SDK。在SDK的声明周期内，只需要调用一次。与CleanSDK 成对调用。
**参数** appfolder：有读写权限的路径，一般在应用程序的配置子目录下。
**调用方式** 同步

__RTCAPI void CleanSDK();

**功能** 清理SDK的所有资源。必须在所有IRtcChannel都释放的情况下调用，否则会异常。下次还要用SDK，必须重新调用InitSDK。
**参数** 无
**调用方式** 同步

设置授权码
__RTCAPI void SetAuthoKey(const char* authokey);

**功能** 设置授权码，授权码从语音服务提供商申请，此key是上层应用程序的唯一标识，记权等行为的依凭。一定要严加保管，不可泄漏给其他人。
**参数** authokey：授权码，正式环境必须是一个有效字符串。测试期间可以传NULL。
**调用方式** 同步

获取错误信息
__RTCAPI const char* GetErrDesc(int ec);

**功能** 将错误码转换为可读的错误信息。
**参数** ec:错误码，参看ValleyRtcDef.h
**调用方式** 同步

获取SDK版本号
__RTCAPI const char* GetSDKVersion();

**功能** 获取当前SDK的版本号。
**参数** 无
**返回值** 错误信息
**调用方式** 同步

__RTCAPI Valley::IRtcChannel*   CreateChannel();

**功能** 创建频道，如果失败返回NULL。必须先调用InitSDK等函数才能调用这个函数。
**参数** 无
**返回值** 频道实例，如果为空，则表示没有调用InitSDK();
**调用方式** 同步


#### IRtcChannel类
此模块的异步事件有：
RespondLogin：登陆结果，附带参数为object_login
RespondChannelAttr：设置频道属性结果，附带参数为object_channel_attr
NotifyConnectionLost：网络连接不上。附带参数为空。
NotifyReConnected：网络重新连接成功。附带参数为空。
NotifyDuplicateLogined：同一个用户id重复登陆，被踢出房间。附带参数为空。
NotifyChannelClose：频道关闭，附带参数为object_error
NotifyChannelAttr：频道属性更新，附带参数为object_channel_attr

void RegisterRtcSink(IRtcSink* sink, bool usePoll);

**功能** 设置回调函数。SDK大部分的事件都是异步的，必须通过这个回调通知上层应用。所以必须保证他的生命周期比IRtcChannel长。
**参数**
sink: 继承IRtcSink，实现他的接口，并处理回调上来的事件。事件分两类，一类是上层调用的异步执行结果，比如调用Login函数，Respond函数里会收到RespondLogin事件。另一类是被动触发的，比如频道里有用户进来，则Notify函数里会收到NotifyUserEnterChannel。
usePoll:是否轮询事件，对于应用来说，有时候希望事件在UI线程或者工作线程处理，可以省去线程转换的麻烦，此时传true，然后在工作线程定时调用Poll函数，SDK会检测是否有新事件产生，如果有，则回调给上层。如果传false,SDK 收到新事件会在自己开的线程里直接回调上层。
**调用方式** 同步

void EnableInterface(int iids);
**功能** 启用子模块，具体模块在ValleyRtcExtInterface.h里。
**参数** iids 模块id。一般是XXX::IID。其中 IRtcAudio::IID IRtcUsers::IID必须在Login调用前启用才有效。
**调用方式** 同步

int Login(const char* channelid, const char* userid, const char* userinfo=NULL);

**功能** 登陆频道
**参数** channelid 频道id，任意字符串。测试环境下（授权码为空），只能填写数字1-25。userid:任意字符串，不可为空。userinfo:  可选，上层自定义信息。登陆后，通过object_user可以获取到。
**返回值** 错误码
**调用方式** 异步

void Logout();
**功能** 退出频道。
**参数** 无
**调用方式** 同步

int GetLoginStatus();
**功能** 获取当前登陆状态
**参数** 无
**调用方式** 同步
**返回值**
STATUS_NONE = 0,
STATUS_LOGINING = 1,
STATUS_LOGINED  = 2,

int SetChannelAttr(const char* name, const char* value);
**功能** 给频道设置自定义属性。如果value为空，则表示删除此键值对。
**参数**  name: 自定义字符串 value:自定义字符串，可为NULL。
**返回值** 错误码
**调用方式** 异步

int GetChannelAttr(const char* name, string_ptr& value);

**功能** 获取频道自定义属性。
**参数** name:自定义字符串，value，智能指针。如果没有指定的name，为空。
**返回值** 错误码
**调用方式** 同步

IExtInterface*  GetInterface(int iid);

**功能** 获取模块接口，必须在之前调用EnableInterface，否则返回NULL。
**参数** iid 模块id。
**返回值** 返回模块基类指针，上层需要自己转换。
**调用方式** 同步

void Release(bool sync);
**功能** 释放IRtcChannel实例。
**参数** sync: 是否等待资源释放完毕才返回，如传true，则会阻塞一定的时间，传false，则上层必须保证，不会立即退出进程，否则可能崩溃。
**调用方式** sync为true,则同步

void Poll();
**功能**  轮询事件。必须在RegisterRtcSink里为usePoll传true。调用间隔不宜大于500ms。
**参数** 无
**调用方式** 同步

#### 模块接口 IRtcUsers
此模块下的异步事件有：
RespondKickOff：返回踢人结果，附带参数为object_userid
RespondUserAttr：返回设置用户属性结果，附带参数为object_user_attr
NotifyUserEnterChannel：通知新用户进入频道，附带参数为object_user
NotifyUserLeaveChannel：通知有用户离开频道，附带参数为object_userid
NotifyKickOff：通知有用户被踢出频道，附带参数为object_userid
NotifyUserAttr：通知有用户属性更新，附带参数为object_user_attr

int GetUserCount();
**功能** 查询当前频道的人数
**参数** 无
**返回值** 返回人数，如果为负值，则表示错误码。
**调用方式** 同步

int GetUserList(user_sheet_ptr& userlist);
**功能** 获取频道人数列表
**参数** userlist 用以放用户列表的智能指针。如果没有获取到列表，则为空。
**返回值** 错误码
**调用方式** 同步

int  GetUser(const char* uid, user_ptr& user);

**功能** 根据userid获取用户信息
**参数** uid:userid user: 用户信息对象
**返回值** 错误码
**调用方式** 同步

int  KickOff(const char* uid);

**功能** 将用户踢出频道
**参数** uid:userid
**返回值** 错误码
**调用方式** 异步

int  SetUserAttr(const char* uid, const char* name, const char* value);

**功能** 设置用户属性
**参数** uid:userid name:自定义字符串，不可为空，value自定义字符串，为空，则删除此键值对。
**返回值** 错误码
**调用方式** 异步

int  GettUserAttr(const char* uid, const char* name, string_ptr& value);
**功能** 获取用户属性
**参数** uid：userid name:自定义字符串，不可为空 value:传入空智能指针，如果获取失败，则为空，上层使用前要判断。
**返回值** 错误码
**调用方式** 同步

#### 模块接口IRtcAudio
此模块下的异步事件有：
RespondDisableUserSpeak：禁言用户结果，附带参数为object_disable_speak
RespondBlockUser： 拒听某用户讲话，附带参数为object_block_speak
NotifyDisableUserSpeak：用户被禁言，附带参数为object_disable_speak
NotifyUserSpeaking：某用户正在讲话，附带参数为object_user_speaking

int BlockUser(const char* uid, bool block);
**功能** 拒听某用户发言，其他人还可以听到他说话。
**参数** uid:userid  block:是否拒听
**返回值** 错误码
**调用方式** 异步

int   DisableUserSpeak(const char* uid, bool disspeak);
**功能** 禁言用户，所有人都不能听到他说话。
**参数** uid:userid disspeak:是否禁言
**返回值** 错误码
**调用方式** 异步

void  EnableSpeak(bool enable)
**功能** 允许自己讲话
**参数** enable:是否要讲话
**调用方式** 异步

 bool  GetSpeakEnabled();
 **功能** 当前自己是否能讲话
 **参数** 无
 **返回值** 是否能讲话
 **调用方式** 同步
 
 void  EnablePlayout(bool enable);
 **功能** 是否要播放其他人讲话。
** 参数** enable:是否播放
** 调用方式** 异步
 
 bool  GetPlayoutEnabled();
 **功能** 是否在播放
 **参数** 无
 **返回值** 是否能播放
 **调用方式** 同步
 
#### 模块IRtcAudioSystem
int SetPlayoutVolume(int volume);
**功能 **设置播放音量
**参数** volume：有效值0-100。
**返回值** 错误码
**调用方式** 同步

int GetPlayoutVolume()
**功能** 获取当前播放音量
**参数** 无
**返回值** 音量，如果为负数，则表示错误码
**调用方式** 同步

void  SetSpeakerphoneOn(bool bOn);
**功能** 设置是否扬声器模式播放。只对于Android 和iOS系统有效。
**参数** bon：是否为扬声器模式。
**调用方式** 同步

bool GetSpeakphoneOn()
**功能** 是否为扬声器模式
**参数** 无
**返回值** 是否为扬声器模式
**调用方式** 同步

#### 模块IRtcDeviceControler
int  Enable(int types, bool enable)
**功能** 启用/禁用子功能。
**参数** 
types 以下类型的组合
```CPP
		typeCtrlByHeadset = 0x01  //受耳机插拔控制(插上耳机，无回声，那么高音质，否则讲话模式)
		typeAec = 0x04    //回声抑制控制
		typeNs = 0x08    //噪声抑制控制
		typeAgc = 0x010   //自动增益控制
		typeVad = 0x020   //静音检测控制 
		typeEcho = 0x40   //而返控制
		typeMusicMode         = 0x1000 // 音乐房间, 默认就是音乐房间 
		typeHookRecord        = 0x2000 // HookCapture  PushRecordAudioFrame 有效
		typeHookPlayout       = 0x4000 // HookPlayout  GetPlayoutAudioFrame 有效
		typeBackgroundMusic   = 0x8000 // 支持背景音乐  PushBackgroudAudioFrame 有效 
```

  enable 启用/禁用
**返回值** 错误码
**调用方式** 同步


bool IsEnabled(int type)
**功能** 查询子功能是否启用
**参数** type 功能类型
**返回值** bool 是否启用
**调用方式** 同步

int  GetPlayoutAudioFrame(char* pcm, int len, int& samprate, int& channel)
**功能** 获取当前拦截到的播放语音帧
**参数** pcm 语音数据，16位深度；len 语音帧长度，字节为单位；samplrate 采样率; channel 声道数
**返回值** 错误码
**调用方式** 同步

int  PushRecordAudioFrame(char* pcm, int len, int samprate, int channel)
**功能** 发送采集到的声音 typeHookRecord=true有效
**参数** pcm 语音数据，16位深度；len 语音帧长度，字节为单位；samplrate 采样率; channel 声道数
**返回值** 错误码
**调用方式** 同步

int  PushBackgroudAudioFrame(int trackindex,char* pcm, int len, int samprate, int channel, float scale, bool bPlayout)
**功能** 发送背景音乐 typeBackgroundMusic=true有效
**参数** trackindex 音轨id,有效值0-4  pcm 语音数据，16位深度；len 语音帧长度，字节为单位；samplrate 采样率; channel 声道数；scale,调整声音大小，默认1；bPlayout 是否本地播放，默认为false。
**返回值** 错误码
**调用方式** 同步

#### 模块IAudioPlayer
作为插件，此模块接收mp3文件，解析后回调到上层，然后由上层调用PushBackgroudAudioFrame推送到播放器。
void SetCallback(ICallback* pCallback)

**功能** 设置回调接口，用于接收解析文件后的帧数据
**参数** pCallback 回调接口
**返回值** 无
**调用方式** 同步

int  PlayFile(const char* filename, bool loopflag)

**功能** 播放文件，目前支持mp3文件
**参数** filename 全路径mp3文件；loopflag 是否循环播放
**返回值** 错误码
**调用方式** 同步

void Stop()
**功能** 停止播放文件
**参数** 无
**返回值** 无
**调用方式** 同步

void Pause(bool bPause)
**功能** 暂停放文件
**参数** bPause 暂停/恢复
**返回值** 无
**调用方式** 同步

int  SetVolume(float sc)
**功能** 设置播放音量系数
**参数** sc 有效范围[0.0-4.0],sc=1.0则按正常音量。
**返回值** 错误码
**调用方式** 同步

 int  SetPlayTime(ulong_t timePosition)
 **功能** 设置播放起始点
**参数**  timePosition 时间起始点，单位毫秒。
**返回值** 错误码
**调用方式** 同步

int  GetPlayedTime(ulong_t& timePosition)
 **功能** 获取当前播放时间点。
**参数**  timePosition 播放时间点，单位毫秒。
**返回值** 错误码
**调用方式** 同步

int  GetFileTime(ulong_t& timeSpan)
 **功能** 获取文件播放总时长。
**参数**  timeSpan 时长。
**返回值** 错误码
**调用方式** 同步

#### 模块IAudioPlayerDemo
此模块简单封装了IAudioPlayer，把数据推送到播放器。
int  Play(int trackIndex,void* pRtcDeviceControler, const char* filename, bool loopflag, float scVolume, bool bSendToNet, bool bPlayToDevice)
 **功能** 播放mp3文件。
**参数**  trackIndex 音轨标识 pRtcDeviceControler 设备控制模块指针；filename 全路径mp3文件；loopflag 循环播放标记；scVolume 音量控制；bSendToNet 是否分享给频道成员；bPlayToDevice 是否本地播放。
**返回值** 错误码
**调用方式** 同步

void Stop(int trackIndex)
**功能** 停止播放文件
**参数** trackIndex 音轨标识
**返回值** 无
**调用方式** 同步

int  SetVolume(int trackIndex,float sc)
**功能** 设置播放音量系数
**参数** trackIndex 音轨标识 sc 有效范围[0.0-4.0],sc=1.0则按正常音量。
**返回值** 错误码
**调用方式** 同步

 int  SetPlayTime(int trackIndex,ulong_t timePosition)
 **功能** 设置播放起始点
**参数**  trackIndex 音轨标识 timePosition 时间起始点，单位毫秒。
**返回值** 错误码
**调用方式** 同步

int  GetPlayedTime(int trackIndex,ulong_t& timePosition)
 **功能** 获取当前播放时间点。
**参数**  trackIndex 音轨标识 timePosition 播放时间点，单位毫秒。
**返回值** 错误码
**调用方式** 同步

int  GetFileTime(int trackIndex,ulong_t& timeSpan)
 **功能** 获取文件播放总时长。
**参数**  trackIndex 音轨标识 timeSpan 时长。
**返回值** 错误码
**调用方式** 同步

 int  GetPlayEndCount(int trackIndex)
 **功能** 统计循环播放了多少遍
**参数** trackIndex 音轨标识
**返回值** 播放次数
**调用方式** 同步


#### 附带错误码
```c++
    #define ERR_SUCCEED 0         // 操作成功
    #define ERR_NOT_LOGINED  -1   // 未登录成功       
    #define ERR_ALREADY_RUN  -2   // 已经在运行   
    #define ERR_USER_NOTFOUND -3  // 未找到用户   
    #define ERR_EXCUTING -4		  // 正在执行中 
    #define ERR_NOT_INITIALIZE -5 // 未初始化
    #define ERR_UNSUPPORT -6      // 功能不支持
    #define ERR_ARGUMENT  -7	  // 参数错误

    #define ERR_CHANNEL_EXPIRED 1        // 频道已经失效
    #define ERR_CONNECT_SERVER_FAILED 2  // 连接服务器失败
    #define ERR_REQUEST_TIMEOUT 3  // 请求超时
    #define ERR_CONFIG 4  // 配置信息错误 
    #define ERR_NET_POOL 5  // 网络不好
    #define ERR_VERSION_UNSUPPORTED 6  // 版本不支持
    #define ERR_AUTHO_FAILED 7  // 授权失败 
    #define ERR_NOT_ENOUGH_CHANNEL 8  // 频道资源不足
    #define ERR_SERVER_ERROR 9  // 服务器错误
    #define ERR_OPEN_RECORD 10  // 打开采集失败
    #define ERR_OPEN_PLAYOUT 11  // 打开播放失败
    #define ERR_RECORD_PERMISSION 12  // 没有录音权限

    #define ERR_UNDEFINED 100  // 未定义错误

    #define ECODE_OPENFILE_FAILED 1001,
    #define ECODE_UNSUPPORT_FORMAT 1002,
    #define ECODE_PARAMS_ERROR 1003,
```



