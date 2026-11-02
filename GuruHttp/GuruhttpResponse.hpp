#ifndef __GURUHTTPRESPONSE_HPP__
#define __GURUHTTPRESPONSE_HPP__

#include <GuruCompat.h>

#if defined __GURUWIN32__
//////////////////////////////////////////////////////////////////////
//	Link: Ws2_32.lib
//////////////////////////////////////////////////////////////////////
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
	#include <GuruCompat/gettickcount.h>
#endif

#include <map>
#include <string>
#include <fstream>

#include <GuruToken.hpp>
#include <GuruDirectorio.hpp>
#include <GuruFechaHora.hpp>
#include <GuruBuffer.hpp>
#include <GuruBuffer_algo.hpp>

#include "GuruhttpRequest.hpp"
#include "GuruhttpExcepcion.hpp"
#include "Guruhttp_algo.hpp"
#include "GuruhttpVersion.hpp"

#if defined GURUHTTP_LISTAR
	#if !defined GURUHTTP_ARCHIVO_CSS
		#define GURUHTTP_ARCHIVO_CSS "GuruhttpServidor.css"
//		#error Archivo CSS no definido, Defina GURUHTTP_ARCHIVO_CSS con un nombre de archivo.
	#endif
#endif

#if !defined GURUHTTP_VERSION
	#error Etiqueta GURUHTTP_VERSION Sin Definir.
#endif

using namespace std;

class GuruhttpResponse;

class GuruhttResponseCabecera {
	friend class GuruhttpResponse;
public:
	GuruhttResponseCabecera(GuruhttpRequest& requerimiento):response(requerimiento) { cabecera.reserve(1024); };
	virtual ~GuruhttResponseCabecera() {};

	void agregar(const string& atributo, const string& valor) {
		_atributos.insert(make_pair(atributo, valor));
	}

	void agregar(const char* atributo, const char* valor) {
		_atributos.insert(make_pair(string(atributo), string(valor)));
	}

	void status(int codigo, const string& causa) {
		this->_status_code = codigo;

		_status += "HTTP/1.1 " + GuruStringAlgo::itos(codigo) + " " + causa;
	}

	inline int status() { return this->_status_code; }

	void cookie(const string& nombre, const string& valor, const string& parametros="") {
		cabecera += "Set-Cookie: " + nombre + "=" + valor + GuruSocketBase::crlf();
	}

	void cookies(const map<string, string>& cookies) {
		if (cookies.empty()) return;

		for (map<string, string>::const_iterator it = cookies.begin(); it != cookies.end(); ++it)
			cabecera += "Set-Cookie: " + it->first + "=" + it->second + GuruSocketBase::crlf();
	}

	void atributos(map<string, string>& atributos) {
		if (atributos.empty()) return;

		for (map<string, string>::const_iterator it = atributos.begin(); it != atributos.end(); ++it)
			cabecera += it->first + ": " + it->second + GuruSocketBase::crlf();
	}

	string atributo(const string& valor) {
		map<string, string>::iterator it(_atributos.find(valor));

		if (it == _atributos.end()) return string();

		return it->second;
	}


	inline void fin() {
		cabecera += GuruSocketBase::crlf();
	}

	void enviar(const std::string cuerpo="") {

		cabecera.clear();
	
		cabecera += _status + GuruSocketBase::crlf();

		// agrego los atributos propios de este response
		for (map<string, string>::const_iterator it = _atributos.begin(); it != _atributos.end(); ++it)
			cabecera += it->first + ": " + it->second + GuruSocketBase::crlf();

		// agrego atributos que estan definidos en el sitio por ejem.
		for (map<string, string>::const_iterator it = response.config()->sacarAtributosExtra().begin(); it != response.config()->sacarAtributosExtra().end(); ++it)
			cabecera += it->first + ": " + it->second + GuruSocketBase::crlf();

		// las cookies que me vienen en el request se las devuelvo.
		for (map<string, string>::const_iterator it = response.cookies().begin(); it != response.cookies().end(); ++it)
			cabecera += "Set-Cookie: " + it->first + "=" + it->second + GuruSocketBase::crlf();

		cabecera += GuruSocketBase::crlf();

//		cout << "Cabecera:" << static_cast<int>(cabecera.length()) << endl;
//		cout << "Cuerpo:" << static_cast<int>(cuerpo.length()) << endl;

//		cout << cabecera << endl;
//		cout << cuerpo << endl;

		// encabezado
		send(response.recibir().interfaz()->sacar_socket().handle(), cabecera.c_str(), static_cast<int>(cabecera.length()), 0);

		// cuerpo
		if (!cuerpo.empty())
			send(response.recibir().interfaz()->sacar_socket().handle(), cuerpo.c_str(), static_cast<int>(cuerpo.length()), 0);
	}

private:
	std::string			_status;
	uint16_t			_status_code;
	std::string			cabecera;
	map<string, string> _atributos;	///< extra atributos para el response
//	map<string, string> _cookies;	///< key=nombreCookie, value=valor de la Cookie

