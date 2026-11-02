#ifndef __GURUSQLITE_HPP__
#define __GURUSQLITE_HPP__

#include <string>
#include <exception>
#include <iostream>
#include <sqlite/sqlite3.h>

#include <GuruCompat.h>
#include <GuruBuffer.hpp>
#include <GuruToken.hpp>
#include <GuruLog.hpp>

using namespace std;

namespace GuruSQLite {

	static const char* version = "2.1.5";
	static const char* version_number = "200105";

	typedef std::basic_string <unsigned char> SQLiteString; // Ejem: SQLiteString s = sqlite3_column_text(stmts, 0);

	enum datatypes { type_none=0, type_integer = 1, type_float = 2, type_blob = 4, type_null = 5, type_text = 3 };

	class GuruSQLiteException: public std::exception {
	public:
		GuruSQLiteException(int errorcode, const string& msg=""):exception(), _msg(msg), _errorcode(errorcode) {}
		GuruSQLiteException(const string& msg, int errorcode=0) :exception(), _msg(msg), _errorcode(errorcode) {}
		virtual ~GuruSQLiteException() throw() {}

		const char* what() const throw() {
			if (_msg.empty()) {
				return sqlite3_errstr(_errorcode);
			} else 
				return _msg.c_str(); 
		}
		const int errorcode() const { return this->_errorcode; }
	private:
		string	_msg;
		int		_errorcode;
	};

	class registro {
		friend class resultado;
	public:
		registro():sentencia(NULL),tipo_columna(0)/*, columnas(0)*/ {}
		registro(sqlite3_stmt** sentencia):sentencia(sentencia),tipo_columna(0)/*, columnas(0)*/ { }
		virtual ~registro() {}

		string& getString(int columna, string& dato) {
			if (columna > sqlite3_column_count(*sentencia)) return dato;

//			return dato = reinterpret_cast<const char*>(sqlite3_column_text(sentencia, columna));
//			return dato = (const char*)(sqlite3_column_text(sentencia, columna)); // no probado
//			dato = (const char*)sqlite3_column_text(sentencia, columna);

			const char *ptr_dato;
			if ((ptr_dato = (const char*)sqlite3_column_text(*sentencia, columna)) != NULL)
				dato=ptr_dato;
			return dato;
		}

		string getString(int columna, const string& val_def="") {
			if (columna > sqlite3_column_count(*sentencia)) return val_def;

			const char* a = (const char*)sqlite3_column_text(*sentencia, columna);

			return (a == NULL) ? string(val_def) : static_cast<const char*>(a);
		}

		GuruBuffer& getBlob(int columna, GuruBuffer& dato) {
			return dato;
		}

		GuruBuffer getBlob(int columna) {
			return GuruBuffer();
		}

		double getDouble(int columna, double val_def=-1) {
			if (columna > sqlite3_column_count(*sentencia)) return val_def;

			return (*sentencia == NULL) ? val_def : static_cast<float>(sqlite3_column_int(*sentencia, columna));
		}

		int getInt(int columna, int val_def=-1) {
			if (columna > sqlite3_column_count(*sentencia)) return val_def;

			return (*sentencia == NULL) ? val_def : sqlite3_column_int(*sentencia, columna);
		}

		::uint16_t getUInt16(int columna, ::uint16_t val_def=-1) {
			if (columna > sqlite3_column_count(*sentencia)) return val_def;

			return (*sentencia == NULL) ? val_def : static_cast< ::uint16_t >(sqlite3_column_int(*sentencia, columna));
		}

		::uint32_t getUInt32(int columna, ::uint32_t val_def=-1) {
			if (columna > sqlite3_column_count(*sentencia)) return val_def;

			return (*sentencia == NULL) ? val_def : static_cast< ::uint32_t >(sqlite3_column_int(*sentencia, columna));
		}

