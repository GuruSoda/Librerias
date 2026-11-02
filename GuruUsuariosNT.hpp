#ifndef __GURUUSUARIOSNT_HPP__
#define __GURUUSUARIOSNT_HPP__

//////////////////////////////////////////////////////////////////////
//	Link: Netapi32.lib
//////////////////////////////////////////////////////////////////////

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

/* // otra opcion para lo anterior
#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <windows.h>
#include <winsock2.h>
*/

#include <string>

#include <Windows.h>
#include <lm.h>
#include <Lmaccess.h>
#include <Lmapibuf.h>

#include <GuruNT.hpp>
#include <GuruDominioNT.hpp>
#include <GuruBuffer.hpp>
#include <GuruError.hpp>
#include <GuruString_algo.hpp>

namespace GuruUsuariosNT {


	// desbloquea al usuario pasado por parametro.
	static bool desbloquear(const std::string& usuario, const std::string& servidor) {
		USER_INFO_1*	buffer = NULL;
		NET_API_STATUS	codigo;

		if ((codigo = NetUserGetInfo(GuruStringAlgo::ansi_unicode(servidor).c_str(), GuruStringAlgo::ansi_unicode(usuario).c_str(), 1, (BYTE**)&buffer)) != NERR_Success) throw GuruNT::GuruNTException(GuruError().descripcion(codigo));

		if (buffer->usri1_flags & UF_LOCKOUT) {
			buffer->usri1_flags = buffer->usri1_flags ^ UF_LOCKOUT;

			if ((codigo = NetUserSetInfo(GuruStringAlgo::ansi_unicode(servidor).c_str(), GuruStringAlgo::ansi_unicode(usuario).c_str(), 1, (BYTE*)buffer, NULL)) != NERR_Success) {
				NetApiBufferFree(buffer);
				throw GuruNT::GuruNTException(GuruError().descripcion(codigo));
			}
		}

		return true;
	}

	// pone la marca de "clave nunca expira"
	static bool claveNuncaExpira(const std::string& usuario, const std::string& servidor) {
		USER_INFO_1*	buffer = NULL;
		NET_API_STATUS	codigo;

		if ((codigo = NetUserGetInfo(GuruStringAlgo::ansi_unicode(servidor).c_str(), GuruStringAlgo::ansi_unicode(usuario).c_str(), 1, (BYTE**)&buffer)) != NERR_Success) throw GuruNT::GuruNTException(GuruError().descripcion(codigo));

		if (!(buffer->usri1_flags & UF_DONT_EXPIRE_PASSWD)) {
			buffer->usri1_flags = buffer->usri1_flags ^ UF_DONT_EXPIRE_PASSWD;

			if ((codigo = NetUserSetInfo(GuruStringAlgo::ansi_unicode(servidor).c_str(), GuruStringAlgo::ansi_unicode(usuario).c_str(), 1, (BYTE*)buffer, NULL)) != NERR_Success) {
				NetApiBufferFree(buffer);
				throw GuruNT::GuruNTException(GuruError().descripcion(codigo));
			}
		}

		return true;
	}

	// saca la marca de "clave nunca expira"
	static bool limpiarClaveNuncaExpira(const std::string& usuario, const std::string& servidor) {
		USER_INFO_1*	buffer = NULL;
		NET_API_STATUS	codigo;

		if ((codigo = NetUserGetInfo(GuruStringAlgo::ansi_unicode(servidor).c_str(), GuruStringAlgo::ansi_unicode(usuario).c_str(), 1, (BYTE**)&buffer)) != NERR_Success) throw GuruNT::GuruNTException(GuruError().descripcion(codigo));

		if (buffer->usri1_flags & UF_DONT_EXPIRE_PASSWD) {
			buffer->usri1_flags = buffer->usri1_flags ^ UF_DONT_EXPIRE_PASSWD;

			if ((codigo = NetUserSetInfo(GuruStringAlgo::ansi_unicode(servidor).c_str(), GuruStringAlgo::ansi_unicode(usuario).c_str(), 1, (BYTE*)buffer, NULL)) != NERR_Success) {
				NetApiBufferFree(buffer);
				throw GuruNT::GuruNTException(GuruError().descripcion(codigo));
			}
		}

		return true;
	}

