#ifndef __GURUHTTPEXCEPCION_H__
#define __GURUHTTPEXCEPCION_H__

#include <GuruCompat.h>

#if defined __GURUWIN32__
// Para compilar Windows.h y Winsock2.h en el mismo fuente
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef _WINSOCKAPI_
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
	#endif

	#include <windows.h>
	#include <winsock2.h>
#elif defined __GURULINUX__
   #include <pthread.h>
   #include <sys/types.h>
   #include <sys/socket.h>
   #include <netinet/in.h>
   #include <netdb.h>
   #include <arpa/inet.h>
   #include <unistd.h>
#endif

#include <exception>

#include <GuruSocket/GuruSocket.hpp>
#include <GuruSocket/GuruSocketBase.hpp>
#include <GuruFechaHora.hpp>

#include "GuruhttpVersion.hpp"

#if !defined GURUHTTP_VERSION
#error Etiqueta GURUHTTP_VERSION Sin Definir.
#endif

const char Error_400[] = "<!DOCTYPE html><html><head><title>400 Requerimiento Erroneo</title></head><body><h1>Requerimiento Erroneo</h1><p>El requerimiento enviado no es valido o esta mal formado.</p></body></html>";
const char Error_403[] = "<!DOCTYPE html><html><head><title>403 Prohibido</title></head><body><h1>Prohibido</h1><p>Lo que Ud. busca ... no se le permite tenerlo.</p></body></html>";
const char Error_404[] = "<!DOCTYPE html><html><head><title>404 No Encontrado</title></head><body><h1>No Encontrado</h1><p>Lo que Ud. busca ... aca no se encuentra.</p></body></html>";
const char Error_405[] = "<!DOCTYPE html><html><head><title>405 Metodo no permitido</title></head><body><h1>Metodo no permitido</h1><p>El metodo no esta permitido o creado en el GuruServidorWeb.</p></body></html>";
const char Error_416[] = "<!DOCTYPE html><html><head><title>416 Rango solicitado no satisfactorio</title></head><body><h1>Rango invalido</h1><p>Rango especificado en el request es incorrecto.</p></body></html>";
const char Error_500[] = "<!DOCTYPE html><html><head><title>500 Error interno del servidor</title></head><body><h1>Error interno del servidor</h1><p>Se ha encontrado un error ... y no lo pude arreglar.</p></body></html>";
const char Error_501[] = "<!DOCTYPE html><html><head><title>501 No implementado</title></head><body><h1>No implementado</h1><p>Esto que Ud. quiere hacer ... todavia no lo escribi.</p></body></html>";
const char Error_Desconocido[] = "<!DOCTYPE html><html><head><title>Error Desconocido</title></head><body><h1>Error Desconocido</h1><p>Este error aparece por que no escribi un codigo de error.</p></body></html>";

// Codigos de Error HTTP
// - 1xx: Informational - Request received, continuing process
// - 2xx: Success - The action was successfully received, understood, and accepted
// - 3xx: Redirection - Further action must be taken in order to complete the request
// - 4xx: Client Error - The request contains bad syntax or cannot be fulfilled
// - 5xx: Server Error - The server failed to fulfill an apparently valid request

class GuruhttpExcepcion:public std::exception {
public:
		GuruhttpExcepcion(uint16_t numero_error, const string& opcional=""):_codigo(numero_error), extra_informacion(opcional) {};
		virtual ~GuruhttpExcepcion() throw() {};

