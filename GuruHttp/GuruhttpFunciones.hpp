#ifndef __GURUHTTPFUNCIONES_H__
#define __GURUHTTPFUNCIONES_H__

#include <string>
#include <vector>
#include <map>

#include <GuruInterprete/GuruInterprete.hpp>
#include <GuruHttp/GuruhttpRequest.hpp>
#include <GuruHttp/Guruhttp_algo.hpp>

using namespace std;

class GuruhttpFunciones: public GuruInterprete::GuruFunciones<GuruhttpFunciones> {
	GuruhttpRequest* request;
	GuruhttpResponse* response;

	vector<map<string, string>::iterator>::iterator it_campos;
	std::pair<string, string> campo;

public:
	GuruhttpFunciones (GuruhttpRequest* request):request(request) {
		cargar_funcion(this, &GuruhttpFunciones::parametro, "http.parametro");
		cargar_funcion(this, &GuruhttpFunciones::atributo, "http.atributo");

		// para extraer los campos/atributo del request de la forma en que el navegador las entrego
		cargar_funcion(this, &GuruhttpFunciones::init_campos, "http.init_campos");
		cargar_funcion(this, &GuruhttpFunciones::extraer_campo, "http.extraer_campo");
		cargar_funcion(this, &GuruhttpFunciones::extraer_campo_llave, "http.extraer_campo_llave");
		cargar_funcion(this, &GuruhttpFunciones::extraer_campo_valor, "http.extraer_campo_valor");

		cargar_funcion(this, &GuruhttpFunciones::cookie, "http.cookie");
		cargar_funcion(this, &GuruhttpFunciones::header, "http.header");
		cargar_funcion(this, &GuruhttpFunciones::recurso, "http.recurso");
		cargar_funcion(this, &GuruhttpFunciones::requerimiento, "http.requerimiento");
		cargar_funcion(this, &GuruhttpFunciones::encode, "http.urlencode");
		cargar_funcion(this, &GuruhttpFunciones::decode, "http.urldecode");
		cargar_funcion(this, &GuruhttpFunciones::absoluto, "http.absoluto");
		cargar_funcion(this, &GuruhttpFunciones::cache, "http.cache");
		cargar_funcion(this, &GuruhttpFunciones::iniciar_session, "http.iniciar_session");
		cargar_funcion(this, &GuruhttpFunciones::detener_session, "http.detener_session");
		cargar_funcion(this, &GuruhttpFunciones::data_session, "http.data_session");
		cargar_funcion(this, &GuruhttpFunciones::archivo, "http.archivo");
		cargar_funcion(this, &GuruhttpFunciones::mime, "http.mime");

		it_campos = request->campo().end();
	};

	virtual ~GuruhttpFunciones() {};

	void cargar_encabezados(GuruhttpRequest* request, GuruhttpResponse* response) {
		this->request  = request;
		this->response = response;
	}

	string parametro(const vector<string>& param) {
		return (param.size() == 1) ? request->parametro(param.at(0)) : string();
	}

	string atributo(const vector<string>& param) {
		return (param.size() == 1) ? request->atributo(param.at(0)) : string();
	}

	string init_campos(const vector<string>& param) {
		it_campos = request->campo().begin();

//		if (it_campos == request->campo().end()) return string("0");

		return string("1");
	}

	string extraer_campo(const vector<string>& param) {
		if (it_campos != request->campo().end()) {
			campo.first = (*it_campos)->first;
			campo.second = (*it_campos)->second;

			it_campos++;

			return string("1");
		} else 
			return string("0");
	}

	string extraer_campo_llave(const vector<string>& param) {
		return campo.first;
	}

	string extraer_campo_valor(const vector<string>& param) {
		return campo.second;
	}

	string cookie(const vector<string>& param) {
		if (param.size() != 1 && param.size() != 2) return string();

		string nombre,valor;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: nombre=param.at(i); break;
			case 1: valor=param.at(i); break;
			}
		}

		// hay que retornar el valor
		if (valor.empty()) {
			return request->cookie(nombre);
		} else { // hay que poner el valor
			request->cookie(nombre, valor);
			return string();
		}
	}

	string header(const vector<string>& param) {
		if (param.size() != 2) throw string("Se necesitan dos parametros");

		response->atributo(param.at(0), param.at(1));

		return string();
	}

	string recurso(const vector<string>& param) {
		return (param.size() == 0) ? request->recurso().ruta_absoluta() : string();
	}

	string requerimiento(const vector<string>& param) {
		return (param.size() == 0) ? request->requerimiento().requerimiento() : string();
	}

	string encode(const vector<string>& param) {
		return (param.size() == 1) ? GuruhttpAlgo::URLEncode(param.at(0)) : string();
	}

	string decode(const vector<string>& param) {
		return (param.size() == 1) ? GuruhttpAlgo::UrlDecode(param.at(0)) : string();
	}

	string absoluto(const vector<string>& param) {
		if (param.size() != 1) return string();

		string tmp;

		tmp=request->config()->dirApps() +  "/" + param.at(0);

		#ifdef __GURUWIN32__
			if (tmp.find('/') != string::npos)
				std::replace_if(tmp.begin(), tmp.end(),bind2nd(std::equal_to<char>(), '/') , '\\');
		#endif

		return tmp;
	}

	string cache(const vector<string>& param) {
		if (param.size() != 1 && param.size() != 2) return string();

		string nombre,valor;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: nombre=param.at(i); break;
			case 1: valor=param.at(i); break;
			}
		}

		// hay que retornar el valor
		if (valor.empty()) {
		} else { // hay que poner el valor
		}

	return string();
	}

	string iniciar_session(const vector<string>& param) {
		string tmp(GuruStringAlgo::rand_str_min());

		request->cookie("GuruServerID", tmp);
		
		return tmp;
	}

	string detener_session(const vector<string>& param) {
		string tmp(GuruStringAlgo::rand_str_min());

		request->cookie("GuruServerID", tmp);
		
		return tmp;
	}

	string data_session(const vector<string>& param) {
		string tmp(GuruStringAlgo::rand_str_min());

		request->cookie("GuruServerID", tmp);
		
		return tmp;
	}

	string archivo(const vector<string>& param) {
		request->recurso().ruta_absoluta(param.at(0));
		return string();
	}

	string mime(const vector<string>& param) {
		if (param.empty()) return string();

		return GuruhttpAlgo::mime(param.at(0));
	}
};

#endif
