#ifndef _GURUSOCKETLEER_H_
#define _GURUSOCKETLEER_H_

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
#endif

#include <string>

#include <GuruSocket/GuruSocket.hpp>
#include <GuruSocket/GuruSocketBase.hpp>
#include <GuruAlgo.hpp>

using namespace std;

class GuruSocketLeer: public IGuruSocketLeer {
public:
	GuruSocketLeer(const GuruSocket& conexion=GuruSocket()):IGuruSocketLeer(conexion){
#if defined __GURUSOCKETLEER_DEBUG__
		cout << "Contruyendo GuruSocketLeer:" << sacar_socket().handle() << endl;
#endif
	};

	GuruSocketLeer(const GuruSocketLeer& copia) {
		GuruSocketBase::poner_socket(copia._socket);
		GuruSocketBase::poner_timeout(copia._timeout);
	};

	virtual ~GuruSocketLeer() {
#if defined __GURUSOCKETLEER_DEBUG__
		cout << "Destruyendo GuruSocketLeer:" << sacar_socket().handle() << endl;
#endif
	};

	virtual IGuruSocketLeer& operator >> (string& buff){

		recibir(buff);

		return *this;
	}

	GuruSocketLeer& operator >> (unsigned long buff){
		return *this;
	}

	virtual string recibir (int total_a_leer=0) {
		string tmp;
		return recibir(tmp);
	}

	virtual char* recibir(char* buffer, size_t total) {

		uint64_t	tamanio_buffer = BufferEntrada(_socket.handle());
		uint64_t	recibidos=0;

		char*		inicio_buffer=buffer;

		do {
			if (static_cast<uint64_t>(total - recibidos) < static_cast<uint64_t>(tamanio_buffer))
				tamanio_buffer = static_cast<uint32_t>(total - recibidos);

			int leidos = recv(_socket.handle(), buffer, static_cast<int>(tamanio_buffer), MSG_NOSIGNAL);

			if (leidos == SOCKET_ERROR || leidos == 0) {
				int a = WSAGetLastError();
				_socket.cerrar();
				throw GuruSocketException(GuruError().descripcion(a));
			}

			buffer += leidos;

			recibidos += leidos;

		} while (recibidos < total);

		buffer = inicio_buffer;

		return buffer;
	}

	// Recibe una cadena de caracteres hasta el <Enter>, procesando los caracteres de control, es decir, respeta el backspace y el enter.
	virtual string& recibir(string& cadena, int total_a_leer=0) {
		fd_set	read;

		struct	timeval struc_tiempo_espera = {0};

		int		cant_sockets = 0;
		char	caracter = 0;
		char	caracter_anterior = 0;
		int		leidos = 0;
		int		cant_caracteres=0;

		cadena.clear();

		do {
			struc_tiempo_espera.tv_sec = _socket.timeout();
			struc_tiempo_espera.tv_usec = 0;

			FD_ZERO(&read);

			FD_SET(_socket.handle(), &read);

			cant_sockets = select ( (int)_socket.handle() + 1, &read, NULL, NULL, &struc_tiempo_espera);

			// pregunto por time out o por error en el socket
			if (cant_sockets == 0 || cant_sockets == SOCKET_ERROR) {
				int a = WSAGetLastError();
				_socket.cerrar();
				throw GuruSocketException(GuruError().descripcion(a));
			}

			caracter_anterior = caracter;
			leidos = recv (_socket.handle(), &caracter, 1, 0); // saco de a un byte

			if (leidos == 0 || leidos == SOCKET_ERROR) {
				int a = WSAGetLastError();
				_socket.cerrar();
				throw GuruSocketException(GuruError().descripcion(a));
			}

			if (caracter == CR) continue;

			if (caracter == LF && caracter_anterior == CR) {
				cadena.erase(cant_caracteres); // borro el CRLF
				break;
			}

			if (caracter == 0x08) { // es backspace
				if (cant_caracteres) {
					cant_caracteres--;
					cadena.erase(cant_caracteres);
				}
				continue;
			}

			cadena.push_back(caracter);
			cant_caracteres++;

			if (total_a_leer == cant_caracteres) break;

		} while (true);

		return cadena;
	}
};

#endif
