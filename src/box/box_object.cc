/*
 * box.object.cc
 *
 *  Created on: Aug 11, 2016
 *      Author: a11
 */
#include "box_object.h"
/**
 * [msgs[headers][body]]
 *
 * #msg:(head1)(head2)..(body)#
 */
namespace box { namespace field{


const int Header::_MaxItemsCount=512;

//
//size_t _GetOccurCount(const string& s , const char seperator)
//{
//	size_t n = std::count(s.begin(), s.end(), seperator);
//	return n;
//}
//size_t _GetOccurCount(const char* array ,const size_t len, const char seperator)
//{
//	if(!array||len<=0){
//		return 0;
//	}
//	size_t count = 0,index=0;
//	while(index<len&&*(array+index)!='\0'){
//		if(*(array+index)==seperator)
//			count++;
//		index++;
//	}
//	return count;
//}

std::vector<HeaderItem> splitMetalist(const char* buffer, const int bufferLen) {
	std::vector<HeaderItem> ret;

	if(buffer==NULL ||bufferLen<=0){
		return ret;
	}

	string metastr=std::string(buffer,bufferLen);
	string item;

	std::vector<std::string> elems;
	std::stringstream ss(metastr);
	while(std::getline(ss,item,BOX_RECORD_SEPERATOR)){
		elems.push_back(std::move(item));
	}
	for(std::vector<string>::iterator it=elems.begin();
			it!=elems.end();it++){
		//std::shared_ptr<HeaderItem> retPtr_tmp=std::make_shared<HeaderItem>();
		HeaderItem retPtr_tmp;
		std::vector<std::string> kv_elems;
		std::stringstream kv_ss(*it);
		string kv_item;
		while(std::getline(kv_ss,kv_item,BOX_KV_SEPERATOR)){
			kv_elems.push_back(std::move(kv_item));
		}
		if(kv_elems.size()!=2){
			retPtr_tmp.isvalid=false;
		}else{
			retPtr_tmp.isvalid=true;
			strncpy(retPtr_tmp.key_v,(kv_elems[0]).c_str(),sizeof retPtr_tmp.key_v );
			strncpy(retPtr_tmp.value_v,(kv_elems[1]).c_str(),sizeof retPtr_tmp.value_v);
			ret.push_back(retPtr_tmp);
		}
	}


	return ret;
}
std::vector<MessageItem> SplitMsg(const char* buffer,const int bufferLen,int &msgcount){
	char* buffer_msg_start=(char*)buffer;
	char* buffer_msg_end;
	std::vector<MessageItem>  ret;

	buffer_msg_end=strchr(buffer_msg_start,BOX_MSG_SEPERATOR);
	while(buffer_msg_start!=buffer_msg_end){
		MessageItem obj_shard;
		auto s=_splitMsg(buffer_msg_start,buffer_msg_end-buffer_msg_start,obj_shard);
		//std::shared_ptr<MessageItem> obj_ptr=std::make_shared<MessageItem>(obj_shard);
		if(s){
			ret.push_back(obj_shard);
		}
		buffer_msg_start=buffer_msg_end;
		buffer_msg_end=strchr(buffer_msg_start,BOX_MSG_SEPERATOR);
	}

	msgcount=ret.size();

	return ret;
}

bool _splitMsg(const char* buffer, const int bufferLen, MessageItem &msg) {
	if (buffer == NULL || bufferLen <= 0) {
		return false;
	}

	//meta
	//{
		const char* buffer_start = buffer;
		const char* buffer_data_start = strchr(buffer, BOX_FIELD_SEPERATOR);
		const std::string buffer_meta_str(buffer_start,
				buffer_data_start - buffer_start);
		auto metalist = splitMetalist(buffer_meta_str.c_str(),
					buffer_meta_str.length());

		auto debug_size=metalist.size();

auto count=0;

		for (std::vector<HeaderItem>::iterator it =
				metalist.begin(); it != metalist.end(); it++) {
			msg.hd_ptr.setHeader(&(*it));

			if (std::string(it->key_v) == "filehandle") {
				msg.bd_ptr.setBodyValue(std::string(it->value_v));
			}
		}


		const char* buffer_meta_end = strrchr(buffer,
				BOX_FIELD_SEPERATOR)+1;



		if (!msg.bd_ptr.isFilehandle) {
			if(buffer_meta_end<buffer+bufferLen){
				auto s1=buffer_meta_end;
				auto s2=bufferLen-(buffer_meta_end-buffer);
				msg.bd_ptr.setBodyValue(buffer_meta_end,
						bufferLen-(buffer_meta_end-buffer));
			}
		}
	//}

	return true;
}


map<string,string> MappingHeaders(const Header* headerPtr){
	map<string,string> ret;
	for(int i=0;i<headerPtr->_MaxPtrLen;i++){
		auto headeritem=*(headerPtr->_Hptr+i);
		string key(headeritem.key_v);
		string value(headeritem.value_v);
		ret.insert(std::make_pair(key,value));
	}
	return ret;

}



}}
