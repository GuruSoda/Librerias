#ifndef __GURUUNZIP_H__
#define __GURUUNZIP_H__

#include <GuruCompat.h>

#include <list>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <time.h>

#include <GuruBuffer.hpp>
#include <GuruError.hpp>
#include <GuruDirectorio.hpp>

#include <zlib/unzip.h>

#if defined __GURUWIN32__
#include <sys\utime.h>
#elif defined __GURULINUX__
#include <utime.h>
#endif

using namespace std;

class GuruUnzip
{
public:
	GuruUnzip(const string& archivo):nombre_archivo(archivo),archivo_zip(NULL) {};

	virtual ~GuruUnzip(void);

	void extraerTodo(const string& dir_destino);
	void extraer(const string& archivo, const string& dir_destino);
	GuruBuffer& extraer(const string& archivo, GuruBuffer& buffer);
	void contenido(map<string, unz_file_info>& info_archivos);

	class iterator;

	iterator begin() { return iterator(*this);};
	iterator end() {return iterator(*this, true);};

	class iterator {
	public:
		iterator (GuruUnzip& Archivos):_zip(Archivos),itfinal(false),finzip(false) {
			_zip.abrir();
			_zip.codigo_api=unzGoToFirstFile(_zip.archivo_zip);
			if ((_zip.codigo_api=unzGetCurrentFileInfo(_zip.archivo_zip, &_zip.info_actual_archivo, _zip.nombre_archivo_actual, MAX_PATH, NULL, 0, NULL, 0)) != UNZ_OK) throw GuruError().cargar_descripcion("Error retornando informacion del archivo compactado.");
		}

		iterator (GuruUnzip& Archivos, bool esfinal):_zip(Archivos),itfinal(esfinal),finzip(true) {};
		virtual ~iterator(void) {if (!itfinal) _zip.cerrar();};

		bool operator!=(const iterator& it2) const {
			if (this->finzip != it2.finzip) return true;
			else return false;
		}

		unz_file_info& informacion() {return _zip.info_actual_archivo;}

		string nombre() {return string(_zip.nombre_archivo_actual);}

		void extraer(const string& directorio) {
			_zip.extraer_actual_archivo(directorio);
		}

		void operator++() {
			_zip.codigo_api=unzGoToNextFile(_zip.archivo_zip);

			if (_zip.codigo_api == UNZ_END_OF_LIST_OF_FILE) {
				finzip = true;
				return ;
			}

			if (_zip.codigo_api != UNZ_OK) {
				finzip = true;
				throw GuruError().cargar_descripcion("Error pasando al siguiente archivo compactado.");
			}

			if ((_zip.codigo_api=unzGetCurrentFileInfo(_zip.archivo_zip, &_zip.info_actual_archivo, _zip.nombre_archivo_actual, MAX_PATH, NULL, 0, NULL, 0)) != UNZ_OK) throw GuruError().cargar_descripcion("Error retornando informacion del archivo compactado.");
		}

	protected:
		GuruUnzip& _zip;
		bool 		itfinal;
		bool		finzip;
	};

private:
	void abrir();
	void cerrar();

	void extraer_actual_archivo(const string& directorio);

private:
	string		nombre_archivo;
	unzFile		archivo_zip;
	int 		codigo_api;

	unz_file_info info_actual_archivo;
	char 		nombre_archivo_actual[MAX_PATH];
};

#endif
