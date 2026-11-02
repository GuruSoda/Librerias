#if !defined _GURUSHA1_HPP__
	#define _GURUSHA1_HPP__

#include <stdio.h>
#include <string>
#include <string.h>

using namespace std;

#include <polarssl/sha1.h>

class GuruSHA1 {
public:
	GuruSHA1() {};
	virtual ~GuruSHA1() {};

	static std::string sha1(const char *buffer) {
		return sha1((const unsigned char*)buffer, strlen(buffer));
	}

	static std::string sha1(const unsigned char* buffer, size_t tamanio_buffer) {
		sha1_context sha1ctx;
		unsigned char checksum[42]; // en realidad son 40 bytes

	//	inicializo la estructura
		sha1_starts(&sha1ctx);

	//	hago el checksum ..
		sha1_update(&sha1ctx, const_cast<unsigned char*>(buffer), tamanio_buffer);

	//  finalizo el checksum
		sha1_finish(&sha1ctx, checksum);

		return checksum_a_string(checksum);
	}

	static std::string archivo(const std::string& nombre_archivo) {
		unsigned char checksum[42]; // en realidad son 40 bytes

		sha1_file((char*)nombre_archivo.c_str(), checksum);

		return checksum_a_string(checksum);
	}

	static string checksum_a_string(unsigned char*checksum) {
		static const char* binToStr = "0123456789abcdef";
		char		cadena_sha1[45] = {0};
		unsigned char*	bin_chksum = checksum;
		char*		inicio = &cadena_sha1[0];

 		for (int i=0;i<20;i++) {
 			unsigned char c = *bin_chksum++;

 			*inicio++ = binToStr[(c >> 4) & 0xF];
 			*inicio++ = binToStr[(c >> 0) & 0xF];
 		}

		return string(cadena_sha1);
	}

};

#endif // !defined _GURUSHA1_H__
