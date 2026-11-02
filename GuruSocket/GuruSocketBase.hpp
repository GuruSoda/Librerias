#ifndef _GURUSOCKETBASE_H_
#define _GURUSOCKETBASE_H_

#include "GuruSocket.hpp"

class GuruSocketBase
{
public:
	GuruSocketBase(const GuruSocket& conexion, unsigned int timeout= default_time_out):_socket(conexion),_timeout(timeout) {};
	GuruSocketBase(const GuruSocketBase& copia):_socket(copia._socket),_timeout(copia._timeout) {};

	virtual ~GuruSocketBase(void) {};

	inline void poner_socket(const GuruSocket& nuevo){_socket = nuevo;};
	inline GuruSocket& sacar_socket() {return _socket;};

	inline void poner_timeout(unsigned int timeout) {_timeout=timeout;};
	inline unsigned int sacar_timeout() { return _timeout;};

	inline static const char* crlf() {return "\r\n";};

	enum {CR='\r', LF='\n', CRLF=0x1013};
protected:
	GuruSocket		_socket;
	unsigned int	_timeout;
};

class IGuruSocketLeer:public GuruSocketBase {
public:
	IGuruSocketLeer(const GuruSocket& conexion=GuruSocket()):GuruSocketBase(conexion){};
	IGuruSocketLeer(const IGuruSocketLeer& copia):GuruSocketBase(copia){};

	virtual IGuruSocketLeer& operator >> (string& buff) = 0;
	virtual char* recibir(char* buffer, size_t total) = 0;
	virtual string& recibir(string& cadena, int total_a_leer=0) = 0;
	virtual string recibir(int total_a_leer=0) {
			string tmp;
			return recibir(tmp, total_a_leer);
	}
};

class IGuruSocketEscribir:public GuruSocketBase {
public:
	IGuruSocketEscribir(const GuruSocket& conexion=GuruSocket()):GuruSocketBase(conexion){};
	IGuruSocketEscribir(const IGuruSocketEscribir& copia):GuruSocketBase(copia){};

	virtual IGuruSocketEscribir& operator << (const string& buff) {return *this;}
	virtual IGuruSocketEscribir& operator << (unsigned long valor){return *this;}
	virtual void enviar(const char* buffer, int cantidad) {}
};

class IEnviar {
public:
	IEnviar():PtrEnviar(NULL) {}
	IEnviar(IGuruSocketEscribir* escritura):PtrEnviar(escritura) {}

	IGuruSocketEscribir* interfaz() {return PtrEnviar;}
	void interfaz(IGuruSocketEscribir* escritura) {PtrEnviar=escritura;}

	virtual IGuruSocketEscribir& operator << (const string& buff) {return PtrEnviar->operator<<(buff);}

private:
	IGuruSocketEscribir*	PtrEnviar;
};

class IRecibir {
public:
	IRecibir():PtrRecibir(NULL) {}
	IRecibir(IGuruSocketLeer* lectura):PtrRecibir(lectura) {}

	IGuruSocketLeer* interfaz() {return PtrRecibir;}
	void interfaz(IGuruSocketLeer* lectura) {PtrRecibir=lectura;}

	virtual IGuruSocketLeer& operator >> (string& buff) {return PtrRecibir->operator>>(buff);}
	virtual char* recibir(char* buffer, size_t total) { return PtrRecibir->recibir(buffer, total); }
	virtual string& recibir(string& cadena, int total_a_leer=0) {return PtrRecibir->recibir(cadena, total_a_leer);}
	virtual string recibir(int total_a_leer=0) {return PtrRecibir->recibir(total_a_leer);}

private:
	IGuruSocketLeer*	PtrRecibir;
};

/*
class IRecibir {
public:
	IRecibir():data(NULL) {}

	virtual IRecibir& operator >> (string& buff) {return *this;}
	virtual string& recibir(string& cadena, int total_a_leer=0) {return cadena;}
	virtual string recibir(int total_a_leer = 0) { return string(); }

private:
	void* data;
};
*/
#endif
