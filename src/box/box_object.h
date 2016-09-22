/*
 * box.object.h
 *
 *  Created on: Aug 11, 2016
 *      Author: a11
 */
#include "common/common_define.h"
#include <algorithm>
#include <memory>
#include <cstdarg>
#include <fstream>

#ifndef SRC_BOX_BOX_OBJECT_H_
#define SRC_BOX_BOX_OBJECT_H_
namespace box {namespace field {
using namespace fdfs2qq;
using namespace std;
const int BUFFER_KK_SIZE=128;
const int BUFFER_KV_SIZE=1024;
const int BUFFER_MAX_LINE=4096;

struct HeaderItem {
	bool isvalid;
	char key_v[BUFFER_KK_SIZE];
	char value_v[BUFFER_KV_SIZE];
	HeaderItem():isvalid(false)
	{
//		std::string s;
//		memcpy( key_v, "", sizeof(key_v));
//		memcpy( value_v, "", sizeof(value_v));
	}
	HeaderItem& operator = (const HeaderItem& copy){
		if(copy.isvalid){
			isvalid=true;
			std::copy(copy.key_v,copy.key_v+BUFFER_KK_SIZE,key_v);
			std::copy(copy.value_v,copy.value_v+BUFFER_KV_SIZE,value_v);
		}
		 return *this;
	}
	HeaderItem(const HeaderItem&copy):isvalid(false){
		if(copy.isvalid){
			isvalid=true;
			std::copy(copy.key_v,copy.key_v+BUFFER_KK_SIZE,this->key_v);
			std::copy(copy.value_v,copy.value_v+BUFFER_KV_SIZE,this->value_v);
		}
	}
};

class Header{
public:
	static const int _MaxItemsCount;
	HeaderItem* _Hptr;
	int _MaxPtrLen;
	~Header(){
		delete []_Hptr;
	}
	Header():_MaxPtrLen(0){
		_Hptr=new HeaderItem[this->_MaxItemsCount];
	}
	Header(const Header &copy){
		_Hptr=new HeaderItem[copy._MaxItemsCount];
		_MaxPtrLen=copy._MaxPtrLen;

		std::copy(copy._Hptr,copy._Hptr+copy._MaxPtrLen,this->_Hptr);
	}
	char* getHeader(const char* key);
//	void setHeader(const HeaderItem* item);

	void setHeader(const HeaderItem* fmt) {

		if(fmt->isvalid){
			*(this->_Hptr+this->_MaxPtrLen)=*fmt;
			this->_MaxPtrLen++;
		}

	    return ;
	}
};
class Body{
private:
	string fileHandle;//filename unixdomain
	char* dataPtr;
	int dataLen;
public:
	bool isFilehandle;
	Body():isFilehandle(false),fileHandle(""),dataPtr(NULL),dataLen(0){
	}
	string getFilehandle() const{
		return this->fileHandle;
	}
	void setBodyValue(const char* dataptr,const int datalen){
		this->isFilehandle=false;
		this->dataPtr=(char*)(dataptr);
		this->dataLen=datalen;
	}
	void setBodyValue(const string filename){
		this->isFilehandle=true;
		this->fileHandle=filename;
	}

};


struct MessageItem{
	Header hd_ptr;
	Body bd_ptr;
};
extern std::vector<HeaderItem> splitMetalist(const char* buffer,const int bufferLen);
extern std::vector<MessageItem> SplitMsg(const char* buffer,const int bufferLen,int &msgcount);
bool _splitMsg(const char* buffer, const int bufferLen, MessageItem &msg);
//bool _splitMsg(const char* buffer, const int bufferLen, std::shared_ptr<MessageItem> &msg);
//extern size_t _GetOccurCount(const string&, const char seperator);
//extern size_t _GetOccurCount(const char* array ,const size_t len, const char seperator);


//extern map<string,string> mappingHeaders(const Header* headerPtr);
extern map<string,string> MappingHeaders(const Header* headerPtr);

}}

#endif /* SRC_BOX_BOX_OBJECT_H_ */
