#ifndef __GURUHTTPREQUEST_H__
#define __GURUHTTPREQUEST_H__

#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <list>
#include <stdlib.h>

#include <GuruAlgo.hpp>
#include <GuruString_algo.hpp>
#include <GuruBuffer.hpp>
#include <GuruSingleton.hpp>
#include <GuruSocket/GuruSocketBase.hpp>

#include "GuruhttpConfig.hpp"
#include "GuruhttpSession.hpp"
#include "Guruhttp_algo.hpp"

using namespace std;

/*
class GuruHttpCache {
	typedef map<string, string> dataCache;
	typedef map<uint64_t, dataCache > contenedorCache;

public:
	GuruHttpCache() {};
	virtual ~GuruHttpCache() {};

	void cache(uint32_t id, const string& llave, const string& valor) {

	}

	string cache(uint32_t id, const string& llave) {
		contenedorCache::iterator lugar;
		string temp_parametro;

		if ((lugar = data.find(id)) != data.end()) {
			dataCache::iterator ladata;

			if ((ladata=lugar->second.find(llave)) != lugar->second.end()) {
				return ladata->second;
			}
			
		}

		return temp_parametro;

		return string();
	}

private:
	contenedorCache data;
};
*/
class RecursoHttp {
public:
	inline string& directorio() {return str_directorio;};
	inline string& subdirectorio() {return str_subdirectorio;};
	inline string& archivo() {return str_archivo;};
	inline string& ruta_relativa() {return str_ruta_relativa = directorio() + subdirectorio() + archivo();};
	inline string& ruta_absoluta() {return str_ruta_absoluta;};
	string& sacarExtensionArchivo() { return data_archivo.sacar_extension(); };
	inline GuruDirectorio::GuruArchivo& info() { return data_archivo;};

	void directorio(const string& a) {str_directorio=a;};
	void subdirectorio(const string& a) {str_subdirectorio=a;};
	void archivo(const string& a) {str_archivo=a;};
	void ruta_relativa(const string& a) {str_ruta_relativa=a;};
	void ruta_absoluta(const string& a) {
		str_ruta_absoluta=a;
		data_archivo=GuruDirectorio::propiedades(a);
	};
	void info(const GuruDirectorio::GuruArchivo& data) { /*clear(); */ data_archivo=data; str_ruta_absoluta=data.nombre();};
	
	void clear() {
		str_directorio.clear();
		str_subdirectorio.clear();
		str_archivo.clear();
		str_ruta_relativa.clear();
		str_ruta_absoluta.clear();

		data_archivo.limpiar();
	};
protected:
	string 	str_directorio;
	string 	str_subdirectorio;
	string 	str_archivo;
	string 	str_ruta_relativa;
	string 	str_ruta_absoluta;

	GuruDirectorio::GuruArchivo	data_archivo;
};

class RequerimientoHttp {
public:
	RequerimientoHttp():enum_metodo(no_soportado) {};
	typedef enum {get, post, head, put, del, trace, options, no_soportado} metodo;
	typedef enum {http0, http10, http11, http2} version;

	inline metodo	tipo_metodo() { return enum_metodo; };
	inline void		tipo_metodo(metodo enum_metodo) { this->enum_metodo = enum_metodo; };
	inline string&	str_metodo() { return _str_metodo; };
	inline void		str_metodo(const string& _str_metodo) { this->_str_metodo = _str_metodo; };
	inline string&	recurso() { return str_recurso; };
	inline void		recurso(const string& str_recurso) { this->str_recurso = str_recurso; };
	inline string&	str_version() { return _str_version; };
	inline version  http_version() { return enum_version; }
	inline void		http_version(version enum_version) { this->enum_version = enum_version; }
	inline string&	requerimiento() { return str_requerimiento; };
	inline void		requerimiento(const string& str_requerimiento) { this->str_requerimiento = str_requerimiento; };

protected:
	metodo	enum_metodo;
	version	enum_version;
	string	_str_metodo;	
	string	str_recurso;
	string	_str_version;
	string	str_requerimiento;
};

class PostHttp {
public:
	PostHttp () {};

	inline string campo(const string& nombre_atributo) {
		map<string, string>::iterator lugar;
		string temp_atributo;

		if ((lugar = campos.find(nombre_atributo)) != campos.end()) temp_atributo = lugar->second;

		return temp_atributo;
	};

	inline void campo(const string& nombre_atributo, const string& valor_atributo) {
		campos.insert(make_pair(nombre_atributo, valor_atributo));
	}

//	inline void data(std::string& data_post) { _data = data_post; }
	inline char* data() { return _data.buf(); }
	inline GuruBufferChar& configData() { return _data; }

	inline char* body() { return _data.buf(); }

private:
	map<string, string>		campos;		// key=nombre, value=Fabricio D. Rossi
	GuruBufferChar			_data;
};

/// \brief Encapsula una respuesta HTTP.
 /**
 * ResponseHttp tiene toda la informacion para realiazar la respuesta HTTP.
 * \sa 	RecursoHttp, RequerimientoHttp, PostHttp
 */
/*
class ResponseHttp {
public:
	ResponseHttp():_codigo_estado(0) {};
	virtual ~ResponseHttp() {};

	/// retorna el mapa con los extra atributos a ser insertados en el response
	inline const map<string, string>& atributos() {return _atributo;}

	uint16_t codigo_estado() {return _codigo_estado;};
	void codigo_estado(uint16_t codigo) {_codigo_estado = codigo;};

	/// Carga el content-type para utilzar en la siguiente respuesta.
	void content_type(const string& type) {_content_type=type;};

	/// returna el content-type para utilzar en la siguiente respuesta.
	string& content_type() {return _content_type;};

private:
	uint16_t			_codigo_estado;	///< Codigo de estado del request.
	string				_content_type;
};
*/

