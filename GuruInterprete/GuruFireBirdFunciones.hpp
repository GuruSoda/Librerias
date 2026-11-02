#ifndef __GURUFIREBIRDFUNCIONES_HPP__
#define __GURUFIREBIRDFUNCIONES_HPP__

#include <GuruInterprete/GuruInterprete.hpp>

#include <GuruFDBBase.h>

#include <stdexcept>
#include <string>

class GuruFireBirdFunciones: public GuruInterprete::GuruFunciones<GuruFireBirdFunciones> {
	iterator_fdb* 	it;
	iterator_fdb* 	itfinal;
	GuruFDBBase 	sql;
	FDBRegistro		registro;
	string			msg_error;

public:
	GuruFireBirdFunciones ():it(NULL),itfinal(NULL) {
		cargar_funcion(this, &GuruFireBirdFunciones::abrir, "firebird.abrir");
		cargar_funcion(this, &GuruFireBirdFunciones::cerrar, "firebird.cerrar");
		cargar_funcion(this, &GuruFireBirdFunciones::ejecutar, "firebird.ejecutar");
		cargar_funcion(this, &GuruFireBirdFunciones::extraer, "firebird.extraer");
		cargar_funcion(this, &GuruFireBirdFunciones::columna, "firebird.columna");
		cargar_funcion(this, &GuruFireBirdFunciones::nombre, "firebird.nombre");
		cargar_funcion(this, &GuruFireBirdFunciones::nombre, "firebird.titulo");
		cargar_funcion(this, &GuruFireBirdFunciones::error, "firebird.error");
	};

	virtual ~GuruFireBirdFunciones() {
		borrar_iteradores();
	};

	string abrir(const vector<string>& param) {
		string archivo;
		string usuario("SYSDBA");
		string clave("masterkey");
		string servidor;

		try {
			for (unsigned int i=0;i<param.size();i++) {
				switch (i) {
				case 0: archivo=param.at(i); break;
				case 1: usuario=param.at(i); break;
				case 2: clave=param.at(i); break;
				case 3: servidor=param.at(i); break;
				}
			}
		} catch (std::out_of_range&) {}

		try {
			sql.abrir(archivo, usuario, clave, servidor);
		}catch (IBPP::Exception& e) {
			msg_error=e.what();
			return string();			
		}

		return string("1");
	}

	string cerrar(const vector<string>& param) {

		borrar_iteradores();

		return string();
	}

	string ejecutar(const vector<string>& param) {

		borrar_iteradores();

		it=new iterator_fdb(sql.begin(param.at(0)));
		itfinal=new iterator_fdb(sql.end());

		unsigned int total=(*it)->columnas();
		if (total) {
			return string(GuruStringAlgo::itos(total));
		} else {
			borrar_iteradores();
			return string();
		}
	}

	string extraer(const vector<string>& param) {

		if (it == NULL || itfinal == NULL) return string();

		if (*it == *itfinal) return string();

		registro = it->operator *();

		++(*it);

		return string("1");
	}

	string columna(const vector<string>& param) {
		return registro.columna(atoi(param.at(0).c_str())).str();
	}

	string nombre(const vector<string>& param) {
		return registro.columna(param.at(0)).str();
	}

	string error(const vector<string>& param) {
		return msg_error;
	}

	void borrar_iteradores() {
		if (it) {
			delete it;
			delete itfinal;

			it=NULL;
			itfinal=NULL;
		}
	}
};

#endif
