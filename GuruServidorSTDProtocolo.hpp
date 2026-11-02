#ifndef __GURUSERVIDORSTD_PROTOCOLO_HPP__
#define __GURUSERVIDORSTD_PROTOCOLO_HPP__

#include <GuruSocket/ServicioProtocolo.hpp>

template <class T, class LEER=GuruSocketLeer, class ESCRIBIR=GuruSocketEscribir>
class GuruServidorSTDProtocolo
{
public:
	GuruServidorSTDProtocolo() {};
	virtual ~GuruServidorSTDProtocolo(void) {};

	virtual void servicio (InfoConexion* info) {
		LEER		recibir(info->socket());
		ESCRIBIR	enviar(info->socket());

		IRecibir	ileer(&recibir);
		IEnviar		iescribir(&enviar);

		ServicioProtocolo<T> _servicio(ileer, iescribir);

		_servicio.info(info);

		_servicio.main();
	}
};

#endif
