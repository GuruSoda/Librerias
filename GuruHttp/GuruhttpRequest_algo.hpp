#ifndef __GURUHTTPREQUEST_ALGO_HPP__
#define __GURUHTTPREQUEST_ALGO_HPP__

#include <GuruDirectorio.hpp>
#include <GuruSingleton.hpp>
#include <GuruBuffer.hpp>

#include "GuruhttpRequest.hpp"
#include "GuruhttpConfig.hpp"
#include "Guruhttp_algo.hpp"

namespace ProcesarRequest {

	class content {
	public:
		content(const string& disposition) {
			GuruTokenEx token(";");

			for (size_t total=token.extraer(disposition);total>0;total--) {
				GuruTokenEx token2("=");

				if (token2.extraer(token[total-1]) == 2)
					content_disposition.insert(make_pair(token2[0], token2[1]));
			}
		};

		virtual ~content() {};

		string atributo(const string& atri) {
			map<string, string>::iterator lugar;
			string temp_atributo;

			if ((lugar = content_disposition.find(atri)) != content_disposition.end()) temp_atributo = lugar->second;

			return temp_atributo;
		}
	private:
		map<string, string> content_disposition;
	};

	template <typename T> // T=GuruhttpRequest
	void extraer_metodo(T& requerimiento) {

		if (requerimiento.requerimiento().requerimiento().empty()) throw GuruhttpExcepcion(400);

		string::const_iterator caracter(requerimiento.requerimiento().requerimiento().begin());

		string metodo;

		// voy hasta el final del requerimiento
		while (caracter != requerimiento.requerimiento().requerimiento().end() && *caracter != ' ')
			metodo += *caracter++;

		// segun RFC2616 - 5.1.1 Method: The method is case-sensitive.
		if (metodo.compare("GET")==0)
			requerimiento.requerimiento().tipo_metodo(RequerimientoHttp::get);
		else if (metodo.compare("POST")==0)
			requerimiento.requerimiento().tipo_metodo(RequerimientoHttp::post);
		else if (metodo.compare("HEAD")==0)
			requerimiento.requerimiento().tipo_metodo(RequerimientoHttp::head);
		else if (metodo.compare("PUT")==0)
			requerimiento.requerimiento().tipo_metodo(RequerimientoHttp::put);
		else if (metodo.compare("DELETE")==0)
			requerimiento.requerimiento().tipo_metodo(RequerimientoHttp::del);
		else if (metodo.compare("TRACE")==0)
			requerimiento.requerimiento().tipo_metodo(RequerimientoHttp::trace);
		else if (metodo.compare("OPTIONS")==0)
			requerimiento.requerimiento().tipo_metodo(RequerimientoHttp::options);
		else {
			requerimiento.requerimiento().tipo_metodo(RequerimientoHttp::no_soportado);
			throw GuruhttpExcepcion(400);
		}

		requerimiento.requerimiento().str_metodo(metodo);
	}

	template <typename T> // T=GuruhttpRequest
	void extraer_recurso(T& requerimiento) {
		if (requerimiento.requerimiento().requerimiento().empty()) throw GuruhttpExcepcion(400);

		string::const_iterator caracter=requerimiento.requerimiento().requerimiento().begin();
		string::const_iterator fin=requerimiento.requerimiento().requerimiento().end();

		// voy hasta el final del metodo
		while (caracter != fin && *caracter != ' ')
			caracter++;

		if (caracter == fin) throw GuruhttpExcepcion(400);

		if (*caracter == ' ') caracter++;

		string tmp;
		// saco el recurso
		while (caracter != fin && *caracter != ' ' && *caracter != '?')
			tmp += *caracter++;

		if (caracter == fin) throw GuruhttpExcepcion(400);

		if (tmp.find('%') != std::string::npos)
			requerimiento.requerimiento().recurso(GuruhttpAlgo::UrlDecode(tmp));
		else
			requerimiento.requerimiento().recurso(tmp);

//		requerimiento.atributo("uri", requerimiento.requerimiento().recurso());
	}


