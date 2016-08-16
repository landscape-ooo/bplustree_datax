#include "zerocopy_stream.h"
//static
void transfer::fstream::MMAP_FILE_CLOSE(
		char * &mapped, unsigned long &st_size) {
	/* 释放存储映射区 */
	if ((munmap((void *) mapped, st_size)) == -1) {
		perror("munmap");
	}
}
//member funct
void transfer::fstream::STREAM_READ(
		const std::string filename,std::string &readbuffer) {
	struct stat buf;
	if (stat(filename.c_str(), &buf) != 1) { //check exist
		std::ifstream t;
		t.open(filename.c_str(),std::ios::in|std::ios::binary);
		t.seekg(0, std::ios::end);
		size_t size = t.tellg();
		readbuffer = std::string(size, ' ');
		t.seekg(0);
		t.read(&readbuffer[0], size); //read memory
	}
}

unsigned long transfer::fstream::Getfilesize(
		const std::string filename) {
		int fd;
		struct stat sb;
		/* 打开文件 */
		if ((fd = open(filename.c_str(), O_RDONLY)) < 0) {
			perror("open");
			return 0;
		}

		/* 获取文件的属性 */
		if ((fstat(fd, &sb)) == -1) {
			perror("fstat");
			return 0;
		}
		if(fd){
			close(fd);
		}
		return sb.st_size;
}
//static
void transfer::fstream::MMAP_FILE(
		const std::string filename, char * &dataptr, const unsigned long& size) {
	int fd;
	/* 打开文件 */
	if ((fd = open(filename.c_str(), O_RDONLY)) < 0) {
		perror("open");
	}

	/* 将文件映射至进程的地址空间 */
	if ((dataptr = (char *) mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0))
			== (void *) -1) {
		perror("mmap");
	}

	/* 映射完后, 关闭文件也可以操纵内存 */
	close (fd);

//	    /* 释放存储映射区 */
//	    if ((munmap((void *)mapped, sb.st_size)) == -1) {
//	        perror("munmap");
//	    }
}
