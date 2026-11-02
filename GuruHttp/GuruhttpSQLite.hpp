#ifndef __GURUHTTPSQLITE_HPP__
#define __GURUHTTPSQLITE_HPP__

#include "GuruhttpREST.hpp"

#include <GuruSQLite.hpp>

/*
Desde Windows se puede probar asi:
curl -d "{\"consulta\":\"select name, year from game\", \"limite\":10}" -H "Content-Type: application/json" -X POST http://localhost/sqlite/consulta

Desde Linux se puede probar asi:
curl -d '{"consulta":"select name, year from game", "limite":10}' -H "Content-Type: application/json" -X POST http://localhost/sqlite/consulta

----------------------------------------------------------------------------------------------------------------
Para listar las tablas existentes con una sola consulta:
curl -d "{\"consulta\":\"select * from sqlite_master\", \"limite\":20}" -H "Content-Type: application/json" -X POST http://localhost:8080/sqlite/consulta

----------------------------------------------------------------------------------------------------------------
Codigos HTTP enviados por el sitio:

400: No vino la llave "consulta" o esta sin datos.
404: No existe el directorio virtual.
405: La consulta SQL tiene algun error.
500: No se puedo abrir la base de datos o el contenido del POST no es JSON valido.

----------------------------------------------------------------------------------------------------------------
Ejemplo de uso:

GuruhttpDirVirtual	HttpSQLite;

HttpSQLite.sitio("/sqlite/");
HttpSQLite.agregarHost("*");
HttpSQLite.interfaz(new GuruhttpSitio<GuruHttpSQLite>());
HttpSQLite.velocidad(0);
HttpSQLite.extra_ptr((void*)*opt_base->filename); // nombre del archivo de base de datos.
HttpSQLite.agregarAtributosExtra("Access-Control-Allow-Origin", "*");
HttpSQLite.env("base", "Base de datos del M.A.M.E.");

*/

class GuruHttpSQLite : public GuruHttpREST {
	GuruSQLite::bd base;
	int limite_json;
public:
	GuruHttpSQLite():limite_json(100) {}
	virtual ~GuruHttpSQLite() {}

	void enviar_error(const std::string& msg, int codigo=10000) {

	}

	Json::Value& consultaJSON(const string& consulta, Json::Value& resultado, int limite) {

		resultado.clear();

		if (consulta.empty()) return resultado;

		try {

			GuruSQLite::resultado res = base.ejecutar(consulta);

			int i = 0;

			for (GuruSQLite::resultado::iterator it = res.begin(), itend = res.end(); it != itend && i < limite; ++it, i++) {
				Json::Value val;

				for (int i = 0; i < it->columnas(); i++) {
					if (it->is_text(i))
						val[it->nombre_columna(i)] = it->getString(i);
					else if (it->is_integer(i))
						val[it->nombre_columna(i)] = static_cast<Json::Value::UInt64>(it->getInt64(i));
					else if (it->is_float(i))
						val[it->nombre_columna(i)] = it->getDouble(i);
					else
						val[it->nombre_columna(i)] = it->getString(i);
				}

				resultado.append(val);
			}
		} catch (GuruSQLite::GuruSQLiteException& e) {
			throw GuruhttpExcepcion(405, e.what());
		}

		return resultado;
	}

	virtual void procesarPost() {
		try {
			base = GuruSQLite::conectar(std::string((char*)request()->config()->extra_ptr()), NULL, GuruSQLite::bd::modo::lectura);

			if (dir().compare("/consulta")) {
				Json::Reader	lector;
				Json::Value		json;

				if (!lector.parse(std::string(post().body()), json, false)) {
					cout << lector.getFormattedErrorMessages() << endl;
					cout << json.toStyledString() << endl;
					throw GuruhttpExcepcion(500);
				}

				std::string consulta(json["consulta"].asString());
				int limite = json["limite"].asInt();

				if (consulta.empty()) throw GuruhttpExcepcion(400);
				if (limite > limite_json) limite = limite_json;

				Json::Value		resultado;
				consultaJSON(consulta, resultado, limite);

				if (resultado.empty()) enviar_json("[]");

//				cout << resultado.toStyledString() << endl;

				enviar_json(resultado.toStyledString());
			} else {
				throw GuruhttpExcepcion(404);
			}

		} catch (GuruSQLite::GuruSQLiteException) {
			throw GuruhttpExcepcion(500);
		}
	};

};

/*
Ejemplo de uso:

	GuruhttpDirVirtual	GuruHttpSQLite;

	GuruHttpSQLite.sitio("/sqlite/");
	GuruHttpSQLite.agregarHost("*");
	GuruHttpSQLite.interfaz(new GuruhttpSitio<GuruHttpSQLite>());
	GuruHttpSQLite.velocidad(0);
	GuruHttpSQLite.extra_ptr("mame214.sqlite3"); // --> aqui va el nombre archivo de sqlite
	GuruHttpSQLite.agregarAtributosExtra("Access-Control-Allow-Origin", "*");
	GuruHttpSQLite.env("base", "Base de datos del M.A.M.E.");
*/

#endif
