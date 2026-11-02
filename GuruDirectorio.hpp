#ifndef __GURUDIRECTORIO_H__
#define __GURUDIRECTORIO_H__

#include <GuruCompat.h>
#include <GuruAlgo.hpp>
#include <GuruVarEntorno.hpp>
#include <GuruFechaHora.hpp>

#if defined GURUDIRECTORIO_CON_REGEXP
#include <boost/regex.hpp>
#define GURUDIRECTORIO_REGEXP(x,y) boost::regex_match(x, boost::regex(y, boost::regbase::normal | boost::regbase::icase))
#else
#define GURUDIRECTORIO_REGEXP(x,y) true
#endif

#include <time.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <string>
#include <stack>
#include <cstdio>
#include <iterator>

#ifdef __GURUWIN32__
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef _WINSOCKAPI_
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
	#endif

	#include <windows.h>

	#include <io.h>
	#include <sys/stat.h>
	#include <time.h>
#elif defined __GURULINUX__
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <stdint.h>
	#include <dirent.h>
	#include <unistd.h>
	#include <GuruCompat/splitpath.hpp>
	#include <utime.h>
	#include <fstream>
#endif

namespace GuruDirectorio {

	static const char* version = "3.0.0";
	static const char* version_number = "300000";

#if defined GURUDIRECTORIO_UNICODE
	typedef std::wstring	type_string;

	static const wchar_t* dir_actual = L".";
	static const wchar_t* dir_padre = L"..";

	static const wchar_t* comodin = L"*.*";

	static const char divisor_directorio_win = L'\\';
	static const char divisor_directorio_lx = L'/';

#define DIR_ACTUAL L"."
#define DIR_PADRE  L".."

#ifdef __GURUWIN32__
	static const wchar_t divisor_directorio = L'\\';
	static const wchar_t no_divisor_directorio = L'/';

#if defined __GURUUNICODE__
#define CONVERT(x) x
#else
#define CONVERT(x) GuruStringAlgo::unicode_ansi(x)
#endif
#elif defined __GURULINUX__
	static const char divisor_directorio = L'/';
	static const char no_divisor_directorio = L'\\';
#endif
#else
	typedef std::string type_string;

	static const char* dir_actual = ".";
	static const char* dir_padre = "..";

	static const char* comodin = "*.*";

	static const char divisor_directorio_win = '\\';
	static const char divisor_directorio_lx = '/';

#define DIR_ACTUAL "."
#define DIR_PADRE  ".."

#ifdef __GURUWIN32__
	static const char divisor_directorio = '\\';
	static const char no_divisor_directorio = '/';

#if defined __GURUUNICODE__
#define CONVERT(x) GuruStringAlgo::ansi_unicode(x)
#else
#define CONVERT(x) x
#endif
#elif defined __GURULINUX__
	static const char divisor_directorio = '/';
	static const char no_divisor_directorio = '\\';

#define CONVERT(x) x
#endif
#endif

	template<typename T>
	class basic_GuruSplitPath {
	public:
		basic_GuruSplitPath(const T& path = "") :splitted(false) { this->path = path; };
		virtual ~basic_GuruSplitPath() {};

		basic_GuruSplitPath<T>& split(const T& path) { this->path = path; split(); return *this; };

		T& nombre() { if (!splitted) split(); return _nombre; };
		T& ext() { if (!splitted) split(); return _ext; };
		T& dir() { if (!splitted) split(); return _dir; };
		T& drive() { if (!splitted) split(); return _drive; };
		T nombre_ext() {
			if (!splitted) split();

			return _nombre + _ext;
		};

	private:
		void split() {
			if (path.empty()) return;

			this->_drive.clear();
			this->_dir.clear();
			this->_nombre.clear();
			this->_ext.clear();

			char fname[_MAX_FNAME] = { 0 };
			char ext[_MAX_EXT] = { 0 };
			char dir[_MAX_DIR] = { 0 };
			char drive[_MAX_DRIVE] = { 0 };

			_splitpath(this->path.c_str(), drive, dir, fname, ext);

			splitted = true;

#if defined GURUDIRECTORIO_UNICODE
			this->_nombre = CONVERT(fname);
			this->_ext = CONVERT(ext);
			this->_dir = CONVERT(dir);
			this->_drive = CONVERT(drive);
#else
			this->_nombre = fname;
			this->_ext = ext;
			this->_dir = dir;
			this->_drive = drive;
#endif
		};

		T	path;

		T	_nombre;
		T	_ext;
		T	_dir;
		T	_drive;

		bool	splitted;
	};

	typedef basic_GuruSplitPath<type_string>  GuruSplitPath;

