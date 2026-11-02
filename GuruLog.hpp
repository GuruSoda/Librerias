#ifndef __GURULOG_HPP__
#define __GURULOG_HPP__

#include <GuruCompat.h>
#include <GuruDirectorio.hpp>
#include <GuruAlgo.hpp>
#include <GuruString_algo.hpp>
#include <GuruAcceso.hpp>
#include <GuruAsync.hpp>

#include <iostream> // para cout
#include <fstream>
#include <string>

#if defined __GURUWIN32__
#include <Windows.h>
#elif defined __GURULINUX__
#include <unistd.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
#endif

// Convencion: "%-5p [%t]: %m%n
//   DEBUG [main]: Message 1
//   WARN  [main]: Message 2
//
// 2014-07-02 20:52:39 DEBUG HelloExample:19 - This is debug : mkyong
// 2014-07-02 20:52:39 INFO  HelloExample:23 - This is info : mkyong
// 2014-07-02 20:52:39 WARN  HelloExample:26 - This is warn : mkyong
// 2014-07-02 20:52:39 ERROR HelloExample:27 - This is error : mkyong
// 2014-07-02 20:52:39 FATAL HelloExample:28 - This is fatal : mkyong
//
// Interesante info:
// http://www.codejava.net/coding/common-conversion-patterns-for-log4js-patternlayout
//
//
// PreProcesador:
// NOLOG -> No inlcluye los mensajes de log, ni los compila.
// GURULOG_CPP -> Se debe incluir en el proyecto el archivo GuruLog.cpp, esto es para poder incluir el GuruLog en archivos .cpp.
// GURULOG_FILENAME -> Agrega en la linea de los el nombre del archivo fuente y el numero de linea.

namespace GuruLog {

///////////////////[Definiciones]/////////////////////////////////////
#if !defined NOLOG
	#define GURULOG_TRACE(x) if (GuruLog::_nivel >= GuruLog::level_trace) GuruLog::trace(x, std::string(__FILE__) + ':' + GuruStringAlgo::itos(__LINE__))
#define GURULOG_DEBUG(x) if (GuruLog::_nivel >= GuruLog::level_debug) {GuruLog::debug(x, std::string(__FILE__) + ':' + GuruStringAlgo::itos(__LINE__));}
	#define GURULOG_INFO(x)  if (GuruLog::_nivel >= GuruLog::level_info) GuruLog::info(x, std::string(__FILE__) + ':' + GuruStringAlgo::itos(__LINE__))
	#define GURULOG_WARN(x)  if (GuruLog::_nivel >= GuruLog::level_warn) GuruLog::warn(x, std::string(__FILE__) + ':' + GuruStringAlgo::itos(__LINE__))
	#define GURULOG_ERROR(x) if (GuruLog::_nivel >= GuruLog::level_error) GuruLog::error(x, std::string(__FILE__) + ':' + GuruStringAlgo::itos(__LINE__))
	#define GURULOG_FATAL(x) if (GuruLog::_nivel >= GuruLog::level_fatal) GuruLog::fatal(x, std::string(__FILE__) + ':' + GuruStringAlgo::itos(__LINE__))
	#define GURULOG_ALL(x)   if (GuruLog::_nivel >= GuruLog::level_all) GuruLog::all(x, std::string(__FILE__) + ':' + GuruStringAlgo::itos(__LINE__))
#else
	#define GURULOG_TRACE(x) 
	#define GURULOG_DEBUG(x)
	#define GURULOG_INFO(x)
	#define GURULOG_WARN(x)
	#define GURULOG_ERROR(x)
	#define GURULOG_FATAL(x)
	#define GURULOG_ALL(x)

	// para concatenas macros
	// http://stackoverflow.com/questions/1489932/how-to-concatenate-twice-with-the-c-preprocessor-and-expand-a-macro-as-in-arg
#endif

	static const char* str_niveles[] = { "NONE", "FATAL", "ERROR", "WARN ", "INFO ", "DEBUG", "TRACE", "ALL  "};

	typedef enum { undefined = -1, level_none = 0, level_fatal, level_error, level_warn, level_info, level_debug, level_trace, level_all } nivel;


#if defined GURULOG_CPP
	extern GuruLog::nivel			_nivel;
#else 
	static GuruLog::nivel			_nivel = GuruLog::level_none;
#endif
/*
	////////////////// Prueba 1 ////////////////////////////
	static GuruLog::nivel* level_log() {
//		static GuruLog::nivel			nivel_estatico = GuruLog::level_none;
		static GuruLog::nivel* nivel_estatico = NULL;
		
		if (nivel_estatico == NULL)
			nivel_estatico = new GuruLog::nivel(GuruLog::level_none);

		return nivel_estatico;
	}

	/////////////////// Prueba 2 /////////////////////////
	class def_level {
	public:
		static GuruLog::nivel level () { return _nivel; }
		static void  level(GuruLog::nivel n) { _nivel = n; }

	protected:
		static GuruLog::nivel _nivel;
	};

	GuruLog::nivel def_level::_nivel = GuruLog::level_none;
*/
	// para cuando se redirije por pantalla
	static std::ostream		salida(clog.rdbuf());

	// para cuando se redirije a un archivo
	static std::string		archivo_nombre;
	static std::ofstream	archivo_salida;

	// formato de la linea de log
	static std::string		formato_linea("%fecha% %nivel% %archivo% - %msg%");

