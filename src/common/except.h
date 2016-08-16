/*
 * except.h
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */

#ifndef SRC_FDFS2QQ_EXCEPT_H_
#define SRC_FDFS2QQ_EXCEPT_H_
namespace fdfs2qq {
enum QqReturnFlag {
	success_not_return = 0,
	success_ok = 1,
	error_runtime_qq = -1,
	error_runtime_invalide = -2,
	error_qq_response = -3,
	error_qq_md5sum_error = -4,
	error_sender_duplicate = -5

};

class SendQqErrorResult {
public:
	SendQqErrorResult() :
			retflag(fdfs2qq::success_ok), qq_retcode(0) {

	}
	/**
	 * -100 unknown except
	 * －5 duplicate sender ,not ovverwrite
	 * -4 md5sum error
	 * -3 qq response error , more detail in retcode
	 *
	 * -1  runtime_invalide_error
	 * -2  runtime_qq_error
	 * 0   success no ret
	 * 1   success
	 *
	 */
	int retflag;
	int qq_retcode;
	std::string error_reason;

	friend std::ostream& operator <<(std::ostream& os,
			const SendQqErrorResult& q) {

		std::string str;
		char buffer[150];
		int n;
		//json
		n = sprintf(buffer, "{\"retflag\":%d,", q.retflag);
		str.append(std::string(buffer, n));
		n = sprintf(buffer, "\"qq_retcode\":%d,", q.qq_retcode);
		str.append(std::string(buffer, n));
		n = sprintf(buffer, "\"error_reason\":\"%s\"}",
				q.error_reason.empty() ? "none" : q.error_reason.c_str());
		str.append(std::string(buffer, n));

		os << str;
		return os;
	}

};

class runtime_invalide_error: public std::runtime_error {
public:
	runtime_invalide_error(const std::string& msg) :
			std::runtime_error(msg) {

	}
	runtime_invalide_error(const char* msg) :
			std::runtime_error(msg) {

	}
};

class runtime_qq_error: public std::runtime_error {
public:
	runtime_qq_error(const std::string& msg) :
			std::runtime_error(msg) {

	}
	runtime_qq_error(const char* msg) :
			std::runtime_error(msg) {

	}
};

}
#endif /* SRC_FDFS2QQ_EXCEPT_H_ */