	template <typename T>
	class basic_GuruArchivo {
		T	 		_nombre;
		T			_extension;
		time_t		_fecha;
#ifdef __GURUWIN32__
		DWORD		_atributos;
#elif defined __GURULINUX__
		uint32_t	_atributos;
#endif
		uint64_t	_tamanio;

	public:
		basic_GuruArchivo() { limpiar(); };
		basic_GuruArchivo(const T& nombre, uint64_t tamanio) :_fecha(0), _atributos(0), _tamanio(tamanio) { this->_nombre = nombre; };
		virtual ~basic_GuruArchivo() {};

		inline void tamanio(uint64_t t) { _tamanio = t; };
		inline const uint64_t tamanio() const { return _tamanio; };

		inline void nombre(const T& t) { _nombre = t; };
		inline const T& nombre() const { return _nombre; };

		inline void extension(const T& t) { _extension = t; };
		inline T extension() { return _extension; };

		inline void atributos(unsigned int t) { _atributos = t; };
		inline const unsigned int atributos() const { return _atributos; };

		inline void fecha(time_t t) { _fecha = t; };
		inline const time_t fecha() const { return _fecha; };

#ifdef __GURUWIN32__
		inline bool EsDirectorio() { return (_atributos & FILE_ATTRIBUTE_DIRECTORY) ? true : false; };
		inline bool EsArchivo() { return (_atributos != INVALID_FILE_ATTRIBUTES && !(_atributos & FILE_ATTRIBUTE_DIRECTORY)); };
		inline bool EsOculto() { return (_atributos & _A_HIDDEN) ? true : false; };
		inline bool EsEnlace() { return (_atributos & FILE_ATTRIBUTE_REPARSE_POINT) ? true : false; };
#elif defined __GURULINUX__
		inline bool EsDirectorio() { return S_ISDIR(_atributos) ? true : false; };
		inline bool EsArchivo() { return S_ISREG(_atributos) ? true : false; };
		inline bool EsOculto() { return (_atributos) ? true : false; };
		inline bool EsEnlace() { return S_ISLNK(_atributos) ? true : false; };
#endif

		inline bool Existe() { return (_nombre.empty() && _fecha == 0 && _tamanio == 0) ? false : true; }

		void limpiar() { _tamanio = 0; _nombre.clear(); _extension.clear(); _fecha = 0; _atributos = 0; };
		operator bool() { return _nombre.empty(); };

		T& sacar_extension() {
			if (_extension.empty()) {
				return _extension = GuruDirectorio::GuruSplitPath(_nombre).ext();
			} else
				return _extension;
		};
	};

	typedef basic_GuruArchivo<type_string> GuruArchivo;

	template <typename T>
	bool existe(const T& nombre) {
#ifdef __GURUWIN32__
		DWORD atributos = GetFileAttributes(CONVERT(nombre).c_str());
//		if (atributos == INVALID_FILE_ATTRIBUTES) cout << "invalido" << endl;
//		cout << GetLastError() << endl;
//		if (GetLastError() == ERROR_FILE_NOT_FOUND) cout << "not found" << endl;
		return (atributos == INVALID_FILE_ATTRIBUTES && GetLastError() != ERROR_SUCCESS) ? false : true;
#elif defined __GURULINUX__
		struct stat buffer;
		return (stat(nombre.c_str(), &buffer) == 0) ? true : false;
#endif
	}

	template <typename T>
	bool esArchivo(const T& nombre) {
#ifdef __GURUWIN32__
		DWORD atributos = GetFileAttributes(CONVERT(nombre).c_str());
		return ((atributos != INVALID_FILE_ATTRIBUTES) && !(atributos & FILE_ATTRIBUTE_DIRECTORY)) ? true : false;
#elif defined __GURULINUX__
		struct stat buffer;
		if (stat(nombre.c_str(), &buffer) == 0) {
			return S_ISREG(buffer.st_mode) ? true : false;
		} else return false;
#endif
	}

	template <typename T>
	bool esDirectorio(const T& nombre) {
#ifdef __GURUWIN32__
		// INVALID_FILE_ATTRIBUTES
		DWORD atributos = GetFileAttributes(CONVERT(nombre).c_str());
		return ((atributos != INVALID_FILE_ATTRIBUTES) && (atributos & FILE_ATTRIBUTE_DIRECTORY)) ? true : false;
#elif defined __GURULINUX__
		struct stat buffer;
		if (stat(nombre.c_str(), &buffer) == 0) {
			// return ((buffer.st_mode & S_IFMT) == S_IFDIR) ? true : false;
			return S_ISDIR(buffer.st_mode) ? true : false;
		} else return false;
#endif
	}

