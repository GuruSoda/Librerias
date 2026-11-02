#ifndef __GURUFDBBASE_H__
#define __GURUFDBBASE_H__

#include <iostream>
#include <time.h>
#include <vector>
#include <list>
#include <sstream>

#include <ibpp/ibpp.h>
#include <GuruError.hpp>
#include <GuruAlgo.hpp>

#include <boost/any.hpp>

using namespace std;

class tupla{
public:
	tupla() {};

	boost::any columna(size_t num) {
		if (num <= columnas.size())
			return columnas.at(num - 1);
		else 
			return boost::any();
	};

	void agregar(boost::any& data) {
		columnas.push_back(data);
	};

private:
	vector<boost::any> columnas;
};

template <class T>
std::string FDBFechaRFC822(T fecha)
{
	static const char* meses[] = {"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};

    ostringstream oss;
	// La Fecha
	oss << std::setw(2) << std::setfill('0') << fecha.Day() << " " << meses[fecha.Month()] << " " << std::setw(4) << std::setfill('0') << fecha.Year();

	return oss.str();
}

template <class T>
std::string FDBFecha(T fecha)
{
	ostringstream oss;
	// La Fecha
	oss << std::setw(2) << std::setfill('0') << fecha.Day() << "/" << std::setw(2) << std::setfill('0') << fecha.Month() << "/" << std::setw(4) << std::setfill('0') << fecha.Year();

	return oss.str();
}

template <class T>
std::string FDBHora(T hora)
{
	ostringstream oss;
	// La Hora
	oss << std::setw(2) << std::setfill('0') << hora.Hours() << ":" << std::setw(2) << std::setfill('0') << hora.Minutes() << ":" << std::setw(2) << std::setfill('0') << hora.Seconds();

	return oss.str();
}

template <class T>
time_t FDBFechaHora_time_t(T fechahora)
{
	struct tm timeinfo = {0};

	timeinfo.tm_year = fechahora.Year() - 1900;
	timeinfo.tm_mon = fechahora.Month() - 1;
	timeinfo.tm_mday = fechahora.Day();
	timeinfo.tm_hour = fechahora.Hours();
	timeinfo.tm_min = fechahora.Minutes();
	timeinfo.tm_sec = fechahora.Seconds();
	timeinfo.tm_isdst = -1;

	return mktime(&timeinfo);
}

template <class T>
void FDBtime_t_FechaHora(time_t fechahora_time_t, T& fechahora_timestamp)
{
	struct tm* timeinfo = {0};

	timeinfo = localtime(&fechahora_time_t);

	fechahora_timestamp.SetDate(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
	fechahora_timestamp.SetTime(timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

#define	__INIT_STATEMENT__ \
IBPP::Transaction tr = IBPP::TransactionFactory(conexion()); \
IBPP::Statement	st = IBPP::StatementFactory(bd, tr);

#define	__INIT_STATEMENT_START__ \
IBPP::Transaction tr = IBPP::TransactionFactory(conexion()); \
IBPP::Statement	st = IBPP::StatementFactory(bd, tr); \
tr->Start();

class GuruFDBBase;

class FDBColumna {
	friend class iterator_fdb;
public:
	FDBColumna() {};
	string& str() { return _str;};

	template <typename T>
	T como () {
		try {
			return boost::any_cast<T>(dato_fdb);
		} catch(boost::bad_any_cast&) {
			ostringstream oss;

			oss << "Cast no Valido: " << typeid(T).name() << " <> " << typeid(dato_fdb).name();
			throw oss.str();
		}
	};

	inline string& nombre() { return nombre_columna;}

	IBPP::SDT tipo() {return tipo_fdb;};

protected:
	IBPP::SDT		tipo_fdb;
	boost::any		dato_fdb;
	string			_str;
	string			nombre_columna;

};

class FDBRegistro
{
	friend class iterator_fdb;
public:
	FDBRegistro() {};

	inline FDBColumna& columna (unsigned int nro) {
		if (nro > resultado.size()) throw string("Invalido Numero de Columna.");

		return resultado.at(nro-1);
	};

	inline FDBColumna& columna (const string& nombre) {
		for (vector<FDBColumna>::iterator it=resultado.begin();it!=resultado.end();++it)
			if (GuruStringAlgo::stringequals(it->nombre(), nombre)) return *it;

		throw string("Nombre de Columna invalida.");
	};

	inline unsigned int columnas () {return static_cast<unsigned int>(resultado.size());}

protected:
	vector<FDBColumna>	resultado;
};

class iterator_fdb
{
public:
	iterator_fdb(GuruFDBBase& base, const string& consulta);
	iterator_fdb(GuruFDBBase& base):base(base) {};

	virtual ~iterator_fdb() {}

	void operator++() {
		sacar_registro();
	}

	FDBRegistro* operator->() {
		return static_cast<FDBRegistro*>(&registro);
	}

	FDBRegistro operator*() const {
		return registro;
	}

	bool operator!=(const iterator_fdb& it2) {
		if (registro.resultado.size() != it2.registro.resultado.size()) return true;
		else return false;
	}

	inline bool operator==(const iterator_fdb& it2) {
		return (registro.resultado.size() == it2.registro.resultado.size()) ? true : false;
	}

protected:
	void sacar_registro();
	void cerrar_iteracion();

	GuruFDBBase&		base;
	IBPP::Database		bd;
	IBPP::Transaction	tr;
	IBPP::Statement		st;
	FDBRegistro			registro;
};

class GuruFDBBase
{
public:
	GuruFDBBase(const string& bdd, const string& usuario = "SYSDBA", const string& clave = "masterkey", const string& servidor = ""):bd(NULL),archivo_bdd(bdd),usuario_bdd(usuario),clave_bdd(clave), servidor_bdd(servidor) {
		#if defined IBPP_WINDOWS
			IBPP::ClientLibSearchPaths("..\\win32");
		#endif
	};

	GuruFDBBase() {
		#if defined IBPP_WINDOWS
			IBPP::ClientLibSearchPaths("..\\win32");
		#endif
	};

	GuruFDBBase(const GuruFDBBase& copia) { *this = copia;}

	virtual ~GuruFDBBase(void);

	void abrir (const string& bdd, const string& usuario = "SYSDBA", const string& clave = "masterkey", const string& servidor = "") {
		bd=NULL;
		archivo_bdd=bdd;
		usuario_bdd=usuario;
		clave_bdd=clave;
		servidor_bdd=servidor;

		conectar();
	}

	bool cerrar() {
		return true;
	};

	void crear_bdd(const char* ddl[]);
	void crear_bdd(const string& ddl);
	void borrar_bdd();

	bool ejecutar(const string& consulta);

	inline string bdd() {return archivo_bdd;}

	void conectar();
	inline void desconectar();

	IBPP::Statement statement(IBPP::Statement& st);
	IBPP::Database& conexion();

	iterator_fdb begin(const string& consulta) { return iterator_fdb(*this, consulta);};
	iterator_fdb end() { return iterator_fdb(*this);};

protected:
	IBPP::Database bd;

	string archivo_bdd;
	string usuario_bdd;
	string clave_bdd;
	string servidor_bdd;
};

#endif
