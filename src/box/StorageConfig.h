/*
 * appconfig.h
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */

#ifndef SRC_FDFS_APPCONFIG_H_
#define SRC_FDFS_APPCONFIG_H_
#include "common/common_define.h"
#include "common/logger.h"
#include "common/INIReader.h"
#include "common/tools.h"
#include "common/mq.h"
using namespace std;
namespace fdfs2qq {
struct EventClient {
  int fd;
  struct bufferevent *buf_ev;
};
const int FILEID_MAXLEN=128;
struct RESPONSE_HEADER{
	char fileid[FILEID_MAXLEN];//....
	char ext_volumns[FILEID_MAXLEN];//   /data2/data
	char ext_status[3];//404 ? 307?
};

enum RESPONSE_STATUS{

	NEWONE=200,
	CACHE=307,
	EXIST=304,
	NOTFOUND=404
};

enum CMD{
	LSM=1,//no response ,save it
	REGIST=2,//check and response
};


struct StorageVolumnObject{
	string grpid; //gjfs11
	string volumnid; // M02
	string volumnstr; // /data2/data
	string subdir;// 26/79/
	StorageVolumnObject(const StorageVolumnObject& oth){
		this->grpid = oth.grpid;
		this->volumnid = oth.volumnid;
		this->subdir = oth.subdir;
		this->volumnstr = oth.volumnstr;
	}
	StorageVolumnObject(const string& msgstr, const string volumnstr = "") {
		/*gjfstmp2/M00/00/19/CgP_ylcD2nqAAVr6AADKanPqNRs485.jpg*/
		auto vec1 = fdfs2qq::split(msgstr, '/');
		this->grpid = vec1[0];
		this->volumnid = vec1[1];
		this->subdir = vec1[2] + "/" + vec1[3];
		if (!volumnstr.empty()) {
			this->volumnstr = volumnstr;
		}
	}
	StorageVolumnObject(){

	}
	StorageVolumnObject& operator=(
			StorageVolumnObject const& other) {

		if (this != &other) {
			grpid = other.grpid;
			volumnid = other.volumnid;
			volumnstr = other.volumnstr;
			subdir = other.subdir;
		}
		return *this;
	}

};


struct StorageFileObject{
	StorageFileObject(const StorageVolumnObject& obj_parent,string fileid )\
			:grpid(obj_parent.grpid),volumnid(obj_parent.volumnid),\
			 volumnstr(obj_parent.volumnstr),subdir(obj_parent.subdir){
		this->global_fileid=obj_parent.grpid+"/"+obj_parent.volumnid+"/"+obj_parent.subdir;
	}
	//copy
	StorageFileObject(const StorageFileObject& oth){
		this->global_fileid=oth.global_fileid;
		this->grpid=oth.grpid;
		this->volumnid=oth.volumnid;
		this->volumnstr=oth.volumnstr;
		this->subdir=oth.subdir;
	}
	string global_fileid; //gjfs16/M02/26/79/CgEHQlbFlTCA0I56AAC3eom7WoI19.jpeg


	string physical_store_path ;
	///data2/data/26/79/CgEHQlbFlTCA0I56AAC3eom7WoI19.jpeg
	string grpid; //gjfs11
	string volumnid; // M02
	string volumnstr; // /data2/data
	string subdir;// 26/79/
	StorageFileObject(){

	}
	StorageFileObject(const string& msgstr, const string volumnstr = "") {
		/*gjfstmp2/M00/00/19/CgP_ylcD2nqAAVr6AADKanPqNRs485.jpg*/
		auto vec1 = fdfs2qq::split(msgstr, '/');
		if (vec1.size() == 5) {
			this->global_fileid = msgstr;
			this->grpid = vec1[0];
			this->volumnid = vec1[1];
			this->subdir = vec1[2] + "/" + vec1[3];
			if (!volumnstr.empty()) {
				this->volumnstr = volumnstr;
				stringstream sm;
				sm << this->volumnstr << "/" << this->subdir << "/" << vec1[4];
				this->physical_store_path = sm.str();
			}
		}
	}
	StorageFileObject& operator = (StorageFileObject const& other) {

			if (this != &other) {
				global_fileid = other.global_fileid;
				grpid = other.grpid;
				volumnid = other.volumnid;
				subdir = other.subdir;
				volumnstr = other.volumnstr;
			}
			return *this;
		}
private:
	StorageFileObject(const StorageVolumnObject& obj_parent);
};

struct msgInfo{
	int clientId;
	string fileId;
	string fileId_hash;
	StorageFileObject FileObject ;
};


RESPONSE_HEADER StringToResponseObject(const string resp);
string ResponseObjectToString(const RESPONSE_HEADER& obj);
StorageFileObject ResponseObject2StorageFileObject(const RESPONSE_HEADER& resp);
RESPONSE_HEADER StorageFileObject2ResponseObject(const StorageFileObject& resp);




class StorageConfig {
public:

//	static std::vector<string>  PIC_EXT_LIST ;

	static	const std::string FDFS_STORAGE_CONF;
	static	const INIReader* readerPtr;

	static	const std::string GROUP_ID ;

	static	std::map<std::string, std::string> VolumnsDict;

	static const int MAX_VOLUMNS_COUNT;


	static INIReader*   getReader();

	static	int GetVolumnsCount() ;
	static std::string GET_GROUP_ID();
	/**
	 * return :
	 *    key:value
	 *      1:/data1
	 *      2:/data2
	 */
	static std::map<std::string,std::string> GetVolumnsDict();
	/**
	 * params:_ret ,return
	 *   :volumnsid 0-100(string)
	 *
	 */
	static int getStoreFolderByOrder(
			std::vector<StorageVolumnObject>& _ret,const string& volumnsid) ;



	/**
	 * @storagepath
	 */
	static bool GetfileListOfStoragepath(
			const string& storagepath,
			std::vector<string>& file_list);



	static void GetGlobalIdByFilelist(const vector<string>& filelist,\
			const fdfs2qq::StorageVolumnObject& ret,vector<string>& gid_list );

private:
	/**
	 * @param dirPath ,is regex !!!
	 */
	static bool _GetFilelistByPath(const std::string& dirPath,
			std::vector<std::string>& files) ;

};

extern fdfs2qq::concurrent_queue<StorageFileObject> G_ItemProduce_Mq;
extern ::pthread_cond_t G_Condition_variable;


}
#endif /* SRC_FDFS_APPCONFIG_H_ */
