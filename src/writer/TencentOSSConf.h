/*
 * TencentOSSConf.h
 *
 *  Created on: Aug 16, 2016
 *      Author: a11
 */

#ifndef SRC_WRITER_OBJECTSERVICE_TENCENTOSSCONF_H_
#define SRC_WRITER_OBJECTSERVICE_TENCENTOSSCONF_H_
#include <ctime>
#include "common/tools.h"
#include "transfer/tcp_transfer.h"
namespace writer{ namespace tencent{ namespace TencentOSSConf{
	const   string IP=fdfs2qq::GetIniReaderStr("tencent_pic.host");
	const   string PORT=fdfs2qq::GetIniReaderStr("tencent_pic.port");
}}}

namespace writer{namespace tencent{

using namespace fdfs2qq;
class QueryInfo {

public:
	QueryInfo():retflag(1),qq_retcode(-1){

	}

public:
	int retflag;
	int qq_retcode;
	std::string error_reason;


	store_photo_sdk::info_head_t head_info;
	store_photo_sdk::pic_info_list list;
	map<string,string> profile;
	std::string md5;
	float width;
	float height;
	friend std::ostream& operator << (std::ostream& os
			,const QueryInfo& q) {
		std::string str;
		if(q.retflag<0){ //error
			str.append(	String_Format("{\"retflag\":%d,\"qq_retcode\":%d}",q.retflag,q.qq_retcode));
			os<<str;
			return os;
		}else{

		}
		//success
		store_photo_sdk::info_head_t info_head=q.head_info;
		store_photo_sdk::pic_info_list list=q.list;
		map<string,string> _profile=_profile;

		str.append(	String_Format("{\"retflag\":%d,\"qq_retcode\":%d,",q.retflag,q.qq_retcode));
		if(!_profile.size()){
			time_t  time_t_var=(int )info_head.buildtime;
			std::string ret_time=std::asctime(std::localtime(&time_t_var )) ;
			ret_time.replace(ret_time.end()-1,ret_time.end(),"");
			str.append(	String_Format("\"build time\":\"%s\",",ret_time.c_str()));

			str.append(	String_Format("\"fsize\":%u,", info_head.fsize));
			str.append(	String_Format("\"chid\":%u,\"fid\":%u,", info_head.chid, info_head.fid));
			str.append(	String_Format("\"update time\":%u,", info_head.updatetime));
			str.append(	String_Format("\"slices num\":%u,", info_head.num));
		}else{
			str.append(	String_Format("frame_count -> %s\n", _profile["frame_count"].c_str()));
			str.append(	String_Format("origin_height -> %s\n", _profile["origin_height"].c_str()));
			str.append(	String_Format("origin_width -> %s\n", _profile["origin_width"].c_str()));
			str.append(	String_Format("origin_md5 -> %s\n", ::transfer::tcp::client::Mem2hex(_profile["origin_md5"].c_str(), 16).c_str()));
			str.append(	String_Format("origin_size -> %s\n", _profile["origin_size"].c_str()));
			str.append(	String_Format("origin_type -> %s\n", _profile["origin_type"].c_str()));
		}
		for (unsigned i = 0; i < list.list.size(); ++i) {
			str.append(	String_Format("\"md5 [%d]\":\"%s\",", i,
					::transfer::tcp::client::Mem2hex(list.list[i].fmd5.c_str(), 16).c_str()));
			str.append(	String_Format("\"width [%d]\":%u,", i, list.list[i].width));
			str.append(	String_Format("\"height [%d]\":%u,", i, list.list[i].height));
		}
		// last must is "," ,replace it
		str.replace(str.end()-1,str.end(),"}");
		os<<str;
		return os;
	}
};
}}

#endif /* SRC_WRITER_OBJECTSERVICE_TENCENTOSSCONF_H_ */
