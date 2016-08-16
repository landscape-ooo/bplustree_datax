/*
 * tools.h
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */

#ifndef SRC_FDFS2QQ_TOOLS_H_
#define SRC_FDFS2QQ_TOOLS_H_
#include "common_define.h"
#include "logger.h"
namespace fdfs2qq {

std::vector<std::string> split(const std::string&, const char);
std::string implode(char, const std::vector<std::string>&);
std::string _bast64_decode(const std::string & in);
std::string _bast64_encode(const std::string& s);
bool init_dir(const std::string&);
int read_file(const std::string &, std::string &);


int Handle_error(const char* msg) ;
uintmax_t Readhugefile(const char* fname,std::vector<std::string>& vecRef) ;

/**
 * @see http://stackoverflow.com/questions/114085/fast-string-hashing-algorithm-with-low-collision-rates-with-32-bit-integer
 */
uint32_t Gethash(const std::string&);

std::string GetMd5sum(const std::string & file_blob);

std::string GetMd5sum(const char* file_blob_ptr, const std::size_t s_file_blob);

std::string String_Format(const char* fmt, ...);



}

#endif /* SRC_FDFS2QQ_TOOLS_H_ */