	GuruhttpRequest&	response;
};

/// \brief Encapsula una respuesta HTTP.
 /**
 * ResponseHttp tiene toda la informacion para realiazar la respuesta HTTP.
 * \sa 	RecursoHttp, RequerimientoHttp, PostHttp
 */
class GuruhttpResponse {
	GuruhttpRequest&		requerimiento;
	GuruhttResponseCabecera	cabecera;
	IEnviar&				GuruSocketResponse;
//	uint16_t				_codigo_estado;	///< Codigo de estado del request.
	string					_content_type; ///< content-type para utilzar en la siguiente respuesta
	bool					_controlar_cache;
public:
	GuruhttpResponse(IEnviar& enviar, GuruhttpRequest& requerimiento):GuruSocketResponse(enviar),requerimiento(requerimiento),cabecera(requerimiento) {
		_controlar_cache = true;
	};
	virtual ~GuruhttpResponse() {};

	// solo para cuando se lanza una excepcion.
	inline uint16_t	codigo_estado() {return cabecera._status_code; };
	inline void		codigo_estado(uint16_t codigo) { cabecera._status_code = codigo;};

	inline bool	controlar_cache() { return _controlar_cache; };
	inline void	controlar_cache(bool control) { _controlar_cache = control; };

	/// Carga el content-type para utilzar en la siguiente respuesta.
	void content_type(const string& type) {_content_type=type;};
	/// returna el content-type para utilzar en la siguiente respuesta.
	string& content_type() {return _content_type;};

	/// Para poner un nuevo atributo en el response
	inline void atributo(const string& nombre_parametro, const string& valor_parametro){
		cabecera._atributos.insert(make_pair(nombre_parametro, valor_parametro));
	}

	inline string atributo(const string& parametro) {
		return cabecera.atributo(parametro);
	}