		::uint64_t getUInt64(int columna, ::uint64_t val_def=-1) {
			if (columna > sqlite3_column_count(*sentencia)) return val_def;

			return (*sentencia == NULL) ? val_def : static_cast< ::uint64_t >(sqlite3_column_int64(*sentencia, columna));
		}

		::int16_t getInt16(int columna, ::int16_t val_def=-1) {
			if (columna > sqlite3_column_count(*sentencia)) return val_def;

			return (*sentencia == NULL) ? val_def : static_cast< ::int16_t >(sqlite3_column_int(*sentencia, columna));
		}

		::int32_t getInt32(int columna, int32_t val_def=-1) {
			if (columna > sqlite3_column_count(*sentencia)) return val_def;

			return (*sentencia == NULL) ? val_def : static_cast< ::int32_t >(sqlite3_column_int(*sentencia, columna));
		}

		::int64_t getInt64(int columna, ::int64_t val_def=-1) {
			if (columna > sqlite3_column_count(*sentencia)) return val_def;

			return (*sentencia == NULL) ? val_def : static_cast< ::int64_t >(sqlite3_column_int64(*sentencia, columna));
		}

		string getDateTimeRFC822(int columna) {
			return string();
		}

		bool data() {
			return (sqlite3_column_count(*sentencia) != 0) ? true : false;
		}

		bool is_null(int columna) {
			return (sqlite3_column_type(*sentencia, columna) == SQLITE_NULL) ? true : false;
		}

		bool is_integer(int columna) {
			return (sqlite3_column_type(*sentencia, columna) == SQLITE_INTEGER) ? true : false;
		}

		bool is_float(int columna) {
			return (sqlite3_column_type(*sentencia, columna) == SQLITE_FLOAT) ? true : false;
		}

		bool is_text(int columna) {
			return (sqlite3_column_type(*sentencia, columna) == SQLITE3_TEXT) ? true : false;
		}

		bool is_blob(int columna) {
			return (sqlite3_column_type(*sentencia, columna) == SQLITE_BLOB) ? true : false;
		}

		void operator = (const registro& reg) {
			this->sentencia = reg.sentencia;
			this->tipo_columna = reg.tipo_columna;
//			this->columnas = sqlite3_column_count(*reg.sentencia);
		}

		string nombre_columna(int columna) { return string(sqlite3_column_name(*sentencia, columna)); };
		int columnas() { return sqlite3_column_count(*sentencia); };

	private:
		sqlite3_stmt**	sentencia;
		int				tipo_columna;
//		int				columnas;
	};

	static inline string comillas(const string& cadena) {
		return string("\"" + cadena + "\"");
	};

	class resultado {
		friend class bd;
	public:
		resultado() {
			this->sentencia = NULL;
			reg.sentencia = &this->sentencia; 
			_copia = false;
		}
		resultado(const sqlite3_stmt* sentencia) {
			this->sentencia = const_cast<sqlite3_stmt*>(sentencia);
			this->reg.sentencia = &this->sentencia; 
			this->_copia = true;
		}
		resultado(const resultado& res_copia) {
			this->sentencia = res_copia.sentencia;
			this->reg = res_copia.reg;
			this->reg.sentencia = &this->sentencia;
			this->_copia = true;

			// res_copia._copia = false;
		}
		virtual ~resultado() {
			if (_copia) 
				finalizar(); 
		}
/*
		resultado& move(resultado& copia) {
			operator=(copia);

			copia._copia = false;	
			_copia = true;

			return *this;
		}		
*/
		class iterator {
		public:
			iterator():sentencia(NULL), reg(NULL), finalizado(true) {}
			iterator(sqlite3_stmt** sentencia):sentencia(sentencia),reg(sentencia) {
				if (*sentencia == NULL) finalizado = true;
				else finalizado = false;
			}
			virtual ~iterator() {}

			bool operator != (const iterator& it) {
				if (this->finalizado != it.finalizado) return true;
				else return false;
			}

			bool operator == (const iterator& it) {
				if (this->finalizado == it.finalizado) return true;
				else return false;
			}

