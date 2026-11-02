#ifndef __GURUPROTOCOLOFUNCIONES_HPP__
#define __GURUPROTOCOLOFUNCIONES_HPP__

#include <string>
#include <stdexcept>
#include <GuruSocket/GuruProtocoloCliente.hpp>

using namespace std;

class GuruProtocoloFunciones: public GuruInterprete::GuruFunciones<GuruProtocoloFunciones> {
		GuruSocket						sock;
		GuruProtocoloCliente			cliente;
		GuruProtocoloCliente::iterator	it;
		ProtocoloRespuesta				resp;
		string							msj_error;
public:
	GuruProtocoloFunciones () {
		cargar_funcion(this, &GuruProtocoloFunciones::conectar, "prot.open");
		cargar_funcion(this, &GuruProtocoloFunciones::close, "prot.close");
		cargar_funcion(this, &GuruProtocoloFunciones::error, "prot.error");
		cargar_funcion(this, &GuruProtocoloFunciones::cmd, "prot.cmd");
		cargar_funcion(this, &GuruProtocoloFunciones::simple, "prot.cmd_linea");
		cargar_funcion(this, &GuruProtocoloFunciones::listado, "prot.cmd_multilinea");
		cargar_funcion(this, &GuruProtocoloFunciones::linea, "prot.linea");
		cargar_funcion(this, &GuruProtocoloFunciones::respuesta, "prot.respuesta");

		inicializar_GuruSocket();
	};

	virtual ~GuruProtocoloFunciones() {
		limpiar_GuruSocket();
	};

	string conectar(const vector<string>& param) {
		string servidor;
		string puerto;

		try {
			for (unsigned int i=0;i<param.size();i++) {
				switch (i) {
				case 0: servidor=param.at(i); break;
				case 1: puerto=param.at(i); break;
				}
			}
		} catch (std::out_of_range& ) {}

		try {
			sock = FabricaSocketConectar(servidor, atoi(puerto.c_str()));

			if (sock.conectado()) {
				cliente.cargar_handle(sock);
				return string(1, '1');
			} else {
				return string (1, '0');
			}
		}catch (GuruSocketException& e) {
			msj_error= e.what();
			return string(1, '0');
		}

		return string();
	}

	string close(const vector<string>& param) {
		cliente.cortar();
		return string(1, '1');
	}

	string cmd(const vector<string>& param) {
		if (param.size() != 1) return string();

		try {
			cliente.cmd(param.at(0));
			return string(1, '0');
		} catch(GuruSocketException& e) {
			msj_error=e.what();
		}

		return string();
	}

	string simple(const vector<string>& param) {
		if (param.size() != 1) return string();

		try {
			return cliente.cmd_linea(param.at(0)).texto();
		} catch(GuruSocketException& e) {
			msj_error=e.what();
		}

		return string();
	}

	string listado(const vector<string>& param) {
		if (param.size() != 1) return string(1, '0');

		it=cliente.begin(param.at(0));

		return string(1, '1');
	}

	string linea(const vector<string>& param) {
		if (it->tipo() == GuruProtocolo::codigo_desconocido())
			return string(1, '0');
		else if (it->tipo() == GuruProtocolo::codigo_mal()) {
			msj_error = it->texto();
			return string(1, '0');
		} else {

			resp = *it;

			++it;

			return string(1, '1');
		}
	}

	string respuesta(const vector<string>& param) {
		return resp.texto();
	}

	string error(const vector<string>& param) {
		return msj_error;
	}
};

#endif
