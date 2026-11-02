#ifndef __GURUSERVIDORTCP_HPP__
#define __GURUSERVIDORTCP_HPP__

#include <GuruSocket/GuruSocketServidor.hpp>
#include <GuruSocket/ServicioProtocolo.hpp>

template <class T, class LEER=GuruSocketLeer, class ESCRIBIR=GuruSocketEscribir>
class GuruServidorTCP: public GuruSocketServidor
{
public:
	GuruServidorTCP(::uint16_t puerto, const string& interfaz = "0.0.0.0")
		:GuruSocketServidor(puerto, interfaz) {};
	GuruServidorTCP():GuruSocketServidor() {};

	virtual ~GuruServidorTCP(void) {};

	virtual void servicio (InfoConexion* info) {
		LEER		recibir(info->socket());
		ESCRIBIR	enviar(info->socket());

		IRecibir	ileer(&recibir);
		IEnviar		iescribir(&enviar);

		T _servicio;

		_servicio.lectura(ileer);
		_servicio.escritura(iescribir);
		_servicio.info(info);

		_servicio.main();
	}
};

template <class T, class Y, class LEER=GuruSocketLeer, class ESCRIBIR=GuruSocketEscribir>
class GuruServidorTCPEx: public GuruSocketServidor
{
public:
	GuruServidorTCPEx(const Y& data, ::uint16_t puerto, const string& interfaz = "0.0.0.0")
		:GuruSocketServidor(puerto, interfaz),data(data) {};
	virtual ~GuruServidorTCPEx(void) {};

	virtual void servicio (InfoConexion* info) {
		LEER		recibir(info->socket());
		ESCRIBIR	enviar(info->socket());

		IRecibir	ileer(&recibir);
		IEnviar		iescribir(&enviar);

		T _servicio(data);

		_servicio.lectura(ileer);
		_servicio.escritura(iescribir);
		_servicio.info(info);

		_servicio.main();
	}

private:
	Y& data;
};

#endif