			iterator& operator = (const iterator& it) {
				this->finalizado = it.finalizado;
				return *this;
			}

			void operator++ () {
				int result = sqlite3_step(*sentencia);

				switch (result) {
				case SQLITE_ROW: return; break;
				case SQLITE_DONE:
					if (int salida = sqlite3_finalize(*sentencia) != SQLITE_OK) throw GuruSQLiteException(sqlite3_errstr(salida));
					*sentencia = NULL;
					finalizado = true;
					break;
				default:
					throw GuruSQLiteException(sqlite3_errstr(result));
				}
			}

			registro* operator -> () { return &reg; }

//			registro& operator* () { return reinterpret_cast<registro&>(res->reg); }

		private:
			sqlite3_stmt**			sentencia;
			GuruSQLite::registro	reg;
			bool					finalizado;
		};

/*
// truco para modificar un objeto constante, llamado desde el operador de asignacion ( = )
		static sqlite3_stmt* copiar_sentencia(resultado& e) {
			e._copia = false;

			return e.sentencia;
		}
*/
		resultado& operator = (const resultado& res2) {
			if (this->sentencia != NULL && this->sentencia != res2.sentencia) finalizar();

			// truco de modificar un objeto constante.
	//		this->sentencia = res2.copiar_sentencia(const_cast<resultado&>(res2));

			this->sentencia = res2.sentencia;
			this->reg = res2.reg;
			this->reg.sentencia = &this->sentencia;
			this->_copia = true;

			(const_cast<resultado&>(res2))._copia = false;

//			res2.copia(false);

			return *this;
		}
/*
	#if defined __GURULINUX__
		resultado& operator = (const resultado& res2) {
			if (this->sentencia != NULL) finalizar();

			this->sentencia = res2.sentencia;
			this->reg = res2.reg;
			this->reg.sentencia = &this->sentencia;
			this->_copia = true;

			return *this;
		}
	#endif
*/
		void reset() {if (int salida=sqlite3_reset(sentencia) != SQLITE_OK) throw GuruSQLiteException(sqlite3_errstr(salida));}

		registro* operator -> () { return &this->reg; }
		registro& operator* () { return reg;}

		GuruSQLite::registro& registro_actual() { return reg; }

		GuruSQLite::resultado::iterator begin() {
//			fetch();
			GuruSQLite::resultado::iterator it = GuruSQLite::resultado::iterator(&this->sentencia);

			it.operator++();

			return it; //iterator(&this->sentencia);
		}
		iterator end() { return iterator(); }

		bool empty() {
			// puede recibir un NULL y retorna 0
			return (sqlite3_data_count(sentencia) == 0) ? true : false;
		};

		inline void finalizar() {
			sqlite3_finalize(sentencia);
			sentencia = NULL;
		}

		int columnas() { return sqlite3_column_count(sentencia); };
		string nombre_columna(int columna) { return string(sqlite3_column_name(sentencia, columna));};

		bool extraer_registro() {
			int result = sqlite3_step(sentencia);
			bool ret=false;

			switch (result) {
			case SQLITE_ROW: ret = true;
					break;
			case SQLITE_DONE:
					if (int salida = sqlite3_reset(sentencia) != SQLITE_OK) 
						throw GuruSQLiteException(sqlite3_errstr(salida));

					if (int salida = sqlite3_clear_bindings(sentencia) != SQLITE_OK) 
						throw GuruSQLiteException(sqlite3_errstr(salida));

					if (int salida = sqlite3_finalize(sentencia) != SQLITE_OK) 
						throw GuruSQLiteException(sqlite3_errstr(salida));

					sentencia = NULL;
					ret = false;
					break;
			default:
					throw GuruSQLiteException(sqlite3_errstr(result));
			}

			return ret;
		}

		// igual que fetch, solo por compatibilidad.
		bool extraer() {
			return extraer_registro();
		}

		bool fetch() {
			return extraer_registro();
		}

