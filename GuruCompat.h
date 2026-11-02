//////////////////////////////////////////////////////////////////////
// GuruCompat
// Header que permite la compatibilidad de C/C++ para distintos
// Sistemas Operativos
//////////////////////////////////////////////////////////////////////
#ifndef	__GURUCOMPATIBILIDAD__
#define __GURUCOMPATIBILIDAD__

// #include "GuruTiposDeDatos.h"

//////////////////////////////////////////////////////////////////////
// *******************************************************************
// Etiquetas para compilar en Windows (Win32)
// *******************************************************************
//////////////////////////////////////////////////////////////////////
#if (defined __WIN32__) || (defined WIN32) || (defined _WIN32)

// Para no tener errores con la compilacion de sockets, include de winsock2.h y windows.h
// Para compilar Windows.h y Winsock2.h en el mismo fuente
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms737629(v=vs.85).aspx
// otro dato:
// https://blogs.msdn.microsoft.com/oldnewthing/20091130-00/?p=15863
//	#ifndef WIN32_LEAN_AND_MEAN
//		#define WIN32_LEAN_AND_MEAN
//	#endif

	#ifndef __AUTOR_H__
		#define		__AUTOR_H__
		static char autor[] = "Autor: Fabricio D. Rossi (El Guru)";
	#endif

// Esta etiqueta define que estoy en windows
	#define	__GURUWIN32__

	#if defined(_UNICODE) || defined(UNICODE)
		#define __GURUUNICODE__
	#endif

	#ifdef _MSC_VER  // Estoy usando el Visual C++???
		// para deshabilitar todos los warning los que saben dicen:
		// definir las siguientes etiquetas:
		// _CRT_SECURE_NO_DEPRECATE		   
		// _CRT__NONSTDC_NO_DEPRECATE
		// _USE_32BI_T_TIME_T

		#pragma warning(disable: 4786) // error en la compilacion de templates stl
		#pragma warning(disable: 4290) // warning C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
		#pragma warning(disable: 4996) // warning C4996: 'foo' was declared deprecated
	#endif

// Esta etiqueta "Visual C++" lo define por cada clase que crea
	#if _MSC_VER >= 1000
		#pragma once
	#endif // _MSC_VER >= 1000

// Esta etiqueta sirve para crear librerias (class GURU_API GuruError {})
	#ifdef GURU_EXPORTS
		#define GURU_API __declspec(dllexport)
		#pragma warning( disable: 4251 )
	#else
		#define GURU_API //__declspec(dllimport)
	#endif

// Compatibilidad con el standard de C
	#define		stat64	_stat64

// Tipos de datos
	#if defined(_MSC_VER) && (_MSC_VER < 1300)	// MSVC 6 should be < 1300
		typedef short int16_t;
		typedef unsigned short uint16_t;
		typedef int int32_t;
		typedef unsigned int uint32_t;
	#endif

// Para mas informacion:
// Fundamental Types: https://msdn.microsoft.com/en-us/library/cc953fe1(v=vs.100).aspx
// Data Type Ranges:  https://msdn.microsoft.com/en-us/library/s3f49ktz(v=vs.100).aspx
//
	typedef signed char				int8_t;			// 128 - 127
	typedef unsigned char			uint8_t;		// 0 - 255
	typedef __int16					int16_t;		// 32,768 - 32,767
	typedef unsigned __int16		uint16_t;		// 0 - 65,535
	typedef __int32					int32_t;		// 2,147,483,648 - 2,147,483,647
	typedef unsigned __int32		uint32_t;		// 0 - 4,294,967,295
	typedef __int64					int64_t;		// 9,223,372,036,854,775,808 - 9,223,372,036,854,775,807
	typedef unsigned __int64		uint64_t;		// 0 - 18,446,744,073,709,551,615

	#define DIVISOR_DIRECTORIO		'\\'

	#define	MSG_NOSIGNAL			0

//////////////////////////////////////////////////////////////////////
// *******************************************************************
// Etiquetas para compilar en Linux
// *******************************************************************
//////////////////////////////////////////////////////////////////////
#elif ((defined linux) || (defined _AIX) || (defined __linux) )
// #elif ((defined linux) || (defined _AIX))
    #define __GURULINUX__

	#include <stdint.h>
	#include <errno.h>

// Etiqueta vacia para no definir .dll en linux (Obvio !!!!!)
    #define GURU_API

	#define	WINAPI

// Esta etiqueta sirve para la funcion stricmp que no existe en linux
    #define stricmp strcasecmp
    #define strnicmp strncasecmp

    #define  MAX_PATH   260
    #define  _MAX_FNAME	254
    #define  _MAX_EXT	254
	#define  _MAX_DIR	254
	#define	 _MAX_DRIVE  10

// sacado de winsock.h de microsoft (en linux no existe)

	#define		SOCKET                  	int
	#define		INVALID_SOCKET  		(SOCKET)(~0)
	#define		SOCKET_ERROR            	(-1)

	#define 	Sleep(dwMilliseconds) 		usleep(dwMilliseconds*static_cast<useconds_t>(1000)) // 1 Segundo: 1000000
	#define		closesocket(Guru_Socket) 	close(Guru_Socket);
	#define		WSAGetLastError()			GetLastError()
	#define		GetLastError()			errno
	#define		_atoi64				atoll
	#define		OutputDebugString(X)

	#define		DIVISOR_DIRECTORIO			'/'
//////////////////////////////////////////////////////////////////////
#endif // defined linux | win32

#endif // #ifndef	__GURUCOMPATIBILIDAD__