	template <typename T> // T=GuruhttpRequest
	void extraer_version(T& requerimiento) {
		string::const_iterator caracter = requerimiento.requerimiento().requerimiento().begin();
		string::const_iterator fin = requerimiento.requerimiento().requerimiento().end();

		// voy hasta el final del metodo
		while (caracter != fin && *caracter != ' ')
			caracter++;

		if (caracter == fin) throw GuruhttpExcepcion(400);

		if (*caracter == ' ') caracter++;

		// voy hasta el final del recurso
		while (caracter != fin && *caracter != ' ')
			caracter++;

		if (*caracter == ' ') caracter++;

		if (caracter == fin) throw GuruhttpExcepcion(400);

		// saco la version HTTP
		string version;
		while (caracter != fin && *caracter != ' ')
			version += *caracter++;

		if (version.compare("HTTP/1.1") == 0) {
			requerimiento.requerimiento().http_version(RequerimientoHttp::http11);
		} else if (version.compare("HTTP/1.0") == 0) {
			requerimiento.requerimiento().http_version(RequerimientoHttp::http10);
		} else if (strncmp(version.c_str(), "HTTP/0", 6) == 0) {
			requerimiento.requerimiento().http_version(RequerimientoHttp::http0);
		} else if (version.compare("HTTP/2") == 0) {
			requerimiento.requerimiento().http_version(RequerimientoHttp::http2);
		} else throw GuruhttpExcepcion(400);
	}

	//////////////////////////////////////////////////////////////////////
	// Tarea: Cargar los campos del requerimiento
	//////////////////////////////////////////////////////////////////////
	template <typename T> // T=GuruhttpRequest
	void cargar_campos_multipart(T& requerimiento, map<string, string>& campos) {
		string linea;
		string llave;
		string valor;

		campos.clear();

		do{
			requerimiento.recibir() >> linea;

			if (linea.empty()) break;

			llave.clear();valor.clear();

			string::const_iterator caracter(linea.begin());

			while(caracter != linea.end() && *caracter != ':')
				llave += *caracter++;

			while (*caracter == ':' || *caracter == ' ')
				caracter++;

			while (caracter != linea.end())
				valor += *caracter++;

			campos.insert(make_pair(llave, valor));
		}while(true);
	}

	template <typename T> // T=GuruhttpRequest
	string extraer_boundary(T& requerimiento) {
		string tmp(requerimiento.atributo("Content-Type"));
		string::size_type lugar;
		string limite;

		if ((lugar=tmp.find("boundary=")) != string::npos)
			limite = tmp.substr(lugar + strlen("boundary="));

		return limite;
	}

	template <typename T> // T=GuruhttpRequest
	void recibir_archivo(T& requerimiento, const string& directorio, const string& limite) {

		ofstream archivo_viniendo(string(requerimiento.config()->sacarDirApps() + directorio).c_str(), ios::trunc | ios::binary);

		if (!archivo_viniendo) {
			cout << GuruError().descripcion() << endl;
			throw GuruhttpExcepcion(400);
		}

		char	caracter = 0;
		string	tmp;
		::uint32_t leidos=0;

		// Archivo:winamp5621_lite_en-us.exe - Tiempo: 25312
		// Archivo:winamp5621_lite_en-us.exe - Tiempo: 8437
	//	uint64_t inicio_envio = GetTickCount();

		char inicio_limite=*limite.begin();

		try{
			do {
				do {
					if (recv (requerimiento.recibir().interfaz()->sacar_socket().handle(), &caracter, 1, 0) < 1) throw GuruSocketException(WSAGetLastError());
					leidos++;

					if (inicio_limite == caracter) break;
					else archivo_viniendo << caracter;
				} while (8);

				if (leidos > 580) 
					int a=1;

				tmp=caracter;
				string::iterator c=tmp.begin();

				if (*c == *limite.begin()) {
					string::const_iterator it(limite.begin());
					string::iterator it_actual=c;
					string::size_type tamanio(limite.size());
					string::size_type indice=1;

					while (indice<tamanio) {
						if (*c != *it) break;

						if (leidos > 610) 
							int b=1;

						if (recv (requerimiento.recibir().interfaz()->sacar_socket().handle(), &caracter, 1, 0) < 1) throw GuruSocketException(WSAGetLastError());
						leidos++;

						tmp += caracter;
						c=tmp.end();
						c--;
						indice++;
						it++;
					}

					if (indice == tamanio) // encontre la cadena
						throw 1;
					else {
						c=it_actual;
						archivo_viniendo << tmp;
						continue;
					}
				}
			} while (8);
		} catch (int) {
		}

		archivo_viniendo.close();

	//	uint64_t fin_envio = GetTickCount();
	// Solo para verificar el tiempo que llevo hacer el upload
	//	cout << "Archivo:" << nombre << " - Tiempo: " << (fin_envio-inicio_envio) << endl;
	}