		void copia(bool estado) { this->_copia = estado; }
		bool copia() const { return this->_copia; }

	private:
		GuruSQLite::registro reg;
		sqlite3_stmt* sentencia;
		bool _copia;
	};

	class sentencia {
		friend class bd;
	public:
		sentencia() {};
		sentencia(const sentencia& copia):stmt(copia.stmt),texto_sentencia(copia.texto_sentencia),bd(copia.bd) {};
		virtual ~sentencia() {};

		inline void consulta(const string& texto) { texto_sentencia = texto; }
		inline string& consulta() { return texto_sentencia; }

		inline bool bind(int pos, double doble) {
			return (sqlite3_bind_double(stmt, pos, doble) == SQLITE_OK) ? true : false;
		}

		inline bool bind(int pos, int _int) {
			return (sqlite3_bind_int(stmt, pos, _int) == SQLITE_OK) ? true : false;
		}

		inline bool bind(int pos, uint32_t _int) {
			return (sqlite3_bind_int(stmt, pos, _int) == SQLITE_OK) ? true : false;
		}

		inline bool bind(int pos, int64_t _int) {
			return (sqlite3_bind_int64(stmt, pos, _int) == SQLITE_OK) ? true : false;
		}

		inline bool bind(int pos, uint64_t _int) {
			return (sqlite3_bind_int64(stmt, pos, _int) == SQLITE_OK) ? true : false;
		}

		inline bool bind(int pos, const string& cadena, bool copy = true) {
			return (sqlite3_bind_text(stmt, pos, cadena.c_str(), static_cast<int>(cadena.length()), ((copy == true) ? SQLITE_TRANSIENT : SQLITE_STATIC))) ? true : false;
		}

		inline bool bind(int pos, const char* cadena, bool copy = true) {
			return (sqlite3_bind_text(stmt, pos, cadena, static_cast<int>(strlen(cadena)), ((copy == true) ? SQLITE_TRANSIENT : SQLITE_STATIC))) ? true : false;
		}

		sentencia& operator = (const sentencia& s2) {
			texto_sentencia = s2.texto_sentencia;
			stmt = s2.stmt;
			bd = s2.bd;
		}

		void finalizar() {
			if (sqlite3_reset(stmt) != SQLITE_OK)
				throw GuruSQLiteException(sqlite3_errmsg(bd), sqlite3_extended_errcode(bd));

			if (sqlite3_clear_bindings(stmt) != SQLITE_OK)
				throw GuruSQLiteException(sqlite3_errmsg(bd), sqlite3_extended_errcode(bd));
		}

	private:
		sqlite3_stmt*	stmt;
		string			texto_sentencia;
		sqlite3*		bd;					// solo para el manejo de excepciones
	};

	class bd {
	public:
		bd():_handle(NULL) {}
		bd(sqlite3* handle):_handle(handle) {}
//		bd(const string& bdd) {}
		virtual ~bd() { /* desconectar();*/ }

		enum modo { 
			lectura				= 0x01, 
			lecturaEscritura	= 0x02, 
			crear				= 0x04,
			noCrear				= 0x08,
		};

		// Metodo ejecutar
		// Ejecuta una sentencia y retorna un objeto resultado.
		// Retorna un objeto vacio, para acceder al primer dato y los consecutivos hay que llamar a fetch.
		GuruSQLite::resultado ejecutar(const string& consulta) {
			sqlite3_stmt* sentencia=NULL;

			if (int salida = sqlite3_prepare_v2(_handle, consulta.c_str(), -1, &sentencia, NULL) != SQLITE_OK) {
				throw GuruSQLiteException(sqlite3_errmsg(_handle), sqlite3_extended_errcode(_handle));
			}

			return GuruSQLite::resultado(sentencia);
		}

