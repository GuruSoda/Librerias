#ifndef __GURUSOCKETFUNCIONES_HPP__
#define __GURUSOCKETFUNCIONES_HPP__

#include <GuruInterprete/GuruInterprete.hpp>
#include <GuruSocket/GuruSocket.hpp>
#include <GuruSocket/GuruSocketEscribir.hpp>
#include <GuruSocket/GuruSocketLeer.hpp>

#include <stdexcept>

class GuruSocketFunciones: public GuruInterprete::GuruFunciones<GuruSocketFunciones> {
	GuruSocket 			sock;
	GuruSocketEscribir 	escribir;
	GuruSocketLeer		leer;
	string				msj_error;
public:
	GuruSocketFunciones () {
		cargar_funcion(this, &GuruSocketFunciones::conectar, "socket.open");
		cargar_funcion(this, &GuruSocketFunciones::cortar, "socket.close");
		cargar_funcion(this, &GuruSocketFunciones::enviar, "socket.send");
		cargar_funcion(this, &GuruSocketFunciones::recibir, "socket.recv");
		cargar_funcion(this, &GuruSocketFunciones::error, "socket.error");
		cargar_funcion(this, &GuruSocketFunciones::listado_interfaces, "socket.interfaces");
	};

	virtual ~GuruSocketFunciones() {};

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
				escribir.poner_socket(sock);
				leer.poner_socket(sock);
				return string("1");
			} else {
				return string ();
			}
		}catch (GuruSocketException& e) {
			msj_error= e.what();
			return string();
		}

		return string();
	}

	string cortar(const vector<string>& param) {
		sock.cerrar();

		msj_error.clear();

		return string();
	}

	string enviar(const vector<string>& param) {
		if (param.size() != 1) return string();

		try {
			escribir << param.at(0);

			return string("1");
		}catch (GuruSocketException& e) {
			msj_error= e.what();
			return string();
		}
	}

	string recibir(const vector<string>& param) {
		string tmp;

		try {
			leer >> tmp;

			return tmp;
		}catch (GuruSocketException& e) {
			msj_error= e.what();
			return string();
		}
	}

	string error(const vector<string>& param) {
		return msj_error;
	}

	string listado_interfaces(const vector<string>& param) {
		vector<string> listado;
		interfaces(listado);

		string str_inter;
		for (vector<string>::iterator it=listado.begin();it!=listado.end();++it)
			str_inter+=*it + " | ";

		return str_inter;
	}

};

#endif
