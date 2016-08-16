/*
 * appconfig.h
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */

#ifndef SRC_FDFS_APPCONFIG_H_
#define SRC_FDFS_APPCONFIG_H_
namespace fdfs2qq {
class StorageConfig {
public:
	static std::string FDFS_STORAGE_CONF_CONTENT;
	static const std::string FDFS_STORAGE_CONF;
	static const int MAX_VOLUMNS_COUNT;
	static const std::string GROUP_ID;

private:
	static std::string GET_GROUP_ID();
	static std::map<std::string, std::string> GetVolumnsList();
	static int GetVolumnsCount();
	static const std::string PIC_EXT_LIST[];
	static const std::string getContentByConfpath();
	static std::map<std::string, std::string> VolumnsList;

};

}
#endif /* SRC_FDFS_APPCONFIG_H_ */