	template <typename T> // T=GuruhttpRequest
	void recibir_archivoV2(T& requerimiento, const string& archivo, const string& limite) {
		// creo el archivo que esta viniendo en el formulario
		ofstream archivo_viniendo(archivo.c_str(), ios::trunc | ios::binary);

		if (!archivo_viniendo) {
			cout << GuruError().descripcion() << endl;
			throw GuruhttpExcepcion(400);
		}

		char		caracter = 0;
		string		tmp;
		::uint32_t	leidos=0;
		basic_GuruBuffer<char>	buffer(BufferEntrada(requerimiento.recibir().interfaz()->sacar_socket().handle())*1000);

		// Archivo:winamp5621_lite_en-us.exe - Tiempo: 25312
		// Archivo:winamp5621_lite_en-us.exe - Tiempo: 8437
	//	uint64_t inicio_envio = GetTickCount();

		char inicio_limite=*limite.begin();

		try {
			do {
				do {
					if (recv (requerimiento.recibir().interfaz()->sacar_socket().handle(), &caracter, 1, 0) < 1) throw GuruSocketException(GuruError().descripcion(WSAGetLastError()));
					leidos++;

					if (inicio_limite == caracter) break;
					else {
						if (buffer.cantidad_data() >= buffer.cantidad_buf()) {
							::int64_t total=0;
							if (*buffer.end() == '\r') total = 1;
							else if (*buffer.end() == '\n') {
								basic_GuruBuffer<char>::iterator it=buffer.end();it--;
								if (*it == '\n') total = 2;
							}

							archivo_viniendo.write(buffer.buf(), static_cast<std::streamsize>(buffer.cantidad_data() - total));

							buffer.clear_data();
							if (total == 1) buffer << '\r'; 
							else if (total == 2){
								buffer << '\r';
								buffer << '\n';
							}

						}

						buffer << caracter;
					}
				} while (8);

				// puede ser que se haya terminado el archivo.
				if (inicio_limite == caracter) {
					if (buffer.cantidad_data() >= 2) {
						basic_GuruBuffer<char>::iterator it=buffer.end();
						::uint64_t total=0;
						if (*it == '\n') {
							it--;total++;
							if (*it == '\r') {
								total++;
							}
						}
						
						archivo_viniendo.write(buffer.buf(), static_cast<std::streamsize>(buffer.cantidad_data() - total));

						buffer.clear_data();
						if (total == 1)
							buffer << '\n';
						else if (total == 2) {
							buffer << '\r';
							buffer << '\n';
						}
					}
				}

				// hasta aqui, los datos recibidos estan en disco.

				tmp=caracter;
				string::iterator c=tmp.begin();

				if (*c == *limite.begin()) {
					string::const_iterator it(limite.begin());
					string::iterator it_actual=c;
					string::size_type tamanio(limite.size());
					string::size_type indice=1;

					while (indice<tamanio) {
						if (*c != *it) break;

						if (recv (requerimiento.recibir().interfaz()->sacar_socket().handle(), &caracter, 1, 0) < 1) throw GuruSocketException(GuruError().descripcion(WSAGetLastError()));
						leidos++;

						tmp += caracter;
						c=tmp.end();
						c--;
						indice++;
						it++;
					}

					if (indice == tamanio) // encontre la cadena
						throw 1;
					else {
						c=it_actual;
						for (string::iterator it=tmp.begin(),end=tmp.end();it!=end;++it)
							buffer << *it;
					}
				}
			} while (8);
		} catch (int) {
		}

		archivo_viniendo.close();

	//	uint64_t fin_envio = GetTickCount();
	// Solo para verificar el tiempo que llevo hacer el upload
	//	cout << "Archivo:" << nombre << " - Tiempo: " << (fin_envio-inicio_envio) << endl;
	}


