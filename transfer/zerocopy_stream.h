/*
 * iostream.h
 *
 *  Created on: Aug 16, 2016
 *      Author: a11
 */

#ifndef SRC_TRANSFER_ZEROCOPY_STREAM_H_
#define SRC_TRANSFER_ZEROCOPY_STREAM_H_
#include <sys/mman.h>
#include <sys/stat.h>
#include <fstream>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <error.h>
using namespace std;
namespace transfer { namespace fstream {

extern void MMAP_FILE_CLOSE(char * &mapped, unsigned long &st_size) ;
extern unsigned long Getfilesize(const string filename);
extern void MMAP_FILE(const std::string filename, char * &dataptr, const unsigned long& size);

extern void STREAM_READ(const std::string filename,string &);

}}




#endif /* SRC_TRANSFER_ZEROCOPY_STREAM_H_ */
