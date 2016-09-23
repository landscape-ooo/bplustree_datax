/*
 * appconfig.cc
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */
#include "StorageConfig.h"

namespace fdfs2qq {



StorageVolumnObject::StorageVolumnObject(const StorageVolumnObject& oth):isvalid(true) {
	grpid = oth.grpid;
	volumnid = oth.volumnid;
	subdir = oth.subdir;
	volumnstr = oth.volumnstr;
}
StorageVolumnObject::StorageVolumnObject(const string& msgstr,
		const string volumnstr = ""):isvalid(true) {
	/*gjfstmp2/M00/00/19/CgP_ylcD2nqAAVr6AADKanPqNRs485.jpg*/
	auto vec1 = fdfs2qq::split(msgstr, '/');
	if (vec1.size() > 0) {
		this->grpid = vec1[0];
		this->volumnid = vec1[1];
		this->subdir = vec1[2] + "/" + vec1[3];
		if (!volumnstr.empty()) {
			this->volumnstr = volumnstr;
		}
	}
}
StorageVolumnObject::StorageVolumnObject():isvalid(false),
			grpid(""),volumnid(""),volumnstr(""),subdir(""){

}
StorageVolumnObject::~StorageVolumnObject() {

}
StorageVolumnObject& StorageVolumnObject::operator=(
		StorageVolumnObject const& other) {

	if (other.isvalid) {
		isvalid=true;
		grpid = other.grpid;
		volumnid = other.volumnid;
		volumnstr = other.volumnstr;
		subdir = other.subdir;
	}
	return *this;
}

std::string StorageVolumnObject::to_string() const {
	std::vector<std::string> v;
	v.push_back(this->grpid);
	v.push_back(this->volumnid);
	v.push_back(this->volumnstr);
	v.push_back(this->subdir);

	char delim = '\t';
	return fdfs2qq::implode(delim, v);
}



StorageVolumnObject StringToStorageVolumnObject(const string& msgstr){
	StorageVolumnObject obj;
	char delim = '\t';
	auto vec1 = fdfs2qq::split(msgstr, delim);
	if (vec1.size() ==4) {
		obj.isvalid=true;
		obj.grpid = vec1[0];
		obj.volumnid = vec1[1];
		obj.volumnstr = vec1[2];
		obj.subdir = vec1[3] ;
	}
	return obj;
}

RESPONSE_HEADER StringToResponseObject(const string fileid,const string volumns,const string status){
	RESPONSE_HEADER header;
    memset(&header, 0, sizeof(header));
    //auto lastchar= fileid.length()>FILEID_MAXLEN?FILEID_MAXLEN-1:fileid.length();
    strncpy(header.fileid, fileid.c_str(),FILEID_MAXLEN);
    header.fileid[FILEID_MAXLEN-1] = '\0'; // my_char_array contains "some"

    strncpy(header.ext_volumns, volumns.c_str(),FILEID_MAXLEN);
    header.ext_volumns[FILEID_MAXLEN-1] = '\0'; // my_char_array contains "some"

	memcpy(&header.ext_status,status.c_str(),status.length());
	return header;
}
RESPONSE_HEADER StringToResponseObject(const string resp){
	RESPONSE_HEADER header;
	memcpy(&header,&resp,resp.length());
	return header;
}
string ResponseObjectToString(const RESPONSE_HEADER& obj){
	size_t relen=sizeof(RESPONSE_HEADER);
	char tmp[relen];
//	string tmp;
//	tmp.resize(relen);
	memcpy(&tmp,&obj,relen);

	return std::string(tmp,relen);
}
StorageFileObject ResponseObject2StorageFileObject(const RESPONSE_HEADER& resp){
//	string volumns(resp.ext_volumns,sizeof resp.ext_volumns);
//	string fileid(resp.fileid,sizeof resp.fileid);
	string volumns(resp.ext_volumns);
		string fileid(resp.fileid);

	StorageFileObject obj(fileid,volumns);
	return obj;
}

//RESPONSE_HEADER StorageFileObject2ResponseObject(const StorageFileObject& resp){
//
//}


const std::string StorageConfig::FDFS_STORAGE_CONF =
		"/usr/local/webserver/fdfs/etc/storage.conf";
const INIReader* StorageConfig::readerPtr=getReader();

const std::string StorageConfig::GROUP_ID = GET_GROUP_ID();

std::map<std::string, std::string> StorageConfig::VolumnsDict =
		GetVolumnsDict();

const int StorageConfig::MAX_VOLUMNS_COUNT = GetVolumnsCount();


INIReader*   StorageConfig::getReader(){
    INIReader* reader=new INIReader("/usr/local/webserver/fdfs/etc/storage.conf");

    if (reader->ParseError() < 0) {
        Logger::error("Can't load 'storage.conf'\n");
        return NULL;
    }
    return reader;
}

int StorageConfig::GetVolumnsCount() {
	return StorageConfig::VolumnsDict.size();
}
/**
 * return :
 *    key:value
 *      1:/data1
 *      2:/data2
 */
std::map<std::string,std::string> StorageConfig::GetVolumnsDict() {
	std::map<std::string,std::string> dataVolumnsDic;
	int volumnsid=0;
	while(true){
		const std::string find = "store_path";
		string volumnstr=find+fdfs2qq::to_string(volumnsid);
		string getfromreader=readerPtr->Get("",volumnstr,"");
		if(getfromreader.empty()) break;

		getfromreader+="/data";
		string volumnsid_tmp;
		if (volumnsid <= 9) {
			volumnsid_tmp= "M0" + fdfs2qq::to_string(volumnsid);
		}else{
			volumnsid_tmp= "M" + fdfs2qq::to_string(volumnsid);
		}

		dataVolumnsDic.insert(std::make_pair(volumnsid_tmp,getfromreader));
		volumnsid++;
	}
	return dataVolumnsDic;
}
/**
 * params:_ret ,return
 *   :volumnsid 0-100(string)
 *
 */
int StorageConfig::getStoreFolderByOrder(
		std::vector<StorageVolumnObject>& _ret,const string& volumnsid) {
	// /data2/data/FF/FF

//	string prefix_tmp=prefix;
	if(VolumnsDict.find(volumnsid)==VolumnsDict.end()){
		throw std::runtime_error("mapping error");
	}
//	if(prefix.empty()){
//		prefix_tmp=VolumnsDict[volumnsid]+"/data/";
//	}
	//build volumns

//	int volumns_id = atoi(volumnsid.c_str());
//	if ( volumns_id < 0 ) {
//		throw std::runtime_error("error assert volumns style");
//	}

	//build subdir
	//default is 256
	std::vector<std::string> _tmp_v_path1;
	std::string vlist[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
			"A", "B", "C", "D", "E", "F" };
	std::vector<std::string> v(vlist, vlist + 16);
	for (int ii = 0; ii < v.size(); ii++) {
		for (int jj = 0; jj < v.size(); jj++) {
			_tmp_v_path1.push_back(v[ii] + v[jj]);
		}
	}
	stringstream sm;
	for(std::vector<string>::iterator it=_tmp_v_path1.begin();it!=_tmp_v_path1.end();
			it++){
		for(std::vector<string>::iterator it2=_tmp_v_path1.begin();it2!=_tmp_v_path1.end();
					it2++){
		/* /data2/data/FF/FF  */
		StorageVolumnObject obj("");
		obj.grpid=GROUP_ID;
		stringstream sm;
		sm<<*it<<"/"<<*it2;
		obj.subdir=sm.str();
		obj.volumnid=volumnsid;
		obj.volumnstr=VolumnsDict[volumnsid];
		_ret.push_back(obj);
		}
	}

	return 1;
}


/**
 * @storagepath
 */
bool StorageConfig::GetfileListOfStoragepath(
		const string& storagepath,
		std::vector<string>& file_list) {
	//get conf
	std::vector<string>  PIC_EXT_LIST =
				{ "jpg", "jpeg", "bmp", "png","gif" };

	for(auto it=PIC_EXT_LIST.begin();it!=PIC_EXT_LIST.end();
			it++){
		auto item=*it;
//	for (string item:StorageConfig::PIC_EXT_LIST) {
		std::vector<string> file_list_tmp;
		file_list_tmp.clear();
		const std::string full_path = storagepath + "/*." + item;
		if (_GetFilelistByPath(full_path, file_list_tmp) ) {
			file_list.insert(file_list.end(), file_list_tmp.begin(), file_list_tmp.end());
		}
	}

	//build path
	if (file_list.empty()) {
		return false;
	}
	//push
	return true;

}

/**
 * @param dirPath ,is regex !!!
 */
bool StorageConfig::_GetFilelistByPath(const std::string& dirPath,
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
	std::string grpid=readerPtr->Get("","group_name","");
	if (grpid.empty()){
		Logger::error("null grpid from *readerPtr");
	}
	return grpid;
}

void StorageConfig::GetGlobalIdByFilelist(const vector<string>& \
			filelist,const fdfs2qq::StorageVolumnObject& ret ,vector<string>& gid_list){
	//for(auto filename:filelist){
	for(auto it=filelist.begin();it!=filelist.end();it++){
		auto filename(*it);
		stringstream sm;
		sm<<ret.grpid<<"/"<<ret.volumnid<<"/"<<ret.subdir<<"/"<<filename;
		gid_list.push_back(sm.str());
	}
}



StorageFileObject Strfileid2StorageFileObject(const string& fileid){
		StorageVolumnObject tmp_obj;
		char delim = '/';
		auto vec1 = fdfs2qq::split(fileid, delim);
		string volumnid;
		if (vec1.size()>=4) {
			tmp_obj.grpid = vec1[0];
			volumnid=vec1[1];
			tmp_obj.volumnid = volumnid;
			tmp_obj.subdir = vec1[2]+std::string(1,delim)+vec1[3] ;

			auto st_VolumnsDict=StorageConfig::GetVolumnsDict();
			if(st_VolumnsDict.find(volumnid)!=st_VolumnsDict.end()){
				tmp_obj.volumnstr =st_VolumnsDict[volumnid];
			}
		}else{
			throw std::runtime_error("filid not parse");
		}
		StorageFileObject obj(tmp_obj,fileid);

		return obj;

}





} //end namespace
