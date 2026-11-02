#ifndef __GURUSHA4_HPP__
#define __GURUSHA4_HPP__

#include <string>

#include <polarssl/sha4.h>

class GuruSHA4
{
public:
	GuruSHA4(void) {};
	virtual ~GuruSHA4(void) {};

	static std::string sha384(unsigned char* buffer, size_t tamanio_buffer) {
		unsigned char checksum[52]; // en realidad son 50
		sha4_context contexto;

		sha4_starts(&contexto, 1);
		sha4_update(&contexto, buffer, tamanio_buffer);
		sha4_finish(&contexto, checksum);

		return checksum384_a_string(checksum);
	}

	static std::string sha512(unsigned char* buffer, size_t tamanio_buffer) {
		unsigned char checksum[66]; // en realidad son 64
		sha4_context contexto;

		sha4_starts(&contexto, 0);
		sha4_update(&contexto, buffer, tamanio_buffer);
		sha4_finish(&contexto, checksum);

		return checksum512_a_string(checksum);
	}

	static std::string archivo_sha384(const std::string& nombre_archivo) {
		unsigned char checksum[52]; // en realidad son 50

		if (sha4_file((char*)nombre_archivo.c_str(), checksum, 1) == 0)
			return checksum384_a_string(checksum);
		else
			return std::string("0");
	}

	static std::string archivo_sha512(const std::string& nombre_archivo) {
		unsigned char checksum[66]; // en realidad son 64

		if (sha4_file((char*)nombre_archivo.c_str(), checksum, 0) == 0)
			return checksum512_a_string(checksum);
		else
			return std::string("0");
	}

private:
	static std::string checksum384_a_string(unsigned char* checksum) {
		static const char*	binToStr = "0123456789abcdef";
		char			cadena_sha[98] = {0}; // son 96
		unsigned char*		bin_chksum = checksum;
		char*			inicio = &cadena_sha[0];

 		for (int i=0;i<48;i++) {
 			unsigned char c = *bin_chksum++;

 			*inicio++ = binToStr[(c >> 4) & 0xF];
 			*inicio++ = binToStr[(c >> 0) & 0xF];
 		}

		return std::string(cadena_sha);
	}

	static std::string checksum512_a_string(unsigned char* checksum) {
		static const char*	binToStr = "0123456789abcdef";
		char			cadena_sha[130] = {0}; // son 128
		unsigned char*		bin_chksum = checksum;
		char*			inicio = &cadena_sha[0];

 		for (int i=0;i<64;i++) {
 			unsigned char c = *bin_chksum++;

 			*inicio++ = binToStr[(c >> 4) & 0xF];
 			*inicio++ = binToStr[(c >> 0) & 0xF];
 		}

		return std::string(cadena_sha);
	}
};

#endif

