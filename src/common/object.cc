/*
 * object.cc
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */
#include "object.h"

namespace fdfs2qq {

OBJECT_ID_4SUBDIR& OBJECT_ID_4SUBDIR::operator=(
		OBJECT_ID_4SUBDIR const& other) {

	if (this != &other) {
		//std::lock_guard<std::mutex> _mylock(mtx), _otherlock(other.mtx);

		grpid = other.grpid;
		fdfs_store_path = other.fdfs_store_path;
		physical_store_path = other.physical_store_path;
		volumns_id = other.volumns_id;
		subdir_volumns = other.subdir_volumns;
	}
	return *this;

}
OBJECT_ID_4SUBDIR::OBJECT_ID_4SUBDIR() {
}
OBJECT_ID_4SUBDIR::OBJECT_ID_4SUBDIR(const OBJECT_ID_4SUBDIR& obj) {
	grpid = obj.grpid;
	fdfs_store_path = obj.fdfs_store_path;
	physical_store_path = obj.physical_store_path;
	volumns_id = obj.volumns_id;
	subdir_volumns = obj.subdir_volumns;
}
OBJECT_ID_4SUBDIR::~OBJECT_ID_4SUBDIR() {

}

std::string OBJECT_ID_4SUBDIR::get_volumnsByObj(const std::string& value) {

	int volumns_id = atoi(value.c_str());
	if (volumns_id <= 0 && value != "0") {
		throw fdfs2qq::runtime_invalide_error("error assert volumns style");
	}
	if (volumns_id <= 9) {
		return "M0" + value;
	}
	return "M" + value;
}
std::string OBJECT_ID_4SUBDIR::get_Fdfs_store_pathByObj(
		const std::string& _group_id, const std::string& _subdir_volumns,
		const std::string& _volumns_id) {
	//std::string group_id = obj.grpid;

	const std::string flag_str = "data/";
	std::string tmp_path = _subdir_volumns;
	while (tmp_path.find(flag_str) != std::string::npos) {
		tmp_path = tmp_path.substr(tmp_path.find(flag_str) + flag_str.length());
	}
	//std::string subdir_volumns = obj.subdir_volumns;
	std::string subdir_volumns_tmp = tmp_path;

//	std::string value = obj.volumns_id;

	char buffer[100];
	int char_len = sprintf(buffer, "%s/%s/%s", _group_id.c_str(),
			_volumns_id.c_str(), subdir_volumns_tmp.c_str());
	return std::string(&buffer[0], char_len);

}

//std::string OBJECT_ID_4SUBDIR::to_string() const {
//	std::vector<std::string> v;
//	v.push_back(this->grpid);
//	v.push_back(this->fdfs_store_path);
//	v.push_back(this->physical_store_path);
//	v.push_back(this->volumns_id);
//	v.push_back(this->subdir_volumns);
//
//	char delim = '\t';
//	return fdfs::implode(delim, v);
//}
//void OBJECT_ID_4SUBDIR::_deserilize(const std::string& vstr) {
//	std::vector<std::string> v;
//	char delim = '\t';
//	v = fdfs::split(vstr, delim);
//	if (v.size() > 4) {
//		this->grpid = v[0];
//		this->fdfs_store_path = v[1];
//		this->physical_store_path = v[2];
//		this->volumns_id = v[3];
//		this->subdir_volumns = v[4];
//	}
//}

OBJECT_ID_4FILE::~OBJECT_ID_4FILE() {
	//delete dir_info_ptr;
}

std::string OBJECT_ID_4FILE::getMetafilename(const OBJECT_ID_4FILE& obj) {
	if (obj.filename.empty()) {
		return "";
	}
	return obj.filename + "-m";
}
std::string OBJECT_ID_4FILE::getGlobalId(const OBJECT_ID_4FILE& obj) {
	if (obj.filename.empty()) {
		return "";
	}
	std::string _vloumns = obj.dir_info_ptr.fdfs_store_path;
	return _vloumns + "/" + obj.filename;
}
std::string OBJECT_ID_4FILE::to_string() const {

	return this->global_object_id;
}
//std::string OBJECT_ID_4FILE::to_string() const {
//	std::vector<std::string> v;
//	v.push_back(this->global_object_id);
//	v.push_back(this->filename);
//	v.push_back(this->metafilename);
//	v.push_back(this->dir_info_ptr.to_string());
//	char delim = '\r';
//	return fdfs::implode(delim, v);
//}
//void OBJECT_ID_4FILE::_deserilize(const std::string& str) {
//	std::vector<std::string> v;
//	v = fdfs::split(str, '\r');
//	if (v.size() > 3) {
//		this->global_object_id = v[0];
//		this->filename = v[1];
//		this->metafilename = v[2];
//		std::string dir = v[3];
//		OBJECT_ID_4SUBDIR obj;
//		obj._deserilize(dir);
//		this->dir_info_ptr = obj;
//	}
//}

}

