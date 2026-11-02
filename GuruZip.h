#ifndef __GURUZIP_H__
#define __GURUZIP_H__

#include <zlib/zip.h>
#include <GuruBuffer.hpp>
#include <GuruError.hpp>

#include <map>
#include <string>
#include <fstream>

#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;

namespace fs = boost::filesystem;

class GuruZip {
public:
	GuruZip(const string& nombre):nombre_archivo(nombre){};
	virtual ~GuruZip() {};

	void agregar_archivo(const string& nombre_en_zip, const string& nombre_en_disco);

	void grabar();

private:
	string 	nombre_archivo;
	zipFile	archivo_zip;

	map<string, string> archivos;
};

#endif
