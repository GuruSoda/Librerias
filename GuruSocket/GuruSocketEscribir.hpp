#ifndef _GURUSOCKETESCRIBIR_H_
#define _GURUSOCKETESCRIBIR_H_

#include <GuruCompat.h>

#if defined __GURUWIN32__
// Para compilar Windows.h y Winsock2.h en el mismo fuente
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef _WINSOCKAPI_
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
	#endif

	#include <windows.h>
	#include <winsock2.h>
#elif defined __GURULINUX__
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <GuruCompat/xtoa.h>
#endif

#include <string>

#include "GuruSocket.hpp"
#include "GuruSocketBase.hpp"

using namespace std;

const char CR[] = "\r";

class GuruSocketEscribir: public IGuruSocketEscribir {
public:
	GuruSocketEscribir(const GuruSocket& conexion=GuruSocket()):IGuruSocketEscribir(conexion) {};
	virtual ~GuruSocketEscribir() {};

	GuruSocketEscribir& operator << (const string& buffer){
		enviar(buffer.c_str(), (int) buffer.size());
		return *this;
	}

	GuruSocketEscribir& operator << (const char* buffer){
		enviar(buffer, (int) strlen(buffer));
		return *this;
	}

	GuruSocketEscribir& operator << (unsigned long valor){
		std::string tmp(GuruStringAlgo::itos(valor));

		enviar(tmp.c_str(), static_cast<int>(tmp.length()));

		return *this;
	}

	void enviar(const char* buffer, int cantidad){
		int salida_send = send (_socket.handle(), buffer, cantidad, 0);

		if (salida_send == SOCKET_ERROR) {
			int a = LastError();
			_socket.cerrar();
			throw GuruSocketException(a);
		}
	}
};

#endif

