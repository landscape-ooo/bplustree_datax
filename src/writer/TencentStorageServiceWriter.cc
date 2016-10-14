#include "TencentStorageServiceWriter.h"

namespace writer{ namespace tencent{
string TencentStorageServiceWriter::_GetOptionalMapData(
		const map<string, string>& mappingheader, const string key,
		const bool must ) {
	if (mappingheader.size() < 1)
		return "";
	if (mappingheader.find(key) == mappingheader.end()) {
		if (must) {
			//throw std::runtime_error("Gak!must key "+key+ + " is null,pls check");
			fdfs2qq::Logger::error("Gak!must key "+key+ + " is null,pls check");
		}
		return "";
	}

	return mappingheader.at(key);
}
TencentStorageServiceWriter * TencentStorageServiceWriter::messageHeaderFormat(
		const box::field::MessageItem* msgPtr){
	for(int i=0;i<msgPtr->hd_ptr._MaxPtrLen;i++){
		this->messageHeaderFormat(&msgPtr->hd_ptr);
	}

	return this;
}
TencentStorageServiceWriter * TencentStorageServiceWriter::messageBodyFormat(
		const box::field::MessageItem* msgPtr){
	this->messageBodyFormat(&msgPtr->bd_ptr);
	return this;
}



TencentStorageServiceWriter* TencentStorageServiceWriter::messageBodyFormat(
		const box::field::Body* bodyPtr) {

	char *metaPtr;
	unsigned long datasize, metasize;
	string md5sum;
	if (bodyPtr->isFilehandle) {
		this->filename = bodyPtr->getFilehandle();
		//mmap data
		unsigned long datasize = transfer::fstream::Getfilesize(this->filename) ;
		if (datasize!= 0) {
			this->databuffer.assign(datasize, ' ');
//			char* ptr_start=(char*)databuffer.c_str();
//			transfer::fstream::MMAP_FILE(this->filename,
//					ptr_start, datasize);
			transfer::fstream::STREAM_READ(this->filename,this->databuffer);
			md5sum = fdfs2qq::GetMd5sum(this->databuffer.c_str(),
					this->databuffer.length());
		}
	}
	// 元数据填充info字段
	std::string info;
	if (!this->filename_meta.empty()) {

		unsigned long datasize = transfer::fstream::Getfilesize(this->filename_meta) ;
		if (datasize!= 0) {
			info.assign(datasize, ' ');
			transfer::fstream::STREAM_READ(this->filename_meta,info);
		}
	}

	if (!this->is_overwrite_flag) {
		sdkcodec.encode_raw_overwrite_v2_req(this->uid, this->fileid, info,
				this->databuffer, this->data_sha1, this->para, this->msgbodyStr,
				reqid);
	}
	sdkcodec.encode_raw_upload_v2_req(uid, fileid, info, this->databuffer,
			this->data_sha1, this->para, this->msgbodyStr, reqid);
	return this;
}
TencentStorageServiceWriter* TencentStorageServiceWriter::messageHeaderFormat(
		const box::field::Header* headPtr) {

	map < string, string > mappingheader = box::field::MappingHeaders(
			headPtr);
	// fileid
	// 注意fileid唯一标识一个图片，若重复上传会报-6102错误
	this->fileid = _GetOptionalMapData(mappingheader, "fileid", true);

	//异步序列号id,客户端使用，服务端原样返回
	this->reqid = (unsigned) fdfs2qq::Gethash(this->fileid);

	this->filename_meta = _GetOptionalMapData(mappingheader, "file_meta",
			false);
	// 用户id
	this->uid = _GetOptionalMapData(mappingheader, "uid", false);

	string overwrite_flag = _GetOptionalMapData(mappingheader,
			"overwrite_flag", false);

	if (!overwrite_flag.empty()) {
		if(std::stoi( overwrite_flag )==1){//only special 1
			is_overwrite_flag = true;
		}
	}
	return this;
}

void TencentStorageServiceWriter::messageInputStream() {
}

string TencentStorageServiceWriter::messageOutputStream() {
	if(  !this->msgbodyStr.empty()){
		transfer::tcp::client* cl = new transfer::tcp::client();
		transfer::tcp::client::connect_sever(cl->pCurrentServer, TencentOSSConf::IP,
				TencentOSSConf::PORT);
		transfer::tcp::client::send_req(cl->pCurrentServer, this->msgbodyStr);
		auto rsp = transfer::tcp::client::recv_rsp(cl->pCurrentServer);
		return rsp;
	}else{
		return std::string("");
	}

}
/**
 * return string:fileid
 */
bool TencentStorageServiceWriter::_decode_response(string& rsp,
		store_photo_sdk::pic_info_list &pic_info, map<string, string> &profile,
		string& fileid) {

	int complete = 0;
	complete = this->sdkcodec.store_is_complete(rsp.c_str(), rsp.size());
	rsp.resize(complete);

	unsigned reqid = 0;
	int retcode = 0;

	sdkcodec.decode_head(rsp, reqid, retcode);

	if (retcode != 0 && retcode != -6102) {
		char buffer[100];
		int len = sprintf(buffer, "fileid %s ->reqid %d, error: %d\n",fileid.c_str(), reqid, retcode);
		//throw std::runtime_error("Gak!" + std::string(buffer, len));
		fdfs2qq::Logger::error("Gak!" + std::string(buffer, len));
	}

	unsigned chid, fid;
	sdkcodec.decode_raw_upload_v2_rsp(rsp, fileid, chid, fid, _q.list,
			_q.profile);

	_q.qq_retcode = retcode;
	_q.retflag = 1;
	if (retcode != 0) {
		char* buffer;
		int n;
		buffer = (char*) malloc(50);
		n = sprintf(buffer, "error in retcode,result retcode=\t%d", retcode);

		_q.error_reason = std::string(buffer, n);
		_q.qq_retcode = retcode;
		_q.retflag = -1;
		return false;
	}

	return true;
}
void TencentStorageServiceWriter::sprintf_response(const string& rsp_origin) {

	store_photo_sdk::pic_info_list pic_info;
	map < string, string > profile;
	string rsp(rsp_origin);
	string fileid;
	_decode_response(rsp,pic_info,profile,fileid);
}

}}
