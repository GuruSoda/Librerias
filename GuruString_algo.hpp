#ifndef __GURUSTRING_ALGO_HPP__
#define __GURUSTRING_ALGO_HPP__

#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>
#include <sstream>
#include <locale>

#if __cplusplus > 201103L
#include <codecvt>
#endif

#include <GuruCompat.h>
#include <GuruBuffer.hpp>

#if defined __GURUWIN32__
// Para compilar Windows.h y Winsock2.h en el mismo fuente
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef _WINSOCKAPI_
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
	#endif
	
	#include <windows.h>

#elif defined __GURULINUX__
	#include <utf8cpp/utf8.h>
#endif

using namespace std;

namespace GuruStringAlgo {

	inline string comillas(const string& cadena) {
		return string("\"" + cadena + "\"");
	};

	inline string comillas_dobles(const string& cadena) {
		return string("\"" + cadena + "\"");
	};

	inline string comilla_simple(const string& cadena) {
		return string("\'" + cadena + "\'");
	};

	// Prueba para crear un objeto string que s4a case insensitive
	struct ci_char_traits : public char_traits<char> {
		static bool eq(char c1, char c2) { return toupper(c1) == toupper(c2); }
		static bool ne(char c1, char c2) { return toupper(c1) != toupper(c2); }
		static bool lt(char c1, char c2) { return toupper(c1) < toupper(c2); }

		static int compare(const char* s1, const char* s2, size_t n) {
			while (n-- != 0) {
				if (toupper(*s1) < toupper(*s2)) return -1;
				if (toupper(*s1) > toupper(*s2)) return 1;
				++s1; ++s2;
			}
			return 0;
		}

		static const char* find(const char* s, int n, char a) {
			while (n-- > 0 && toupper(*s) != toupper(a)) {
				++s;
			}
			return s;
		}
	};

	// otro ejemplo de string personaliado
	// sacado de http://stackoverflow.com/questions/4590472/implement-string-class-with-custom-behavior
	struct sequence_traits : char_traits<char>
	{
		//'a' and 'z' are equal
		//'b' and 'y' are equal
		//'c' and 'x' are equal, and so on.
		//that implies, 'a' + 'z' == 'b' + 'y' == 'c' + 'x' == 'd'  + 'w == so on
		//same for upper cases!
		static bool eq(const char& left, const char& right)
		{
			return (left == right) || (left + right == 'a' + 'z') || (left + right == 'A' + 'Z');
		}

		static int compare(const char *first1, const char *first2, size_t count)
		{
			for (; 0 < count; --count, ++first1, ++first2)
				if (!eq(*first1, *first2))
					return (lt(*first1, *first2) ? -1 : +1);
			return (0);
		}
	};

	typedef std::basic_string<char, ci_char_traits> istring;

	/////////////////////////////////////////////////////////////////////////////////
	// Compara dos string, sin importar mayusculas, minusculas y locales
	/////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	class CompareChar {
		const std::locale& loc_;
	public:
		CompareChar(const std::locale& loc = std::locale()) :loc_(loc) {}
		virtual ~CompareChar() {}

