#ifndef __GURUHTTPCLIENTE_HPP__
#define __GURUHTTPCLIENTE_HPP__

#include <curl/curl.h>

#define HTTPCLIENTEGURUBUFFER
// #define HTTPCLIENTESTREAM
// #define HTTPCLIENTEVECTOR

#include <GuruString_algo.hpp>

#include <string>
#include <exception>
#include <vector>

#if defined HTTPCLIENTEGURUBUFFER
#include <sstream>
#include <GuruBuffer.hpp>
#endif

/*
//--------------------------------------------------------------------
POST:
Ejemplo de uso:
GuruHttpCliente::response res2;
GuruHttpCliente::opciones_post opt2;

opt2.proxy_server("http://frossi:Amapola1@10.1.1.86:3128");
opt2.atributo("Content-Type: application/json");
opt2.data("{\"username\":\"Fabrcio\",\"clave\":\"Nada\"}");
res2 = GuruHttpCliente::post(string("http://www.planetaguru.com.ar/catalogadorws/"), opt2);

cout << "Status Code:" << res2.status_code() << endl;
cout << res2.body() << endl;

//--------------------------------------------------------------------
Formulario POST:
Ejemplo de uso:
GuruHttpCliente::response res;
GuruHttpCliente::opciones_post opt;

GuruHttpCliente::opciones_post::tag_input a;

// para un texto en un campo
a.campo = "Nombre_Apellido";
a.data_str = "De las Mercedez San Martin.";

opt.agregar_form(a);
a.clear();

// para un archivo en disco
a.campo = "Imagen";
a.data_file = "c:\\Cosas\\05.10.jpg";
opt.agregar_form(a);
a.clear();

// para enviar un buffer
GuruBufferChar buffer;

GuruBufferAlgo::archivo_buffer("c:\\Cosas\\475_1000.jpg", buffer);

a.campo = "Imagen4";
a.data_buffer = buffer.buf();
a.data_buffer_length = buffer.cantidad_data();
a.data_buffer_nombre = "475-1000.jpg";
opt.agregar_form(a);

res = GuruHttpCliente::post("http://localhost/catalogadorws/subir", opt);

buffer.liberar();

//--------------------------------------------------------------------
GET:
Ejemplo de uso:
GuruHttpCliente::response res2;
GuruHttpCliente::opciones_get opt2;

opt2.proxy_server("http://frossi:Amapola1@10.1.1.86:3128");
res2 = GuruHttpCliente::get(string("http://www.planetaguru.com.ar/catalogadorws/?g=gng"), opt2);
cout << "Status Code:" << res2.status_code() << endl;
cout << res2.body() << endl;
*/

namespace GuruHttpCliente {

	const char default_user_agent[] = "GuruHttpClient/0.1.3";

	class GuruHttpException : public std::exception {
	public:
		GuruHttpException(const string& msg, int codigo=0) :exception(), _msg(msg), _codigo(codigo) {};
		GuruHttpException(int codigo, const string& msg="") :exception(), _msg(msg), _codigo(codigo) {};
		virtual ~GuruHttpException() throw() {};

		const int codigo() const throw() { return _codigo; }
		const char* what() const throw() { return _msg.c_str(); }

	private:
		std::string	_msg;
		int			_codigo;
	};

	class response {
	protected:
		friend class proxy_response;

		std::vector<std::string> _header;
	#if defined HTTPCLIENTEGURUBUFFER
		GuruBufferChar* _body;
	#elif defined HTTPCLIENTESTREAM
		stringstream*	_body;
	#elif defined HTTPCLIENTEVECTOR
		std::vector<char>* _body;
	#endif
		bool	_copia;
		size_t	_tamanio;
		double	_tiempo;
		long	_response_code;
	public:
		response() :_body(NULL), _copia(false), _tamanio(0), _tiempo(0), _response_code(0) {};
		response(const response& copia) {
			this->_body = copia._body;
			this->_copia = true;
			this->_tamanio = copia._tamanio;
			this->_tiempo = copia._tiempo;
			this->_response_code = copia._response_code;

			(const_cast<response&>(copia))._copia = false;
		};
		virtual ~response() {
			if (_copia) clear();
		};