	void enviar_contenido_archivo() {
		ifstream	is(requerimiento.recurso().ruta_absoluta().c_str(), ios::in | ios::binary);
		int			salida_send = 0;

		::uint32_t	tamanio_buffer=BufferEntrada(requerimiento.recibir().interfaz()->sacar_socket().handle());
		// calculo el tamanio optimo del buffer de salida para cuando se limita la velocidad
		if (requerimiento.config()->velocidad()) {
			::uint32_t tmp_vel=requerimiento.config()->velocidad();
			while (tmp_vel > tamanio_buffer) tmp_vel=tmp_vel/2;
			
			tamanio_buffer=tmp_vel;
		}

		basic_GuruBuffer<char>	buffer_archivo(tamanio_buffer + 2);

		if (!is) return ;

		::uint64_t	inicio_envio=0, fin_envio=0, total_bytes_enviados_seg=0, cont_enviados=0, enviados=0;
		::uint32_t	total_tiempo;

		if (requerimiento.config()->velocidad()) inicio_envio = GetTickCount();

		do{
			is.read(buffer_archivo.buf(), tamanio_buffer);

			if (is.gcount() == 0) break;

			enviados = (int)send (requerimiento.recibir().interfaz()->sacar_socket().handle(), buffer_archivo.const_buf(), (int)is.gcount(), (int)0);

			if (enviados == SOCKET_ERROR) break;

			cont_enviados += enviados;

			if (requerimiento.config()->velocidad()) {
				total_bytes_enviados_seg += enviados;

				if (total_bytes_enviados_seg >= requerimiento.config()->velocidad()) {
					fin_envio = GetTickCount();
					total_tiempo = static_cast<const ::uint32_t>(fin_envio - inicio_envio);
					if (total_tiempo <= 1000) {
						// cout << "Espera en ms:" << total_tiempo << endl;
						if (static_cast<const ::uint32_t>(1000) != total_tiempo) {
							Sleep((static_cast<const ::uint32_t>(1000) - total_tiempo));
					 	} else {
					 		Sleep(1000);
					 	}
					}
					total_bytes_enviados_seg = 0;
					inicio_envio = GetTickCount();
				}
			}

		}while(is.good() && enviados != SOCKET_ERROR);

		is.close();
	}
/*
	void enviar_pagina(ostringstream& oss) {
		GuruhttpResponseHeader	header;

		_codigo_estado = 200;
		header.agregar_status(_codigo_estado, "OK");

		header.agregar("Server", GURUHTTP_VERSION);
		header.agregar("Date", GuruFechaHora::fechaRFC1123());
		header.agregar("Accept-Ranges", "bytes");
		header.agregar("Content-Length", GuruStringAlgo::itos(oss.str().length()));
		header.agregar("Last-Modified", GuruFechaHora::fechaRFC1123());
		header.agregar("Cache-Control", "no-cache");

		if (!_content_type.empty())
			header.agregar("Content-Type", _content_type);
		else 
			header.agregar("Content-Type", "text/html");

		header.agregar_cookie(_cookies);
		header.agregar_parametros(_atributos);

		header.enviar(requerimiento.recibir().interfaz()->sacar_socket().handle());

		send (requerimiento.recibir().interfaz()->sacar_socket().handle(), oss.str().c_str(), static_cast<int>(oss.str().length()), 0);
	}
	*/
	void enviar(const string& buffer) {

		cabecera.status(200, "OK");

		cabecera.agregar("Server", GURUHTTP_VERSION);
		cabecera.agregar("Date", GuruFechaHora::fechaRFC1123());
		cabecera.agregar("Accept-Ranges", "bytes");
		cabecera.agregar("Content-Length", GuruStringAlgo::itos(buffer.length()));
		cabecera.agregar("Last-Modified", GuruFechaHora::fechaRFC1123());
		cabecera.agregar("Cache-Control", "no-cache");

		if (!_content_type.empty()) cabecera.agregar("Content-Type", _content_type);

//		cout << buffer << endl;

		cabecera.enviar(buffer);
	}

	template <typename T> // T=GuruhttpRequest
	void enviar_redireccion(T& requerimiento, const string& nueva_url) {

		cabecera.status(301, "Moved Permanently");
		cabecera.agregar("Server", GURUHTTP_VERSION);
		cabecera.agregar("Date", GuruFechaHora::fechaRFC1123());
		cabecera.agregar("Location", nueva_url);

		cabecera.enviar();
	}

	void enviar_header_archivo() {
		if (!requerimiento.recurso().info().Existe()) throw GuruhttpExcepcion(404);

		cabecera.status(200, "OK");
		cabecera.agregar("Server", GURUHTTP_VERSION);
		cabecera.agregar("Date", GuruFechaHora::fechaRFC1123());
		cabecera.agregar("Accept-Ranges", "bytes");
		cabecera.agregar("Content-Length", GuruStringAlgo::itos(requerimiento.recurso().info().tamanio()));
		cabecera.agregar("Last-Modified", GuruFechaHora::fecha_RFC1123(requerimiento.recurso().info().fecha()));
//		cabecera.agregar("Connection", "keep-alive");

		if (!_content_type.empty())
			cabecera.agregar("Content-Type", _content_type);
		else 
			cabecera.agregar("Content-Type", GuruhttpAlgo::mime(requerimiento.recurso().ruta_absoluta()));

		cabecera.enviar();
	}