	template <typename T>
	GuruArchivo& propiedades(const T& nombre_archivo, GuruArchivo& prop) {

		if (nombre_archivo.empty()) return prop;

#ifdef __GURUWIN32__
		WIN32_FILE_ATTRIBUTE_DATA dataarchivo = { 0 };
		if (GetFileAttributesEx(CONVERT(nombre_archivo).c_str(), GetFileExInfoStandard, &dataarchivo)) {
			SYSTEMTIME stUTC;
			FileTimeToSystemTime(&dataarchivo.ftLastWriteTime, &stUTC);

			SYSTEMTIME st;
			SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &st);

			struct tm stm = { 0 };

			stm.tm_year = st.wYear - 1900;
			stm.tm_mon = st.wMonth - 1;
			stm.tm_mday = st.wDay;

			stm.tm_hour = st.wHour;
			stm.tm_min = st.wMinute;
			stm.tm_sec = st.wSecond;

			prop.nombre(nombre_archivo);
			prop.tamanio(dataarchivo.nFileSizeLow + ((UINT64)dataarchivo.nFileSizeHigh << 32));
			prop.fecha(mktime(&stm));
			prop.atributos(dataarchivo.dwFileAttributes);
		}
#elif defined __GURULINUX__
		struct stat64 fileinfo;
		if (stat64(nombre_archivo.c_str(), &fileinfo) == 0) {
			prop.nombre(nombre_archivo);
			prop.tamanio(fileinfo.st_size);
			prop.fecha(fileinfo.st_mtime);
			prop.atributos(fileinfo.st_mode);
		}
#endif
		return prop;
	}

	template <typename T>
	GuruArchivo propiedades(const T& nombre_archivo) {
		GuruArchivo prop;
		propiedades(nombre_archivo, prop);

		return prop;
	}

	///////////////////////////////////////////////////////////
	// no chequea la existencia del archivo que genera.
	///////////////////////////////////////////////////////////
	static inline string archivo_temporal() {
		string tmp;

#ifdef __GURUWIN32__
		// Segun Microsoft TMP->TEMP->USERPROFILE
#if defined __GURUUNICODE__
		TCHAR buffer[MAX_PATH + 1];
		if (GetTempPath(MAX_PATH, buffer))
			tmp = GuruStringAlgo::unicode_ansi(std::wstring(buffer));
#else
		char buffer[MAX_PATH + 1];
		if (GetTempPath(MAX_PATH, buffer))
			tmp = buffer;
#endif
#elif defined __GURULINUX__
		// segun ISO/IEC 9945 (POSIX):
		if ((tmp = GuruVarEntorno::variable("TMPDIR")).empty())
			if ((tmp = GuruVarEntorno::variable("TMP")).empty())
				if ((tmp = GuruVarEntorno::variable("TEMP")).empty())
					if ((tmp = GuruVarEntorno::variable("TEMPDIR")).empty())
						tmp = "/tmp/";
#endif

		tmp += GuruStringAlgo::rand_str_min();

		//		if (GuruDirectorio::propiedades_archivo(tmp).Existe()) cout << "Existe tmp!" << endl;

		return tmp;
	}

	//////////////////////////////////////////////////////////////////////
	// Esta funcion arregla el directorio para que tenga la barra al final
	//////////////////////////////////////////////////////////////////////
	template<class T>
	T comoDirectorio(const T& str_directorio) {
		if (str_directorio.empty()) return str_directorio;

		if (str_directorio.at(str_directorio.size() - 1) != GuruDirectorio::divisor_directorio) {
			return str_directorio + GuruDirectorio::divisor_directorio;
		}

		return str_directorio;
	};

	//////////////////////////////////////////////////////////////////////
	// Esta funcion arregla el directorio para que tenga la barra al final
	//////////////////////////////////////////////////////////////////////
	template<class T>
	T comoDirectorioSinBarraFinal(const T& str_directorio) {
		if (str_directorio.empty()) return string();

		T dire(str_directorio);
		typename T::iterator its; its = dire.end();
		--its; while (isspace(*its)) --its;
		if (*its == GuruDirectorio::divisor_directorio) dire.erase(its);

		return dire;
	};

	//////////////////////////////////////////////////////////////////////
	// Esta funcion cambia de directorio al procesos donde se ejecuta
	//////////////////////////////////////////////////////////////////////
	template<class T>
	bool cambiarDirectorio(const T& str_directorio) {
#ifdef __GURUWIN32__
		if (SetCurrentDirectory(CONVERT(str_directorio).c_str()) == TRUE) return true;
		else return false;
#elif defined __GURULINUX__
		if (chdir(str_directorio.c_str()) == 0) return true;
		else return false;
#endif
	};

	// divisor == divisor de directorio deseado a tener, si no lo tiene utiliza el del SO
	template <typename T>
	T normalizarDirectorio(const T& directorio, const typename T::value_type divisor = GuruDirectorio::divisor_directorio) {
		typename T::value_type origen, destino;

		if (divisor == GuruDirectorio::divisor_directorio_win) { // divisor de windows
			origen = GuruDirectorio::divisor_directorio_lx;
			destino = GuruDirectorio::divisor_directorio_win;
		} else { // como si fuera linux
			origen = GuruDirectorio::divisor_directorio_win;
			destino = GuruDirectorio::divisor_directorio_lx;
		}

		if (directorio.find(origen) != T::npos) {
			T tmp;
			std::replace_copy(directorio.begin(), directorio.end(), std::back_inserter(tmp), origen, destino);
			return tmp;
		} else {
			return directorio;
		}
	}

	template<class T>
	bool crearDirectorioRecursivo(const T& str_directorio) {
		if (str_directorio.empty()) return false;

		T tmp(GuruDirectorio::normalizarDirectorio(comoDirectorio(str_directorio)));

		size_t lugar, inicio = 0;
		T subruta;

		while ((lugar = tmp.find_first_of(GuruDirectorio::divisor_directorio, inicio)) != T::npos) {
			subruta = tmp.substr(0, lugar + 1);

			if (!GuruDirectorio::existe(subruta)) {
#ifdef __GURUWIN32__
				BOOL ret = 0;
				if ((ret = CreateDirectory(CONVERT(subruta).c_str(), NULL)) == 0) {
					if (ret != ERROR_ALREADY_EXISTS)
						return false;
				}
#elif defined __GURULINUX__
				if (mkdir(subruta.c_str(), S_ISVTX) != 0) return false;
#endif
			}

			inicio = lugar + 1;
		}

		return true;
	};

	// Retorna la ruta de actual proceso
	static string rutaProceso() {

#ifdef __GURUWIN32__
	#if defined __GURUUNICODE__
		TCHAR exe[MAX_PATH];
	#else 
		char exe[MAX_PATH];
	#endif
		if (!GetModuleFileName(NULL, exe, MAX_PATH)) return string();

#if defined __GURUUNICODE__
		return GuruStringAlgo::unicode_ansi(std::wstring(exe));
#else 
		return string(exe);
#endif

#elif defined __GURULINUX__
		char exe[MAX_PATH];
		if (!readlink("/proc/self/exe", exe, MAX_PATH)) return string();
		return string(exe);
#endif
	}

	//////////////////////////////////////////////////////////////////////
	// Esta funcion borra un archivo
	//////////////////////////////////////////////////////////////////////
	template<class T>
	bool borrarArchivo(const T& str_archivo) {
	#ifdef __GURUWIN32__
		return (DeleteFile(CONVERT(str_archivo).c_str()) != 0) ? true : false;
	#elif defined __GURULINUX__
		return (std::remove(str_archivo.c_str()) == 0) ? true : false;
	#endif
	};

	// Basado en
	// http://stackoverflow.com/questions/10195343/copy-a-file-in-a-sane-safe-and-efficient-way
	//
	template<class T>
	bool copiarArchivo(const T& origen, const T& destino) {
	#ifdef __GURUWIN32__
		return (CopyFile(CONVERT(origen).c_str(), CONVERT(destino).c_str(), FALSE)) ? true : false;
	#elif defined __GURULINUX__
		ifstream source(origen.c_str(), ios::binary);
		ofstream dest(destino.c_str(), ios::binary);

		if (!source.is_open() || !source.is_open()) return false;

		istreambuf_iterator<char> begin_source(source);
		istreambuf_iterator<char> end_source;
		ostreambuf_iterator<char> begin_dest(dest);

		copy(begin_source, end_source, begin_dest);

		source.close();
		dest.close();

		return true;
	#endif
	};

	template<class T>
	bool moverArchivo(const T& origen, const T& destino) {
	#ifdef __GURUWIN32__
		return (MoveFileEx(CONVERT(origen).c_str(), CONVERT(destino).c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING) != 0) ? true : false;
	#elif defined __GURULINUX__
		if (rename(origen.c_str(), destino.c_str()) != 0) {
			if (errno == EXDEV) { // distintos filesystem
				if (!copiarArchivo(origen, destino)) return false;
				if (!borrarArchivo(origen)) return false;
			} else
				return false;
		}
		return true;
	#endif
	}

