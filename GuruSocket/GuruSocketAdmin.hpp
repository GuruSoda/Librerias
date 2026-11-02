#ifndef _GURUSOCKETADMIN_H_
#define _GURUSOCKETADMIN_H_

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
	#include <pthread.h>
#endif

#include <GuruAcceso.hpp>
#include <map>

/*
class GuruSocketAdmin {
public:
	GuruSocketAdmin():ultimo_id(0) {};
	virtual ~GuruSocketAdmin() {};

	uint64_t adminstrar(SOCKET _socket) {
		InfoConexion servicio;

		servicio._socket = GuruSocket(_socket);
		servicio._id = nuevo_id();

		exclusividad.esperar_acceso(mapa_conexiones);
		conexiones.insert(make_pair(servicio._id, servicio));
		exclusividad.permitir_acceso(mapa_conexiones);

		return servicio._id;
	}

	void liberar(uint64_t id) {
		exclusividad.esperar_acceso(mapa_conexiones);
		conexiones.erase(id);
		exclusividad.permitir_acceso(mapa_conexiones);
	};

	InfoConexion* conexion(uint64_t id) {
		exclusividad.esperar_acceso(mapa_conexiones);
		map<uint64_t, InfoConexion>::iterator it = conexiones.find(id);

		if (it == conexiones.end()) 
			cerr << "GuruSocketAdmin:: No se ha encontrado el ID." << endl;
			//throw GuruSocketException("No se ha encontrado el ID.");

		exclusividad.permitir_acceso(mapa_conexiones);

		return &it->second;
	}

protected:
	enum accesos {mapa_conexiones, sacar_id, total};

	uint64_t ultimo_id;
	map<uint64_t, InfoConexion> conexiones;
	GuruAcceso	exclusividad;

	uint64_t nuevo_id() {
		exclusividad.esperar_acceso(sacar_id);
		ultimo_id++;
		exclusividad.permitir_acceso(sacar_id);

		return ultimo_id;
	}
};
*/
#endif
