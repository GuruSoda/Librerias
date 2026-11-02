#ifndef __GURUSISTEMAFUNCIONES_HPP__
#define __GURUSISTEMAFUNCIONES_HPP__

#include <GuruInterprete/GuruInterprete.hpp>
#include <GuruCompat.h>
#include <GuruVarEntorno.hpp>
#include <GuruVariable.hpp>
#include <GuruString_algo.hpp>
#include <GuruAlgo.hpp>

#if defined __GURUWIN32__
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef _WINSOCKAPI_
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
	#endif

	#include <windows.h>

#elif defined __GURULINUX__
	#include <unistd.h>
	#include <limits.h>
#endif

#include <iomanip>

using namespace std;

class GuruSistemaFunciones: public GuruInterprete::GuruFunciones<GuruSistemaFunciones> {

public:
	GuruSistemaFunciones () {
		cargar_funcion(this, &GuruSistemaFunciones::hostname, "sis.hostname");
		cargar_funcion(this, &GuruSistemaFunciones::variable, "sis.var");
		cargar_funcion(this, &GuruSistemaFunciones::fecha, "sis.fecha");
	};

	virtual ~GuruSistemaFunciones() {};

	string variable(const vector<string>& param) {
		return GuruVarEntorno::variable(param.at(0));
	}

	string hostname(const vector<string>& param) {
			std::string tmp;
		#if defined __GURUWIN32__
			#if defined __GURUUNICODE__
			TCHAR nombre[MAX_COMPUTERNAME_LENGTH + 1];
			DWORD largo = MAX_COMPUTERNAME_LENGTH;
			if (GetComputerName(nombre, &largo))
				tmp = GuruStringAlgo::unicode_ansi(nombre);
			#else
			char nombre[MAX_COMPUTERNAME_LENGTH + 1];
			DWORD largo=MAX_COMPUTERNAME_LENGTH;
			if (GetComputerName(nombre, &largo))
				tmp=nombre;
			#endif
		#elif defined __GURULINUX__
			char nombre[HOST_NAME_MAX];
			if (gethostname(nombre, HOST_NAME_MAX) == 0)
				tmp=nombre;
		#endif

		return tmp;
	}

	string fecha(const vector<string>& param) {
		time_t epoc=0;

		if (param.size() == 2) 
			epoc=_atoi64(param.at(1).c_str());

		return GuruAlgo::armarFecha(param.at(0), epoc);
	}

};

#endif