class GuruhttpSesion {
public:
	GuruhttpSesion() {};
	virtual ~GuruhttpSesion() {};

	string valor(const string llave) {
		map<std::string, std::string>::iterator it=sesion.find(llave);

		if (it == sesion.end()) return string();

		return it->second;
	}

	map<std::string, std::string>& copy_sesion(map<std::string, std::string>& data_sesion) { sesion.clear(); sesion = data_sesion; return sesion;}
	map<std::string, std::string>& data_sesion() { return sesion;}

	void agregar(const string& key, const string& value) {
		sesion.insert(make_pair(key, value));

		GuruSingletonEx<httpSessiones>::instancia().agregar(id, key, value);
	}

	void cookie_id(const string& id) {
		this->id = id;
	}

private:
	map<std::string, std::string>	sesion;
	string							id;
};

/// \brief Encapsula una peticion HTTP.
 /**
 * La GuruhttpRequest tiene toda la informacion enviada de una peticion HTTP.
 * Adicionalmente tiene tambien la informacion de la configuracion del sitio.
 * \sa 	RecursoHttp, RequerimientoHttp, PostHttp
 */
class GuruhttpRequest
{
public:
	GuruhttpRequest(IRecibir& recibir):GuruSocketRecepcion(recibir),confdirvirtual(NULL) {};
	virtual ~GuruhttpRequest() {};

	/// \brief Busca un atributo en la lista de atributos del requerimiento.
	/// \param nombre_atributo nombre del atributo a buscar
	/// \return el valor de atributo buscado
	inline string atributo(const string& nombre_atributo) {
		map<string, string, GuruStringAlgo::ci_less>::iterator lugar;
		string temp_atributo;

		if ((lugar = _atributos.find(nombre_atributo)) != _atributos.end()) temp_atributo = lugar->second;

		return temp_atributo;
	};

	/// \brief Guarda un atributo del requerimiento.
	/// \param nombre_atributo nombre del atributo.
	/// \param valor_atributo el valor de atributo.
	inline void atributo(const string& nombre_atributo, const string& valor_atributo) {

		std::pair<map<string, string, GuruStringAlgo::ci_less>::iterator, bool> it;

		it = _atributos.insert(make_pair(nombre_atributo, valor_atributo));

		campos.push_back(it.first);
	}

	inline map<string, string, GuruStringAlgo::ci_less>& atributos() { return _atributos; }

	inline string parametro(const string& nombre_parametro){
		map<string, string>::iterator lugar;
		string temp_parametro;

		if ((lugar = parametros.find(nombre_parametro)) != parametros.end()) temp_parametro = lugar->second;

		return temp_parametro;
	};

	inline void parametro(const string& nombre_parametro, const string& valor_parametro){
			parametros.insert(make_pair(nombre_parametro, valor_parametro));
	}

	inline map<string, string>& parametro() { return parametros; }

	inline std::vector<map<string, string>::iterator>& campo() { return campos; }

	/// \brief Saca la cookie del requerimiento segun el nombre de la cookie.
	/// \param nombre_cookie Nombre de la Cookie a buscar.
	inline string cookie(const string& nombre_cookie){
		map<string, string>::iterator lugar;
		string temp_parametro;

		if ((lugar = _cookies.find(nombre_cookie)) != _cookies.end()) temp_parametro = lugar->second;

		return temp_parametro;
	};

	/// Para poner una cookie
	inline void cookie(const string& nombre_parametro, const string& valor_parametro){
		_cookies.insert(make_pair(nombre_parametro, valor_parametro));
	};

	inline map<string, string>& cookies() { return _cookies; }

/*
	/// Para poner una cookie
	inline void enviar_cookie(const string& nombre_parametro, const string& valor_parametro){
		server_cookies.push_back(make_pair(nombre_parametro, valor_parametro));
	};

	inline string enviar_cookie(const string& nombre_cookie){
		
		for (list<pair<string, string> >::iterator it=server_cookies.begin();it!=server_cookies.end();++it) {
			if (it->first.compare(nombre_cookie) == 0)
				return it->second;
		}

		return string();
	};
*/

	/// Retorna toda la informacion del recurso pedido.
	inline RecursoHttp&			recurso() {return _recurso;};
	/// Retorna la informacion detalla del requerimiento.
	inline RequerimientoHttp&	requerimiento() {return _requerimiento;};
	inline PostHttp&			post() {return _post;};
	inline GuruhttpSesion&		sesion() {return _sesion;};
	inline IRecibir&			recibir() {return GuruSocketRecepcion;};
	/// Para almacenar la configuracion del sitio.
	inline void					config(GuruhttpDirVirtual* conf) {confdirvirtual=conf;};
	/// Para retornar la configuracion del sitio.
	inline GuruhttpDirVirtual*	config() {return confdirvirtual;};

protected:
	RecursoHttp			_recurso;		///< analisis del URI Ejem: /imagenes/robotech.jpg
	RequerimientoHttp	_requerimiento;	///< req http es: request method, URI, and protocol version
	PostHttp			_post;			///< Si
	GuruhttpSesion		_sesion;		///< Datos guardados de la sesion.
	map<string, string> parametros;		///< key=host, value=localhost:8080
	map<string, string, GuruStringAlgo::ci_less> _atributos;		///< key=imagen, value=robotech.jpg
	map<string, string> _cookies;		///< key=nombreCookie, value=valor de la Cookie
	GuruhttpDirVirtual*	confdirvirtual; ///< Configuracion del sitio
	IRecibir&			GuruSocketRecepcion;

	std::vector<map<string, string>::iterator> campos;
};

#endif
