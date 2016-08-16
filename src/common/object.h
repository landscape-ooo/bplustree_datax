/*
 * object.h
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */

#ifndef SRC_FDFS2QQ_OBJECT_H_
#define SRC_FDFS2QQ_OBJECT_H_
#include "common_define.h"
#include "except.h"
namespace fdfs2qq{


struct Thread_Info {
	int thread_id;
	std::string thread_msg;
};

struct OBJECT_ID_4SUBDIR {

	OBJECT_ID_4SUBDIR();
	OBJECT_ID_4SUBDIR(const OBJECT_ID_4SUBDIR& obj);
	OBJECT_ID_4SUBDIR& operator=(OBJECT_ID_4SUBDIR const& other);
	~OBJECT_ID_4SUBDIR();
	/**
	 * gjfs16
	 * @see from conf
	 */
	std::string grpid;
	/*
	 * gjfs16/M02/26/79/
	 */
	std::string fdfs_store_path;
	/*
	 *   /data2
	 *   /data/fastdfs
	 *   @see from conf
	 **/
	std::string physical_store_path;

	/**
	 * int 1-100
	 * eg:M02 --->2
	 */
	std::string volumns_id;
	/**
	 * data/26/79
	 */
	std::string subdir_volumns;

	static std::string get_volumnsByObj(const std::string& value);

	static std::string get_Fdfs_store_pathByObj(const std::string&,
			const std::string&, const std::string&);

//	std::string to_string() const;
//	void _deserilize(const std::string& vstr);
};

struct OBJECT_ID_4FILE {
	bool IS_PROGRAME_THREAD_STOP; //just send by  produce thread ,to stop consumer

	~OBJECT_ID_4FILE();
	OBJECT_ID_4SUBDIR dir_info_ptr;

//	const OBJECT_ID_4SUBDIR* dir_info_ptr;
	/**
	 *gjfs16/M02/26/79/CgEHQlbFlTCA0I56AAC3eom7WoI19.jpeg
	 *
	 */
	std::string global_object_id;

	/**
	 * CgEHQlbFlTCA0I56AAC3eom7WoI19.jpg
	 */
	std::string filename;
	/**
	 * CgEHQlbFlTCA0I56AAC3eom7WoI19.jpg-m
	 */
	std::string metafilename;

	bool is_recovery_fromlog;

	static std::string getMetafilename(const OBJECT_ID_4FILE& obj);
	static std::string getGlobalId(const OBJECT_ID_4FILE& obj);

	std::string to_string() const;
//	void _deserilize(const std::string& str);

};


}




#endif /* SRC_FDFS2QQ_OBJECT_H_ */