	//////////////////////////////////////////////////////////////////////
	// RFC 1867
	// Aqui se procesa: "Content-type: multipart/form-data"
	//////////////////////////////////////////////////////////////////////
	template <typename T> // T=GuruhttpRequest
	void cargar_parametros_multipart_form_data(T& requerimiento) {
		string limite_campo(ProcesarRequest::extraer_boundary(requerimiento));
		string limite_encontrado;
		string content_disposition;
		map<string, string> campos;

		limite_campo.insert(0, "--");

		requerimiento.recibir() >> limite_encontrado;

		// verifico si es el inicio...
		if (!GuruStringAlgo::stringequals(limite_campo, limite_encontrado)) return ;

		do {
			ProcesarRequest::cargar_campos_multipart(requerimiento, campos);
		
			map<string, string>::iterator it(campos.find("Content-Disposition"));
			if (it != campos.end()) 
				content_disposition=it->second;
			else 
				continue;

			if (content_disposition.find("form-data;") == 0) {

				content cont(content_disposition);

				string archivo;
				if (!(archivo=cont.atributo("filename")).empty()) {

					string nombre;
					if (!(nombre=cont.atributo("name")).empty()) {
						
						// creo el directorio por si no existe
						GuruDirectorio::crearDirectorioRecursivo(GuruDirectorio::comoDirectorio(requerimiento.config()->env("temp")));
						// creo el nombre con la ruta
						archivo = GuruDirectorio::comoDirectorio(requerimiento.config()->env("temp")) + archivo;

						//ProcesarRequest::recibir_archivo(requerimiento, GuruDirectorio::GuruSplitPath(archivo).nombre_ext(), "\r\n" + limite_encontrado);
						ProcesarRequest::recibir_archivoV2(requerimiento, archivo, limite_encontrado);

						requerimiento.post().campo(nombre, archivo);
						nombre=requerimiento.recibir().recibir(2); // -- o \r\n?
						
						// fin de los parametros post????
						if (nombre.compare("--") == 0) 
							break;
					}
				} else {
					string nombre;
					if (!(nombre=cont.atributo("name")).empty()) {
					
						string valor, tmp;
		//				do {
							requerimiento.recibir() >> tmp;
							valor += tmp;
		//				} while (!tmp.empty());

						requerimiento.post().campo(nombre, valor);

						// saco el boudary
						requerimiento.recibir() >> limite_encontrado;
						// fin de los parametros post????
						if (GuruStringAlgo::stringequals(limite_campo + "--", limite_encontrado))
							break ;
					}
				}
			}

		} while (8);
	}

	template <typename T> // T=GuruhttpRequest
	void cargar_campos_parametro(T& requerimiento, const string& parametros) {
		string::const_iterator caracter(parametros.begin());

		do {
			string var;
			string valor;

			// saco el nombre de la variable
			while (caracter != parametros.end() && *caracter != '=')
				var += *caracter++;

			if (caracter == parametros.end()) throw GuruhttpExcepcion(400);
			else if (*caracter == '=') caracter++;

			// saco el valor de la variable
			while (caracter != parametros.end() && *caracter != '&' && *caracter != ' ')
				valor += *caracter++;

			requerimiento.parametro(var, GuruhttpAlgo::UrlDecode(valor));

			if (caracter == parametros.end() || *caracter == ' ') break;

			if (*caracter == '&') caracter++;
			else throw GuruhttpExcepcion(400);
		} while (caracter != parametros.end());
	}

