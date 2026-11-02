#ifndef __GURUHTTPCLIENTEREST_HPP__
#define __GURUHTTPCLIENTEREST_HPP__

#include "GuruhttpCliente.hpp"

#include <jsoncpp/json.h>

#include <iostream>
/*
POST:
Ejemplo de uso:

GuruHttpCliente::rest::response res2;
GuruHttpCliente::opciones_post opt2;

opt2.proxy_server("http://frossi:Amapola1@10.1.1.86:3128");

res2 = GuruHttpCliente::rest::post(string("http://www.planetaguru.com.ar/catalogadorws/"), string("{\"username\":\"Fabrcio\",\"clave\":\"Nada\"}"), opt2);

cout << "Status Code:" << res2.status_code() << endl;
cout << res2.body() << endl;
//--------------------------------------------------------------------
GET:
Ejemplo de uso:
GuruHttpCliente::rest::response res2;
GuruHttpCliente::opciones_get opt2;

opt2.proxy_server("http://frossi:Amapola1@10.1.1.86:3128");

res2 = GuruHttpCliente::rest::get(string("http://www.planetaguru.com.ar/catalogadorws/"), opt2);

cout << "Status Code:" << res2.status_code() << endl;
std::cout << "Name:" << res.json()["Name"].asString() << std::endl;
*/

namespace GuruHttpCliente {
	namespace rest {

		class response :public GuruHttpCliente::response {
			friend class proxy_response_rest;

			Json::Value* _json;
		public:
			response() :GuruHttpCliente::response(), _json(NULL) {};

			response(const GuruHttpCliente::rest::response& copia) :GuruHttpCliente::response() {
				_json = copia._json;

				GuruHttpCliente::response::_body = copia._body;
				GuruHttpCliente::response::_tamanio = copia._tamanio;
				GuruHttpCliente::response::_tiempo = copia._tiempo;
				GuruHttpCliente::response::_response_code = copia._response_code;
				GuruHttpCliente::response::_copia = true;

				(const_cast<response&>(copia))._copia = false;
			};

			virtual ~response() {
				if (_copia) {
					this->clear();
				}
			};

			Json::Value& json() {
				return *_json;
			}

			void clear() {
				GuruHttpCliente::response::clear();

				if (_json) { /* cout << "Borrando json..." << endl; */ delete _json; _json = NULL; }
			}

			GuruHttpCliente::rest::response& operator = (const GuruHttpCliente::response& res2) {
				if (this != &res2) {
					GuruHttpCliente::response::operator=(res2);
					//	this->_copia = true;

					//	res2.copia(false);
				}

				return *this;
			}

			GuruHttpCliente::rest::response& operator = (const GuruHttpCliente::rest::response& res2) {
				if (this != &res2) {
					GuruHttpCliente::response::operator=(res2);
					this->_json = res2._json;
					//		_copia = true;

					//		res2.copia(false);
				}

				return *this;
			}
		};

		/*
		static GuruHttpCliente::rest::response& move(GuruHttpCliente::rest::response& copia) {

	            copia.copia(false);
				
				return copia;
			}
		*/

		class proxy_response_rest {
		public:
			proxy_response_rest(GuruHttpCliente::rest::response& res) :res(res) {}
			virtual ~proxy_response_rest() {}

			void leer() {
				Json::Reader lector;
				res._json = new Json::Value;

				if (res.status_code() >= 200 && res.status_code() < 300) {
					if (!lector.parse(res._body->begin_data(), res._body->end_data(), *res._json, false))
						std::cerr << "Error parseando el json" << endl;
				}

			}
		private:
			GuruHttpCliente::rest::response& res;
		};

		GuruHttpCliente::rest::response get(const std::string& url, opciones_get& opt) {
			GuruHttpCliente::rest::response respuesta_rest;
			GuruHttpCliente::rest::proxy_response_rest proxy_respuesta(respuesta_rest);

	//		opt.atributo("Accept: application/json");
			respuesta_rest = GuruHttpCliente::get(url, opt);

			proxy_respuesta.leer();

			return respuesta_rest;
		}

		response get(const std::string& url) {
			opciones_get opt;

			return GuruHttpCliente::rest::get(url, opt);
		}

		GuruHttpCliente::rest::response post(const std::string& url, const string& data, opciones_post& opt) {
			GuruHttpCliente::rest::response respuesta_rest;
			GuruHttpCliente::rest::proxy_response_rest proxy_respuesta(respuesta_rest);

			opt.data(data.c_str());
			opt.atributo("Accept: application/json");
			opt.atributo("Content-Type: application/json");

			respuesta_rest = GuruHttpCliente::post(url, opt);

			proxy_respuesta.leer();

			return respuesta_rest;
		}

	}
}

#endif