	void enviar_header_pagina(const string& pagina) {

		cabecera.status(200, "OK");

		cabecera.agregar("Server", GURUHTTP_VERSION);
		cabecera.agregar("Date", GuruFechaHora::fechaRFC1123());
		cabecera.agregar("Accept-Ranges", "bytes");
		cabecera.agregar("Content-Length", GuruStringAlgo::itos(pagina.length()));
		cabecera.agregar("Last-Modified", GuruFechaHora::fechaRFC1123());

		if (!_content_type.empty())
			cabecera.agregar("Content-Type", _content_type);
		else 
			cabecera.agregar("Content-Type", "text/html");

		cabecera.enviar();
	}

#if defined GURUHTTP_LISTAR
	// Para generar el listado, toma el directorio desde la ruta_absuluta del recurso.
	// Cargar en recurso.ruta_absoluta el directorio a listar
	ostringstream& armar_listado_tipo_google(ostringstream& oss) {

		// primero lo cargo en un contenedor para luego ordenarlo por fecha.
		list<GuruDirectorio::GuruArchivo> listadoDirectorio;
		list<GuruDirectorio::GuruArchivo> listadoArchivo;

		try {
			GuruDirectorio::iterator itfin;

			for (GuruDirectorio::iterator it(requerimiento.recurso().ruta_absoluta());it!=itfin;++it) {
				if (it->EsArchivo())
					listadoArchivo.push_back(*it);
				else if (it->EsDirectorio()) {
					listadoDirectorio.push_back(*it);
				}
			}
		}catch (std::exception&){}

		string orden(requerimiento.parametro("o"));
		string columna(requerimiento.parametro("c"));

		GuruDirectorio::OrdenaGuruArchivo::columna col(GuruDirectorio::OrdenaGuruArchivo::inombre);
		GuruDirectorio::OrdenaGuruArchivo::orden ord(GuruDirectorio::OrdenaGuruArchivo::asc);

		if (columna.compare("nombre") == 0)
			col = GuruDirectorio::OrdenaGuruArchivo::inombre;
		else if (columna.compare("bytes") == 0)
			col = GuruDirectorio::OrdenaGuruArchivo::bytes;
		else if (columna.compare("fecha") == 0)
			col = GuruDirectorio::OrdenaGuruArchivo::fecha;

		if (orden.compare("asc") == 0)
			ord = GuruDirectorio::OrdenaGuruArchivo::asc;
		else if (orden.compare("desc") == 0)
			ord = GuruDirectorio::OrdenaGuruArchivo::desc;

		listadoDirectorio.sort(GuruDirectorio::OrdenaGuruArchivo(col, ord));
		listadoArchivo.sort(GuruDirectorio::OrdenaGuruArchivo(col, ord));

		listadoDirectorio.insert(listadoDirectorio.end(), listadoArchivo.begin(), listadoArchivo.end());

		oss << "<html xmlns=\"http://www.w3.org/1999/xhtml\"> \
				<head> \
					<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /> \
					<!-- <link href=\"" << requerimiento.recurso().directorio() << GURUHTTP_ARCHIVO_CSS "\" rel=\"stylesheet\" type=\"text/css\" /> --> \
					<title>Listado de Directorio en http://" << requerimiento.atributo("Host") + requerimiento.requerimiento().recurso()  << "</title> \
				</head> \
				<style> \
					table { height: 0%; width: auto; } \
					.encabezado { font-size: medium; font-style: normal; font-weight: bold; text-transform: capitalize; text-align: center; white-space: nowrap; } \
					.titulo { font-size: large; font-style: normal; font-weight: bold; padding-bottom: 10px; } \
					.archivo { font-size: small; text-align: right; white-space: nowrap; padding-left: 2em; } \
					.icono-archivo { background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAIAAACQkWg2AAAABnRSTlMAAAAAAABupgeRAAABHUlEQVR42o2RMW7DIBiF3498iHRJD5JKHurL+CRVBp+i2T16tTynF2gO0KSb5ZrBBl4HHDBuK/WXACH4eO9/CAAAbdvijzLGNE1TVZXfZuHg6XCAQESAZXbOKaXO57eiKG6ft9PrKQIkCQqFoIiQFBGlFIB5nvM8t9aOX2Nd18oDzjnPgCDpn/BH4zh2XZdlWVmWiUK4IgCBoFMUz9eP6zRN75cLgEQhcmTQIbl72O0f9865qLAAsURAAgKBJKEtgLXWvyjLuFsThCSstb8rBCaAQhDYWgIZ7myM+TUBjDHrHlZcbMYYk34cN0YSLcgS+wL0fe9TXDMbY33fR2AYBvyQ8L0Gk8MwREBrTfKe4TpTzwhArXWi8HI84h/1DfwI5mhxJamFAAAAAElFTkSuQmCC\") left top no-repeat; padding-left: 1.5em; } \
					.icono-directorio { background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAAd5JREFUeNqMU79rFUEQ/vbuodFEEkzAImBpkUabFP4ldpaJhZXYm/RiZWsv/hkWFglBUyTIgyAIIfgIRjHv3r39MePM7N3LcbxAFvZ2b2bn22/mm3XMjF+HL3YW7q28YSIw8mBKoBihhhgCsoORot9d3/ywg3YowMXwNde/PzGnk2vn6PitrT+/PGeNaecg4+qNY3D43vy16A5wDDd4Aqg/ngmrjl/GoN0U5V1QquHQG3q+TPDVhVwyBffcmQGJmSVfyZk7R3SngI4JKfwDJ2+05zIg8gbiereTZRHhJ5KCMOwDFLjhoBTn2g0ghagfKeIYJDPFyibJVBtTREwq60SpYvh5++PpwatHsxSm9QRLSQpEVSd7/TYJUb49TX7gztpjjEffnoVw66+Ytovs14Yp7HaKmUXeX9rKUoMoLNW3srqI5fWn8JejrVkK0QcrkFLOgS39yoKUQe292WJ1guUHG8K2o8K00oO1BTvXoW4yasclUTgZYJY9aFNfAThX5CZRmczAV52oAPoupHhWRIUUAOoyUIlYVaAa/VbLbyiZUiyFbjQFNwiZQSGl4IDy9sO5Wrty0QLKhdZPxmgGcDo8ejn+c/6eiK9poz15Kw7Dr/vN/z6W7q++091/AQYA5mZ8GYJ9K0AAAAAASUVORK5CYII=\") left top no-repeat; padding-left: 1.5em; } \
					.icono-directorio-padre { background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAAmlJREFUeNpsU0toU0EUPfPysx/tTxuDH9SCWhUDooIbd7oRUUTMouqi2iIoCO6lceHWhegy4EJFinWjrlQUpVm0IIoFpVDEIthm0dpikpf3ZuZ6Z94nrXhhMjM3c8895977BBHB2PznK8WPtDgyWH5q77cPH8PpdXuhpQT4ifR9u5sfJb1bmw6VivahATDrxcRZ2njfoaMv+2j7mLDn93MPiNRMvGbL18L9IpF8h9/TN+EYkMffSiOXJ5+hkD+PdqcLpICWHOHc2CC+LEyA/K+cKQMnlQHJX8wqYG3MAJy88Wa4OLDvEqAEOpJd0LxHIMdHBziowSwVlF8D6QaicK01krw/JynwcKoEwZczewroTvZirlKJs5CqQ5CG8pb57FnJUA0LYCXMX5fibd+p8LWDDemcPZbzQyjvH+Ki1TlIciElA7ghwLKV4kRZstt2sANWRjYTAGzuP2hXZFpJ/GsxgGJ0ox1aoFWsDXyyxqCs26+ydmagFN/rRjymJ1898bzGzmQE0HCZpmk5A0RFIv8Pn0WYPsiu6t/Rsj6PauVTwffTSzGAGZhUG2F06hEc9ibS7OPMNp6ErYFlKavo7MkhmTqCxZ/jwzGA9Hx82H2BZSw1NTN9Gx8ycHkajU/7M+jInsDC7DiaEmo1bNl1AMr9ASFgqVu9MCTIzoGUimXVAnnaN0PdBBDCCYbEtMk6wkpQwIG0sn0PQIUF4GsTwLSIFKNqF6DVrQq+IWVrQDxAYQC/1SsYOI4pOxKZrfifiUSbDUisif7XlpGIPufXd/uvdvZm760M0no1FZcnrzUdjw7au3vu/BVgAFLXeuTxhTXVAAAAAElFTkSuQmCC\") left top no-repeat; padding-left: 1.5em; } \
					.archivo-nombre { text-align: left; padding-right: 1.5em; } \
					.archivo-bytes { text-align: right; padding-left: 1.5em; } \
					.archivo-fecha { text-align: right; padding-left: 1.5em; } \
				</style> \
				<body> \
				<table > \
				<tr class=\"titulo\"> \
					<td colspan=\"4\">" << requerimiento.recurso().directorio() + requerimiento.recurso().subdirectorio() << "</td> \
				</tr> \
				<tr class=\"encabezado\"> \
					<td ></td> \
					<td><a href=?c=nombre&o=" << ((col==GuruDirectorio::OrdenaGuruArchivo::inombre && ord==GuruDirectorio::OrdenaGuruArchivo::asc) ? "desc" : "asc") << ">Nombre</a></td> \
					<td><a href=?c=bytes&o=" << ((col==GuruDirectorio::OrdenaGuruArchivo::bytes && ord==GuruDirectorio::OrdenaGuruArchivo::asc) ? "desc" : "asc") << ">Bytes</a></td> \
					<td><a href=?c=fecha&o=" << ((col==GuruDirectorio::OrdenaGuruArchivo::fecha && ord==GuruDirectorio::OrdenaGuruArchivo::asc) ? "desc" : "asc") << ">Fecha</a></td> \
				</tr>" ;

		// primero pongo el nivel superior
		if (!requerimiento.recurso().subdirectorio().empty()) {
			oss << "<tr> <td class=\"icono-directorio-padre\"></td>";
			oss << "<td class=\"archivo-nombre\"><a href=..>..</a></td>";
			oss << "<td class=\"archivo-bytes\">[..]</td>";
			oss << "<td class=\"archivo-fecha\">..</td> </tr>";
		}

		string nombre_codificado;

		for (list<GuruDirectorio::GuruArchivo>::iterator itr=listadoDirectorio.begin();itr!=listadoDirectorio.end();++itr) {
			nombre_codificado = GuruhttpAlgo::URLEncode(itr->nombre());
			// columna de icono
			oss << "<tr class=\"archivo\">";
			if (itr->EsDirectorio()) oss << "<td class=\"icono-directorio\"></td>";
			else if (itr->EsArchivo()) oss << "<td class=\"icono-archivo\"></td>";
			else oss << "<td></td>";

			// columna de nombre
			if (itr->EsArchivo()) oss << "<td class=\"archivo-nombre\"> <a href=\"" << nombre_codificado << "\">" << itr->nombre() << "</a></td>";
			else if (itr->EsDirectorio()) oss << "<td class=\"archivo-nombre\"> <a href=\"" << nombre_codificado << "/\">" << itr->nombre() << "</a></td>";
			else oss << "<td></td>";

			// columna de tamanio/tipo
			if (itr->EsArchivo()) oss << "<td class=\"archivo-bytes\">" << itr->tamanio() << "</td>";
			else if (itr->EsDirectorio()) oss << "<td class=\"archivo-bytes\">[DIR]</td>";
			else oss << "<td class=\"archivo-bytes\">[000]</td>";

			// columna de fecha
			oss << "<td class=\"archivo-fecha\">" << GuruAlgo::fechaHoraRFC822(itr->fecha()) << "</td></tr>";
		}

		oss << "</table></body></html>";

		return oss;
	}

