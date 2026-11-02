#if !defined __GURUSQLITE_ALGO_HPP__
#define __GURUSQLITE_ALGO_HPP__

#include <GuruSQLite.hpp>
#include <GuruCache.hpp> 
#include <sstream>

#include <jsoncpp/json.h>

namespace GuruSQLiteAlgo {

	static Json::Value& result_a_json(GuruSQLite::resultado& res, Json::Value& root) {
		int cols = res.columnas();

		root["Columnas"] = cols;

		for (int a = 0; a < cols; a++) root["Nombres"][GuruStringAlgo::int64astr(a)] = res.nombre_columna(a);

		Json::Value registros;
		int total = 0;
		for (GuruSQLite::resultado::iterator it = res.begin(), itend = res.end(); it != itend; ++it, total++) {

			for (int a = 0; a < cols; a++) registros[GuruStringAlgo::int64astr(a)] = it->getString(a);

			root["Registros"].append(registros);
		}

		root["Total"] = total;

		return root;
	}

	template <typename T>
	static uint64_t id (GuruSQLite::bd& bdd, T& cache, const std::string& tabla, const std::string& columna, const std::string& columnaid, const std::string& valor) {
		uint64_t id=0;
		try {
			id = cache(valor);
		} catch (GuruCacheException&) {
			try {
				id = bdd.ejecutarEX("select " + columnaid + " from " + tabla + " where " + columna + "=" + GuruSQLite::comillas(valor))->getUInt64(0, 0);

				if (id == 0) {
					bdd.exec("insert into " + tabla + "(" + columna + ") values (" + GuruSQLite::comillas(valor) + ")");

					id = static_cast<uint64_t>(bdd.int_sqlite3_last_insert_rowid());
				}

				cache.insertar(valor, id);
			} catch (GuruSQLite::GuruSQLiteException& e) {
				cerr << e.what() << endl;
				throw ;
			}
		}

		return id;
	};

	template <typename T>
	static uint64_t getID(GuruSQLite::bd& bdd, T& cache, const std::string& tabla, const std::string& columna, const std::string& columnaid, const std::string& valor) {
		uint64_t id = 0;
		try {
			id = cache(valor);
		} catch (GuruCacheException&) {
			string consulta;

			GuruSQLite::resultado res;
			try {
				consulta = "select " + columnaid + " from " + tabla + " where " + columna + "=" + GuruSQLite::comillas(valor);

				res = bdd.ejecutar(consulta);

				if (!res.empty()) {
					id = res->getUInt64(0);
					cache.insertar(valor, id);
				}
			}
			catch (GuruSQLite::GuruSQLiteException&) {
				throw;
			}
		}

		return id;
	};

	template <typename T>
	static uint64_t getInt(GuruSQLite::bd& bdd, const T& tabla, const T& columna, const T& columnaid, const T& valor) {
		uint64_t id = 0;

		try {
			id = bdd.ejecutarEX("select " + columnaid + " from " + tabla + " where " + columna + "=" + GuruSQLite::comillas(valor))->getInt64(0, 0);
		} catch (GuruSQLite::GuruSQLiteException&) {
		}

		return id;
	};

};

#endif
