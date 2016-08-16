/*
 * tools.cc
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */
#include "tools.h"

using namespace std;
std::vector<std::string> fdfs2qq::split(const std::string& text, char sep) {
	std::vector<std::string> tokens;
	std::size_t start = 0, end = 0;
	while ((end = text.find(sep, start)) != std::string::npos) {
		std::string temp = text.substr(start, end - start);
		if (temp != "")
			tokens.push_back(temp);
		start = end + 1;
	}
	std::string temp = text.substr(start);
	if (temp != "")
		tokens.push_back(temp);
	return tokens;
}
std::string fdfs2qq::implode(char delim, const std::vector<std::string>& strings) {
	std::string full;
	std::string _d;
	_d.push_back(delim);
	for (int i = 0; i < strings.size(); i++) {
		full += (strings[i]);
		if (i != (strings.size() - 1)) {
			full += _d;
		}
	}
	return full;
}

std::string fdfs2qq::_bast64_encode(const std::string& s) {
	unsigned char const* bytes_to_encode;
	unsigned int in_len;

	const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";

	bytes_to_encode = (unsigned char*) &s.c_str()[0];
	in_len = s.size();

	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4)
					+ ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2)
					+ ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4)
				+ ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2)
				+ ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';

	}

	return ret;

}
std::string fdfs2qq::_bast64_decode(const std::string & in) {
	std::string out;

	std::vector<int> T(256, -1);
	for (int i = 0; i < 64; i++)
		T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] =
				i;

	int val = 0, valb = -8;
	char * p = new char[in.length() + 1];
	std::strcpy(p, in.c_str());

	for (int i = 0; i < in.length(); i++) {
		char c = *p;
		if (T[c] == -1)
			break;
		val = (val << 6) + T[c];
		valb += 6;
		if (valb >= 0) {
			out.push_back(char((val >> valb) & 0xFF));
			valb -= 8;
		}
		p++;
	}
	return out;
}

bool fdfs2qq::init_dir(const std::string& logpath_full) {
	//test dir exist ?
		struct stat sb;
		if (!(stat(logpath_full.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))) {
			//not a dir
			std::string s = logpath_full;
			mode_t mode = 0775;
			int mdret;

			size_t pre = 0, pos;
			std::string dir;

			if (s[s.size() - 1] != '/') {
				s += '/';
			}
			while ((pos = s.find_first_of('/', pre)) != std::string::npos) {
				dir = s.substr(0, pos++);
				pre = pos;
				if (dir.size() == 0)
					continue; // if leading / first time is 0 length
				if ((mdret = mkdir(dir.c_str(), mode)) && errno != EEXIST) {
					return true;
				}
			}
			return true;
		}
		return true;
}
int fdfs2qq::read_file(const std::string &filename, std::string &pic_data) {

	std::ifstream in;
	in.open(filename.c_str());
	if(!in.is_open()){
		throw std::runtime_error("code:002,"+ filename + " is empty");
		return -1;
	}
	std::stringstream buffer;
	buffer << in.rdbuf();

	pic_data.assign(buffer.str());
	in.close();
	return 0;
}

int Handle_error(const char* msg) {
		fdfs2qq::Logger::error(msg);
		return -1;
	}
uintmax_t Readhugefile(const char* fname,
				std::vector<string>& vecRef) {

			uintmax_t m_numLines = 0;
			size_t length;
			int fd = open(fname, O_RDONLY);
			if (fd == -1){
				fdfs2qq::Logger::error(std::string(fname)+",could not open");
				return m_numLines;
			}

			// obtain file size
			struct stat sb;
			if (fstat(fd, &sb) == -1){
				fdfs2qq::Logger::error(std::string(fname)+",could not fstat");
				return m_numLines;
			}

			length = sb.st_size;

			const char* addr = static_cast<const char*>(mmap(NULL, length,
					PROT_READ,
					MAP_PRIVATE, fd, 0u));
			if (addr == MAP_FAILED){
				fdfs2qq::Logger::error(std::string(fname)+",could not mmap");
				return m_numLines;
			}

			auto f = addr;
			auto l = f + length;

			char buff[fdfs2qq::MAX_BUFFER_SIZE];
			while (f && f != l) {
				char* ori = (char*) f;
				if ((f = static_cast<const char*>(memchr(f, '\n', l - f)))) {
					m_numLines++, f++;
					std::string p(ori, f - ori - 1);
					vecRef.push_back(p);
				}
			}
			return m_numLines;
}


uint32_t fdfs2qq::Gethash(const std::string& str_ctr){
	const char *s = (const char *) str_ctr.c_str();
	uint32_t hash = 0;

	for (; *s; ++s) {
		hash += *s;
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}
std::string fdfs2qq::GetMd5sum(const char* file_blob_ptr,const std::size_t s_file_blob){
		unsigned char digest[MD5_DIGEST_LENGTH];


	    MD5(reinterpret_cast<unsigned char*>(const_cast<char*>(file_blob_ptr)), s_file_blob, (unsigned char*)&digest);


	    char mdString[33];

	    for(int i = 0; i < 16; i++)
	            sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

	    return std::string(&mdString[0],strlen(mdString));
}
std::string fdfs2qq::GetMd5sum(const std::string & file_blob){
	std::size_t s=file_blob.length();
	return GetMd5sum(file_blob.c_str(),s);
}

std::string fdfs2qq::String_Format(const char* fmt, ...){
    int size = 512;
    char* buffer = 0;
    buffer = new char[size];
    va_list vl;
    va_start(vl, fmt);
    int nsize = vsnprintf(buffer, size, fmt, vl);
    if(size<=nsize){ //fail delete buffer and try again
        delete[] buffer;
        buffer = 0;
        buffer = new char[nsize+1]; //+1 for /0
        nsize = vsnprintf(buffer, size, fmt, vl);
    }
    std::string ret(buffer);
    va_end(vl);
    delete[] buffer;
    return ret;
}