		inline bool operator () (const T& a, const T& b) {
			return(toupper(a, loc_) == toupper(b, loc_));
		}
	};

	template <class T>
	inline bool stringequals(const T& s1, const T& s2) {
		return((s1.size() == s2.size()) && equal(s1.begin(), s1.end(), s2.begin(), CompareChar<typename T::value_type>()));
	}

	// find substring(case insensitive)
	//  Ejem: int f1 = ci_find_substr(cadena, subcadena);
	template<typename T>
	inline typename T::size_type ci_find_substr(const T& str1, const T& str2) {
		typename T::const_iterator it = std::search(str1.begin(), str1.end(), str2.begin(), str2.end(), CompareChar<typename T::value_type>());

		if (it != str1.end())
			return static_cast<typename T::size_type>(it - str1.begin()); // Posicion donde empieza la cadena encontrada
		else
			return T::npos; // NO existe una subcadena
	}

	template <typename T>
	inline std::string int64astr(const T numero)
	{
		// uint64_t = 18446744073709551615 (21 Digitos)
		int cant_digitos = 0;
		T tmp = numero;

		while ((tmp / 10) != 0) {
			cant_digitos++;
			tmp = (tmp / 10);
		}

		cant_digitos++;

		tmp = numero;

		string buffer;

		for (int i = 0; i < cant_digitos; i++) {
			buffer += (char)((tmp % 10) + '0');
			tmp = (tmp / 10);
		}

		std::reverse(buffer.begin(), buffer.end());

		return buffer;
	}

	//////////////////////////////////////////////////////////////////////
	// Esta funcion utiliza ostringstream para convertir de entero a string
	template <typename T>
	inline std::string itoss(const T entero) {
		std::ostringstream oss;
		oss << entero;
		return oss.str();
	}

	template <typename T>
	inline std::string itos(const T n) {
		return GuruStringAlgo::int64astr(n);
	}

	template <typename T>
	inline std::string intastr(const T n) {
		return GuruStringAlgo::int64astr(n);
	}


	/////////////////////////////////////////////////////////////////////
	// Genera una cadena con letras minusculas al azar, default 20 caracteres
	/////////////////////////////////////////////////////////////////////
	inline string rand_str_min(int largo = 20) {
		string tmp;
		std::srand(static_cast<unsigned int>(std::time(0)));

		for (int i = 0; i < largo; i++) tmp += char((std::rand() % 25) + 97); // solo letras minusculas

		return tmp;
	}

	/////////////////////////////////////////////////////////////////////
	// Genera una cadena con numeros al azar, default 20 caracteres
	/////////////////////////////////////////////////////////////////////
	inline string rand_str_int(int largo = 20) {
		string tmp;
		std::srand(static_cast<unsigned int>(std::time(0)));

		for (int i = 0; i < largo; i++) tmp += char((std::rand() % 10) + 48); // solo numeros

		return tmp;
	}

	/////////////////////////////////////////////////////////////////////
	// Genera una cadena con numeros, letras mayusculas y minusculas al azar, default 20 caracteres
	/////////////////////////////////////////////////////////////////////
	inline string rand_str(int largo = 20) {
		string tmp;
		static char ascii[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

		std::srand(static_cast<unsigned int>(std::time(0)));

		for (int i = 0; i < largo; i++) tmp += ascii[std::rand() % 62];

		return tmp;
	}

	template <class T>
	static T trim_copy(const T& param) {

		if (param.empty()) return param;

		T tmp(param);
		string::iterator it(tmp.begin());

		while (isspace(static_cast<unsigned char>(*it)) || iscntrl(static_cast<unsigned char>(*it))) {
			tmp.erase(it);
		}

		it = tmp.end();
		it--;

		while (isspace(static_cast<unsigned char>(*it)) || iscntrl(static_cast<unsigned char>(*it))) {
			tmp.erase(it);
			it = tmp.end();
			it--;
		}

		return tmp;
	}

	template <class T>
	static T& trim(T& param) {

		if (param.empty()) return param;

		string::iterator it(param.begin());

		while (isspace(static_cast<unsigned char>(*it)) || iscntrl(static_cast<unsigned char>(*it))) {
			param.erase(it);
			if (param.empty()) return param;
		}

		it = param.end();

		if (it != param.begin()) it--;

		while (isspace(static_cast<unsigned char>(*it)) || iscntrl(static_cast<unsigned char>(*it))) {
			param.erase(it);
			it = param.end();
			if (it != param.begin()) it--;
		}

		return param;
	}

	template <class T>
	static T string_lower(const T& param) {
		T tmp(param);

		transform(param.begin(), param.end(), tmp.begin(), ::tolower);

		return tmp;
	}

	template <class T>
	static T string_lower_loc(const T& param) {
		const std::locale loc_;

		T tmp;

		for (std::string::const_iterator it = param.begin(), itend = param.end(); it != itend; ++it) tmp += std::tolower(*it, loc_);

		return tmp;
	}

	template <class T>
	static T string_upper(const T& param) {
		T tmp(param);

		transform(param.begin(), param.end(), tmp.begin(), ::toupper);

		return tmp;
	}

	template <class T>
	static T string_upper_loc(const T& param) {
		const std::locale loc_;

		T tmp;

		for (std::string::const_iterator it = param.begin(), itend = param.end(); it != itend; ++it) tmp += std::toupper(*it, loc_);

		return tmp;
	}

#if __cplusplus > 201103L

	static wstring s2ws(const std::string& str) {
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.from_bytes(str);
	}

	static string ws2s(const std::wstring& wstr) {
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.to_bytes(wstr);
	}
#endif

#if defined __GURUWIN32__
/*
	otro ejemplo:
	std::wstring s2ws(const std::string& s) {
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
	}

	std::wstring stemp = s2ws(myString);
	LPCWSTR result = stemp.c_str();
*/

	// Extraido de
	// http://stackoverflow.com/questions/23264818/storing-unicode-utf-8-string-in-stdstring?noredirect=1&lq=1
	static std::string toUtf8(const std::wstring &str) {
		std::string ret;
		int len = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0, NULL, NULL);
		if (len > 0) {
			ret.resize(len);
			WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.length(), &ret[0], len, NULL, NULL);
		}
		return ret;
	}

	static std::wstring ansi_unicode(const std::string& str_ansi) {
		GuruBufferWChar bufferW;
		GuruBufferChar bufferA(str_ansi.size());

		bufferA.agregar(str_ansi.c_str(), str_ansi.size() * sizeof(char));

		int bytes = MultiByteToWideChar(CP_ACP, 0, &str_ansi[0], (int)str_ansi.size(), NULL, 0);
		if (bytes > 0) {
			bufferW.reservar(bytes + 2);
			MultiByteToWideChar(CP_ACP, 0, bufferA.begin_data(), (int)str_ansi.size(), bufferW.buf(), bytes);
			return std::wstring(bufferW.begin_data());
		} else
			return std::wstring();
	}

	static std::wstring ansi_unicode(const char* str_ansi) {
		size_t		len_ansi = strlen(str_ansi);
		GuruBufferWChar bufferW;
		GuruBufferChar bufferA(len_ansi);

		bufferA.agregar(str_ansi, len_ansi * sizeof(char));

		int bytes = MultiByteToWideChar(CP_ACP, 0, &str_ansi[0], (int)len_ansi, NULL, 0);
		if (bytes > 0) {
			bufferW.reservar(bytes + 2);
			MultiByteToWideChar(CP_ACP, 0, bufferA.begin_data(), (int)len_ansi, bufferW.buf(), bytes);
			return std::wstring(bufferW.begin_data());
		} else
			return std::wstring();
	}

	// http://forums.codeguru.com/showthread.php?231165-Windows-SDK-String-How-to-convert-between-ANSI-and-UNICODE-strings
	// Otra forma de convertir (portable)
	//	size_t wcstombs_s(char *mbstr, const wchar_t *wcstr, size_t count	);
	// std::string ws(s.size(), L' '); ws.resize(mbstowcs(&ws[0], s.c_str(), s.size());
	//	ejem: https://vctipsplusplus.wordpress.com/tag/widechartomultibyte-example/
	// * Anteior a C++11, el compilador no garantiza que la memoria sea contigua.
	// string -> wstring:
	// http://stackoverflow.com/questions/2573834/c-convert-string-or-char-to-wstring-or-wchar-t
	static std::string unicode_ansi(const std::wstring& str_unicode) {
		GuruBufferWChar bufferW(str_unicode.size() + 2);
		GuruBufferChar bufferA;

		bufferW.agregar(str_unicode.c_str(), str_unicode.size() * sizeof(wchar_t));

		if (str_unicode.empty()) return std::string();

		int bytes = WideCharToMultiByte(CP_ACP, 0, &str_unicode[0], -1, NULL, 0, NULL, NULL);
		if (bytes > 0) {
			bufferA.reservar(bytes + 2);
			WideCharToMultiByte(CP_ACP, 0, bufferW.begin_data(), (int)str_unicode.size(), bufferA.buf(), bytes, NULL, NULL);
			return std::string(bufferA.begin_data());
		} else
			return std::string();
	}

	static std::string unicode_ansi(const char* str_unicode) {
		char* str_utf8 = NULL;
		string tmp;

		if (str_unicode) {
			int str_utf8_len = WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)str_unicode, -1, NULL, 0, NULL, NULL);
			if (str_utf8_len > 0) {
				str_utf8 = (char*)malloc(str_utf8_len * sizeof(char));
				if (str_utf8) {
					if (WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)str_unicode, -1, str_utf8, str_utf8_len, NULL, FALSE) == 0) {
						tmp = str_utf8;
						free(str_utf8);
					}
				}
			}
		}

		return tmp;
	}

