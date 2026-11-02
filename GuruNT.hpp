#ifndef __GURUNT_HPP__
#define __GURUNT_HPP__

#include <string>

namespace GuruNT {
	class GuruNTException : public std::exception {
	public:
		GuruNTException(const std::string& msg) :msg(msg) {}
		virtual ~GuruNTException() throw() {};

		virtual const char* what() const throw() { return msg.c_str(); };
	private:
		std::string msg;
	};

};

#endif
