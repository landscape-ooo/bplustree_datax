#ifndef _STORE_ERRORNO_H_
#define _STORE_ERRORNO_H_

namespace store_photo_sdk
{

//网络错误
const int ERR_NETWORKER_SEND=-100;		//网络发送失败
const int ERR_NETWORKER_CHOICE_SERVER=-101;		//选择服务器错误
const int ERR_NETWORKER_GET_CONNECT=-102;		//从连接池获取连接错误
const int ERR_NETWORKER_SET_BLOCK=-103;		//设置socket错误
const int ERR_NETWORKER_RECV=-104;		//网络发送失败

const int ERR_NOTENOUTHBUF = -200;		//超过包大小限制
const int ERR_NULLPOINTER = -201;			//编码时传入指针为空
const int ERR_DECODE_PARSE = -202;		//解码失败
const int ERR_PROTOTYPE = -203;			//校验包类型错误,目前只支持protobuf类型
const int ERR_SERIALIZE = -204;			//编码失败
const int ERR_RECV_HEAD = -205;			//接收回包失败
const int ERR_MAX_BUFFER= -206;

const int ERR_MAGICNUM = -300;			//校验magicnum错误
const int ERR_MSG_LENGTH = -301;			//接收包的包长度错误

const int ERR_HTTP_DECODE = -401;			//http协议包解码失败
const int ERR_HTTP_CMD_TYPE = -402;			//http协议包请求类型错误
const int ERR_HTTP_AUTHORIZATION = -403;	//http协议包Authorization字段校验失败
	
}
#endif

