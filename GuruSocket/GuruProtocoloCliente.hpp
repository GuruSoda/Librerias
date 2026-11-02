#ifndef __GURUPROTOCOLOCLIENTE_H__
#define __GURUPROTOCOLOCLIENTE_H__

#include <GuruProtocolo.hpp>
#include <GuruSocket/GuruSocketLeer.hpp>
#include <GuruSocket/GuruSocketEscribir.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/erase.hpp>

#include <vector>

using namespace std;

class ProtocoloRespuesta {
public:
	ProtocoloRespuesta():_tipo(GuruProtocolo::codigo_desconocido()) {};
	ProtocoloRespuesta(const ProtocoloRespuesta& copia):_tipo(copia._tipo),_texto(copia._texto) {};

	inline void tipo(int t) {_tipo = t;};
	inline int tipo() const {return _tipo;};
	inline string& texto() {return _texto;};

protected:
	int		_tipo;
	string	_texto;

	template <typename T> friend inline ProtocoloRespuesta FabricaRespuesta(const T& cadena);
	template <class LEER, class ESCRIBIR> friend class basic_GuruProtocoloCliente;
};

template <typename T>
inline ProtocoloRespuesta FabricaRespuesta(const T& cadena) {
	string linea;

	linea = boost::trim_copy(cadena);

	ProtocoloRespuesta respuesta;

	if (boost::istarts_with(linea, GuruProtocolo::str_bien())) {
		respuesta._tipo = GuruProtocolo::codigo_bien();
		respuesta._texto = boost::ierase_first_copy(linea, GuruProtocolo::str_bien());
	} else if (boost::istarts_with(linea, GuruProtocolo::str_mal())) {
		respuesta._tipo = GuruProtocolo::codigo_mal();
		respuesta._texto = boost::ierase_first_copy(linea, GuruProtocolo::str_mal());
	} else if (boost::istarts_with(linea, GuruProtocolo::str_noexiste())) {
		respuesta._tipo = GuruProtocolo::codigo_noexiste();
		respuesta._texto = boost::ierase_first_copy(linea, GuruProtocolo::str_noexiste());
	} else if (boost::istarts_with(linea, GuruProtocolo::str_prohibido())) {
		respuesta._tipo = GuruProtocolo::codigo_prohibido();
		respuesta._texto = boost::ierase_first_copy(linea, GuruProtocolo::str_prohibido());
	} else {
		respuesta._tipo = GuruProtocolo::codigo_desconocido();
		respuesta._texto = linea;
	}

	boost::trim(respuesta._texto);

	return respuesta;
};

template <class LEER=GuruSocketLeer, class ESCRIBIR=GuruSocketEscribir>
class basic_GuruProtocoloCliente
{
public:
	basic_GuruProtocoloCliente(GuruSocket& conexion):leer(conexion),escribir(conexion) {};
	basic_GuruProtocoloCliente():leer(GuruSocket()),escribir(GuruSocket()) {};

	virtual ~basic_GuruProtocoloCliente(void) {};

	class GuruProtocoloClienteException : public std::exception {
	public:
		GuruProtocoloClienteException(int codigo, const string& msg = "") :exception(), _msg(msg), _codigo(codigo) {}
		GuruProtocoloClienteException(const string& msg, int codigo = 0) :exception(), _msg(msg), _codigo(codigo) {}
		virtual ~GuruProtocoloClienteException() throw() {}

		const char* what() const throw() {
				return _msg.c_str();
		}

		const int codigo() const { return this->_errorcode; }
	private:
		string	_msg;
		int		_codigo;
	};

	inline void cargar_handle(GuruSocket& conexion) {
		leer.poner_socket(conexion);
		escribir.poner_socket(conexion);
	};

	inline void conectar(const string& servidor, const int puerto) {
		GuruSocket enchu=FabricaSocketConectar(servidor, puerto);
		cargar_handle(enchu);
	}

	// Recibe una linea y analiza la respuesta
	inline ProtocoloRespuesta respuesta(){ return FabricaRespuesta(leer.recibir());}

	// Envia un comando sin analizar la respuesta
	inline void cmd(const string& comando) { escribir << comando << "\r\n"; }

	// Envia un comando y recibe una respuesta de una sola linea.
	inline ProtocoloRespuesta cmd_linea(const string& comando) {
		escribir << comando << "\r\n";
		return FabricaRespuesta(leer.recibir());
	}

	// Envia un comando y recibe una respuesta multilinea.
	inline vector<ProtocoloRespuesta>& cmd_multilinea(const string& comando, vector<ProtocoloRespuesta>& respuestas) {
		respuestas.clear();
		escribir << comando << "\r\n";

		do{
			ProtocoloRespuesta resp(FabricaRespuesta(leer.recibir()));
			if (resp.tipo() == GuruProtocolo::codigo_bien())
				respuestas.push_back(resp);
			else break;
		}while (8);

		return respuestas;
	}

	inline void cortar() {
		escribir.sacar_socket().cerrar();
		leer.sacar_socket().cerrar();
	};

	class iterator {
	public:
			iterator (const int& tipo = GuruProtocolo::codigo_desconocido()):protocolo(NULL) {respuesta.tipo(tipo);}; // iterador vacio
			iterator (basic_GuruProtocoloCliente* protocolo, const string& comando):protocolo(protocolo) { // iterador de comienzo
				protocolo->escribir << comando << "\r\n"; // envio el comando multilinea

				operator++();
			};
			iterator (basic_GuruProtocoloCliente* protocolo, int codigo):protocolo(protocolo) {respuesta.tipo(codigo);}; // iterador de fin

			virtual ~iterator () {};

			void operator++() {
				respuesta = FabricaRespuesta(protocolo->leer.recibir(linea_respuesta));
			}

			bool operator!=(const iterator& it2) {
				if (respuesta.tipo() != it2.respuesta.tipo()) return true;
				else return false;
			}

			bool operator==(const iterator& it2) {
				if (respuesta._tipo == it2.respuesta._tipo) return true;
				else return false;
			}

			ProtocoloRespuesta* operator->() const {
				return (ProtocoloRespuesta*)&respuesta;
			}

			ProtocoloRespuesta operator*() const {
				return respuesta;
			}

	protected:
			basic_GuruProtocoloCliente* 	protocolo;
			ProtocoloRespuesta 		respuesta;
			string 					linea_respuesta;
	};

	iterator begin(const string& comando) {return iterator(this, comando);};
	iterator end() {return iterator(this, GuruProtocolo::codigo_mal());};

protected:
//	LEER		tmp_lectura;
//	ESCRIBIR	tmp_escritura;

//	IRecibir	leer;
//	IEnviar		escribir;

	LEER		leer;
	ESCRIBIR	escribir;
};

typedef basic_GuruProtocoloCliente<GuruSocketLeer, GuruSocketEscribir> GuruProtocoloCliente;

#endif
