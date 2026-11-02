#ifndef __GURUSERVIDORTCP_PROTOCOLO_HPP__
#define __GURUSERVIDORTCP_PROTOCOLO_HPP__

#include <GuruSocket/GuruSocketServidor.hpp>
#include <GuruSocket/ServicioProtocolo.hpp>

template <class T, class LEER=GuruSocketLeer, class ESCRIBIR=GuruSocketEscribir>
class GuruServidorTCPProtocolo: public GuruSocketServidor
{
public:
	GuruServidorTCPProtocolo(uint16_t puerto, const string& interfaz = "0.0.0.0")
		:GuruSocketServidor(puerto, interfaz) {};
	virtual ~GuruServidorTCPProtocolo(void) {};

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