	template <typename T> // T=GuruhttpRequest
	void enviar_listado(T& requerimiento, const string& ruta_virtual, const string& directorio_real) {
		ostringstream	oss;
		string			directorio_completo;

		directorio_completo = ruta_virtual;
	//	directorio_completo += ruta_virtual;

		oss << "<html>" << GuruSocketBase::crlf();
		oss << "<head>" << GuruSocketBase::crlf();
		oss << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />" << GuruSocketBase::crlf();
		oss << "<title>" << directorio_completo << "</title>" << GuruSocketBase::crlf();
		oss << "</head>" << GuruSocketBase::crlf();

		oss << "<BODY>" << GuruSocketBase::crlf();
		/* el titulo*/
		oss << "<table width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\"> \
			<tr> \
				<td valign=\"top\">Titulo</td> \
			</tr> \
			<tr> \
				<td valign=\"top\">Datos</td> \
			</tr> \
			<tr> \
				<td valign=\"top\">Otros</td> \
			</tr> \
		</table>" << GuruSocketBase::crlf();

		oss << "<table width=\"0%\" border=\"0\" cellpadding=\"0\" cellspacing=\"2%\"> \
			<tr> \
				<td width=\"0%\" valign=\"top\">Nombre</td> \
				<td width=\"0%\" valign=\"top\">Tamanio</td> \
				<td width=\"0%\" valign=\"top\">Fecha</td> \
			</tr>" << GuruSocketBase::crlf();

		GuruDirectorio::iterator itfinal;
		for (GuruDirectorio::iterator it(directorio_real);it!=itfinal;++it) {
			oss << "<tr>" << GuruSocketBase::crlf();
					oss << "<td valign=\"top\">" << "<A HREF=\"" << directorio_completo << it->nombre() << "\">" << it->nombre() << "</A>" << "</td>";
					if (it->EsDirectorio())
						oss << "<td valign=\"top\">" << "DIR" << "</td>";
					else
						oss << "<td valign=\"top\">" << it->tamanio() << "</td>";
					oss << "<td valign=\"top\">" << GuruFechaHora::fecha_asctime(it->fecha()) << "</td>";
			oss << "</tr>" << GuruSocketBase::crlf();
		}

		oss << "</table> </body> </html>" << GuruSocketBase::crlf();

		ostringstream	header;

		requerimiento.response().codigo_estado(200);

		header << "HTTP/1.1 200 OK" << GuruSocketBase::crlf();
		header << "Server: " << GURUHTTP_VERSION  << GuruSocketBase::crlf();
		header << "Date: " << GuruFechaHora::fechaRFC1123() << GuruSocketBase::crlf();
		header << "Accept-Ranges: bytes" << GuruSocketBase::crlf();
		header << "Content-Length: " << oss.str().length() << GuruSocketBase::crlf();
		header << "Content-Type: text/html" << GuruSocketBase::crlf();
		header << "Last-Modified: " << GuruFechaHora::fechaRFC1123() << GuruSocketBase::crlf();
		header << GuruSocketBase::crlf();

		// envio el header del listado
		send (requerimiento.recibir().interfaz()->sacar_socket().handle(), header.str().c_str(), header.str().length(), 0);

		// envio el listado
		send (requerimiento.recibir().interfaz()->sacar_socket().handle(), oss.str().c_str(), oss.str().length(), 0);
	}
#endif

