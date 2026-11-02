#ifndef __GURUFIREBIRD_HPP__
#define __GURUFIREBIRD_HPP__

namespace GuruFirebird {
	class GuruFirebirdException: public exception {
	public:
		GuruFirebirdException(const string& msg):exception(),msg(msg) {}
		virtual ~GuruFirebirdException() throw() {}
		const char* what() const throw() { return msg.c_str(); }
	private:
		string msg;
	};

	class registro {
	public:
		registro():sentencia(NULL) {}
		registro(sqlite3_stmt* sentencia):sentencia(sentencia) {}
		virtual ~registro() {}
	}

	class resultado {
	public:
		resultado():sentencia(NULL) {}
		resultado(sqlite3_stmt* sentencia):sentencia(sentencia),reg(sentencia) {}
		virtual ~resultado() {}
	}

	class bd {
	public:
		bd ():handle(NULL) {}
		bd (sqlite3* handle):handle(handle) {}
		virtual ~bd() {}
	}

	bd conectar (const string& archivo) {
		sqlite3* handle;

		if (int salida=sqlite3_open(archivo.c_str(), &handle) != SQLITE_OK) throw GuruSQLiteException(sqlite3_errstr(salida));

		return bd(handle);
	}
}

#endif