		void crear() {
//			cout << "Creando body..." << endl;
		#if defined HTTPCLIENTEGURUBUFFER
			_body = new GuruBufferChar();
		#elif defined HTTPCLIENTESTREAM
			_body = new stringstream();
		#elif defined HTTPCLIENTEVECTOR
			_body = new std::vector<char>();
		#endif
		}

	#if defined HTTPCLIENTEGURUBUFFER
		const char* body() { if (!_body) crear(); return _body->const_buf(); };
		const char* begin() { if (!_body) crear(); return _body->begin_data(); };
		const char* end() { if (!_body) crear(); return _body->end_data(); };
	#elif defined HTTPCLIENTESTREAM
		const stringstream& body() { if (!_body) crear(); return *_body; };
	#elif defined HTTPCLIENTEVECTOR
		const char* body() { if (!_body) crear(); return &(*_body)[0]; };
		const char* begin() { if (!_body) crear(); return _body->begin(); };
		const char* end() { if (!_body) crear(); return _body->end(); };
	#endif

	#if defined HTTPCLIENTEGURUBUFFER
		GuruBufferChar* body_type() { if (!_body) crear(); return _body; };
	#elif defined HTTPCLIENTESTREAM
		stringstream* body_type() { if (!_body) crear(); return _body; };
	#elif defined HTTPCLIENTEVECTOR
		std::vector<char>* body_type() { if (!_body) crear(); return _body; };
	#endif

		const std::vector<std::string>& header() { return _header; }

		size_t	size() { return _tamanio; }
		double	elapsed() { return _tiempo; }
		long	status_code() { return _response_code; }
		void	clear() { if (_body) { /* cout << "Borrando body..." << endl;  */ delete _body; _body = NULL; } }

		void	copia(bool c) { _copia = c; }
		bool 	copia() { return _copia; }

		response& operator = (const response& res2) {
			if (this != &res2) {
				this->_body = res2._body;
				this->_header = res2._header;
				this->_tamanio = res2._tamanio;
				this->_response_code = res2._response_code;
				this->_copia = true;

				(const_cast<response&>(res2))._copia = false;
			}

			return *this;
		}
	};

	/*
	static GuruHttpCliente::response& move(GuruHttpCliente::response& copia) {
		copia.copia(false);

		return copia;
	}
*/
	class opciones {
		std::string _proxy_server; // 192.168.1.2 || 192.168.1.2:8123 || frossi:soda07--@10.1.1.86:3128 || http://frossi:soda07--@10.1.1.86:3128
		std::string _proxy_usuario;
		std::string _proxy_clave;
		std::string _agent;
		bool		_header;
		long		_timeout;
		vector<std::string> _atributos;

	public:
		opciones():_header(false),_timeout(0) {};
		virtual ~opciones() {};

		inline void proxy_server(const std::string& param) { _proxy_server = param; }
		inline std::string& proxy_server() { return _proxy_server; }

		inline void proxy_usuario(const std::string& param) { _proxy_usuario = param; }
		inline std::string& proxy_usuario() { return _proxy_usuario; }

		inline void proxy_clave(const std::string& param) { _proxy_clave = param; }
		inline std::string& proxy_clave() { return _proxy_clave; }

		inline void agent(const std::string& param) { _agent = param; }
		inline std::string agent() { return _agent; }

		inline void header(bool con) { _header = con; }
		inline bool header() { return _header; }

		inline void timeout(long sec) { _timeout = sec; }
		inline long timeout() { return _timeout; }

		inline void atributo(const std::string& data_header) { _atributos.push_back(data_header); }
		inline vector<std::string>& atributo() { return _atributos; }
	};

	class opciones_get : public opciones {
		std::string _archivo;
	public:
		opciones_get() {}
		virtual ~opciones_get() {}