	template <typename T> // T=GuruhttpRequest
	void enviar_archivo_parcial(T& requerimiento, const string& archivo, const pair<uint64_t, uint64_t>& rango) {

		if (rango.first == static_cast<const ::uint64_t>(-1) && rango.second == static_cast<const ::uint64_t>(-1)) throw GuruhttpExcepcion(416);

		ifstream ifs(archivo.c_str(), ios::in | ios::binary);

		if (!ifs.is_open()) throw GuruhttpExcepcion(404);

		::uint64_t inicio=0,fin=0,total=0,enviar=0;

		ifs.seekg(0, std::ios_base::end);
		total=ifs.tellg();

		inicio = rango.first;

		if (rango.second == 0) 
			fin=total;
		else if (rango.second == -1) {
			inicio = total - rango.first;
			fin = total;
		} else if (rango.first >= 0 && rango.second >=0) { // entonces son bytes que estan en el medio del archivo
					inicio	= rango.first;
					fin		= rango.second;
		}

		// doble chequeo de que esta todo bien, en la RFC lo menciona.
		if (inicio >= fin) throw GuruhttpExcepcion(416);

		ifs.seekg(static_cast<std::ios_base::streamoff>(inicio), std::ios_base::beg);

		enviar=(fin-inicio);
		::uint32_t	tamanio_buffer=BufferEntrada(requerimiento.recibir().interfaz()->sacar_socket().handle());

		if (enviar < tamanio_buffer)
			tamanio_buffer = static_cast<const ::uint32_t>(enviar);

		GuruBuffer	buffer_archivo(tamanio_buffer + 2);

		cabecera.status(206, "Partial content");
		cabecera.agregar("Server", GURUHTTP_VERSION);
		cabecera.agregar("Date", GuruFechaHora::fechaRFC1123());
		cabecera.agregar("Last-Modified", GuruFechaHora::fecha_RFC1123(GuruDirectorio::propiedades(archivo).fecha()));
		cabecera.agregar("Content-Range", "bytes " + GuruStringAlgo::itos(inicio) + "-" + GuruStringAlgo::itos(fin-1) + "/" + GuruStringAlgo::itos(total));
		cabecera.agregar("Content-Length", GuruStringAlgo::itos((fin-inicio)));

		if (!_content_type.empty())
			cabecera.agregar("Content-Type", _content_type);
		else 
			cabecera.agregar("Content-Type", GuruhttpAlgo::mime(archivo));

		cabecera.enviar();

		::uint64_t enviados=0;

		do{
			if ((enviar-enviados) < tamanio_buffer)
				tamanio_buffer = static_cast<const ::uint32_t>(enviar-enviados);

			ifs.read(reinterpret_cast<char*>(buffer_archivo.buf()), tamanio_buffer);

			if (ifs.gcount() == 0) break;

			if (send (requerimiento.recibir().interfaz()->sacar_socket().handle(), reinterpret_cast<const char*>(buffer_archivo.buf()), (int)ifs.gcount(), 0) == SOCKET_ERROR) break;

			enviados += ifs.gcount();

		}while(ifs.good() && (enviar-enviados) != 0);

		ifs.close();
	}

