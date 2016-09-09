/*
 * Fdfs2qqHelper.h
 *
 *  Created on: Aug 4, 2016
 *      Author: a11
 */

#ifndef JOBSCHEDULE_FDFS2QQHELPER_H_
#define JOBSCHEDULE_FDFS2QQHELPER_H_
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <cstring>

// for mmap:
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "fdfs/FdfsConf.h"
namespace fdfs {
using namespace std;
class Fdfs2qqHelper {
private:
//	string logprefix;
//	string grpid;
//	int disk_count;
//	static map<string, std::vector<std::string> > STORE_PREFIX_LIST;
public:

	static void* GetStoreprefix(const string logprefix, const string grpid,
			const int disk_count,map<string, std::vector<std::string> > &STORE_PREFIX_LIST) {
		std::vector<std::string> _tmp_v_path1;
		std::string vlist[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8",
				"9", "A", "B", "C", "D", "E", "F" };
		std::vector<std::string> v(vlist, vlist + 16);
		for (int ii = 0; ii < v.size(); ii++) {
			for (int jj = 0; jj < v.size(); jj++) {
				_tmp_v_path1.push_back(v[ii] + v[jj]);
			}
		}
		///data/log/fdfs2qq/gjfs01_M01_FF_FF_finished_success.log
		std::string formater = "%s_%s_%s_finished_%s.log";

		std::string key_formater = "%s/%s_M0%s";
		//build split  key
		for (int k = 0; k < disk_count; k++) {
			char key_buffer[50];
			string kstr = k >= 10 ? "0" + std::to_string(k) : std::to_string(k);
			int nkey_len = sprintf(key_buffer, key_formater.c_str(),
					logprefix.c_str(), grpid.c_str(), kstr.c_str());
			std::string key(key_buffer, nkey_len);
			if (key.empty())
				continue;

			std::vector<std::string> list_v;
			for (int i = 0; i < _tmp_v_path1.size(); i++) {
				for (int jj = 0; jj < _tmp_v_path1.size(); jj++) {
					char buffer[500];
					int n_len = sprintf(buffer, formater.c_str(), key.c_str(),
							_tmp_v_path1[i].c_str(), _tmp_v_path1[jj].c_str(),
							"error");
					std::string stos_error(buffer, n_len);
					string stos_success(stos_error) ;
					size_t f = stos_error.find("error");
					if(f!=string::npos){
						stos_success.replace(f,
							std::string("error").length(), "success");
					}else{
						stos_success.clear();
					}
					if (stos_error.empty() || stos_success.empty())
						continue;
					list_v.push_back(stos_error);
					list_v.push_back(stos_success);
				}
			}
			STORE_PREFIX_LIST.insert(std::make_pair(key, list_v));
		}

		return NULL;
	}
//	static void get_STORE_PREFIX_LIST(
//			)  {
//		s.insert(STORE_PREFIX_LIST.begin(),STORE_PREFIX_LIST.end());
//	}

	static int Handle_error(const char* msg) {
		fdfs::Logger::error(msg);
		return -1;
	}
	static uintmax_t Readhugefile(const char* fname,
				std::vector<string>& vecRef) {

			uintmax_t m_numLines = 0;
			size_t length;
			int fd = open(fname, O_RDONLY);
			if (fd == -1){
				fdfs::Logger::error(std::string(fname)+",could not open");
				return m_numLines;
			}

			// obtain file size
			struct stat sb;
			if (fstat(fd, &sb) == -1){
				fdfs::Logger::error(std::string(fname)+",could not fstat");
				return m_numLines;
			}

			length = sb.st_size;

			const char* addr = static_cast<const char*>(mmap(NULL, length,
					PROT_READ,
					MAP_PRIVATE, fd, 0u));
			if (addr == MAP_FAILED){
				fdfs::Logger::error(std::string(fname)+",could not mmap");
				return m_numLines;
			}

			auto f = addr;
			auto l = f + length;

			char buff[fdfs::MAX_BUFFER_SIZE];
			while (f && f != l) {
				char* ori = (char*) f;
				if ((f = static_cast<const char*>(memchr(f, '\n', l - f)))) {
					m_numLines++, f++;
					std::string p(ori, f - ori - 1);
					vecRef.push_back(p);
				}
			}


//			std::cout<<fname<<"vecotr size"<<vecRef.size()<<"\n";

			return m_numLines;
		}

	static uintmax_t Readhugefile(const char* fname,
			fdfs::concurrent_queue<string>* const queuePtr) {
		size_t length;
		int fd = open(fname, O_RDONLY);
		if (fd == -1)
			Handle_error("open");

		// obtain file size
		struct stat sb;
		if (fstat(fd, &sb) == -1)
			Handle_error("fstat");

		length = sb.st_size;

		const char* addr = static_cast<const char*>(mmap(NULL, length,
				PROT_READ,
				MAP_PRIVATE, fd, 0u));
		if (addr == MAP_FAILED)
			Handle_error("mmap");

		auto f = addr;
		auto l = f + length;

		uintmax_t m_numLines = 0;
		char buff[fdfs::MAX_BUFFER_SIZE];
		while (f && f != l) {
			char* ori = (char*) f;
			if ((f = static_cast<const char*>(memchr(f, '\n', l - f)))) {
				m_numLines++, f++;
				std::string p(ori, f - ori - 1);
				queuePtr->push(p);
				//std::cout << "^^^" << p << "^^^" << std::endl;
			}
		}

		return m_numLines;
	}
};
}
;

#endif /* JOBSCHEDULE_FDFS2QQHELPER_H_ */