		void archivo(const std::string& param) { _archivo = param; }
		std::string archivo() { return _archivo; }
	};

	class opciones_post : public opciones {
	public:
		enum input_type { none = 0, text, file, buffer };
		typedef struct a {
			input_type		tipo;
			std::string		nombre;				// nombre del campo en el formulario, name=nombre_campo
			std::string		data_str;			// contenido del campo cuando type=text
			std::string		data_file;			// archivo en disco cuando type=file.

			char*			data_buffer;		// cuando en vez de un archivo en disco es un buffer en memoria.
			std::string		data_buffer_nombre;	// nombre que va a tener el archivo en destino.
			uint64_t		data_buffer_length; // largo del buffer en memoria.

			std::string		contenttype;

			a():tipo(none), data_buffer(NULL), data_buffer_length(0) {}
			virtual ~a() {}

			void clear() {
				nombre.clear(); data_str.clear(); data_buffer = NULL; data_buffer_length = 0; data_file.clear(); tipo = none; contenttype.clear();
			}
		}tag_input;

	private:
		vector<tag_input> formulario;
		const char* _data;				// ejem: para Accept:application/json
		uint32_t	_largo_data;
	public:
		opciones_post():_data(NULL),_largo_data(0) {}
		virtual ~opciones_post() {}

		void data(const char* param) { _data = param; }
		const char* data() { return _data; }

		void largo_data(uint32_t param) { _largo_data = param; }
		const uint32_t largo_data() { return _largo_data; }

		void agregar_form(tag_input& nuevo) { 
			if (nuevo.tipo == none) { // si no esta cargado trata de deducirlo
				if (!nuevo.data_str.empty()) nuevo.tipo = text;
				else if (!nuevo.data_file.empty()) nuevo.tipo = file;
				else if (nuevo.data_buffer_length) nuevo.tipo = buffer;				
				else return;
			}
			formulario.push_back(nuevo); 
		}

		vector<tag_input>& form() { return formulario; }
	};

	class proxy_response {
	public:
		proxy_response(response& resp):_response(resp) {};
		virtual ~proxy_response() {};

	#if defined HTTPCLIENTEGURUBUFFER
		GuruBufferChar* body() { return _response.body_type(); }
	#elif defined HTTPCLIENTESTREAM
		stringstream* body() { return _response.body_type(); };
	#elif defined HTTPCLIENTEVECTOR
		std::vector<char>* body() { return _response.body_type(); };
	#endif

		std::vector<std::string>& header() { return _response._header; };

		inline size_t size() { return _response._tamanio; }
		inline void size(size_t tam) { _response._tamanio = tam; }

		inline double elapsed() { return _response._tiempo; }
		inline void elapsed(double tam) { _response._tiempo = tam; }

		inline long response_code() { return _response._response_code; }
		inline void response_code(long code) { _response._response_code = code; }

	private:
		GuruHttpCliente::response& _response;
	};

	typedef struct {
		opciones& opt;
		proxy_response& proxy;
	}data_http;

	static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
		opciones* buffer = static_cast<opciones*>(userdata);

