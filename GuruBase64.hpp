#if !defined __GURUBASE64_HPP__
#define __GURUBASE64_HPP__

#include <string>

#include <polarssl/base64.h>

#include <GuruBuffer.hpp>

namespace GuruBase64 {

	string encode (const std::string& str) {
		GuruBufferChar buf(str.length()*2);
		size_t cantidad=static_cast<size_t>(buf.cantidad_buf());
		std::string tmp;

		if (base64_encode( (unsigned char*)buf.buf(), &cantidad, (unsigned char*)str.c_str(), str.length()) == 0) {

			buf.asignar(static_cast<char>('\0'), cantidad);
			tmp =(char*)buf.buf();
		}

		return tmp;
	}

	string decode (const std::string& str) {
		
		GuruBufferChar buf(str.length()*2);
		size_t cantidad=static_cast<size_t>(buf.cantidad_buf());
		std::string tmp;

		if (base64_decode( (unsigned char*)buf.buf(), &cantidad, (unsigned char*)str.c_str(), str.length()) == 0) {

			buf.asignar(static_cast<char>('\0'), cantidad);
			tmp =(char*)buf.buf();
		}

		return tmp;

	}

}

#endif
