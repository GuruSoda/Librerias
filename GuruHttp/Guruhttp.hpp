#ifndef __GURUHTTP_H__
#define __GURUHTTP_H__

#include <map>
#include <list>
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>

#include <GuruCompat.h>
#include <GuruVariable.hpp>
#include <GuruSingleton.hpp>
#include <GuruCache.hpp>
#include <GuruSocket/GuruSocketServidor.hpp>

#include "GuruhttpInterface.hpp"
#include "GuruhttpExcepcion.hpp"
#include "GuruhttpRequest.hpp"
#include "GuruhttpResponse.hpp"
#include "GuruhttpRequest_algo.hpp"
#include "GuruhttpLog.hpp"

using namespace std;

class Guruhttp: public GuruSocketServicio
{
public:
	Guruhttp() {};
	virtual ~Guruhttp() {};

	// en HTTP/1.0 las conexiones NO son persistentes, a menos que en el request diga lo contrario ( connection: keep-alive )
	// en HTTP/1.1 las conexiones SI son persistentes, a menos que en el request diga lo contrario ( connection: close )
	bool es_requestPersistente(GuruhttpRequest& request) {
		string	connection(request.atributo("Connection"));
		bool	persistente = false;

		if (connection.empty()) {
			if (request.requerimiento().http_version() == RequerimientoHttp::http10) persistente = false;
			if (request.requerimiento().http_version() == RequerimientoHttp::http11) persistente = true;
		} else {
			if (GuruStringAlgo::stringequals(connection, string("keep-alive"))) persistente = true;
			if (GuruStringAlgo::stringequals(connection, string("close"))) persistente = false;
		}

		return persistente;
	}

	void main(void) {

		GuruCacheLRU<std::string, IGuruhttpSitio*> cachesitios;

		int total_requests = 0;
		bool persistente = false;

		do {
			GuruhttpRequest		request(this->recibir);
			GuruhttpResponse	response(this->enviar, request);

			IGuruhttpSitio*		sitio=NULL;

			try {
				ProcesarRequest::procesarRequest(request);

				persistente = es_requestPersistente(request);

				if (persistente) {
					try {
						sitio = cachesitios(request.config()->sitio());
					} catch (GuruCacheException&) {
						sitio = request.config()->interfaz()->sitio();
						cachesitios.insertar(request.config()->sitio(), sitio);
					}
				} else {
					sitio = request.config()->interfaz()->sitio();
				}

				sitio->cargar_requerimiento(&request, &response);

				switch (request.requerimiento().tipo_metodo()) {
					case RequerimientoHttp::get: sitio->procesarGet(); break;
					case RequerimientoHttp::post: sitio->procesarPost(); break;
					case RequerimientoHttp::head: sitio->procesarHead(); break;
					case RequerimientoHttp::put: sitio->procesarPut(); break;
					case RequerimientoHttp::del: sitio->procesarDelete(); break;
					case RequerimientoHttp::trace: sitio->procesarTrace(); break;
					case RequerimientoHttp::options: sitio->procesarOptions(); break;
					case RequerimientoHttp::no_soportado: 
								throw GuruhttpExcepcion(405); break;
				}

				GuruSingleton<GuruhttpLog>::instancia()->escribirLog(request, response);
			} catch (GuruhttpExcepcionSitioNoEncontrado& e) {
//					ProcesarRequest::GuruhttpVar a(request);
//					GuruSingleton<GuruhttpLog>::instancia()->escribirLog(GuruVariable<ProcesarRequest::GuruhttpVar>(&a, &ProcesarRequest::GuruhttpVar::reemplazar).reemplazar("%fecha822% | " + GuruStringAlgo::itos(e.codigo()) + " | %ip_request% | %request% | %request.User-Agent% | %request.Host%"));

				e.enviar_error(SocketEntrada());

//				if (e.codigo() >= 400) {
//					ProcesarRequest::GuruhttpVar a(request);
//					GuruSingleton<GuruhttpLog>::instancia()->escribirLog(GuruVariable<ProcesarRequest::GuruhttpVar>(&a, &ProcesarRequest::GuruhttpVar::reemplazar).reemplazar("%fecha822% | " + GuruStringAlgo::itos(e.codigo()) + " | %ip_request% | %request% | %request.User-Agent% | %request.Host%"));
//				} else if (e.codigo() >= 300) break;
			} catch (GuruhttpExcepcion& e) {
				response.codigo_estado(e.codigo()); // 03/10
				GuruSingleton<GuruhttpLog>::instancia()->escribirLog(request, response); // 03/10

//				ProcesarRequest::grabar_log(request);
				// cout << request.requerimiento().requerimiento() << " | Error: " << e.codigo() << endl;
				// los errores menores a 100 son interno de la aplicacion
				// los errores mayores a 100 son del http server (codigo de error)
				e.enviar_error(SocketEntrada());

				if (e.codigo() >= 300 && e.codigo() < 400) break;

			} catch (GuruSocketException& e) {
	// Esto escribe una linea de error de socket en el archivo error.log del servidor.
	//			GuruhttpVar a(*this, request);
	//			GuruSingleton<GuruhttpLog>::instancia()->escribirLog(GuruVariable<GuruhttpVar>(&a, &Guruhttp::GuruhttpVar::reemplazar).reemplazar("%fecha822% | 000 | %ip_request% | " + string(e.what())));
				GURULOG_DEBUG(GuruLog::str_id() + "Error en Socket:" + GuruStringAlgo::itos(info()->socket().handle()) + " - " + e.what());

				break;
			}

//			break;

			if (!persistente) break;

			if (response.atributo("Connection").compare("close") == 0) break;

			total_requests++;
		}while (8);

		for (GuruCacheLRU<std::string, IGuruhttpSitio*>::iterator it=cachesitios.begin();it!=cachesitios.end();++it) {
			delete it->second.first;
		}
	};
};

#endif