		// Metodo ejecutar
		// Ejecuta una sentencia y retorna un objeto resultado.
		// Retorna un objeto vacio, para acceder al primer dato y los consecutivos hay que llamar a fetch.
		GuruSQLite::resultado ejecutar(sentencia& sent) {

			if (int salida = sqlite3_prepare_v2(_handle, sent.consulta().c_str(), -1, &sent.stmt, NULL) != SQLITE_OK)
				throw GuruSQLiteException(sqlite3_errmsg(_handle), sqlite3_extended_errcode(_handle));

			return GuruSQLite::resultado(sent.stmt);
		}

		// Metodo ejecutar
		// NO crea un objeto resultado, sino es pasado por parametro y retorna el mismo objeto.
		// Aparte de realizar la consulta trae el primer registro.
		GuruSQLite::resultado& ejecutar(const string& consulta, GuruSQLite::resultado& res) {

			res = ejecutar(consulta);

			res.fetch();

			return res;
		}

		// Metodo ejecutarEX:
		// Ejecuta la consulta, si esta todo correcto llama a fetch para traer el primer registro.
		// Retorna un nuevo objeto GuruSQLite::resultado con el primer registro cargado.
		GuruSQLite::resultado ejecutarEX(const string& consulta) {

			sqlite3_stmt* sentencia = NULL;

			if (int salida = sqlite3_prepare_v2(_handle, consulta.c_str(), -1, &sentencia, NULL) != SQLITE_OK) throw GuruSQLiteException(sqlite3_errmsg(_handle), sqlite3_extended_errcode(_handle));

			int result = sqlite3_step(sentencia);

			switch (result) {
			case SQLITE_ROW: break;
			case SQLITE_DONE:
				if (int salida = sqlite3_finalize(sentencia) != SQLITE_OK)
					throw GuruSQLiteException(sqlite3_errmsg(_handle), sqlite3_extended_errcode(_handle));

				sentencia = NULL;
				break;
			default:
				throw GuruSQLiteException(sqlite3_errmsg(_handle), sqlite3_extended_errcode(_handle));
			}

			return GuruSQLite::resultado(sentencia);
		}

		GuruSQLite::resultado ejecutarEX(sentencia& sent) {

			int result = sqlite3_step(sent.stmt);

			switch (result) {
			case SQLITE_ROW: break;
			case SQLITE_DONE: sent.finalizar();
				break;
			default:
				throw GuruSQLiteException(sqlite3_errmsg(_handle), sqlite3_extended_errcode(_handle));
			}

			return GuruSQLite::resultado(sent.stmt);
		}

		// Metodo exec:
		// Ejecuta una consulta que no tiene resultado, ejem: insert, update, etc
		// Si lo ejecutado tiene algun resultado lo descarta.
		bool exec(const string& consulta) {
			sqlite3_stmt* sentencia = NULL;

			if (sqlite3_prepare_v2(_handle, consulta.c_str(), -1, &sentencia, NULL) != SQLITE_OK)
				throw GuruSQLiteException(sqlite3_errmsg(_handle), sqlite3_extended_errcode(_handle));

			int result = sqlite3_step(sentencia);

			if (result == SQLITE_DONE || result == SQLITE_OK || result == SQLITE_ROW)
				sqlite3_finalize(sentencia);
			else 
				throw GuruSQLiteException(sqlite3_errmsg(_handle), sqlite3_extended_errcode(_handle));

			return true;
		}

		// Metodo exec:
		// Ejecuta una consulta que no tiene resultado, ejem: insert, update, etc
		// Si lo ejecutado tiene algun resultado lo descarta.
		bool exec(sentencia& sent) {

			int result = sqlite3_step(sent.stmt);

			if (result == SQLITE_DONE || result == SQLITE_OK || result == SQLITE_ROW) {
				sent.finalizar();
			} else
				throw GuruSQLiteException(sqlite3_errmsg(_handle), sqlite3_extended_errcode(_handle));

			return true;
		}

		template <typename func>
		void ejecutar(const string& consulta, func& functor) {
			resultado res;

			res=ejecutar(consulta);

			for (GuruSQLite::resultado::iterator it=res.begin(); it!=res.end();++it) 
					functor(it);
		}