#else
	static std::wstring ansi_unicode(const std::string& str_ansi) {
		// Convert it to utf-16
		std::wstring utf16line;
		utf8::utf8to16(str_ansi.begin(), str_ansi.end(), back_inserter(utf16line));

		return utf16line;
	}

	static std::wstring ansi_unicode(const char* str_ansi) {
		string utf8line(str_ansi);
		std::wstring utf16line;
		utf8::utf8to16(utf8line.begin(), utf8line.end(), back_inserter(utf16line));

		return utf16line;
	}

	static std::string unicode_ansi(const std::wstring& str_unicode) {
		string utf8line;
		utf8::utf16to8(str_unicode.begin(), str_unicode.end(), back_inserter(utf8line));

		return utf8line;
	}
#endif

#if defined __GURUUNICODE__
	// convierte el actual codigo de caracteres a string
	static string actual_string(const wstring& str ) {
		return GuruStringAlgo::unicode_ansi(str);
	}
#else
	// convierte el actual codigo de caracteres a string
	static string actual_string(const string& str) {
		return str;
	}
#endif


	/*
	Verifica que todos los caracteres de la cadena sean numeros.
	*/
	template <class T>
	bool numerico(const T& cadena) {
		bool resultado=true;	

		for (string::const_iterator it=cadena.begin();it!=cadena.end();++it) {
			if (!isdigit(static_cast<unsigned char>(*it))) {
				resultado=false;
				break;
			}
		}
		return resultado;
	}

	/************************************************************************/
	/* Comparator for case-insensitive comparison in STL assos. containers  */
	/* http://stackoverflow.com/questions/1801892/making-mapfind-operation-case-insensitive */
	/************************************************************************/
	struct ci_less
	{
	  // case-independent (ci) compare_less binary function
	  struct nocase_compare
	  {
		bool operator() (const unsigned char& c1, const unsigned char& c2) const {
			return tolower (c1) < tolower (c2);
		}
	  };
	  bool operator() (const std::string & s1, const std::string & s2) const {
		return std::lexicographical_compare
		  (s1.begin (), s1.end (),   // source range
		  s2.begin (), s2.end (),   // dest range
		  nocase_compare ());  // comparison
	  }
	};
}

#endif