	// chequea si la clave esta expirada.
	static bool claveExpirada(const std::string& usuario, const std::string& servidor) {
		USER_INFO_1*	buffer = NULL;
		NET_API_STATUS	codigo;
		bool			expirada = true;

		if ((codigo = NetUserGetInfo(GuruStringAlgo::ansi_unicode(servidor).c_str(), GuruStringAlgo::ansi_unicode(usuario).c_str(), 1, (BYTE**)&buffer)) != NERR_Success) throw GuruNT::GuruNTException(GuruError().descripcion(codigo));

		// total de segundos que tiene el dominio configurado hasta que el usuario tenga que cambiar la clave.
		time_t maxPassAgeSec = GuruDominioNT::maxPasswordAge() * (24 * 60 * 60);

		// cuantos segundos pasaron desde que el usuario cambio la clave.
		DWORD SecUltimoCambio = buffer->usri1_password_age;

		// la fecha de ahora en segundos.
		time_t ahora = time(NULL);

		// fecha en segundos de cuando el usuario cambio la clave.
		time_t fechaUltimoCambioSec = ahora - SecUltimoCambio;

		// fecha en segundos hasta cuando el usuario tiene tiempo de cambiar la clave.
		time_t fechaLimiteCambioSec = fechaUltimoCambioSec + maxPassAgeSec;

		expirada = (fechaLimiteCambioSec < ahora) ? true : false;

		NetApiBufferFree(buffer);

		return expirada;
	}

	// retorna cuando cambio la clave(first) y cuando se vence la clave(second)
	static std::pair<time_t, time_t> clavesUsuario(const std::string& usuario, const std::string& servidor) {
		USER_INFO_1*	buffer = NULL;
		NET_API_STATUS	codigo;

		if ((codigo = NetUserGetInfo(GuruStringAlgo::ansi_unicode(servidor).c_str(), GuruStringAlgo::ansi_unicode(usuario).c_str(), 1, (BYTE**)&buffer)) != NERR_Success) throw GuruNT::GuruNTException(GuruError().descripcion(codigo));

		// total de segundos que tiene el dominio configurado hasta que el usuario tenga que cambiar la clave.
		time_t maxPassAgeSec = GuruDominioNT::maxPasswordAge() * (24 * 60 * 60);

		// cuantos segundos pasaron desde que el usuario cambio la clave.
		DWORD SecUltimoCambio = buffer->usri1_password_age;

		// la fecha de ahora en segundos.
		time_t ahora = time(NULL);

		// fecha en segundos de cuando el usuario cambio la clave.
		time_t fechaUltimoCambioSec = ahora - SecUltimoCambio;

		// fecha en segundos hasta cuando el usuario tiene tiempo de cambiar la clave.
		time_t fechaLimiteCambioSec = fechaUltimoCambioSec + maxPassAgeSec;

		std::pair<time_t, time_t> fechas;

		fechas.first = fechaUltimoCambioSec;
		fechas.second = fechaLimiteCambioSec;

		NetApiBufferFree(buffer);

		return fechas;
	}

	// retorna cuando cambio la clave(first) y cuando se vence la clave(second)
	static std::pair<time_t, time_t> fechasClavesUsuario(DWORD password_age, DWORD max_password_age) {

		// total de segundos que tiene el dominio configurado hasta que el usuario tenga que cambiar la clave.
		time_t maxPassAgeSec = max_password_age * (24 * 60 * 60);

		// cuantos segundos pasaron desde que el usuario cambio la clave.
		DWORD SecUltimoCambio = password_age;

		// la fecha de ahora en segundos.
		time_t ahora = time(NULL);

		// fecha en segundos de cuando el usuario cambio la clave.
		time_t fechaUltimoCambioSec = ahora - SecUltimoCambio;

		// fecha en segundos hasta cuando el usuario tiene tiempo de cambiar la clave.
		time_t fechaLimiteCambioSec = fechaUltimoCambioSec + maxPassAgeSec;

		std::pair<time_t, time_t> fechas;

		fechas.first = fechaUltimoCambioSec;
		fechas.second = fechaLimiteCambioSec;

		return fechas;
	}

	template <typename T, DWORD nivel_info>
	static T* infoUsuario(const std::string& usuario, const std::string& servidor) {
		T*	buffer = NULL;
		NET_API_STATUS	codigo;

		if ((codigo = NetUserGetInfo(GuruStringAlgo::ansi_unicode(servidor).c_str(), GuruStringAlgo::ansi_unicode(usuario).c_str(), nivel_info, (BYTE**)&buffer)) != NERR_Success) throw GuruNT::GuruNTException(GuruError().descripcion(codigo));

		return buffer;
	}