#ifdef __GURUWIN32__
	// sacado de la pagina de microsoft.
	static void TimetToFileTime(time_t t, FILETIME* pft) {
		LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
		pft->dwLowDateTime = (DWORD)ll;
		pft->dwHighDateTime = ll >> 32;
	}

	static time_t FileTimeToTime(FILETIME* pft) {
		ULARGE_INTEGER ull;
		ull.LowPart = pft->dwLowDateTime;
		ull.HighPart = pft->dwHighDateTime;
		return ull.QuadPart / 10000000ULL - 11644473600ULL;
	}
#elif defined __GURULINUX__
#endif

	//////////////////////////////////////////////////////////////////////
	// Esta funcion le asigna fecha y hora a un archivo
	//////////////////////////////////////////////////////////////////////
	template<class T>
	bool fechaArchivo(const T& nombre, time_t fechahora) {
	#ifdef __GURUWIN32__
		HANDLE harchivo = CreateFile(CONVERT(nombre).c_str(), FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);

		if (harchivo == INVALID_HANDLE_VALUE) return false;

		FILETIME fecha;
		TimetToFileTime(fechahora, &fecha);

		SetFileTime(harchivo, NULL, NULL, &fecha);

		CloseHandle(harchivo);

		return true;

	#elif defined __GURULINUX__
		struct utimbuf new_times;
		struct stat ultimo_stat = {0};
		
		if (stat(nombre.c_str(), &ultimo_stat) < 0) return false;

		new_times.actime = ultimo_stat.st_atime;
		new_times.modtime = fechahora;

		return (utime(nombre.c_str(), &new_times) == 0) ? true : false;
	#endif

		return true;
	};

	//////////////////////////////////////////////////////////////////////
	// Esta funcion returna la fecha y hora en formato time_t de un archivo
	//////////////////////////////////////////////////////////////////////
	template<class T>
	time_t fechaArchivo(const T& str_archivo) {
	#ifdef __GURUWIN32__
		HANDLE harchivo = CreateFile(CONVERT(nombre).c_str(), FILE_READ_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);

		if (harchivo == INVALID_HANDLE_VALUE) return 0;

		FILETIME fecha;
		if (GetFileTime(harchivo, NULL, NULL, &fecha) == 0) return 0;

		return FileTimeToTime(&fecha);
	#elif defined __GURULINUX__
		struct stat ultimo_stat;
		
		if (stat(str_archivo.c_str(), &ultimo_stat) < 0) return 0;

		return ultimo_stat.st_mtime;

	#endif
	};

	// Esta clase sirve para ordenar contenedores
	// Ejem:
	// list = list.sort(OrdenaGuruArchivo(col, ord));
	// map  = map<GuruArchivo, string, OrdenarListado > nada;
	// vector = void sort( vector.begin(), vector.end(), OrdenaGuruArchivo(nombre, asc));
	class OrdenaGuruArchivo {
	public:
		typedef enum {nombre, inombre, bytes, fecha} columna;
		typedef enum {asc, desc} orden;

		OrdenaGuruArchivo(columna col=fecha, orden ord=desc):col(col),ord(ord){};

		bool operator () (const GuruArchivo& a, const GuruArchivo& b) {
			switch (col) {
				case nombre:
							if (ord == asc) {
								if (a.nombre().compare(b.nombre()) < 0)
									return true;
							} else if (a.nombre().compare(b.nombre()) > 0)
								return true;
							break;
				case inombre:
							if (ord == asc) {
								if (a.nombre().compare(b.nombre()) < 0)								
									return true;
							} else if (a.nombre().compare(b.nombre()) > 0)
								return true;
							break;

				case bytes: if (ord == asc) {
								if (a.tamanio() < b.tamanio()) 
									return true;
							} else if (a.tamanio() > b.tamanio()) 
									return true;
							break;
				case fecha: if (ord == asc) {
								if (a.fecha() < b.fecha()) 
									return true;
							} else if (a.fecha() > b.fecha()) 
									return true;
							break;
			}
			return false;
		}
	private:
		columna	col;
		orden	ord;
	};

	template <typename T>
	bool crear_directorio(const T& directorio) {
	#ifdef __GURUWIN32__
		return (CreateDirectory(CONVERT(normalizarDirectorio(directorio)).c_str(), NULL) == TRUE) ? true : false;
	#elif defined __GURULINUX__
		mode_t modo=umask(0);
		return (mkdir (normalizarDirectorio(directorio).c_str(), modo) == 0) ? true : false;
	#endif
	}

	inline const string directorio_actual() {
	#ifdef __GURUWIN32__
		#if defined __GURUUNICODE__
			TCHAR directorio[MAX_PATH];
			if (GetCurrentDirectory(MAX_PATH, directorio) != 0) return GuruStringAlgo::unicode_ansi(directorio);
		#else
			char directorio[MAX_PATH];
			if (GetCurrentDirectory(MAX_PATH, directorio) != 0) return string(directorio);
		#endif
	#elif defined __GURULINUX__
		char directorio[MAX_PATH];
		if (getcwd(directorio, MAX_PATH) != NULL) return string(directorio);
	#endif

		return string();
	}

