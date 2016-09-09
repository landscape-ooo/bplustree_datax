/*
 * TencentStorageServiceWriter.h
 *
 *  Created on: Aug 16, 2016
 *      Author: a11
 */

#ifndef SRC_READER_TENCENTSTORAGESERVICEWRITER_H_
#define SRC_READER_TENCENTSTORAGESERVICEWRITER_H_
#include "box/box_object.h"
#include "store_sdk_codec.h"
#include "store_define.h"
#include "TencentOSSConf.h"
#include "common/tools.h"
#include "transfer/zerocopy_stream.h"
#include "transfer/tcp_transfer.h"
namespace writer{ namespace tencent{
class TencentStorageServiceWriter{
public :
	TencentStorageServiceWriter(){
		this->sdkcodec.set_business_id("ganji");
	}
	void messageInputStream();
	string  messageOutputStream();

	TencentStorageServiceWriter* messageHeaderFormat(const box::field::MessageItem* msgPtr);
	TencentStorageServiceWriter* messageHeaderFormat(const box::field::Header* headPtr);
	TencentStorageServiceWriter* messageBodyFormat(const box::field::Body* bodyPtr);
	TencentStorageServiceWriter* messageBodyFormat(const box::field::MessageItem* msgPtr);

	void  sprintf_response(const string& ) ;



	string msgbodyStr;
private:
	QueryInfo _q;
	static string _GetOptionalMapData(
			const map<string, string>& mappingheader, const string key,
			const bool must ) ;
	bool _decode_response(
			string& rsp,store_photo_sdk::pic_info_list &pic_info,\
			map < string, string > &profile,string&);
private:
	string uid;
	string fileid;
	string filename;
	string filename_meta;
	string waterinfo;

	bool is_overwrite_flag;

	string databuffer;
	char* dataPtr;
	unsigned long datasize;

	char* metaPtr;
	unsigned long metasize;


	string data_sha1;
	store_photo_sdk::SdkCodec sdkcodec;
	store_photo_sdk::upload_param para;
	unsigned reqid;
};

}}




#endif /* SRC_READER_TENCENTSTORAGESERVICEWRITER_H_ */