	static bool bloqueada(const std::string& usuario, const std::string& servidor) {
		bool retorno;

		USER_INFO_1* info = GuruUsuariosNT::infoUsuario<USER_INFO_1, 1>(usuario, servidor);

		retorno = (info->usri1_flags & UF_LOCKOUT) ? true : false;

		NetApiBufferFree(info);

		return retorno;
	}

	static bool estadoClaveNuncaExpira(const std::string& usuario, const std::string& servidor) {
		bool retorno;

		USER_INFO_1* info = GuruUsuariosNT::infoUsuario<USER_INFO_1, 1>(usuario, servidor);

		retorno = (info->usri1_flags & UF_DONT_EXPIRE_PASSWD) ? true : false;

		NetApiBufferFree(info);

		return retorno;
	}

	static std::string Sid_a_str(SID* sid) {
		PSID_IDENTIFIER_AUTHORITY psia;
		DWORD dwSubAuthorities;
		DWORD dwSidRev = SID_REVISION;
		DWORD dwCounter;
		DWORD dwSidSize;

		// Validate the binary SID.
		if (!IsValidSid(sid)) return FALSE;

		// Get the identifier authority value from the SID.
		psia = GetSidIdentifierAuthority(sid);

		// Get the number of subauthorities in the SID.
		dwSubAuthorities = *GetSidSubAuthorityCount(sid);

		// Compute the buffer length.
		// S-SID_REVISION- + IdentifierAuthority- + subauthorities- + NULL
		dwSidSize = (15 + 12 + (12 * dwSubAuthorities) + 1) * sizeof(TCHAR);

		GuruBufferChar buffer(dwSidSize + 2);

		// Add 'S' prefix and revision number to the string.
		dwSidSize = sprintf(buffer.buf(), "S-%lu-", dwSidRev);

		// Add SID identifier authority to the string.
		if ((psia->Value[0] != 0) || (psia->Value[1] != 0)) {
			dwSidSize += sprintf(buffer.buf() + lstrlen(buffer.buf()),
				"0x%02hx%02hx%02hx%02hx%02hx%02hx",
				(USHORT)psia->Value[0],
				(USHORT)psia->Value[1],
				(USHORT)psia->Value[2],
				(USHORT)psia->Value[3],
				(USHORT)psia->Value[4],
				(USHORT)psia->Value[5]);
		} else {
			dwSidSize += sprintf(buffer.buf() + lstrlen(buffer.buf()),
				"%lu",
				(ULONG)(psia->Value[5]) +
				(ULONG)(psia->Value[4] << 8) +
				(ULONG)(psia->Value[3] << 16) +
				(ULONG)(psia->Value[2] << 24));
		}

		// Add SID subauthorities to the string.
		for (dwCounter = 0; dwCounter < dwSubAuthorities; dwCounter++) {
			dwSidSize += sprintf(buffer.buf() + dwSidSize, "-%lu",
				*GetSidSubAuthority(sid, dwCounter));
		}

		return std::string(buffer.buf());
	}

	//////////////////////////////////////////////////////////////////////
	// Tarea: Sacar el SID de un usuario
	// Parametros:
	//				usuario = nombre del usuario a sacar el SID
	//////////////////////////////////////////////////////////////////////
	static std::string sid(const std::string& usuario) {
		SID_NAME_USE	name_use;
		DWORD			tamanio_sid = 0;
		DWORD			tamanio_dominio = 0;
		GuruBufferChar	_sid;
		GuruBufferChar	_dominio;

		// primero saco la cantidad de bytes que mide el SID y el nombre del dominio
		if (!LookupAccountName(NULL, usuario.c_str(), NULL,	&tamanio_sid, NULL, &tamanio_dominio, &name_use)) {

			if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) throw GuruNT::GuruNTException(GuruError().descripcion());

			_sid.reservar(tamanio_sid + 2);
			_dominio.reservar(tamanio_dominio + 2);

			// despues saco el SID
			if (!LookupAccountName(NULL, usuario.c_str(), _sid.buf(), &tamanio_sid, _dominio.buf(), &tamanio_dominio, &name_use)) throw GuruNT::GuruNTException(GuruError().descripcion());

			return Sid_a_str((SID *)_sid.buf());
		}

		return std::string();
	}
};

#endif
