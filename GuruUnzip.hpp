#ifndef __GURUUNZIP_HPP__
#define __GURUUNZIP_HPP__

#include <GuruCompat.h>
#if defined __GURUWIN32__
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif 
	
	#include <windows.h>

#include <sys\utime.h>
#elif defined __GURULINUX__
#include <utime.h>
#endif

#include <GuruError.hpp>
#include <GuruBuffer.hpp>
#include <GuruDirectorio.hpp>
#include <GuruAlgo.hpp>

#include <zlib/unzip.h>
#include <string>
#include <map>
#include <fstream>

using namespace std;

namespace GuruUnzip {

	template <typename T>
	unzFile abrirZip(const T& archivo) {
		unzFile hand;
		return ((hand=unzOpen(archivo.c_str()))==NULL) ? NULL : hand;
	}

	template <typename T>
	void cerrarZip(T& hand) {
		unzClose(hand);
		hand=NULL;
	}

	template <typename T>
	bool infoArchivo(unzFile& hand, unz_file_info& infoZip, T& nombreArchivo) {
		char archivoEnZip[MAX_PATH];

		unzGetCurrentFileInfo (hand, &infoZip, archivoEnZip, MAX_PATH, NULL, 0, NULL, 0);
		nombreArchivo=archivoEnZip;

		return true;
	}

	template <typename T>
	bool primerArchivo(unzFile& hand, unz_file_info& infoZip, T& nombreArchivo) {
		if (unzGoToFirstFile(hand) == UNZ_OK) 
			if (infoArchivo(hand, infoZip, nombreArchivo))
				return true;

		return false;
	}

	template <typename T>
	bool siguienteArchivo(unzFile& hand, unz_file_info& infoZip, T& nombreArchivo) {
		if (unzGoToNextFile(hand) == UNZ_OK)
			if (infoArchivo(hand, infoZip, nombreArchivo))
				return true;

		return false;
	}

	class iterator {
	public:
		iterator (const string& archivoZip):archivoZip(archivoZip) {
			if ((handleZip=abrirZip(archivoZip)) != NULL) {
				if (!primerArchivo(handleZip, infoZip, archivoEnZipActual))
					cerrarZip(handleZip);
			}
		};

		iterator () {handleZip=NULL;};

		virtual ~iterator() {if (handleZip!=NULL) cerrarZip(handleZip);}

		void operator++() {
			if (!siguienteArchivo(handleZip, infoZip, archivoEnZipActual)) 
				cerrarZip(handleZip);
		}

		unz_file_info* operator->() const {
			return const_cast<unz_file_info*>(&infoZip);
		}

		unz_file_info operator*() const {
			return infoZip;
		}

		bool operator!=(const iterator& it2) const {
			return (handleZip != it2.handleZip);
		}

		bool operator==(const iterator& it2) const {
			return (handleZip == it2.handleZip);
		}

		const string& nombreArchivoActual() const {return archivoEnZipActual;};
		const string& nombreZip() const { return archivoZip;};
		const unzFile handle() const { return handleZip;};
	
	private:
		string			archivoZip;
		unzFile			handleZip;
		unz_file_info	infoZip;
		string			archivoEnZipActual;
	};

	template <typename T>
	void extraer(GuruUnzip::iterator& itzip, const T& dir_destino) { 

		if (unzOpenCurrentFile(itzip.handle()) != UNZ_OK) throw GuruError().cargar_descripcion("Error abriendo el archivo compactado.");

		GuruBuffer buffer_archivo(itzip->uncompressed_size+1);

		if (unzReadCurrentFile(itzip.handle(), buffer_archivo.buf(), itzip->uncompressed_size) < 0) throw GuruError().cargar_descripcion("Error leyendo del archivo compactado.");

		if (unzCloseCurrentFile(itzip.handle()) != UNZ_OK) throw GuruError().cargar_descripcion("Error cerrando el archivo compactado.");

		T ruta_completa;
		ruta_completa = dir_destino + itzip.nombreArchivoActual().c_str();

		if (!GuruDirectorio::propiedades_archivo(dir_destino).Existe())
			GuruDirectorio::crear_directorio(dir_destino);

		#ifdef __GURUWIN32__
			if (ruta_completa.find('/') != string::npos) 
				std::replace_if(ruta_completa.begin(), ruta_completa.end(),bind2nd(std::equal_to<char>(), '/') , '\\');
		#elif defined __GURULINUX__
			if (ruta_completa.find('\\') != string::npos) 
				std::replace_if(ruta_completa.begin(), ruta_completa.end(),bind2nd(std::equal_to<char>(), '\\') , '/');
		#endif

		ofstream archivo_nuevo(ruta_completa.c_str(), ios::trunc | ios::binary);

		if (!archivo_nuevo) throw GuruError().descripcion();

		archivo_nuevo.write(reinterpret_cast<const char*>(buffer_archivo.buf()), itzip->uncompressed_size);

		archivo_nuevo.close();

		struct tm st={0};
		memset(&st, 0, sizeof(struct tm));
		st.tm_isdst = -1;

		st.tm_year	= itzip->tmu_date.tm_year - 1900;
		st.tm_mon	= itzip->tmu_date.tm_mon;
		st.tm_mday	= itzip->tmu_date.tm_mday;

		st.tm_hour	= itzip->tmu_date.tm_hour;
		st.tm_min 	= itzip->tmu_date.tm_min;
		st.tm_sec	= itzip->tmu_date.tm_sec;

		struct utimbuf times;

		times.actime = (time_t)mktime(&st);
		times.modtime = (time_t)mktime(&st);

		if (utime(ruta_completa.c_str(), &times) == -1) throw GuruError().descripcion();
	}

	template <typename T>
	map<string, unz_file_info>& mapZip(const T& archivoZip, map<string, unz_file_info>& data) {
		GuruUnzip::iterator itfinal;
		data.clear();

		for (GuruUnzip::iterator it(archivoZip);it!=itfinal;++it) {
			data.insert(make_pair(it.nombreArchivoActual(), *it));
		}

		return data;
	}

	template <typename T>
	void extraerTodo(const T& archivoZip, const T& directorio) {
		GuruUnzip::iterator itfinal;

		for (GuruUnzip::iterator it(archivoZip);it!=itfinal;++it) {
			extraer(it, directorio);
		}
	}

	template <typename T>
	T& cargar(GuruUnzip::iterator& itzip, T& buffer) {
		buffer.liberar();

		if (itzip->uncompressed_size) {
			if (unzOpenCurrentFile(itzip.handle()) != UNZ_OK) throw GuruError().cargar_descripcion("Error abriendo el archivo compactado.");

			buffer.reservar(itzip->uncompressed_size+1);

			if (unzReadCurrentFile(itzip.handle(), buffer.buf(), itzip->uncompressed_size) < 0) throw GuruError().cargar_descripcion("Error leyendo del archivo compactado.");

			if (unzCloseCurrentFile(itzip.handle()) != UNZ_OK) throw GuruError().cargar_descripcion("Error cerrando el archivo compactado.");
		}

		return buffer;
	}
};

#endif