		void enviar_error(const GuruSocket& Socket) {
			this->Socket = Socket;

			string	extra_encabezado;

			switch (_codigo) {
		// Informational
			case 100: break; // Continue
			case 101: break; // Switching Protocols
		// Success
			case 200: break; // OK
			case 201: break; // Created
			case 202: break; // Accepted
			case 203: break; // Non-Authoritative Information
			case 204: break; // No Content
			case 205: break; // Reset Content
			case 206: break; // Partial Content
		// Redirection
			case 300: enviar_encabezado(300, "Multiple Choices", extra_encabezado); break; // Multiple Choices
			case 301: extra_encabezado = "Location: " + extra_informacion;
					  enviar_encabezado(301, "Moved Permanently", extra_encabezado); break; // Moved Permanently
			case 302: extra_encabezado = "Location: " + extra_informacion;
					  enviar_encabezado(302, "Found", extra_encabezado); break; // Found
			case 303: extra_encabezado = "Location: " + extra_informacion;
					  enviar_encabezado(303, "See Other", extra_encabezado); break; // See Other
			case 304: enviar_encabezado(304, "Not Modified"); break; // Not Modified
			case 305: extra_encabezado = "Location: " + extra_informacion;
					  enviar_encabezado(305, "Use Proxy", extra_encabezado); break; // Use Proxy
			case 307: extra_encabezado = "Location: " + extra_informacion;
					  enviar_encabezado(307, "Temporary Redirect", extra_encabezado); break; // Temporary Redirect
		// Client Error
			case 400: enviar(400, Error_400); break; // Bad Request
			case 401: extra_encabezado = "WWW-Authenticate: Basic realm=\"" + extra_informacion + "\"";
					  enviar_encabezado(401, "Unauthorized", extra_encabezado); break; // Unauthorized
		//	case 402: break; // Payment Required
			case 403: enviar(403, Error_403); break; // Forbidden
			case 404: enviar(404, Error_404); break; // Not Found
			case 405: enviar(405, Error_405); break; // Method Not Allowed
			case 406: break; // Not Acceptable
			case 407: break; // Proxy Authentication Required
			case 408: break; // Request Time-out
			case 409: break; // Conflict
			case 410: break; // Gone
			case 411: break; // Length Required
			case 412: enviar_encabezado(412, "Precondition Failed"); break; // Precondition Failed
			case 413: break; // Request Entity Too Large
			case 414: break; // Request-URI Too Large
			case 415: break; // Unsupported Media Type
			case 416: enviar(416, Error_416);break; // Requested range not satisfiable
			case 417: break; // Expectation Failed
		// Server Error
			case 500: enviar(500, Error_500); break; // Internal Server Error
			case 501: enviar(501, Error_501); break; // Not Implemented
			case 502: break; // Bad Gateway
			case 503: break; // Service Unavailable
			case 504: break; // Gateway Time-out
			case 505: break; // HTTP Version not supported
			default: 
					enviar(0, Error_Desconocido); 
				}
		}

		void enviar(uint16_t codigo, const string& texto) {
			ostringstream oss;

			oss << "HTTP/1.1 " << codigo << " Todo Mal" << GuruSocketBase::crlf();
			oss << "Server: " << GURUHTTP_VERSION << GuruSocketBase::crlf();
			oss << "Date: " << GuruFechaHora::fechaRFC1123() << GuruSocketBase::crlf();
			oss << "Content-Length: " << texto.length() << GuruSocketBase::crlf();
			oss << "Content-Type: text/html" << GuruSocketBase::crlf();
			oss << "Cache-Control: no-cache" << GuruSocketBase::crlf();
			oss << GuruSocketBase::crlf(); // fin del encabezado	
			oss << texto << GuruSocketBase::crlf();

			int tamanio = static_cast<int>(oss.str().length());

			send (Socket.handle(), oss.str().c_str(), tamanio, 0);
		}

		void enviar_encabezado(uint16_t codigo, const string& razon, const string& extra_encabezado="") {
			ostringstream oss;

			oss << "HTTP/1.1 " << codigo << " " << razon << GuruSocketBase::crlf();
			oss << "Server: " << GURUHTTP_VERSION << GuruSocketBase::crlf();
			oss << "Date: " << GuruFechaHora::fechaRFC1123() << GuruSocketBase::crlf();
			oss << "Cache-Control: no-cache" << GuruSocketBase::crlf();
		//	oss << "Connection: Keep-Alive" << GuruSocketBase::crlf(); // evito cortar la conexion en 3xx????

			if (!extra_encabezado.empty()) oss << extra_encabezado << GuruSocketBase::crlf();

			oss << GuruSocketBase::crlf(); // fin del encabezado

			int tamanio = static_cast<int>(oss.str().length());

			send (Socket.handle(), oss.str().c_str(), tamanio, 0);
		}

		uint16_t codigo() {return _codigo;};

		virtual const char* what() const throw() { return "";};
private:
        uint16_t		_codigo;
		GuruSocket		Socket;
		string			extra_informacion;
};

class GuruhttpExcepcionSitioNoEncontrado:public GuruhttpExcepcion {
public:
	GuruhttpExcepcionSitioNoEncontrado(uint16_t numero_error, const string& opcional=""):GuruhttpExcepcion(numero_error, opcional) {};
	virtual ~GuruhttpExcepcionSitioNoEncontrado() throw () {};
};

#endif