		inline bool conectado () {
			return (_handle==NULL)? false : true;
		}

		inline void desconectar() {
			sqlite3_close_v2(_handle);
			_handle=NULL;
		}

		inline sqlite3_int64 int_sqlite3_last_insert_rowid() {
			return sqlite3_last_insert_rowid(_handle);
		}

		// Metodo sentencia
		// crea una sentencia SQL.
		// la sentencia puede tener signos de pregunta en lugar de datos ( ? ).
		GuruSQLite::sentencia sentencia(const string& consulta="") {
			sqlite3_stmt* sentencia = NULL;

			if (int salida = sqlite3_prepare_v2(_handle, consulta.c_str(), -1, &sentencia, NULL) != SQLITE_OK)
				throw GuruSQLiteException(sqlite3_errmsg(_handle), sqlite3_extended_errcode(_handle));

			GuruSQLite::sentencia stmt;

			stmt.texto_sentencia = consulta;
			stmt.stmt = sentencia;
			stmt.bd = _handle;

			return stmt;
		};

		inline sqlite3* handle() { return _handle; }
		inline void handle(sqlite3* handle) { _handle = handle; }

	private:
		sqlite3* _handle;
	};

	static void agregar_escalar(GuruSQLite::bd bdd, const string& nombre, void (*xFunc)(sqlite3_context*,int,sqlite3_value**)) {
//		sqlite3_create_function_v2(bdd.handle, nombre.c_str(), 1, SQLITE_DETERMINISTIC, NULL, xFunc, NULL, NULL, NULL, NULL);
	};

	// En caso que la base de datos no exista la crea y ejecuta la creacion.
	static GuruSQLite::bd conectar(const string& archivo, const char* ddl=NULL, int acceso = GuruSQLite::bd::modo::lecturaEscritura) {
		sqlite3* handle = NULL;
		GuruSQLite::bd bdd;
		int ret = 0;
		int	opt_open = 0;

		bool crear_ddl = (GuruDirectorio::existe(archivo) == true) ? false : true;

		if (crear_ddl == true && ddl == NULL) throw GuruSQLiteException(sqlite3_errstr(SQLITE_CANTOPEN));

		if (acceso & GuruSQLite::bd::modo::crear && crear_ddl == false) GuruDirectorio::borrarArchivo(archivo);
		
		if (acceso & GuruSQLite::bd::modo::lecturaEscritura) {
			opt_open |= SQLITE_OPEN_READWRITE;
			if (acceso & GuruSQLite::bd::modo::crear) {
				opt_open |= SQLITE_OPEN_CREATE;
				crear_ddl = true;
			}
		} else if (acceso & GuruSQLite::bd::modo::lectura) opt_open |= SQLITE_OPEN_READONLY;

		if ((ret = sqlite3_open_v2(archivo.c_str(), &handle, opt_open, NULL)) != SQLITE_OK) {

			if (acceso & GuruSQLite::bd::modo::lecturaEscritura && !(acceso & GuruSQLite::bd::modo::noCrear)) opt_open |= SQLITE_OPEN_CREATE;
			else throw GuruSQLiteException(sqlite3_errstr(ret));

			if ((ret = sqlite3_open_v2(archivo.c_str(), &handle, opt_open, NULL)) != SQLITE_OK)
				throw GuruSQLiteException(sqlite3_errstr(ret));
		}

		bdd.handle(handle);

		sqlite3_extended_result_codes(bdd.handle(), 1);

		try {
			if (ddl != NULL && crear_ddl) {
				GuruTokenEx tok(";");

				size_t total = tok.extraer(ddl);
				for (size_t i = 0; i < total; i++) {
				//	cout << tok[i] << endl;
					if (!tok[i].empty())
						bdd.exec(tok[i]);
				}
			}

		} catch (GuruSQLite::GuruSQLiteException& e) {
			cout << e.what() << endl;
			throw;
		}
		
		return bdd;
	}
};

#endif