#ifdef __GURUWIN32__
	static void data_a_GuruArchivo(WIN32_FIND_DATA* fileinfo, GuruDirectorio::GuruArchivo& ga) {
#elif defined __GURULINUX__
	static void data_a_GuruArchivo(struct stat64* fileinfo, GuruDirectorio::GuruArchivo& ga) {
#endif

	ga.limpiar();

	#ifdef __GURUWIN32__
		if (fileinfo != NULL) {
			SYSTEMTIME stUTC;
			FileTimeToSystemTime(const_cast<FILETIME*>(&fileinfo->ftLastWriteTime), &stUTC);

			SYSTEMTIME st;
			SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &st);

			struct tm stm = {0};

			stm.tm_year = st.wYear - 1900;
			stm.tm_mon  = st.wMonth - 1;
			stm.tm_mday = st.wDay;

			stm.tm_hour = st.wHour;
			stm.tm_min  = st.wMinute;
			stm.tm_sec  = st.wSecond;

		#if defined __GURUUNICODE__
			#if defined GURUDIRECTORIO_UNICODE
				ga.nombre(fileinfo->cFileName);
			#else
				ga.nombre(GuruStringAlgo::unicode_ansi(fileinfo->cFileName));
			#endif
		#else
			#if defined GURUDIRECTORIO_UNICODE
				ga.nombre(GuruStringAlgo::ansi_unicode(fileinfo->cFileName));
			#else
				ga.nombre(fileinfo->cFileName);
			#endif
		#endif

			ga.tamanio(fileinfo->nFileSizeLow + ((UINT64)fileinfo->nFileSizeHigh << 32));
			ga.fecha(mktime(&stm));
			ga.atributos(fileinfo->dwFileAttributes);
		} 
	#elif defined __GURULINUX__
		if (fileinfo != NULL) {
			ga.tamanio(fileinfo->st_size);
			ga.fecha(fileinfo->st_mtime);
			ga.atributos(fileinfo->st_mode);
		}
	#endif
	}

