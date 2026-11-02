#ifndef __GURUHTTPALGO_HPP__
#define __GURUHTTPALGO_HPP__

#include <map>
#include <string>
#include <istream>

using namespace std;

#include <GuruToken.hpp>
#include <GuruString_algo.hpp>
#include <GuruCache.hpp>
#include <GuruDirectorio.hpp>
#include <GuruString_algo.hpp> // por ci_less en GuruCache

// #if !defined GURUHTTP_MIME
//	#error Archivo MIME no definido, Defina GURUHTTP_MIME con un nombre de archivo.
// #endif

namespace GuruhttpAlgo {
	template <typename T>
	T mime(const T& archivo) {
		static GuruCacheLFUEX<string, string, GuruStringAlgo::ci_less> cache_mime(0);

#if defined GURUHTTP_MIME
		T ext(GuruDirectorio::GuruSplitPath(archivo).ext());

		if (ext.empty()) return string("application/octet-stream");

		try {
			return cache_mime(GuruDirectorio::GuruSplitPath(archivo).ext());
		}catch (GuruCacheException&) {
			std::ifstream mimes(GURUHTTP_MIME, std::ifstream::in);

			if (!mimes.is_open()) return string("application/octet-stream");

			GuruTokenEx token(" \t");
			char    linea[256];
			T	header;
			T	str_linea;

			mimes.getline(linea, 256);
			while (!mimes.eof()) {
				str_linea = linea;
				GuruStringAlgo::trim(str_linea);
					if (token.extraer(str_linea) == 2) {
						if (GuruStringAlgo::stringequals(token[0], ext)) {
							header = token[1];
							break;
						}
					}
				mimes.getline(linea, 256);
			}

			if (header.empty()) header="application/octet-stream";

			mimes.close();

			cache_mime.insertar(ext, header);

			return header;
		}
#else 
		if (cache_mime.size() == 0) {
			// Imagenes
			cache_mime.insertar(".jpeg","image/jpeg");
			cache_mime.insertar(".jpg","image/jpeg");
			cache_mime.insertar(".ico","image/x-icon");
			cache_mime.insertar(".bmp","image/bmp");
			cache_mime.insertar(".gif","image/gif");
			cache_mime.insertar(".png","image/png");
			cache_mime.insertar(".tif","image/tiff");
			cache_mime.insertar(".tiff","image/tiff");
			cache_mime.insertar(".pcx","image/x-pcx");
			cache_mime.insertar(".svg","image/svg+xml");
			cache_mime.insertar(".webp", "image/webp");

			// Audio
			cache_mime.insertar(".mid","audio/midi");
			cache_mime.insertar(".midi","audio/midi");
			cache_mime.insertar(".mp2","audio/mpeg");
			cache_mime.insertar(".mp3","audio/mpeg");
			cache_mime.insertar(".mp4a","audio/mpeg");
			cache_mime.insertar(".mpga","audio/mpeg");
			cache_mime.insertar(".ogg","audio/ogg");
			cache_mime.insertar(".wav","audio/x-wav");
			cache_mime.insertar(".flac","audio/flac");
			cache_mime.insertar(".m3u","audio/mpegurl");

			// Video
			cache_mime.insertar(".mov","video/quicktime");
			cache_mime.insertar(".mp4","video/mp4");
			cache_mime.insertar(".mpe","video/mpeg");
			cache_mime.insertar(".mpeg","video/mpeg");
			cache_mime.insertar(".mpg","video/mpeg");
			cache_mime.insertar(".avi","video/avi");
			cache_mime.insertar(".mkv","video/x-matroska");
			cache_mime.insertar(".mpv","video/x-matroska");

			// Texto
			cache_mime.insertar(".htm","text/html");
			cache_mime.insertar(".html","text/html");
			cache_mime.insertar(".shtml","text/html");
			cache_mime.insertar(".css","text/css");
			cache_mime.insertar(".php","text/x-php");
			cache_mime.insertar(".cpp","text/x-c++src");
			cache_mime.insertar(".hpp","text/x-c++src");
			cache_mime.insertar(".cc","text/x-c++src");
			cache_mime.insertar(".c","text/x-csrc");
			cache_mime.insertar(".h","text/x-csrc");
			cache_mime.insertar(".srt","text/plain");
			cache_mime.insertar(".txt","text/plain");
			cache_mime.insertar(".log", "text/plain");
			cache_mime.insertar(".guru","text/plain");
			cache_mime.insertar(".ini","text/plain");
			cache_mime.insertar(".sh","text/x-script.sh");
			
			// Otros
			cache_mime.insertar(".js","application/javascript");
			cache_mime.insertar(".doc","application/msword");
			cache_mime.insertar(".swf","application/x-shockwave-flash");
			cache_mime.insertar(".tar","application/x-tar");
			cache_mime.insertar(".xht","application/xhtml+xml");
			cache_mime.insertar(".xhtml","application/xhtml+xml");
			cache_mime.insertar(".xml","application/xml");
			cache_mime.insertar(".zip","application/zip");
			cache_mime.insertar(".pps","application/mspowerpoint");
			cache_mime.insertar(".text","application/plain");
			cache_mime.insertar(".iso","application/x-iso9660-image");
			cache_mime.insertar(".pdf","application/pdf");
			cache_mime.insertar(".json","application/json");
		}

		try {
			return cache_mime(GuruDirectorio::GuruSplitPath(archivo).ext());
		}catch (GuruCacheException&) {
			return string("application/octet-stream");
		}

#endif
	}

