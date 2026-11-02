#ifndef __SERVICIOPROTOCOLO_H__
#define __SERVICIOPROTOCOLO_H__

#include <GuruSocket/GuruSocketServidor.hpp>

template <class T>
class ServicioProtocolo: public GuruSocketServicio
{
	IEnviar& enviar;
	IRecibir& recibir;

public:
	ServicioProtocolo(IRecibir& recibir, IEnviar& enviar):enviar(enviar), recibir(recibir) {};
	virtual ~ServicioProtocolo(void) {};

	void main () {
		T Protocolo(this->enviar, this->recibir);

		Protocolo.main();
	}
};

#endif