	// controlar_cache:
	// Nota:
	// If-Range: El condicional si-rango, esta emplementado en el GuruhttpServidor debido a que hay que tomar
	//			una desicion si hay que enviar un archivo completo o una parte de este.
	void control_cache () {
		string tmp;

		if (!(tmp=requerimiento.atributo("If-Modified-Since")).empty()) {
			if (GuruStringAlgo::stringequals(tmp, GuruFechaHora::fechaRFC1123(requerimiento.recurso().info().fecha())))
				throw GuruhttpExcepcion(304);
		} else if (!(tmp=requerimiento.atributo("If-Unmodified-Since")).empty()) {
			if (GuruStringAlgo::stringequals(tmp, GuruFechaHora::fechaRFC1123(requerimiento.recurso().info().fecha())))
				throw GuruhttpExcepcion(412);
		}

		/* Si no lo utilizo prefiero que no se pregunte, se utilizan con entity-tag
			else if (!(tmp=requerimiento.atributo("If-Match")).empty()) {
		} else if (!(tmp=requerimiento.atributo("If-None-Match")).empty()) {}
		*/
	};

	void enviar_archivo() {

		if (controlar_cache()) control_cache ();

		// Segun HTTP 2616 - 14.35 Range
		if (!requerimiento.atributo("Range").empty()) {
			string ifrango;
			// Segunt HTTP 2616 - 14.27 If-Range
			if (!(ifrango=requerimiento.atributo("If-Range")).empty()) {
				if (GuruStringAlgo::stringequals(ifrango, GuruFechaHora::fechaRFC1123(requerimiento.recurso().info().fecha())))
					GuruhttpResponse::enviar_archivo_parcial(requerimiento, requerimiento.recurso().ruta_absoluta(), GuruhttpAlgo::RangeDecode(requerimiento.atributo("Range")));
			} else
				GuruhttpResponse::enviar_archivo_parcial(requerimiento, requerimiento.recurso().ruta_absoluta(), GuruhttpAlgo::RangeDecode(requerimiento.atributo("Range")));
		} else {
			enviar_header_archivo();

			enviar_contenido_archivo();
		}
	}