	// Acceso a la escritura, de a uno a la vez
	static	GuruBloqueo		acceso;

///////////////////[Funciones]////////////////////////////////////////
	// Inicializacion
	static void init(GuruLog::nivel pref, ostream& out=clog) { 

		if (archivo_salida.is_open()) archivo_salida.close();

		GuruLog::_nivel = pref;
//		def_level::level(pref);

//		*level_log() = pref;

//		GuruLog::nivel n = *level_log();

		if (pref == GuruLog::level_none) return;
		
		salida.rdbuf(out.rdbuf());
	}

	static void init(GuruLog::nivel pref, const std::string& nombre) {

		if (archivo_salida.is_open()) archivo_salida.close();

		GuruLog::_nivel = pref;

		if (pref == GuruLog::level_none) return;

		archivo_salida.open(nombre.c_str());

		if (archivo_salida.is_open())
			salida.rdbuf(archivo_salida.rdbuf());
			//init(pref, archivo_salida);
	}

	static void init_formato(const std::string& formato) { formato_linea = formato; }
	static void init_salida(ostream& out) { salida.rdbuf(out.rdbuf()); }
	static void init_salida(const string& archivo) { }
	// Destruccion

	// Funciones

	static GuruLog::nivel str_nivel(const string& str_nivel) {

		GuruLog::nivel nivel;

		if (GuruStringAlgo::string_lower(str_nivel).compare("all") == 0)
			nivel = GuruLog::level_all;
		else if (GuruStringAlgo::stringequals(str_nivel, std::string("trace")))
			nivel = GuruLog::level_trace;
		else if (GuruStringAlgo::stringequals(str_nivel, std::string("debug")))
			nivel = GuruLog::level_debug;
		else if (GuruStringAlgo::stringequals(str_nivel, std::string("info")))
			nivel = GuruLog::level_info;
		else if (GuruStringAlgo::stringequals(str_nivel, std::string("warn")))
			nivel = GuruLog::level_warn;
		else if (GuruStringAlgo::stringequals(str_nivel, std::string("error")))
			nivel = GuruLog::level_error;
		else if (GuruStringAlgo::stringequals(str_nivel, std::string("fatal")))
			nivel = GuruLog::level_fatal;
		else if (GuruStringAlgo::stringequals(str_nivel, std::string("none")))
			nivel = GuruLog::level_none;
		else
			nivel = GuruLog::level_none;

		return nivel;
	}

	static std::string str_nivel(GuruLog::nivel level) {
		if (level >= GuruLog::level_none && level <= GuruLog::level_all)
			return str_niveles[level];
		else
			return str_niveles[level];

		return string();
	}

#if defined __GURUWIN32__
	static uint64_t id() { return static_cast<uint32_t>(GetCurrentThreadId()); }
#elif defined __GURULINUX__
	static uint64_t id() { return static_cast<uint64_t>(gettid()); }
#endif

	static string str_fechaHora() {
		return GuruAlgo::armarFecha("%DD%/%MM%/%AA% %H%:%M%:%S%");
	}

	static string str_id() {
		return std::string("[" + GuruStringAlgo::itos(GuruLog::id()) + "]");
	}

	static void escribir(GuruLog::nivel level,const string& msg, const string& info="") {
		acceso.bloquear();

		salida << str_fechaHora() << " " << str_niveles[level] << " ";

		if (!info.empty()) salida << GuruDirectorio::GuruSplitPath(info).nombre_ext() << " ";

		salida << "- " << GuruStringAlgo::trim_copy(msg) << endl;

		acceso.desbloquear();
	}

	// Si estas funciones son llamdas directamente no se agrega en nombre del fuente y linea.
#if !defined NOLOG
	inline static void trace(const string& msg, const std::string& info="")		{ if (GuruLog::_nivel >= GuruLog::level_trace) escribir(GuruLog::level_trace, msg, info); }
	inline static void debug(const string& msg, const std::string& info="")		{ if (GuruLog::_nivel >= GuruLog::level_debug) escribir(GuruLog::level_debug, msg, info); }
	inline static void info(const string& msg, const std::string& info = "")	{ if (GuruLog::_nivel >= GuruLog::level_info) escribir(GuruLog::level_info, msg, info);  }
	inline static void warn(const string& msg, const std::string& info = "")	{ if (GuruLog::_nivel >= GuruLog::level_warn) escribir(GuruLog::level_warn, msg, info);  }
	inline static void error(const string& msg, const std::string& info = "")	{ if (GuruLog::_nivel >= GuruLog::level_error) escribir(GuruLog::level_error, msg, info); }
	inline static void fatal(const string& msg, const std::string& info = "")	{ if (GuruLog::_nivel >= GuruLog::level_fatal) escribir(GuruLog::level_fatal, msg, info); }
	inline static void all(const string& msg, const std::string& info = "")		{ if (GuruLog::_nivel >= GuruLog::level_all) escribir(GuruLog::level_all, msg, info);   }
#else
	inline static void trace(const string& msg, const std::string& info = "") { }
	inline static void debug(const string& msg, const std::string& info = "") { }
	inline static void info(const string& msg, const std::string& info = "") { }
	inline static void warn(const string& msg, const std::string& info = "") { }
	inline static void error(const string& msg, const std::string& info = "") { }
	inline static void fatal(const string& msg, const std::string& info = "") { }
	inline static void all(const string& msg, const std::string& info = "") { }
#endif
}

#endif
