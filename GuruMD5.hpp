#if !defined __GURUMD5_HPP__
	#define __GURUMD5_HPP__

#include <stdio.h>
#include <string>
#include <string.h>

using namespace std;

#include <polarssl/md5.h>

class GuruMD5 {
public:
	GuruMD5() {};
	virtual ~GuruMD5() {};

	static std::string md5(const char *buffer) {
		return md5((unsigned char*)buffer, strlen(buffer));
	}

	static std::string md5(const unsigned char* buffer, size_t tamanio_buffer) {
		md5_context md5ctx;
		unsigned char checksum[35]; // en realidad son 32 bytes

	//	inicializo la estructura
		md5_starts(&md5ctx);

	//	hago el checksum ..
		md5_update(&md5ctx, const_cast<unsigned char*>(buffer), tamanio_buffer);

	//  finalizo el checksum
		md5_finish(&md5ctx, checksum);

		return checksum_a_string(checksum);
	}

	static std::string archivo(const std::string& nombre_archivo) {
		unsigned char checksum[35] = {0}; // en realidad son 32 bytes

		md5_file((char*)nombre_archivo.c_str(), checksum);

		return checksum_a_string(checksum);
	}

	static string checksum_a_string(unsigned char*checksum) {
		static const char* binToStr = "0123456789abcdef";
		char		cadena_sha1[35] = {0};
		unsigned char*	bin_chksum = checksum;
		char*		inicio = &cadena_sha1[0];

 		for (int i=0;i<16;i++) {
 			unsigned char c = *bin_chksum++;

 			*inicio++ = binToStr[(c >> 4) & 0xF];
 			*inicio++ = binToStr[(c >> 0) & 0xF];
 		}

		return string(cadena_sha1);
	}

};

#endif // !defined _GURUSHA1_H__