	//////////////////////////////////////////////////////////////////////
	// Ejemplo:
	//		show.php?loc=58&f=aria.jpg
	// Carga los parametros provenientes de un get o un post en sus
	// diferentes variantes.
	//////////////////////////////////////////////////////////////////////
	template <typename T> // T=GuruhttpRequest
	void cargar_parametros(T& requerimiento){
		string parametros;

		switch (requerimiento.requerimiento().tipo_metodo()) {
		case RequerimientoHttp::get:
			break;
		case RequerimientoHttp::post: 
			break;

		default:
			break;
		}

		if (requerimiento.requerimiento().tipo_metodo() == RequerimientoHttp::get) {
			// busco si el uri tiene parametros
			string::size_type lugar=requerimiento.requerimiento().requerimiento().find('?');
			if (lugar != string::npos) {
				parametros = requerimiento.requerimiento().requerimiento().substr(lugar + 1);
				cargar_campos_parametro(requerimiento, parametros);
			}
		} else if (requerimiento.requerimiento().tipo_metodo() == RequerimientoHttp::post) {
				string::size_type lugar = requerimiento.requerimiento().requerimiento().find('?');
				if (lugar != string::npos) {
					parametros = requerimiento.requerimiento().requerimiento().substr(lugar + 1);
					cargar_campos_parametro(requerimiento, parametros);
				}
				// Binario: Content-Type: multipart/form (Archivo)
				// Texto:   Content-Type: application/x-www-form-urlencoded (loc=58&f=aria.jpg&soda=1)
				// Json:	Content-Type: application/json ({"username":"xyz","password":"xyz"})
				// El parametro Content-Type son valores separados por ;, por eso uso un find, vienen varios valores
				if (requerimiento.atributo("Content-Type").find("application/x-www-form-urlencoded") != string::npos) {
					::uint32_t longitud=atoi(requerimiento.atributo("Content-Length").c_str());
					if (longitud > 0) { // 0 es un formulario que no tiene campos, raro pero ya me paso.
						requerimiento.recibir().recibir(parametros, longitud);
						cargar_campos_parametro(requerimiento, parametros);
					}
				} else if (requerimiento.atributo("Content-Type").find("multipart/form-data") != string::npos) // formulario enviado en partes separador por un "Boundary"
							cargar_parametros_multipart_form_data(requerimiento);
				else if (!requerimiento.atributo("Content-Type").empty()) {
					::uint32_t longitud = 0;
					if (!requerimiento.atributo("Content-Length").empty()) longitud = atoi(requerimiento.atributo("Content-Length").c_str());

					// un POST puede no tener entity body:
					// http://lists.w3.org/Archives/Public/ietf-http-wg/2010JulSep/0272.html
					// http://lists.w3.org/Archives/Public/ietf-http-wg/2010JulSep/0276.html
					// http://stackoverflow.com/questions/4191593/is-it-considered-bad-practice-to-perform-http-post-without-entity-body
					// http://stackoverflow.com/questions/7323958/are-put-and-post-requests-required-expected-to-have-a-request-body

					if (longitud > 0) {
						requerimiento.post().configData().reservar(longitud+2); // 1) reservo el buffer.
						requerimiento.recibir().recibir(requerimiento.post().data(), longitud); // 2) cargo la data.
						requerimiento.post().configData().cantidad_data(longitud); // 3) le configuro cuanta data tiene cargado el buffer.
					}
				} // un POST puede no tener body y tampoco tener Content-Type.
		}
	}