#ifdef __GURUWIN32__
	typedef HANDLE			handleGuruDirectorio;
	#define H_INVALIDO_GD	INVALID_HANDLE_VALUE
#elif defined __GURULINUX__
	typedef DIR*			handleGuruDirectorio;
	#define H_INVALIDO_GD	NULL
#endif

	template <typename T>
	handleGuruDirectorio primerArchivo(const T& directorio, GuruDirectorio::GuruArchivo& data, const T& comodin= GuruDirectorio::comodin) {
	#ifdef __GURUWIN32__
		handleGuruDirectorio handle;
		WIN32_FIND_DATA fileinfo;

		T busqueda(GuruDirectorio::comoDirectorio(directorio));
		busqueda += comodin;

		if ((handle = FindFirstFile(CONVERT(busqueda).c_str(), &fileinfo)) != H_INVALIDO_GD) {
			data_a_GuruArchivo(&fileinfo, data);
		}
	#elif defined __GURULINUX__
		handleGuruDirectorio	handle;
		struct stat64			fileinfo = {0};

		if ((handle = opendir(GuruDirectorio::comoDirectorio(directorio).c_str())) != H_INVALIDO_GD) {
			struct dirent* dir = readdir(handle);

			if (dir == NULL) {
				closedir(handle);
				handle=H_INVALIDO_GD;
				return handle;
			} else {
				string tmp;
				tmp = directorio;
				tmp += dir->d_name;

				if (stat64(tmp.c_str(), &fileinfo) == 0) {
					data_a_GuruArchivo(&fileinfo, data);
					data.nombre(dir->d_name);
					return handle;
				} else {
		            data.nombre(dir->d_name);
    		        data.tamanio(0);
		#ifdef _DIRENT_HAVE_D_TYPE
		//          cout << "ERROR!!!:" << strerror(errno) << " - " <<  tmp << endl;
		#endif
    		        data.atributos(S_IFREG);
        		    data.fecha(0);
	    	    }
			}
		}
	#endif
		return handle;
	}

	// El tercer parametro es solo para linux, en win32 no se utiliza
	// T=Handle para la funcion de listado de archivo.
	// Y=Tipo de datos para el directorio a listar.
	template <typename T, typename Y>
	bool siguienteArchivo(T& h, GuruArchivo& data, const Y& directorio="") {
	#ifdef __GURUWIN32__
		WIN32_FIND_DATA fileinfo;
		if (FindNextFile(h, &fileinfo)) {
			data_a_GuruArchivo(&fileinfo, data);
			return true;
		} else {
			FindClose(h);
			h=H_INVALIDO_GD;
			return false;
		}
	#elif defined __GURULINUX__
		struct dirent* dir = readdir(h);

		if (dir == NULL) {
			// if (errno) cout << "ERROR:" << errno << " - " << strerror(errno) << " - " << directorio << endl;
			closedir(h);
			h=H_INVALIDO_GD;
			return false;
		} else {
			Y tmp;
			tmp = directorio;
			tmp += dir->d_name;
			struct stat64 fileinfo = {0};

			if (stat64(tmp.c_str(), &fileinfo) == 0) {
				data_a_GuruArchivo(&fileinfo, data);
				data.nombre(dir->d_name);
			} else {
				data.nombre(dir->d_name);
				data.tamanio(0);
	#ifdef _DIRENT_HAVE_D_TYPE
	//			cout << "ERROR!!!:" << strerror(errno) << " - " <<  tmp << endl;
	#endif
				data.atributos(S_IFREG);
				data.fecha(0);

				//cout << "ERROR:" << strerror(errno) << " - " <<  tmp << endl;
			}
		}

		return true;
	#endif
	}

	template <typename T>
	void cerrarBusqueda(T& h) {
	#ifdef __GURUWIN32__
		FindClose(h);
	#elif defined __GURULINUX__
		closedir(h);
	#endif
		h=H_INVALIDO_GD;
	}

	template <typename T>
	class iterator_basic {
	public:
		iterator_basic(const T& directorio, const T& comodin=GuruDirectorio::comodin):handle(H_INVALIDO_GD) {
			primero(directorio, comodin);
		};

		iterator_basic():handle(H_INVALIDO_GD) {};

		virtual ~iterator_basic() {
			if (handle == H_INVALIDO_GD) cerrarBusqueda(handle);
		};

		void primero(const T& ruta_directorio, const T& comodin) {

			T tmp_ruta_directorio(GuruDirectorio::comoDirectorio(ruta_directorio));

			directorio = tmp_ruta_directorio;

		#ifdef __GURUWIN32__
			if (directorio.find(GuruDirectorio::no_divisor_directorio) != T::npos) 
				std::replace_if(directorio.begin(), directorio.end(),bind2nd(std::equal_to<char>(), GuruDirectorio::no_divisor_directorio) , GuruDirectorio::divisor_directorio);

			directorio += (comodin.empty()) ? GuruDirectorio::comodin : comodin;

		#elif defined __GURULINUX__
			if (directorio.find(GuruDirectorio::no_divisor_directorio) != T::npos)
				std::replace_if(directorio.begin(), directorio.end(), bind2nd(std::equal_to<char>(), GuruDirectorio::no_divisor_directorio), GuruDirectorio::divisor_directorio);

			// no necesita el comodin *.* u otro.
		#endif

			if ((handle=primerArchivo(tmp_ruta_directorio, actual, comodin)) == H_INVALIDO_GD) return ;

			while ((actual.nombre().compare(GuruDirectorio::dir_actual)==0 || actual.nombre().compare(GuruDirectorio::dir_padre)==0) && handle != H_INVALIDO_GD)
				siguienteArchivo(handle, actual, tmp_ruta_directorio);

			if (handle == H_INVALIDO_GD)
				cerrarBusqueda(handle);
		}

		void operator++() {
			do {
				siguienteArchivo(handle, actual, directorio);
			} while ((actual.nombre().compare(GuruDirectorio::dir_actual)==0 || actual.nombre().compare(GuruDirectorio::dir_padre)==0) && handle != H_INVALIDO_GD);
			
			if (handle == H_INVALIDO_GD)
				cerrarBusqueda(handle);
		}

		GuruArchivo* operator->() const {
			return (GuruArchivo*)&actual;
		}

		GuruArchivo operator*() const {
			return actual;
		}

		bool operator!=(const iterator_basic<T>& it2) {
			if (handle != it2.handle) return true;
			else return false;
		}

		bool operator==(const iterator_basic<T>& it2) {
			if (handle == it2.handle) return true;
			else return false;
		}

		T& dir() {return directorio;};

	protected:
		T						directorio;
		GuruArchivo				actual;
		handleGuruDirectorio	handle;
	};

	template <typename T>
	class iterator_recursivo_basic {
	public:
		iterator_recursivo_basic() {};
		iterator_recursivo_basic(const T& directorio, const T& filtro=GuruDirectorio::comodin):filtro(filtro) {
			primero(directorio);
		};

		void primero (const T& directorio) {
			handleGuruDirectorio	handle;
			T						absoluta(GuruDirectorio::comoDirectorio(directorio));
			bool					salida=true;

			do {
				if ((handle = primerArchivo(absoluta, actual, filtro)) == H_INVALIDO_GD) break;

				rama.push(make_pair(handle, make_pair(absoluta, relativa)));

				do {
					while ((actual.nombre().compare(GuruDirectorio::dir_actual) == 0 || actual.nombre().compare(GuruDirectorio::dir_padre) == 0 || (!GURUDIRECTORIO_REGEXP(actual.nombre(), filtro) && actual.EsArchivo() )) && salida == true)
						salida=siguienteArchivo(rama.top().first, actual, rama.top().second.first);

					// se termino el directorio?
					if (salida == false) {
						rama.pop();
						if (rama.size() == 1) return ; // agregado el 16/04/2013, 1, sino examina el raiz
						absoluta = rama.top().second.first; // retrocedo un lugar en la rama de directorios
						relativa = rama.top().second.second;

						salida = true;
					} else {
						if (actual.EsDirectorio()) {
							absoluta.append(actual.nombre());
							absoluta += GuruDirectorio::divisor_directorio;

							relativa += actual.nombre();
							relativa += GuruDirectorio::divisor_directorio;

							break; // vuelvo a pedir el primer archivo del directorio
						} 
						// else dir_actual=sub_dir;
					}
				} while (actual.EsDirectorio());
			} while (actual.EsDirectorio());
		}

		void operator++() {
			bool salida=true;

			do {
				while ((salida=siguienteArchivo(rama.top().first, actual, rama.top().second.first)) == true) {

					if ((actual.nombre().compare(GuruDirectorio::dir_actual) == 0 || actual.nombre().compare(GuruDirectorio::dir_padre) == 0 || (!GURUDIRECTORIO_REGEXP(actual.nombre(), filtro) && actual.EsArchivo())) && salida == true)
						continue;
						
					// aun hay archivos en el directorio?
					if (salida == true) {

						if (actual.EsDirectorio()) {
							relativa = rama.top().second.second + actual.nombre() + GuruDirectorio::divisor_directorio;
							primero(rama.top().second.first + actual.nombre() + GuruDirectorio::divisor_directorio);
							if (actual.EsArchivo()) return;
							else continue;
						}else { 
							relativa=rama.top().second.second;
							break;
						}
					} else { // no hay mas nada en el directorio.
						rama.pop();
					}
				}
				
				if (salida == false ){ // no hay nada en el directorio
					rama.pop();
					if (rama.size() == 0) return;
					relativa = rama.top().second.second;
				}
			}while (salida != true);
		}

		GuruArchivo* operator->() const {
			return const_cast<GuruArchivo*>(&actual);
		}

		GuruArchivo operator*() const {
			return actual;
		}

		bool operator!=(const iterator_recursivo_basic<T>& it2) const {
			return (rama.size() != it2.rama.size());
		}

		bool operator==(const iterator_recursivo_basic<T>& it2) const {
			return (rama.size() == it2.rama.size());
		}

		T& dir() { return rama.top().second.second; };

	private:
		GuruArchivo						actual;
		T								relativa;
		T								filtro;
		stack<pair<handleGuruDirectorio, pair<T, T> > >	rama; // el handle de cada directorio, con su par cadena de ruta absoluta y relativa
	};

	typedef iterator_recursivo_basic<type_string> iterator_recursivo;
	typedef iterator_basic<type_string> iterator;

	template <typename T>
	void test (const T& ruta) {

		//	for (int a = 1; a < 1000; a++) cout << (a % 4) << endl;

		uint64_t	t = 0;
		T			ultimo;
		typename T::value_type	progreso[] = { '|','/','-','\\' };

		for (GuruDirectorio::iterator_recursivo it(GuruDirectorio::comoDirectorio(ruta)), itend; it != itend; ++it) {
			t++;

			if (ultimo.compare(it.dir()) != 0) {
				ultimo = it.dir();
				cout << "\r" << it.dir() << endl;
			} else cout << "\r" << progreso[t % 4];

			if (!GuruDirectorio::existe(ruta + it.dir() + it->nombre()))
				cout << "NO Exite ?!?!?!? " << (ruta + it.dir() + it->nombre()) << endl;
		}

		cout << "t=" << t << endl;
	}
}

#endif
