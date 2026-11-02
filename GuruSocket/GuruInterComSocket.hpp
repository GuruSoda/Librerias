#if !defined __GURUINTERCOMSOCKET_H__
#define __GURUINTERCOMSOCKET_H__

#include "GuruSocket.hpp"
#include "GuruSocketLeer.hpp"
#include "GuruSocketEscribir.hpp"
#include <GuruInterCom.hpp>
#include <GuruLog.hpp>

class GuruInterComSocket : public GuruInterCom
{
public:
	GuruInterComSocket(IEnviar& enviar, IRecibir& recibir):enviar(enviar),recibir(recibir) {};
	virtual ~GuruInterComSocket(){};

protected:
	virtual	void salida(const string& texto) {

		GURULOG_TRACE(" < " + texto);
		this->enviar << texto;
	}

	virtual	string entrada(void) {
		string ingreso;

		this->recibir >> ingreso;

		GURULOG_TRACE(" > " + ingreso);

		return ingreso;
	}

	IEnviar& enviar;
	IRecibir& recibir;
};

#endif