		return size * nmemb;
	}

	static size_t write_memory_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
		data_http* data = static_cast<data_http*>(userdata);

	#if defined HTTPCLIENTEGURUBUFFER
		if (data->proxy.body() != NULL) data->proxy.body()->agregar(ptr, size * nmemb);
	#elif defined HTTPCLIENTESTREAM
		if (data->proxy.body() != NULL) data->proxy.body()->write(ptr, size * nmemb);
	#elif defined HTTPCLIENTEVECTOR

		if (data->proxy.body() != NULL) {

//			if (data->proxy.body()->size()) data->proxy.body()->pop_back();

			for (size_t i = 0; i < (size * nmemb); i++) {
				data->proxy.body()->push_back(*ptr);
				ptr++;
			}

//			data->proxy.body()->push_back(0);
		}

	#endif

		data->proxy.size(data->proxy.size() + (size * nmemb));

		return size * nmemb;
	}

	static size_t header_callback(char *ptr, size_t size, size_t nitems, void *userdata) {
		data_http* data = static_cast<data_http*>(userdata);

		std::string tmp;

		if (data->opt.header()) {
			tmp.append(ptr, (size * nitems) - 2);
			if (!tmp.empty()) data->proxy.header().push_back(tmp);
		}

		if ((size * nitems) > 14) {
			if (strncmp(ptr, "Content-Length:", 14) == 0) {
				tmp.clear();
				tmp.append(ptr, (size * nitems) - 2);

				std::string::size_type lugar = tmp.find(':');

				if (lugar != std::string::npos) {
					std::string valor;
					std::string tmp2(tmp.substr(lugar + 1, tmp.length() - lugar));
					valor = GuruStringAlgo::trim(tmp2);
				#if defined HTTPCLIENTEGURUBUFFER
					#if __cplusplus > 199711L // codigo solo para C++11
						data->proxy.body()->reservar(atoll(valor.c_str()) + 2);
					#else
						data->proxy.body()->reservar(atol(valor.c_str()) + 2);
					#endif
				#elif defined HTTPCLIENTESTREAM
				#elif defined HTTPCLIENTEVECTOR
					data->proxy.body()->resize(atoll(valor.c_str()) + 2);
				#endif
				}
			}
		}

/*
	// opcion 1
		std::string tmp(ptr),llave,valor;

		std::string::size_type lugar = tmp.find(':');

		if (lugar != std::string::npos) {
			llave = GuruStringAlgo::trim(tmp.substr(0, lugar));
			valor = GuruStringAlgo::trim(tmp.substr(lugar + 1, tmp.length() - lugar - 3));

			std::cout << llave << ":" << valor << std::endl;
		} else
			std::cout << tmp << std::endl;
*/
/*
	// opcion 2
		std::string tmp;
		tmp.append(ptr, (size * nitems)-2);
		std::cout << tmp << std::endl;
*/
		return size * nitems;
	}

	void parametros_genericos(CURL* curl, opciones& opt) {
		if (!opt.proxy_server().empty()) curl_easy_setopt(curl, CURLOPT_PROXY, opt.proxy_server().c_str());
		if (!opt.proxy_usuario().empty()) curl_easy_setopt(curl, CURLOPT_PROXYUSERNAME, opt.proxy_usuario().c_str());
		if (!opt.proxy_clave().empty()) curl_easy_setopt(curl, CURLOPT_PROXYPASSWORD, opt.proxy_clave().c_str());

		if (!opt.agent().empty()) curl_easy_setopt(curl, CURLOPT_USERAGENT, opt.agent().c_str());
		else curl_easy_setopt(curl, CURLOPT_USERAGENT, default_user_agent);

		if (opt.timeout() != 0) curl_easy_setopt(curl, CURLOPT_TIMEOUT, opt.timeout());
	}

	response get(const std::string& url, opciones_get& opt) {
		CURL* curl = curl_easy_init();
		if (curl) {
			response respuesta;
			proxy_response proxy_respuesta(respuesta);
			data_http data = { opt, proxy_respuesta };
			char errbuf[CURL_ERROR_SIZE] = {0};
			struct curl_slist *slist = NULL;

			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

			parametros_genericos(curl, opt);

			if (opt.atributo().size() != 0) {
				for (vector<std::string>::iterator it = opt.atributo().begin(), end = opt.atributo().end(); it != end; ++it)
					slist = curl_slist_append(slist, it->c_str());

				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
			}

			curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

			curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, &data);

			if (!opt.archivo().empty()) curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
			else curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

			// curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
			// curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

			CURLcode res;
			res = curl_easy_perform(curl);

			if (slist) curl_slist_free_all(slist);

			if (res == CURLE_OK) {
				long response_code;
				double elapsed;

				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
				curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);

				data.proxy.response_code(response_code);
				data.proxy.elapsed(elapsed);

				curl_easy_cleanup(curl);
				curl = NULL;
			} else {
				size_t len = strlen(errbuf);

				std::string msg_error;
				if (len) msg_error = errbuf;
				else msg_error = curl_easy_strerror(res);

				curl_easy_cleanup(curl);
				curl = NULL;

				throw GuruHttpException(msg_error);
			}

			return respuesta;
		}

		return response();
	}

	response get(const std::string& url) {
		opciones_get opt;

		return get(url, opt);
	}

	response post(const std::string& url, opciones_post& opt) {
		CURL *curl = curl_easy_init();
		if (curl) {
			response respuesta;
			proxy_response proxy_respuesta(respuesta);
			data_http data = { opt, proxy_respuesta };
			char errbuf[CURL_ERROR_SIZE] = { 0 };
			struct curl_slist *slist = NULL;
			struct curl_httppost *formpost = NULL;
			struct curl_httppost *lastptr = NULL;

			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

			curl_easy_setopt(curl, CURLOPT_POST, 1L);

			parametros_genericos(curl, opt);

			curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

			curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, &data);

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

			slist = curl_slist_append(slist, "Expect:");

			if (opt.atributo().size() != 0) {
				for (vector<std::string>::iterator it = opt.atributo().begin(), end = opt.atributo().end(); it != end; ++it)
					slist = curl_slist_append(slist, it->c_str());

				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
			}

			if (opt.largo_data()) {// para enviar data en el post.
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, opt.data());
				curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, opt.largo_data());
			} else if (!opt.form().empty()) {// pregunto si tengo que enviar un formulario.
				for (vector<opciones_post::tag_input>::iterator it = opt.form().begin(), end = opt.form().end(); it != end; ++it) {
					switch (it->tipo) {
					case opciones_post::text:
						/* pongo el nombre del campo */
						curl_formadd(&formpost,
							&lastptr,
							CURLFORM_COPYNAME, it->nombre.c_str(),
							CURLFORM_COPYCONTENTS, it->data_str.c_str(),
							CURLFORM_END);
						break;
					case opciones_post::file:
						curl_formadd(&formpost,
							&lastptr,
							CURLFORM_COPYNAME, it->nombre.c_str(),
							CURLFORM_FILE, it->data_file.c_str(),
							CURLFORM_END);
						break;
					case opciones_post::buffer:
						curl_formadd(&formpost,
							&lastptr,
							CURLFORM_COPYNAME, it->nombre.c_str(), // nombre del campo
							CURLFORM_BUFFER, it->data_buffer_nombre.c_str(), // nombre del archivo que sube
							CURLFORM_BUFFERPTR, it->data_buffer,  // buffer con la data
							CURLFORM_BUFFERLENGTH, it->data_buffer_length, // cantidad de data en el buffer
							CURLFORM_END);
						// CURLFORM_FILENAME
						break;
					case opciones_post::none:
						continue;
						break;
					}
				}

				curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
			}

			// curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
			// curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

			CURLcode res;
			res = curl_easy_perform(curl);

			if (slist) curl_slist_free_all(slist);

			if (formpost) curl_formfree(formpost);

			if (res == CURLE_OK) {
				long response_code;
				double elapsed;

				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
				curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);

				data.proxy.response_code(response_code);
				data.proxy.elapsed(elapsed);

				curl_easy_cleanup(curl);
				curl = NULL;
			} else {
				size_t len = strlen(errbuf);

				std::string msg_error;
				if (len) msg_error = errbuf;
				else msg_error = curl_easy_strerror(res);

				curl_easy_cleanup(curl);
				curl = NULL;

				throw GuruHttpException(msg_error);
			}

			return respuesta;
		}

		return response();
	}
		
	response put(const std::string& url) {
		return response();
	}

	response del(const std::string& url) {
		return response();
	}

	bool conSSL() {
		curl_version_info_data * vinfo = curl_version_info(CURLVERSION_NOW);
		if (vinfo->features & CURL_VERSION_SSL)
			return true;
		else
			return false;
	}
}

#endif
