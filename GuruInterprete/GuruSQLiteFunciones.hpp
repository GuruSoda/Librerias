#ifndef __GURUSQLITEFUNCIONES_HPP__
#define __GURUSQLITEFUNCIONES_HPP__

#include <GuruInterprete/GuruInterprete.hpp>
#include <GuruSQLite.hpp>
#include <sqlite/sqlite3.h>
#include <GuruString_algo.hpp>

#include <string>

class GuruSQLiteFunciones: public GuruInterprete::GuruFunciones<GuruSQLiteFunciones> {
	GuruSQLite::resultado	resultado;
	GuruSQLite::bd			bdd;
	GuruSQLite::resultado::iterator	it,end;
	std::string				msg_error;

public:
	GuruSQLiteFunciones () {
		cargar_funcion(this, &GuruSQLiteFunciones::abrir, "sql.abrir");
		cargar_funcion(this, &GuruSQLiteFunciones::cerrar, "sql.cerrar");
		cargar_funcion(this, &GuruSQLiteFunciones::ejecutar, "sql.ejecutar");
		cargar_funcion(this, &GuruSQLiteFunciones::extraer, "sql.extraer");
		cargar_funcion(this, &GuruSQLiteFunciones::columna, "sql.columna");
		cargar_funcion(this, &GuruSQLiteFunciones::columnas, "sql.columnas");
		cargar_funcion(this, &GuruSQLiteFunciones::nombre, "sql.nombre");
		cargar_funcion(this, &GuruSQLiteFunciones::error, "sql.error");
		cargar_funcion(this, &GuruSQLiteFunciones::comillas, "sql.comillas");
	};

	virtual ~GuruSQLiteFunciones() {
	};

	string abrir(const vector<string>& param) {
		try {
			bdd = GuruSQLite::conectar(param.at(0));
			return string("1");
		} catch (GuruSQLite::GuruSQLiteException& e) {
			msg_error = e.what();
			return string("0");
		}
	}

	string cerrar(const vector<string>& param) {
		bdd.desconectar();
		return string();
	}

	string ejecutar(const vector<string>& param) {
		try {
			msg_error.clear();
			resultado = bdd.ejecutar(param.at(0));
/*
			if (resultado.empty()) return string("0");
			else {
				it = resultado.begin();
				return string("1");
			}
*/
			return string("1");
		} catch (GuruSQLite::GuruSQLiteException& e) {
			msg_error= e.what();
			return string("0");
		}
	}

	string extraer(const vector<string>& param) {
		try {
			if (resultado.fetch())
				return string("1");
			else 
				return string("0");
			/*
					if (it == end) return string("0");

					++it;

					if (it == end) return string("0");
			*/
			
		} catch (GuruSQLite::GuruSQLiteException& e) {
			msg_error = e.what();
			return string("0");
		}
	}

	string columnas(const vector<string>& param) {
		return GuruStringAlgo::itos(resultado.columnas());
	}

	string columna(const vector<string>& param) {
	// pregunto si el compilador tiene el standard c++11
		try {
	#if __cplusplus > 199711L
			return it->getString(std::stoi(param.at(0)));
	#else
			return resultado->getString(atoi(param.at(0).c_str()));
	#endif
		} catch (GuruSQLite::GuruSQLiteException& e) {
			msg_error= e.what();
			return string();
		}
	}

	string nombre(const vector<string>& param) {
	#if __cplusplus > 199711L
			return resultado.nombre_columna(std::stoi(param.at(0)));
	#else
			return resultado.nombre_columna(atoi(param.at(0).c_str()));
	#endif
	}

	string error(const vector<string>& param) {
		return msg_error;
	}

	string comillas(const vector<string>& param) {
		return GuruSQLite::comillas(param.at(0));
	}
};

#endif