	//////////////////////////////////////////////////////////////////////
	// Este metodo genera que el browser sea obligado a descargar el archivo
	// y no a procesarlo.
	// Tendria que aparecerle la ventana de diga "Guardar Como..."
	// El parametro nombre alternativo es para que al cliente le aparezca
	// otro nombre cuando le aparezca el cuatro "Guardar Como ..."
	//////////////////////////////////////////////////////////////////////
	template <typename T> // T=GuruhttpRequest
	void enviar_bajar_archivo(T& requerimiento, const string& ruta_archivo, uint32_t velocidad) {

	// Content-Disposition: attachment;filename=somefile.ext; // lo descarga
	// Content-Disposition: inline;filename=somefile.ext;	// trata de ejecutarlo en el navegador
	// Content-Description: "echen pajaaaaaaaaa ...."; // frase por si lo descarga

		atributo("Content-Type", "application/octet-stream");
		atributo("Content-Disposition", "attachment; filename=" + GuruStringAlgo::comillas(requerimiento.recurso().nombre()));
		atributo("Content-Description", requerimiento.recurso().nombre()); // frase por si lo descarga

		enviar_header_archivo();

	#if defined __GURUWIN32__
		if (velocidad)
			enviar_archivo(requerimiento, ruta_archivo, velocidad);
		else
	#endif
			enviar_archivo(requerimiento, ruta_archivo);
	}
};

#endif
