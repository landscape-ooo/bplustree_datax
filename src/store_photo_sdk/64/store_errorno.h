#ifndef _STORE_ERRORNO_H_
#define _STORE_ERRORNO_H_

namespace store_photo_sdk
{

//�������
const int ERR_NETWORKER_SEND=-100;		//���緢��ʧ��
const int ERR_NETWORKER_CHOICE_SERVER=-101;		//ѡ�����������
const int ERR_NETWORKER_GET_CONNECT=-102;		//�����ӳػ�ȡ���Ӵ���
const int ERR_NETWORKER_SET_BLOCK=-103;		//����socket����
const int ERR_NETWORKER_RECV=-104;		//���緢��ʧ��

const int ERR_NOTENOUTHBUF = -200;		//��������С����
const int ERR_NULLPOINTER = -201;			//����ʱ����ָ��Ϊ��
const int ERR_DECODE_PARSE = -202;		//����ʧ��
const int ERR_PROTOTYPE = -203;			//У������ʹ���,Ŀǰֻ֧��protobuf����
const int ERR_SERIALIZE = -204;			//����ʧ��
const int ERR_RECV_HEAD = -205;			//���ջذ�ʧ��
const int ERR_MAX_BUFFER= -206;

const int ERR_MAGICNUM = -300;			//У��magicnum����
const int ERR_MSG_LENGTH = -301;			//���հ��İ����ȴ���

const int ERR_HTTP_DECODE = -401;			//httpЭ�������ʧ��
const int ERR_HTTP_CMD_TYPE = -402;			//httpЭ����������ʹ���
const int ERR_HTTP_AUTHORIZATION = -403;	//httpЭ���Authorization�ֶ�У��ʧ��
	
}
#endif