	//////////////////////////////////////////////////////////////////////
	// Esta funcion arregla el directoro para que tenga la barra al final
	//////////////////////////////////////////////////////////////////////
	template<class T>
	inline T comoDirectorioHTTP(const T& str_directorio)
	{
		T tmp(str_directorio);

		if (str_directorio.size() == 0) return tmp;

		if (str_directorio.at(str_directorio.size()-1) != '/')
			tmp += '/';

		return tmp;
	};

	// first:
	//		0  ==
	//		-1 ==
	// second:
	//		0  == hasta el final
	//		-1 == desde el final
	//		>0 == posicion del archivo
	// -1,-1 == error en rango
	template <typename T>
	pair<uint64_t, uint64_t> RangeDecode(const T& valorAtributoRange) {
		string tmp(valorAtributoRange.substr(strlen("bytes=")));

		::uint64_t inicio = 0, fin = 0;
		inicio--; fin--;

		if (tmp.empty()) return make_pair(inicio, fin);

		// algunas implementaciones  pueden dar mas de un rango y retornarlo estilo formulario.
		// Ejem: 10-50, 100-220
		// NO IMPLEMENTADO
		if (tmp.find(',') != string::npos) return make_pair(inicio, fin);

		string	strtmp;

		string::iterator caracter = tmp.begin();

		if (*caracter != '-') { // me dice que NO es el final
			while (*caracter != '-' && caracter != tmp.end())
				strtmp += *caracter++;

			if (*caracter != '-') return make_pair(inicio, fin);

			inicio = _atoi64(strtmp.c_str());

			strtmp.clear();

			caracter++;

			while (caracter != tmp.end())
				strtmp += *caracter++;

			if (strtmp.empty()) // hasta el final 
				fin = 0;
			else
				fin = _atoi64(strtmp.c_str());
		} else { // es el rango final del archivo
			caracter++;
			while (caracter != tmp.end())
				strtmp += *caracter++;

			if (strtmp.empty()) return make_pair(inicio, fin);

			inicio = _atoi64(strtmp.c_str());
		}

		return make_pair(inicio, fin);
	}

	template <typename T>
	inline std::string URLEncode(const T& str) {
		ostringstream encstr;
		unsigned char c;

		if (str.empty()) return std::string();

		for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
			c = *it;
			if ((c >= '0') && (c <= '9')) encstr << c;
			else if ((c >= 'A') && (c <= 'Z')) encstr << c;
			else if ((c >= 'a') && (c <= 'z')) encstr << c;
			//		else if((c == '@') || (c == '.') || (c == '/') || (c == '\\') || (c == '-') || (c == '_') || (c == ':') ) encstr << c;
			else if ((c == '.') || (c == '-') || (c == '_')) encstr << c;
			else
				encstr << '%' << std::uppercase << setfill('0') << std::setw(2) << std::hex << static_cast<int>(c);
		}

		return string(encstr.str());
	}

	template <typename T>
	inline char _x2c(T hex_up, T hex_low)
	{
		char digit;

		digit = 16 * (hex_up >= 'A' ? ((hex_up & 0xdf) - 'A') + 10 : (hex_up - '0'));
		digit += (hex_low >= 'A' ? ((hex_low & 0xdf) - 'A') + 10 : (hex_low - '0'));

		return (digit);
	}

	template <typename T>
	inline std::string UrlDecode(const T& url) {
		string::const_iterator caracter(url.begin());
		string tmp_url;

		for (; caracter != url.end(); ++caracter) {
			switch (*caracter) {
			case '+': tmp_url += ' ';
				break;
			case '%': {
				caracter++;
				char first = *caracter++;
				char second = *caracter;
				tmp_url += _x2c(first, second);
			}
					  break;
			default:
				tmp_url += *caracter;
			}
		}

		return string(tmp_url);
	}

}

#endif