	template <typename T> // T=GuruhttpRequest
	void extraer_cookie(T& requerimiento) {
		string str_cookie;

		if ((str_cookie=requerimiento.atributo("Cookie")).empty()) return ;

		GuruToken token(";"); vector<string> cookies;

		if (token.extraer(str_cookie, cookies).size() == 0) return ;

		for (vector<string>::iterator it=cookies.begin();it!=cookies.end();++it) {
			GuruTokenEx token_cookie("=");

			if (token_cookie.extraer(*it) != 0) {
				requerimiento.cookie(token_cookie[0], token_cookie[1]);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////
	// Tarea: Cargar los campos del requerimiento
	//////////////////////////////////////////////////////////////////////
	template <typename T> // T=GuruhttpRequest
	void cargar_campos_request(T& requerimiento) {
		string linea;
		string llave;
		string valor;

		do{
			requerimiento.recibir() >> linea;

			if (linea.empty()) break;

			llave.clear();valor.clear();

			string::const_iterator caracter(linea.begin());

			while(caracter != linea.end() && *caracter != ':')
				llave += *caracter++;

			while ((*caracter == ':' || *caracter == ' ') && caracter != linea.end())
				caracter++;

			while (caracter != linea.end())
				valor += *caracter++;

			requerimiento.atributo(llave, valor);
		}while(true);
	}

	template <typename T> // T=GuruhttpRequest
	void vincular_recurso (T& request) {
			string cp_recurso(request.requerimiento().recurso());

			if (cp_recurso.empty()) return ;

			string::const_iterator	caracter(cp_recurso.begin());
			GuruhttpDirVirtual*		dirconf=NULL;
			string					directorio;

		// recorro para sacar "Solamente" el nombre del directorio virtual
			do {
				directorio += *caracter++;
			}while (caracter != cp_recurso.end() && *caracter != '/');

			// pregunto si me vino un archivo del raiz
			if (caracter == cp_recurso.end()) { // un recurso que NO termina con '/' y le pertenece al '/'
				// pregunto si en un directorio virtual mal escrito (sin la barra final)
				if (directorio.compare("/") != 0 && (dirconf=GuruSingleton<GuruhttpConfig>::instancia()->configuracion(GuruhttpAlgo::comoDirectorioHTTP(directorio), request.atributo("Host"))) != NULL) {
					request.config(dirconf);
					throw GuruhttpExcepcion(301, "http://" + request.atributo("Host") + directorio + "/");
				} else { // pregunto si esta definido el sitio '/' ya que me vino un recurso del raiz
					if ((dirconf=GuruSingleton<GuruhttpConfig>::instancia()->configuracion("/", request.atributo("Host"))) != NULL) {
						request.recurso().directorio("/");
						request.recurso().archivo(directorio.erase(0,1));
					} else {
						throw GuruhttpExcepcionSitioNoEncontrado(404);
					}
				}
			} else {
				directorio += *caracter++;

				// me vino una '/', asi que es un directorio virtual o un subdirectorio del raiz.
				// pregunto si existe el directorio virtual
				dirconf=GuruSingleton<GuruhttpConfig>::instancia()->configuracion(directorio, request.atributo("Host"));
				if (dirconf != NULL) {
						request.recurso().directorio(directorio);
						request.recurso().archivo(GuruDirectorio::GuruSplitPath(cp_recurso).nombre_ext());
						// tomo la cadena con el recurso y le saco el sitio y el archivo para que quede solo el subdirectorio
						request.recurso().subdirectorio(string(cp_recurso).substr(directorio.length(), cp_recurso.length()-request.recurso().archivo().length()-directorio.length()));
				} else if ((dirconf=GuruSingleton<GuruhttpConfig>::instancia()->configuracion("/", request.atributo("Host"))) != NULL) { // el un directorio que le pertenece al sitio '/'
						request.recurso().directorio("/");
						request.recurso().archivo(GuruDirectorio::GuruSplitPath(cp_recurso).nombre_ext());
						// tomo la cadena con el recurso y le saco el sitio y el archivo para que quede solo el subdirectorio
						request.recurso().subdirectorio(string(cp_recurso).substr(request.recurso().directorio().length(), cp_recurso.length()-request.recurso().archivo().length()-request.recurso().directorio().length()));
				}
			}

			if (dirconf != NULL) {
//				if (GuruhttpConfig::instancia()->sacarConfHttp().rechazada(request.recibir().interfaz()->sacar_socket().interfaz_remota_binaria()))
				request.config(dirconf);
			} else
				throw GuruhttpExcepcionSitioNoEncontrado(404);

			request.recurso().ruta_absoluta(dirconf->dirApps() + request.recurso().subdirectorio() + request.recurso().archivo());

			#ifdef __GURUWIN32__
			if (request.recurso().ruta_absoluta().find('/') != string::npos)
				std::replace_if(request.recurso().ruta_absoluta().begin(), request.recurso().ruta_absoluta().end(),bind2nd(std::equal_to<char>(), '/') , '\\');
			#endif

			GuruDirectorio::GuruArchivo a(GuruDirectorio::propiedades(request.recurso().ruta_absoluta()));
			if (a.Existe()) {
				if (!request.recurso().archivo().empty() && a.EsDirectorio()) {
//					string a = "http://" + request.atributo("Host") + request.recurso().subdirectorio() + request.recurso().archivo() + "/";
					throw GuruhttpExcepcion(301, "http://" + request.atributo("Host") + directorio + request.recurso().subdirectorio() + request.recurso().archivo() + "/");
				}

				request.recurso().info(a);
			}
	};

	template <typename T> // T=GuruhttpRequest
	void cargar_data_sesion(T& requerimiento) {

		if (requerimiento.atributo("Cookie").empty()) return ;

		string str_idatlas(requerimiento.cookie("idatlas"));

		if (!str_idatlas.empty()) {
			requerimiento.sesion().cookie_id(str_idatlas);
			GuruSingletonEx<httpSessiones>::instancia().data_sesion(str_idatlas, requerimiento.sesion().data_sesion());
		}
	}

	template <typename T> // T=GuruhttpRequest
	void procesarRequest(T& requerimiento) {
		string		str_req;

	// la primer linea a recibir el es requerimiento, ejem: GET /nada/index.html HTTP/1.1
		requerimiento.recibir() >> str_req;

		GURULOG_DEBUG(GuruLog::str_id() + " Req:" + str_req);
/*
		if (str_req.compare("Connection: keep-alive") == 0) {
			int a = 0;
	//		while (!str_req.empty()) {
	//			GURULOG_DEBUG(GuruLog::str_id() + " Req:" + str_req);
	//			requerimiento.recibir() >> str_req;
	//		}
		}
*/
// 		RFC 7230: 3.1.1.Request Line
//		request-line   = method SP request-target SP HTTP-version CRLF

		// request-line:
		requerimiento.requerimiento().requerimiento(str_req);

		// method
		ProcesarRequest::extraer_metodo(requerimiento);

		// request-target:
		ProcesarRequest::extraer_recurso(requerimiento);

		// HTTP-version:
		ProcesarRequest::extraer_version(requerimiento);

		ProcesarRequest::cargar_campos_request(requerimiento);

		ProcesarRequest::vincular_recurso(requerimiento);

		ProcesarRequest::cargar_parametros(requerimiento);

		ProcesarRequest::extraer_cookie(requerimiento);

		ProcesarRequest::cargar_data_sesion(requerimiento);

		// segun RFC 2068 seccion 19.7.1.1
		if (!requerimiento.atributo("Keep-Alive").empty() && !requerimiento.atributo("Connection").empty()) {
			unsigned int t=atoi(requerimiento.atributo("Keep-Alive").c_str());
			if (t!=0) requerimiento.recibir().interfaz()->poner_timeout(t);
		}
	}

};

#endif
