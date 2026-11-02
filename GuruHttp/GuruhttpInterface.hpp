#ifndef GURUHTTPINTERFACE_H
#define	GURUHTTPINTERFACE_H

#include <GuruCompat.h>

#include "GuruhttpRequest.hpp"
#include "GuruhttpResponse.hpp"

using namespace std;

class IGuruhttpSitio {
public:
	IGuruhttpSitio() {};
	virtual ~IGuruhttpSitio() {};

	void cargar_requerimiento(GuruhttpRequest* req, GuruhttpResponse* res) {_request=req;_response=res;};

	inline GuruhttpRequest* request()   { return _request;};
	inline GuruhttpResponse* response() { return _response;};

	virtual void procesarGet() {};
	virtual void procesarPost() {};
	virtual void procesarPut() {};
	virtual void procesarHead() {};
	virtual void procesarDelete() {};
	virtual void procesarTrace() {};
	virtual void procesarOptions() {};

private:
	GuruhttpRequest*	_request;
	GuruhttpResponse*	_response;
};

class SitioWeb: public IGuruhttpSitio {
public:
	SitioWeb():_init(false),subdirectorios(0),t("/") {}
	virtual ~SitioWeb() {}

	inline RecursoHttp&			recurso() { return request()->recurso(); };
	inline RequerimientoHttp&	requerimiento() { return request()->requerimiento(); };
	inline GuruhttpSesion&		sesion() { return request()->sesion(); };
	inline GuruhttpDirVirtual*	config() { return request()->config(); };
	inline PostHttp&			post() { return request()->post(); };

	inline string				parametro(const string& nombre) { return request()->parametro(nombre); }
	inline string				atributo(const string& nombre) { return request()->atributo(nombre); }

	// Llamar obligatoriamente al inicio de ProcesarGet/ProcesarPut/etc...
	inline void init() {
		subdirectorios = t.extraer(request()->recurso().subdirectorio() + request()->recurso().archivo());
		ruta = "/" + request()->recurso().subdirectorio() + request()->recurso().archivo();
		_init = true;
	}

	void enviar_archivo(const string& archivo, const string& tipo_mime="") {
		request()->recurso().ruta_absoluta(archivo);

		if (!tipo_mime.empty()) response()->content_type(tipo_mime);

		response()->enviar_archivo();
	};

	void enviar_buffer(const string& buffer, const string& tipo_mime="") {

		if (!tipo_mime.empty()) response()->content_type(tipo_mime);

		response()->enviar(buffer);
	}

	void enviar_html(const string& buffer) {

		response()->content_type("text/html");

		response()->enviar(buffer);
	}

	bool uri(const string& subdir) {
		if (!_init) init();

		varReq.clear();

		// es exactamente igual?
		bool igual = (subdir.compare(ruta) == 0) ? true : false;

		if (!igual) { // puede ser que tenga un/os campos variables
			GuruTokenEx r("/");
			r.extraer(subdir);

			if (r[0].empty()) { r.borrar(0); }

			if (t.tokens() != r.tokens()) return false;

			for (size_t it = 0, ir = 0; it < r.tokens() || it < t.tokens(); it++, ir++) {
				if (t[it].compare(r[ir]) != 0) { // no es igual, pero puede tener una parte variable
					size_t indexInicio = r[ir].find('{');
					size_t indexFin = r[ir].find('}');

					if (indexInicio != std::string::npos && indexFin != std::string::npos) { // en la cadena existen los simbolos { y }
						std::string variable(r[ir]);

						variable.erase(indexInicio, 1);
						variable.erase(indexFin - 1, 1);

						varReq.insert(std::pair<std::string, std::string>(variable, t[it]));
					} else {
						varReq.clear();
						return false;
					}
				}
			}

			return (varReq.size() == 0) ? false : true;
		} else
			return  igual; // las dos cadenas son iguales
	}

	std::string urivar(const std::string nombre) {
		map<std::string, std::string>::iterator it = varReq.find(nombre);

		if (it == varReq.end()) return std::string();

		return it->second;
	}

protected:
	GuruTokenEx t;
	map<std::string, std::string> varReq;
	std::string	ruta;
	size_t		subdirectorios;
	bool _init;
};

class GuruhttpInterface {
public:
	GuruhttpInterface() {};
	virtual ~GuruhttpInterface() {};

	virtual IGuruhttpSitio* sitio() = 0;
};

template <class T>
class GuruhttpSitio: public GuruhttpInterface {
public:
	GuruhttpSitio() {};
	virtual ~GuruhttpSitio() {};

	inline IGuruhttpSitio* sitio() { return new T(); }
};

#endif
