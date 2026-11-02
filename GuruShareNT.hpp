#ifndef __GURUSHARENT_HPP__
#define __GURUSHARENT_HPP__

#include <string>

#include <lmshare.h>


namespace GuruShareNT {

	static std::string rutaShare(const std::string& recurso, const std::string& computadora="") {
		PSHARE_INFO_502	BufPtr = { 0 };
		// sacado de aqui:
		// http://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte

		int size_needed = MultiByteToWideChar(CP_UTF8, 0, computadora.c_str(), -1, NULL, 0);
		std::wstring wscomputadora(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, computadora.c_str(), -1, &wscomputadora[0], size_needed);

		size_needed = MultiByteToWideChar(CP_UTF8, 0, recurso.c_str(), -1, NULL, 0);
		std::wstring wsresurso(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, recurso.c_str(), -1, &wsresurso[0], size_needed);

		NET_API_STATUS salida;
		if ((salida = NetShareGetInfo(&wscomputadora[0], &wsresurso[0], 502, (LPBYTE *)&BufPtr)) != ERROR_SUCCESS)
			throw GuruError().descripcion(salida);

		size_needed = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)BufPtr->shi502_path, -1, NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)BufPtr->shi502_path, -1, &strTo[0], size_needed, NULL, NULL);

		if ((salida = NetApiBufferFree(BufPtr)) != ERROR_SUCCESS)
			throw GuruError().descripcion(salida);

		if (*strTo.rbegin() == '\0') {
			return strTo.substr(0, strTo.length() - 1);
		} else 
			return strTo;
	}

};

#endif
