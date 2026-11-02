#ifndef __GURUSOCKETSERVIDOR_RSA_HPP__
#define __GURUSOCKETSERVIDOR_RSA_HPP__

#include <GuruSocket/GuruSocketServidor.hpp>
#include <GuruSocket/GuruSocketLeer.hpp>
#include <GuruSocket/GuruSocketEscribir.hpp>

#include <polarssl/rsa.h>
#include <polarssl/x509.h>

// sacar, solo para prueba
#include <GuruSocket/GuruProtocoloServidor.hpp>
#include <string>

class GuruSocketLeerRSA: public GuruSocketLeer
{
	rsa_context* rsa;

public:
	GuruSocketLeerRSA(const GuruSocket& conexion=GuruSocket()):GuruSocketLeer(conexion){};

	GuruSocketLeerRSA& operator >> (string& buff){

		GuruSocketLeer::recibir(buff);

		return *this;
	}

	virtual string recibir (int total_a_leer=0) {
		string tmp;
		return GuruSocketLeer::recibir(tmp);
	}

	void ponerRSAContext(rsa_context* rsa) {
		this->rsa = rsa;
	}
};

class GuruSocketEscribirRSA: public GuruSocketEscribir
{
	rsa_context* rsa;

public:
	GuruSocketEscribirRSA(const GuruSocket& conexion=GuruSocket()):GuruSocketEscribir(conexion){};

	void ponerRSAContext(rsa_context* rsa) {
		this->rsa = rsa;
	}
};

template <class T>
class GuruSocketServidorRSA: public GuruSocketServidor<T>
{
	rsa_context rsa;

public:
	GuruSocketServidorRSA(uint16_t puerto, const string& interfaz = "0.0.0.0"):GuruSocketServidor<T>(puerto,interfaz) {};
	virtual ~GuruSocketServidorRSA(void) {};

	virtual bool init() {

		rsa_init(&rsa, RSA_PKCS_V15, 0);

		int salida = x509parse_keyfile(&rsa, "servidorrsa", NULL);

		salida=rsa_check_privkey(&rsa);

		return true;
	}

	virtual void servicio (InfoConexion* info){
		GuruSocketLeerRSA 		recibir(info->sacar_socket());
		GuruSocketEscribirRSA 	enviar(info->sacar_socket());

		T	_servicio;

		recibir.ponerRSAContext(&rsa);
		enviar.ponerRSAContext(&rsa);
		_servicio.ponerLectura(recibir);
		_servicio.ponerEscritura(enviar);
		_servicio.ponerInfoConexion(info);		

		_servicio.main();
	}

};
/*
class Dummy: public GuruProtocoloServidor
{
public:
	Dummy() {};
	virtual ~Dummy() {};

	virtual bool init() {
		std::string a;

		recibir >> a;
		enviar << a;
	};
};
*/
#endif
