/*
 * appconfig.cc
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */
#include "StorageConfig.h"

namespace fdfs2qq {

const std::string StorageConfig::FDFS_STORAGE_CONF =
		"/usr/local/webserver/fdfs/etc/storage.conf";
std::string StorageConfig::FDFS_STORAGE_CONF_CONTENT = getContentByConfpath();
const std::string StorageConfig::GROUP_ID = GET_GROUP_ID();
std::vector<std::string> StorageConfig::SUBDIR_PER_PATH_LIST =
		GET_SUBDIR_PATH_LIST();
std::map<std::string, std::string> StorageConfig::VolumnsList_Revert;
std::map<std::string, std::string> StorageConfig::VolumnsList =
		GetVolumnsList();

const int StorageConfig::MAX_VOLUMNS_COUNT = GetVolumnsCount();

const std::string StorageConfig::PIC_EXT_LIST[] =
		{ "jpg", "jpeg", "bmp", "png" };

int StorageConfig::GetVolumnsCount() {
	return StorageConfig::VolumnsList.size();
}

std::map<std::string, std::string> StorageConfig::GetVolumnsList() {
	std::istringstream iss(FDFS_STORAGE_CONF_CONTENT);
	std::map<std::string, std::string> dataVolumnsList;
	std::string line;
	while (std::getline(iss, line)) {
		if (line.c_str()[0] == '#') {
			continue;
		}
		const std::string find = "store_path";
		//line.find("/") != std::string::npos
		//it must be a path
		if (line.find(find) != std::string::npos
				&& line.find("/") != std::string::npos) {
			const std::string delim = "=";
			int _st = line.find(find);
			int _mid;
			if (line.find(delim) != std::string::npos) {
				_mid = line.find(delim);
			}

			std::string key, value;
			if (_st + find.size() == _mid) {
				value = "0";
			} else {
				value = line.substr(_st + find.size(),
						_mid - find.size() - _st);
			}
			value = OBJECT_ID_4SUBDIR::get_volumnsByObj(value);

			key = line.substr(_mid + 1, line.size() - 1);
			dataVolumnsList.insert(std::make_pair(key, value));
		}
	}
	return dataVolumnsList;
}
int StorageConfig::getStoreFolderByOrder(
		std::vector<fdfs::OBJECT_ID_4SUBDIR>& _ret) {

	std::map<std::string, std::string> dataVolumnsList = GetVolumnsList();
	for (std::map<std::string, std::string>::iterator it_map =
			dataVolumnsList.begin(); it_map != dataVolumnsList.end();
			it_map++) {

		for (std::vector<std::string>::iterator it =
				SUBDIR_PER_PATH_LIST.begin(); it != SUBDIR_PER_PATH_LIST.end();
				it++) {

			OBJECT_ID_4SUBDIR obj;
			obj.grpid = GROUP_ID;
			obj.physical_store_path = it_map->first;

			std::string tmp_volumns_id = it_map->second;
			//volumns id
			obj.volumns_id = tmp_volumns_id;
			obj.subdir_volumns = *it;
			std::string fdfs_path = OBJECT_ID_4SUBDIR::get_Fdfs_store_pathByObj(
					GROUP_ID, *it, tmp_volumns_id);
			obj.fdfs_store_path = fdfs_path;
			_ret.push_back(obj);
		}
	}
	return 1;

}

std::vector<std::string> StorageConfig::GET_SUBDIR_PATH_LIST() {

	//default is 256
	std::vector<std::string> _tmp_v_path1, _ret;
	std::string vlist[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
			"A", "B", "C", "D", "E", "F" };
	std::vector<std::string> v(vlist, vlist + 16);
	for (int ii = 0; ii < v.size(); ii++) {
		for (int jj = 0; jj < v.size(); jj++) {
			_tmp_v_path1.push_back(v[ii] + v[jj]);
		}
	}
	for (int ii = 0; ii < _tmp_v_path1.size(); ii++) {
		for (int jj = 0; jj < _tmp_v_path1.size(); jj++) {
			_ret.push_back("data/" + _tmp_v_path1[ii] + "/" + _tmp_v_path1[jj]);
		}
	}
	return _ret;
}

/**
 * @storagepath
 */
bool StorageConfig::getfileListOfStoragepath(
		const fdfs::OBJECT_ID_4SUBDIR& storagepath_ptr,
		//const std::map<std::string, std::string>& store_path_map,
		std::vector<OBJECT_ID_4FILE>& _ret) {
	//get conf
	std::vector<std::string> file_list;

	if (storagepath_ptr.physical_store_path.empty()) {
		return false;
	}
	std::string storagepath;
	int size = storagepath_ptr.physical_store_path.size();
	if (storagepath_ptr.physical_store_path[size - 1] != '/') {
		storagepath.assign(
				storagepath_ptr.physical_store_path + "/"
						+ storagepath_ptr.subdir_volumns);
	} else {
		storagepath.assign(
				storagepath_ptr.physical_store_path
						+ storagepath_ptr.subdir_volumns);
	}
	std::vector<std::string> _tmp_pic_ext;
	unsigned offset = sizeof(StorageConfig::PIC_EXT_LIST)
			/ sizeof(StorageConfig::PIC_EXT_LIST[0]);
	//(StorageConfig::PIC_EXT_LIST[0],StorageConfig::PIC_EXT_LIST + sizeof(StorageConfig::PIC_EXT_LIST) / sizeof(StorageConfig::PIC_EXT_LIST[0]) );
	std::copy(StorageConfig::PIC_EXT_LIST, StorageConfig::PIC_EXT_LIST + offset,
			std::back_inserter(_tmp_pic_ext));

	for (std::vector<std::string>::iterator it = _tmp_pic_ext.begin();
			it != _tmp_pic_ext.end(); it++) {
		const std::string full_path = storagepath + "/*." + *it;
		if (GetFilelistByPath(full_path, file_list) <= 0) {
			continue;
		}
	}
	if (file_list.empty())
		return false;

	for (std::vector<std::string>::iterator it = file_list.begin();
			it != file_list.end(); it++) {
		OBJECT_ID_4FILE obj;
		obj.dir_info_ptr = storagepath_ptr;
		obj.filename = *it;
		obj.metafilename = OBJECT_ID_4FILE::getMetafilename(obj);
		obj.global_object_id = OBJECT_ID_4FILE::getGlobalId(obj);
		_ret.push_back(obj);
	}

	//build path
	if (_ret.empty()) {
		return false;
	}
	//push
	return true;

}

const std::string StorageConfig::getContentByConfpath() {
	//read file
	//parse it
	//return data path
	if (FDFS_STORAGE_CONF.empty())
		return "";

	std::string logStr;
	//
	read_file(FDFS_STORAGE_CONF, logStr);

	return logStr;
}
/**
 * @param dirPath ,is regex !!!
 */
bool StorageConfig::GetFilelistByPath(const std::string& dirPath,
		std::vector<std::string>& files) {

	try {
		glob_t glob_result;
		glob(dirPath.c_str(), GLOB_TILDE, NULL, &glob_result);
		for (unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
			std::string fullpath(glob_result.gl_pathv[i]);
			std::size_t found = fullpath.find_last_of("/\\");
			if (found == std::string::npos)
				continue;
			files.push_back(fullpath.substr(found + 1));
		}
		globfree(&glob_result);
		return true;
	} catch (std::exception &ex) {
		return false;
	}
}

std::string StorageConfig::GET_GROUP_ID() {
	std::string grp_id;
//	int iskey = StorageConfig::getContentByConfpath(FDFS_STORAGE_CONF_CONTENT);
//	if (iskey < 1)
//		throw fdfs::runtime_invalide_error("code:101,error get fdfs conf");

	std::istringstream iss(FDFS_STORAGE_CONF_CONTENT);
	std::string line;
	const std::string find = "group_name=";
	while (std::getline(iss, line)) {
		if (line.c_str()[0] == '#')
			continue;
		if (line.find(find) != std::string::npos) {
			grp_id = line.substr(line.find(find) + find.size(),
					line.size() - 1);
			break;
		}
	}

	return grp_id;
}

}
