#ifndef __GURUHASHFUNCIONES_HPP__
#define __GURUHASHFUNCIONES_HPP__

#include <GuruSHA1.hpp>
#include <GuruSHA4.hpp>
#include <GuruMD5.hpp>

class GuruHashFunciones: public GuruInterprete::GuruFunciones<GuruHashFunciones> {
public:
	GuruHashFunciones () {
		cargar_funcion(this, &GuruHashFunciones::crc_archivo, "hash.md5.archivo");
		cargar_funcion(this, &GuruHashFunciones::crc_buffer, "hash.md5.string");

		cargar_funcion(this, &GuruHashFunciones::md5_archivo, "hash.md5.archivo");
		cargar_funcion(this, &GuruHashFunciones::md5_buffer, "hash.md5.string");

		cargar_funcion(this, &GuruHashFunciones::sha1_archivo, "hash.sha1.archivo");
		cargar_funcion(this, &GuruHashFunciones::sha1_buffer, "hash.sha1.string");

		cargar_funcion(this, &GuruHashFunciones::sha384_archivo, "hash.sha512.archivo");
		cargar_funcion(this, &GuruHashFunciones::sha384_buffer, "hash.sha512.string");

		cargar_funcion(this, &GuruHashFunciones::sha512_archivo, "hash.sha512.archivo");
		cargar_funcion(this, &GuruHashFunciones::sha512_buffer, "hash.sha512.string");
	};

	virtual ~GuruHashFunciones() {
	};

	std::string crc_archivo(const std::vector<string>& param) {
		return string();
	}

	std::string crc_buffer(const std::vector<string>& param) {
		return string();
	}

	std::string md5_archivo(const std::vector<string>& param) {
		return GuruMD5::archivo(param.at(0));
	}

	std::string md5_buffer(const std::vector<string>& param) {
		return GuruMD5::md5(param.at(0).c_str());
	}

	std::string sha1_archivo(const std::vector<string>& param) {
		return GuruSHA1::archivo(param.at(0));
	}

	std::string sha1_buffer(const std::vector<string>& param) {
		return GuruSHA1::sha1((const unsigned char*)param.at(0).c_str(), static_cast<uint32_t>(param.at(0).length()));
	}

	std::string sha384_archivo(const std::vector<string>& param) {
		return GuruSHA4::archivo_sha384(param.at(0));
	}

	std::string sha384_buffer(const std::vector<string>& param) {
		return GuruSHA4::sha384((unsigned char*)param.at(0).c_str(), static_cast<uint32_t>(param.at(0).length()));
	}

	std::string sha512_archivo(const std::vector<string>& param) {
		return GuruSHA4::archivo_sha512(param.at(0));
	}

	std::string sha512_buffer(const std::vector<string>& param) {
		return GuruSHA4::sha512((unsigned char*)param.at(0).c_str(), static_cast<uint32_t>(param.at(0).length()));
	}
};
#endif

