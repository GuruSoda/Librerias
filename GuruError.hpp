#ifndef __GURUERROR_H__
#define __GURUERROR_H__

#include <GuruCompat.h>

#include <GuruString_algo.hpp>
#include <GuruLog.hpp>

//////////////////////////////////////////////////////////////////////
// Linkear con Mpr.lib
//////////////////////////////////////////////////////////////////////

const char NO_MSG_ERROR[] = "Error obteniendo mensaje desde el S.O.: ";
const uint32_t GURUERROR_USUARIO = 0x8000000;

#if defined __GURUWIN32__
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef _WINSOCKAPI_
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
	#endif

	#include <Windows.h>
	#include <lmcons.h>
#elif defined __GURULINUX__
	#include <errno.h>
	#include <string.h>
	#include <stdint.h>

// Para las llamadas al stacktrace
	#include <execinfo.h>
	#include <unistd.h>
#endif

#include <iostream>
#include <string>

using namespace std;

class GURU_API GuruError
{
public:
	GuruError():numero_de_error(GURUERROR_USUARIO){	};

	GuruError(GuruError& copia){
		numero_de_error = copia.numero_de_error;
		descripcion_mensaje = copia.descripcion_mensaje;
	}

	GuruError(const string& mensaje_error, uint32_t numero_error = 0){
		descripcion_mensaje = mensaje_error;
		numero_de_error		= (numero_error) ? numero_error : --numero_error;
	}

	GuruError(const char* mensaje_error, uint32_t numero_error = 0){
		descripcion_mensaje 	= mensaje_error;
		numero_de_error		= (numero_error) ? numero_error : --numero_error;
	}


	virtual ~GuruError() {};

#if defined __GURULINUX__
	static void stacktrace() {
		void *array[50];
		size_t size;

		// get void*'s for all entries on the stack
		size = backtrace(array, 50);

		// print out all the frames to stderr
		backtrace_symbols_fd(array, size, STDERR_FILENO);
	}
#endif

//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar la descripcion del error pasado por parametro
// Parametros:	Numero de error
// Retorna:		Un puntero a la cadena que contiene la descripcion del error, (cadena interna a la clase)
//////////////////////////////////////////////////////////////////////
#ifdef __GURUWIN32__
	const string&	descripcion(uint32_t error = GetLastError()) {
#elif defined __GURULINUX__
    const string&   descripcion(uint32_t error = errno) {
#endif
		uint32_t cant_caracteres;
	#ifdef __GURUWIN32__
		TCHAR* Msg;

		cant_caracteres = FormatMessage(
						FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK ,
						NULL,
						error,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						(LPTSTR) &Msg,
						0,
						NULL
						);
	#elif defined __GURULINUX__
		cant_caracteres = strlen(strerror(error));
	#endif

		numero_de_error = error;

		if (cant_caracteres) {
			#ifdef __GURUWIN32__
				#if defined __GURUUNICODE__
					descripcion_mensaje = GuruStringAlgo::unicode_ansi(Msg);
				#else
					descripcion_mensaje = Msg;
				#endif
				LocalFree( Msg );
			#elif defined __GURULINUX__
				descripcion_mensaje = strerror(error);
			#endif
		} else {
			GURULOG_DEBUG("Error en descripcion: " + GuruStringAlgo::itos(error));

			descripcion_mensaje = NO_MSG_ERROR + GuruStringAlgo::itos(error);
		}

		return descripcion_mensaje;
	}
#ifdef __GURUWIN32__
	const string&	descripcion(uint32_t error, const string& libreria, char* parametros[] = NULL) {
		uint32_t	cant_caracteres = 0;
		TCHAR*		Msg = NULL;
		HMODULE		modulo;

		numero_de_error = error;

		unsigned int anterior_modo_error = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

		modulo = LoadLibraryEx((LPTSTR)libreria.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);

		if (modulo) {
			cant_caracteres = FormatMessage(
						FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_MAX_WIDTH_MASK,
						modulo,
						error,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						(LPTSTR) &Msg,
						0,
						(va_list*)parametros
						);
		} else {
			descripcion(GetLastError());

			SetErrorMode(anterior_modo_error);

			return descripcion_mensaje;
		}

		if (cant_caracteres == 0) {
			if (!descripcion(GetLastError()).empty()) {
				if (modulo) FreeLibrary(modulo);

				return descripcion_mensaje;
			}
		}

		if (cant_caracteres) {
		#if defined __GURUUNICODE__
			descripcion_mensaje = GuruStringAlgo::unicode_ansi(Msg);
		#else
			descripcion_mensaje = Msg;
		#endif

			LocalFree( Msg );
		} else {		
			descripcion_mensaje = NO_MSG_ERROR + GuruStringAlgo::itos(error);
		}

		if (modulo)
			FreeLibrary(modulo);

		SetErrorMode(anterior_modo_error);

		return descripcion_mensaje;
	};
#endif
	const string&	cargar_descripcion(const string& nuevo_mensaje_error, uint32_t numero_error = GURUERROR_USUARIO){
		descripcion_mensaje = nuevo_mensaje_error;
		numero_de_error = numero_error;

		return descripcion_mensaje;
	}

	const string&	mensaje(){ return descripcion_mensaje;}
	uint32_t		numero(){ return numero_de_error;}

private:	// atributos
	uint32_t	numero_de_error;
	string		descripcion_mensaje;
};

#endif
