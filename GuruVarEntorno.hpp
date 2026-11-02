#ifndef __GURUVARENTORNO_H__
#define __GURUVARENTORNO_H__

#include <GuruCompat.h>
#include <GuruString_algo.hpp>

#include <string>
#include <iostream> // cout

#if defined __GURUWIN32__
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef _WINSOCKAPI_
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
	#endif

	#include <windows.h>

#elif defined __GURULINUX__
 #include <stdlib.h>
#endif

class GuruVarEntorno
{
public:
	GuruVarEntorno(void) {} ;
	virtual ~GuruVarEntorno(void) {};

#ifdef __GURUWIN32__
	static std::string variable(const std::string& strVar) {
	#if defined __GURUUNICODE__
/*
		DWORD			bufferSize = 32767; //Limit according to http://msdn.microsoft.com/en-us/library/ms683188.aspx
		std::wstring	buff;
		std::wstring	varEntorno(strVar.begin(), strVar.end());
		buff.resize(bufferSize);

		bufferSize = GetEnvironmentVariableW(varEntorno.c_str(), &buff[0], bufferSize);
		if (bufferSize) buff.resize(bufferSize);

		return string(buff.begin(), buff.end());
*/
		TCHAR varent[32767] = { 0 };
		if (GetEnvironmentVariable(GuruStringAlgo::ansi_unicode(strVar).c_str(), varent, MAX_PATH))
			return GuruStringAlgo::unicode_ansi(wstring(varent));
		else
			return string();
	#else
		char varent[MAX_PATH] = { 0 };
		if (GetEnvironmentVariable(strVar.c_str(), varent, MAX_PATH))
			return string(varent);
		else
			return string();
	#endif

	}

	static std::string reemplazar(const std::string& strConVar) {
		std::string variable;
		std::string cambiar(strConVar);

		std::string::size_type inicio,fin,indice=0;

		do{
			inicio = cambiar.find('%', indice);
			if (inicio==std::string::npos) break;

			fin = cambiar.find('%', inicio+1);
			if (fin==std::string::npos) break;

			if (fin == inicio+1) {
				cambiar.erase(inicio,1);
				indice = fin+1;
				continue;
			}

			variable = cambiar.substr(inicio+1, (fin-inicio)-1);

			string entrada(GuruVarEntorno::variable(variable));

			if (entrada.empty())
				indice = inicio + (fin-inicio)+1;
			else {
				cambiar.replace(inicio, (fin - inicio) + 1, entrada);
				indice = inicio + entrada.length();
			}
			
		}while (8);

		return cambiar;
	}

	#elif defined __GURULINUX__
	static std::string variable(const std::string& strVar) {
		char* varent;

		varent=getenv(strVar.c_str());
		if (varent != NULL)
			return string(varent);
		else
			return string();
	}

	static std::string reemplazar(const std::string& strConVar)
	{
		std::string varent;
		std::string variable;
		std::string cambiar(strConVar);

		std::string::size_type inicio,fin,indice=0;

		do{
			inicio = cambiar.find('$', indice);
			if (inicio==std::string::npos) break;

			fin=inicio+1;
			while ((cambiar.at(fin) != std::string::npos) && isalpha(cambiar.at(fin++)))
				;

			if (fin==std::string::npos) break;

			variable = cambiar.substr(inicio+1, (fin-inicio)-2);

			char* tmp = getenv(variable.c_str());

			if (tmp) {
				varent=tmp;

				cambiar.erase(inicio, (fin-inicio)-1);
				cambiar.insert(inicio, varent);
			}

			indice=fin+1;
		}while (8);

		return cambiar;
	}	
	#endif
};

#endif
