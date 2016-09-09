/*
 * run.test.cc
 *
 *  Created on: Aug 30, 2016
 *      Author: a11
 */
extern "C" {

#define FDFS_LOGIC_FILE_PATH_LEN	10
#define FDFS_FILENAME_BASE64_LENGTH     27
#include "../base64.h"
}
#include <iostream>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "sys/resource.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <netinet/in.h>

//#define ntohl(x)	__DARWIN_OSSwapInt32(x)
#define FDFS_LOGIC_FILE_PATH_LEN      10
#define FDFS_FILE_EXT_NAME_MAX_LEN       6
using namespace std;

int buff2int(const char *buff) {
	return (((unsigned char) (*buff)) << 24)
			| (((unsigned char) (*(buff + 1))) << 16)
			| (((unsigned char) (*(buff + 2))) << 8)
			| ((unsigned char) (*(buff + 3)));
}

int64_t buff2long(const char *buff) {
	unsigned char *p;
	p = (unsigned char *) buff;
	return (((int64_t) (*p)) << 56) | (((int64_t) (*(p + 1))) << 48)
			| (((int64_t) (*(p + 2))) << 40) | (((int64_t) (*(p + 3))) << 32)
			| (((int64_t) (*(p + 4))) << 24) | (((int64_t) (*(p + 5))) << 16)
			| (((int64_t) (*(p + 6))) << 8) | ((int64_t) (*(p + 7)));
}

int main() {
	base64_context g_base64_context;
	string filename = "M09/C3/7D/CgEHIVZ7XGGdcABuAAPhnsEGnbc149.jpg";

	char name_buff[64];
	memset(name_buff, 0, sizeof(name_buff));

	int decoded_len;
	const char* rt_filename = filename.c_str();

	base64_decode_auto(&g_base64_context, rt_filename + FDFS_LOGIC_FILE_PATH_LEN,
			filename.length() - FDFS_LOGIC_FILE_PATH_LEN
					- (FDFS_FILE_EXT_NAME_MAX_LEN + 1), name_buff, &decoded_len);

//	base64_decode_auto(&g_base64_context, (char *) filename.c_str() +
//	FDFS_LOGIC_FILE_PATH_LEN, FDFS_FILENAME_BASE64_LENGTH, name_buff,
//			&decoded_len);
	int storage_ip = 0;
	int file_timestamp = 0;
	int64_t file_size = 0;

	storage_ip = ntohl(buff2int(name_buff));
	file_timestamp = buff2int(name_buff + sizeof(int));
	file_size = buff2long(name_buff + sizeof(int) * 2);

}

