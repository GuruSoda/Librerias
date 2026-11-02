#ifndef __GURUSOCKETARCHIVOSSERVIDOR_H__
#define __GURUSOCKETARCHIVOSSERVIDOR_H__

#include <GuruCompat.h>
#include <GuruSHA1.hpp>
#include <polarssl/sha1.h>
#include <GuruEtiquetaLeer.hpp>
#include <GuruToken.hpp>
#include <GuruAlgo.hpp>
#include <GuruSocket/GuruProtocoloServidor.hpp>
#include <GuruSocket/GuruSocketLeer.hpp>
#include <GuruSocket/GuruSocketEscribir.hpp>
#include <GuruProtocolo.hpp>
#include <GuruVarEntorno.hpp>
#include <GuruVariable.hpp>
#include <GuruBuffer.hpp>
#include <GuruDirectorio.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <exception>

#if defined __GURUWIN32__
#include "ZLib\zlib.h"
#include <direct.h>
#include <sys\utime.h>
#include <Lm.h>

// para el share, tambien link con Netapi32.lib
#include <lm.h>
#include <Lmaccess.h>
#include <Lmapibuf.h>

#elif defined __GURULINUX__
#include <utime.h>
#include <cctype>
#include <stdint.h>
#include <GuruCompat/gettickcount.h>
#endif

namespace GuruArchivosServidor {

#ifndef GURUSOCKETARCHIVOSRECIBIR_NO
	class InformacionArchivoRecibir
	{
		friend class GuruSocketArchivosServidor;

	public:
		InformacionArchivoRecibir() :estado(inconsistente) { limpiar(); };

		enum estado_recibir { inconsistente = 0, recibir, recibiendo, recibido };

		string		nombre;
		string		str_tamanio;
		uint64_t	tamanio;
		string		str_atributos;
		uint32_t	atributos;
		string		str_fecha;
		time_t		fecha;
		string		sha1;
		string		sha1_llegando;
		string		str_agregar;
		uint64_t	agregar;
		bool		compactado;
		bool		mover;
		bool		reemplazar;
		bool		calcular_sha1;

		uint64_t	bytes_recibidos;

		string		destino;							// es el directorio donde hay que ubicarlo.

		string		ruta_completa_directorio_destino;	// la ruta completa del archivo a donde tiene que estar.
		string		ruta_completa_directorio_temporal;	// la ruta completa del archivo en el directorio virtual o temporario.

		estado_recibir	estado;

		ostringstream mensaje_fin_copia;
		ostringstream mensaje_inicio_estado;
	private:
		void limpiar() {
			tamanio = 0;
			atributos = 0;
			fecha = 0;
			agregar = 0;
			compactado = false;
			reemplazar = false;
			mover = true;
			calcular_sha1 = true;
			bytes_recibidos = 0;
			estado = inconsistente;
		};
	};

	class MensajesRecepcion
	{
	public:
		MensajesRecepcion() {};
		virtual ~MensajesRecepcion() {};

		virtual bool Notificar(const InformacionArchivoRecibir& info) { return true; };
	};

	template <class Clase>
	class MensajesRecepcionArchivos : public MensajesRecepcion
	{
		friend class GuruSocketArchivosRecibir;
		typedef bool (Clase::*Metodo)(const InformacionArchivoRecibir&);

	public:
		MensajesRecepcionArchivos(Clase* laclase, Metodo elmetodo) {
			instancia = laclase;
			metodo = elmetodo;
		};

		virtual bool Notificar(const InformacionArchivoRecibir& info) {
			return (instancia->*metodo)(info);
		};

	private:
		Clase*  instancia;
		Metodo  metodo;
	};

#endif

#ifndef GURUSOCKETARCHIVOS_ENVIAR_NO
	class InformacionArchivoEnviar
	{
		template <class LEER, class ESCRIBIR>
		friend class basic_GuruSocketArchivosServidor;

	public:
		InformacionArchivoEnviar() { limpiar(); };
		virtual ~InformacionArchivoEnviar() {};

		enum estado_enviar { inconsistente = 0, enviar, enviando, enviado };

		string		nombre;
		uint64_t	tamanio;
		string		str_atributos;
		uint32_t	atributos;
		string		str_fecha;
		time_t		fecha;
		string		sha1;
		uint64_t	desde;
		uint64_t	hasta;
		bool		compactado;
		bool		calcular_sha1;
		uint32_t	velocidad;
		uint64_t	bytes_recibidos;
		uint32_t	ms_espera;

		estado_enviar	estado;

		void limpiar() {
			tamanio = 0;
			atributos = 0;
			fecha = 0;
			desde = 0;
			hasta = 0;
			compactado = false;
			calcular_sha1 = true;
			velocidad = 0;
			bytes_recibidos = 0;
			ms_espera = 0;
			estado = inconsistente;
		};
	};

	class MensajesEnviar
	{
	public:
		MensajesEnviar() {};
		virtual ~MensajesEnviar() {};

		virtual bool Notificar(const InformacionArchivoEnviar& info) { return true; };
	};

	template <class Clase>
	class MensajesEnviarArchivos : public MensajesEnviar
	{
		friend class GuruSocketArchivosRecibir;
		typedef bool (Clase::*Metodo)(const InformacionArchivoEnviar&);

	public:
		MensajesEnviarArchivos(Clase* laclase, Metodo elmetodo) {
			instancia = laclase;
			metodo = elmetodo;
		};
		virtual ~MensajesEnviarArchivos() {};

		virtual bool Notificar(const InformacionArchivoEnviar& info) {
			return (instancia->*metodo)(info);
		};

	private:
		Clase*  instancia;
		Metodo  metodo;
	};

#endif

	class ParametrosServidorArchivos
	{
	public:
		ParametrosServidorArchivos() :noTemporal(true), extension(".ccg") {
#ifndef GURUSOCKETARCHIVOSRECIBIR_NO
			MensajesRecepcionArchivos = NULL;
#endif
#ifndef GURUSOCKETARCHIVOS_ENVIAR_NO
			MensajesEnviarArchivos = NULL;
#endif

#ifdef __GURUWIN32__
			directorioTemporal = GuruDirectorio::comoDirectorio(GuruVarEntorno::reemplazar("%TEMP%"));
			directorioDestino = GuruDirectorio::comoDirectorio(GuruVarEntorno::reemplazar("%USERPROFILE%\\Entrada"));
#elif defined __GURULINUX__
			directorioTemporal = GuruDirectorio::comoDirectorio(GuruVarEntorno::reemplazar("/tmp"));
			directorioDestino = GuruDirectorio::comoDirectorio(GuruVarEntorno::reemplazar("$HOME/Entrada"));
#endif

			if (directorioTemporal.empty()) {
				directorioTemporal = ".";
				directorioTemporal += DIVISOR_DIRECTORIO;
			}

			if (directorioDestino.empty()) {
				directorioDestino = ".";
				directorioDestino += DIVISOR_DIRECTORIO;
			}
		};

		ParametrosServidorArchivos(const string& directorioTemportal, const string& directorioDestino)
			:directorioTemporal(directorioTemporal), directorioDestino(directorioDestino) {
#ifndef GURUSOCKETARCHIVOSRECIBIR_NO
			MensajesRecepcionArchivos = NULL;
#endif
#ifndef GURUSOCKETARCHIVOS_ENVIAR_NO
			MensajesEnviarArchivos = NULL;
#endif

			noTemporal = false;
		};

		virtual ~ParametrosServidorArchivos() { };

#ifndef GURUSOCKETARCHIVOSRECIBIR_NO
		MensajesRecepcion*	MensajesRecepcionArchivos;
#endif
#ifndef GURUSOCKETARCHIVOS_ENVIAR_NO
		MensajesEnviar*		MensajesEnviarArchivos;
#endif

		bool				noTemporal;
		string				extension;
		string				directorioTemporal;
		string				directorioDestino;
	};

	class ConfiguracionServidorArchivos
	{
		ConfiguracionServidorArchivos() {};

		ParametrosServidorArchivos	parametros;

	public:
		static ConfiguracionServidorArchivos* instancia() {
			static ConfiguracionServidorArchivos GuruSingleton;

			return &GuruSingleton;
		};

		ParametrosServidorArchivos& Configuracion() { return this->parametros; };
		void Configuracion(const ParametrosServidorArchivos& parametro) {
			this->parametros = parametro;

			this->parametros.directorioDestino = GuruDirectorio::comoDirectorio(this->parametros.directorioDestino);
			this->parametros.directorioTemporal = GuruDirectorio::comoDirectorio(this->parametros.directorioTemporal);
		};

#ifndef GURUSOCKETARCHIVOSRECIBIR_NO
		void Notificacion(MensajesRecepcion* mensajes) {
			this->parametros.MensajesRecepcionArchivos = mensajes;
		};

		MensajesRecepcion* NotificacionEnviar() {
			return this->parametros.MensajesRecepcionArchivos;
		};
#endif

#ifndef GURUSOCKETARCHIVOS_ENVIAR_NO
		void Notificacion(MensajesEnviar* mensajes) {
			this->parametros.MensajesEnviarArchivos = mensajes;
		};

		MensajesEnviar* NotificacionRecepcion() {
			return this->parametros.MensajesEnviarArchivos;
		};
#endif
	};

	class GuruSocketArchivosServidor : public GuruProtocoloServidor
	{
	public:
		GuruSocketArchivosServidor(IEnviar& enviar, IRecibir& recibir) :GuruProtocoloServidor(enviar, recibir) {
			parametrosServidor = ConfiguracionServidorArchivos::instancia()->Configuracion();
		};
		virtual ~GuruSocketArchivosServidor(void) {};

#ifndef GURUSOCKETARCHIVOSRECIBIR_NO
		static void	cargar_parametros(const ParametrosServidorArchivos& parametros) {
			ConfiguracionServidorArchivos::instancia()->Configuracion(parametros);
		};

		static void	Notificacion(MensajesRecepcion* msj) {
			ConfiguracionServidorArchivos::instancia()->Notificacion(msj);
		};

		void Notificar(InformacionArchivoRecibir::estado_recibir estado) {
			if (parametrosServidor.MensajesRecepcionArchivos) {
				archivo_llegando.estado = estado;
				parametrosServidor.MensajesRecepcionArchivos->Notificar(archivo_llegando);
			}
		};
#endif
	protected:
		bool init(void) {
			GuruProtocoloServidor::init();

#ifndef GURUSOCKETARCHIVOSRECIBIR_NO
			// El cliente escribe enviar, yo "Recibo" el archivo
			cargar_comando(this, &GuruSocketArchivosServidor::cmd_recibir, "enviar", "Envia un archivo.");
#endif

#ifndef GURUSOCKETARCHIVOS_ENVIAR_NO
			// El clietne escribe recibir, yo "Envio" el archivo
			cargar_comando(this, &GuruSocketArchivosServidor::cmd_enviar, "recibir", "Recibir un archivo.");
#endif

#ifndef GURUSOCKETARCHIVOSNAV_NO
			cargar_comando(this, &GuruSocketArchivosServidor::dir, "dir", "Muestra el contenido de un directorio.");
#endif

			cargar_comando(this, &GuruSocketArchivosServidor::cmd_borrar, "borrar", "Borra el archivo recientemente recibido.");
			cargar_comando(this, &GuruSocketArchivosServidor::cmd_comparar, "comparar", "Compara un archivo en disco con el pasado por parametro.");
			cargar_comando(this, &GuruSocketArchivosServidor::cmd_existe, "existe", "Verifica la existencia de un archivo.");
			cargar_comando(this, &GuruSocketArchivosServidor::cmd_sha1, "sha1", "Realiza un SHA1 sobre un archivo.");
			cargar_comando(this, &GuruSocketArchivosServidor::cmd_copiar, "copiar", "Copiar un archivo de un directorio a otro.");
			cargar_comando(this, &GuruSocketArchivosServidor::cmd_dump, "dump", "Muestra el contenido de un archivo.");
#ifdef __GURUWIN32__
			cargar_comando(this, &GuruSocketArchivosServidor::cmd_share, "share", "Retorna el directorio compartido.");
#endif
			cargar_comando(this, &GuruSocketArchivosServidor::cmd_informacion, "informacion", "Retorna informacion acerca de un archivo.");
			cargar_comando(this, &GuruSocketArchivosServidor::cmd_informacion, "info", "", false);

			cargar_msg_bienvenida("Bienvenido al Servidor de Archivos del Guru.");
			cargar_msg_salida("Gracias por utilizar el servidor de Archivos Guru.");

			return true;
		}

		ParametrosServidorArchivos		parametrosServidor;

#ifndef GURUSOCKETARCHIVOSRECIBIR_NO
		void cmd_recibir(const vector<string>& parametros) {
			try {
//				this->recibir.interfaz()->sacar_socket().timeout(60);
				GURULOG_DEBUG("Recibiendo!");

				GuruEtiquetaLeerEx	tag(parametros);

				archivo_llegando.limpiar();

				archivo_llegando.nombre = tag["Nombre"];
				archivo_llegando.str_tamanio = tag["Tamanio"];
				archivo_llegando.tamanio = _atoi64(archivo_llegando.str_tamanio.c_str());
				archivo_llegando.destino = GuruDirectorio::comoDirectorio(GuruDirectorio::normalizarDirectorio(tag["Destino"]));
				archivo_llegando.str_atributos = tag["Atributos"];
				archivo_llegando.str_fecha = tag["FechaHora"];
				archivo_llegando.sha1 = tag["SHA1"];
				archivo_llegando.str_agregar = tag["Agregar"];
				archivo_llegando.compactado = ((tag["Compactado"].empty()) || (tag["Compactado"].compare("0") == 0)) ? false : true;
				archivo_llegando.mover = ((tag["mover"].empty()) || (tag["mover"].compare("0") == 0)) ? true : false;
				archivo_llegando.reemplazar = ((tag["reemplazar"].empty()) || (tag["reemplazar"].compare("0") == 0)) ? false : true;

				if (archivo_llegando.nombre.empty() || archivo_llegando.str_tamanio.empty()) throw string("Error en parametros");

				if (!archivo_llegando.str_fecha.empty())
					archivo_llegando.fecha = GuruAlgo::RFC822FechaHora(archivo_llegando.str_fecha);

				if (!archivo_llegando.str_agregar.empty())
					archivo_llegando.agregar = _atoi64(archivo_llegando.str_agregar.c_str());

				if (!archivo_llegando.sha1.empty())
					transform(archivo_llegando.sha1.begin(), archivo_llegando.sha1.end(), archivo_llegando.sha1.begin(), ::tolower);

				if (!archivo_llegando.destino.empty())
					archivo_llegando.ruta_completa_directorio_destino = archivo_llegando.destino + archivo_llegando.nombre;
				else
					archivo_llegando.ruta_completa_directorio_destino = parametrosServidor.directorioDestino + archivo_llegando.nombre;

				if (parametrosServidor.noTemporal)
					archivo_llegando.ruta_completa_directorio_temporal = archivo_llegando.ruta_completa_directorio_destino + parametrosServidor.extension;
				else
					archivo_llegando.ruta_completa_directorio_temporal = parametrosServidor.directorioTemporal + archivo_llegando.nombre;

				////////////////////////////[Reemplazo las variables de entorno]///////////////////////
				archivo_llegando.ruta_completa_directorio_destino = GuruVarEntorno::reemplazar(archivo_llegando.ruta_completa_directorio_destino);
				archivo_llegando.ruta_completa_directorio_temporal = GuruVarEntorno::reemplazar(archivo_llegando.ruta_completa_directorio_temporal);
#if defined __GURUWIN32__
				GuruVariable<GuruSocketArchivosServidor> var(this, &GuruSocketArchivosServidor::sacarRutaShare, '{', '}');

				archivo_llegando.ruta_completa_directorio_destino = var.reemplazar(archivo_llegando.ruta_completa_directorio_destino);
				archivo_llegando.ruta_completa_directorio_temporal = var.reemplazar(archivo_llegando.ruta_completa_directorio_temporal);

				archivo_llegando.destino = var.reemplazar(archivo_llegando.destino);
#endif
				Notificar(InformacionArchivoRecibir::recibir);

				if (!GuruDirectorio::existe(archivo_llegando.destino)) GuruDirectorio::crearDirectorioRecursivo(archivo_llegando.destino);

				bool recibir = true;

				if (existe_archivo_a_recibir() && !archivo_llegando.reemplazar) {
					archivo_llegando.mensaje_inicio_estado << "Existe=1 Msg=\"El archivo ya existe.\"";

					BIEN(archivo_llegando.mensaje_inicio_estado.str());

					recibir = false;
				} else if (continuar_archivo_a_recibir() && !archivo_llegando.reemplazar) {
					uint64_t tamanio = GuruDirectorio::propiedades(archivo_llegando.ruta_completa_directorio_destino + ".ccg").tamanio();

					archivo_llegando.mensaje_inicio_estado << "Existe=2 Msg=\"El archivo es el mismo pero con distinta cantidad de bytes.\" agregar=" << tamanio;

					archivo_llegando.agregar = tamanio;
				}

				if (recibir)
					recibir_contenido();

				if (!archivo_llegando.sha1_llegando.empty())
					archivo_llegando.mensaje_fin_copia << "SHA1=" << archivo_llegando.sha1_llegando;

				archivo_llegando.mensaje_fin_copia << " Recibidos=" << archivo_llegando.bytes_recibidos;

				BIEN(archivo_llegando.mensaje_fin_copia.str());
			} catch (const string& e) {
				MAL(e);
			}
		}

		void cmd_borrar(const vector<string>& parametros) {
			try {
				if (parametros.size() != 2) throw string("Error en parametros.");

				std::string archivo(parametros.at(1));

#if defined __GURUWIN32__
				GuruVariable<GuruSocketArchivosServidor> var(this, &GuruSocketArchivosServidor::sacarRutaShare, '{', '}');

				archivo = var.reemplazar(archivo);
#endif

				if (!GuruDirectorio::existe(archivo)) throw(string("El Archivo no Existe."));

				if (GuruDirectorio::esDirectorio(archivo)) throw(string("NO se puede borrar un directorio."));

				if (!GuruDirectorio::esArchivo(archivo)) throw(string("NO es un archivo."));

				if (remove(archivo.c_str()) != 0) throw string(strerror(errno));

				BIEN();
			} catch (string& e) {
				MAL(e);
			}
		}

		void cmd_copiar(const vector<string>& parametros) {
// http://stackoverflow.com/questions/10195343/copy-a-file-in-a-sane-safe-and-efficient-way

/*
// COPY-ALGORITHM-C++-WAY
			clock_t start, end;
			start = clock();

			ifstream source("from.ogv", ios::binary);
			ofstream dest("to.ogv", ios::binary);

			istreambuf_iterator<char> begin_source(source);
			istreambuf_iterator<char> end_source;
			ostreambuf_iterator<char> begin_dest(dest);
			copy(begin_source, end_source, begin_dest);

			source.close();
			dest.close();

			end = clock();

			cout << "CLOCKS_PER_SEC " << CLOCKS_PER_SEC << "\n";
			cout << "CPU-TIME START " << start << "\n";
			cout << "CPU-TIME END " << end << "\n";
			cout << "CPU-TIME END - START " << end - start << "\n";
			cout << "TIME(SEC) " << static_cast<double>(end - start) / CLOCKS_PER_SEC << "\n";
*/
/*
// KISS-C++-Streambuffer-WAY
			clock_t start, end;
			start = clock();

			ifstream source("from.ogv", ios::binary);
			ofstream dest("to.ogv", ios::binary);

			dest << source.rdbuf();

			source.close();
			dest.close();

			end = clock();

			cout << "CLOCKS_PER_SEC " << CLOCKS_PER_SEC << "\n";
			cout << "CPU-TIME START " << start << "\n";
			cout << "CPU-TIME END " << end << "\n";
			cout << "CPU-TIME END - START " << end - start << "\n";
			cout << "TIME(SEC) " << static_cast<double>(end - start) / CLOCKS_PER_SEC << "\n";
*/
/*
// LINUX-WAY // requires kernel >= 2.6.33
			clock_t start, end;
			start = clock();

			int source = open("from.ogv", O_RDONLY, 0);
			int dest = open("to.ogv", O_WRONLY | O_CREAT, 0644); // O_TRUNC

			// struct required, rationale: function stat() exists also
			struct stat stat_source;
			fstat(source, &stat_source);

			sendfile(dest, source, 0, stat_source.st_size);

			close(source);
			close(dest);

			end = clock();

			cout << "CLOCKS_PER_SEC " << CLOCKS_PER_SEC << "\n";
			cout << "CPU-TIME START " << start << "\n";
			cout << "CPU-TIME END " << end << "\n";
			cout << "CPU-TIME END - START " << end - start << "\n";
			cout << "TIME(SEC) " << static_cast<double>(end - start) / CLOCKS_PER_SEC << "\n";

*/
		}

		bool existe_archivo_a_recibir() {
			struct stat64 buffer;

			if (stat64(archivo_llegando.ruta_completa_directorio_destino.c_str(), &buffer) == 0) {
				if ((archivo_llegando.tamanio == static_cast<uint64_t>(buffer.st_size)) && (archivo_llegando.fecha == buffer.st_mtime)) {
					return true;
				}
			}

			return false;
		}

		bool continuar_archivo_a_recibir() {
			struct stat64 buffer;

			if (stat64(string(archivo_llegando.ruta_completa_directorio_destino + ".ccg").c_str(), &buffer) == 0) {
				if ((archivo_llegando.tamanio > static_cast<uint64_t>(buffer.st_size)) && (archivo_llegando.fecha == buffer.st_mtime)) {
					return true;
				}
			}

			return false;
		}

		void recibir_contenido() {
			ofstream archivo(archivo_llegando.ruta_completa_directorio_temporal.c_str(), ((archivo_llegando.agregar == 0) ? ios::trunc | ios::binary : ios::app | ios::binary));

			if (!archivo) throw GuruError().descripcion();

			BIEN(archivo_llegando.mensaje_inicio_estado.str());

			uint64_t	tamanio_archivo_local = 0;
			uint32_t	bytes_leidos;
			uint64_t	tamanio_buffer = BufferEntrada(recibir.interfaz()->sacar_socket().handle());
			GuruBufferChar	buffer_archivo(static_cast<size_t>(tamanio_buffer + 2));
			sha1_context sha1ctx;

			archivo.seekp(0, ios::beg);
			archivo.seekp(0, ios::end);
			tamanio_archivo_local = archivo.tellp();

			if (archivo_llegando.calcular_sha1) sha1_starts(&sha1ctx);

			string		msg_error;

//			if (archivo_llegando.agregar != 0) archivo_llegando.bytes_recibidos = archivo_llegando.agregar-1;

			GURULOG_TRACE("Time out en Socket: " + GuruStringAlgo::itos(recibir.interfaz()->sacar_socket().timeout()));

			if (archivo_llegando.tamanio != 0)
				do {
					if (static_cast<uint64_t>(archivo_llegando.tamanio - archivo_llegando.bytes_recibidos) < static_cast<uint64_t>(tamanio_buffer))
						tamanio_buffer = static_cast<uint32_t>(archivo_llegando.tamanio - archivo_llegando.bytes_recibidos);

					bytes_leidos = recv(recibir.interfaz()->sacar_socket().handle(), buffer_archivo.buf(), static_cast<int>(tamanio_buffer), MSG_NOSIGNAL);
//					Sleep(120000);

					if (bytes_leidos == SOCKET_ERROR || bytes_leidos == 0) {
						int a = WSAGetLastError();
						// en visual studio 2005 no me sale el mensaje de error.
						if (a == 0 && bytes_leidos == 0) msg_error = "Connection reset by peer";
						else msg_error = GuruError().descripcion(a);

						GURULOG_TRACE("Error en Socket: " + msg_error);

						recibir.interfaz()->sacar_socket().cerrar();
						break;
					}

					archivo.write(buffer_archivo.buf(), bytes_leidos);

					if (archivo.fail()) {
						msg_error = GuruError().descripcion();
						break;
					}

					archivo_llegando.bytes_recibidos += bytes_leidos;

					if (archivo_llegando.calcular_sha1) sha1_update(&sha1ctx, (uint8_t*)buffer_archivo.buf(), bytes_leidos);

					Notificar(InformacionArchivoRecibir::recibiendo);

				} while ((archivo_llegando.bytes_recibidos + tamanio_archivo_local) < archivo_llegando.tamanio);

				archivo.close();

				//////////////////////////////////////////////////////////////////////
				// Cambio la fecha del archivo.
				//////////////////////////////////////////////////////////////////////
				struct utimbuf times;

				times.actime = archivo_llegando.fecha;
				times.modtime = archivo_llegando.fecha;

				utime(archivo_llegando.ruta_completa_directorio_temporal.c_str(), &times);

				//////////////////////////////////////////////////////////////////////
				// Me fijo si termino con algun error
				//////////////////////////////////////////////////////////////////////
				if (!msg_error.empty())  throw msg_error;

				//////////////////////////////////////////////////////////////////////
				// Me fijo si llego el archivo completo
				//////////////////////////////////////////////////////////////////////
				if ((archivo_llegando.bytes_recibidos + tamanio_archivo_local) != archivo_llegando.tamanio)
					throw string("El archivo no ha llegado en forma completa.");

				//////////////////////////////////////////////////////////////////////
				// Genero el SHA1.
				//////////////////////////////////////////////////////////////////////
				if (archivo_llegando.calcular_sha1) {
					uint8_t checksum[42]; // en realidad son 40 bytes
					// sha1_final(&sha1ctx);
					sha1_finish(&sha1ctx, checksum);

					archivo_llegando.sha1_llegando = GuruSHA1::checksum_a_string(checksum);
				}

				//////////////////////////////////////////////////////////////////////
				// Verifico si es necesario los dos sha1, el remoto y el local.
				//////////////////////////////////////////////////////////////////////
				if (archivo_llegando.calcular_sha1 && !archivo_llegando.sha1.empty())
					if (archivo_llegando.sha1_llegando != archivo_llegando.sha1) throw string("La verificacion SHA1 no coincide (" + archivo_llegando.sha1_llegando + " <> " + archivo_llegando.sha1 + ")");

				//////////////////////////////////////////////////////////////////////
				// Muevo el archivo al lugar definitivo.
				//////////////////////////////////////////////////////////////////////
				if (archivo_llegando.mover && (archivo_llegando.ruta_completa_directorio_destino != archivo_llegando.ruta_completa_directorio_temporal)) {
#if defined __GURUWIN32__
					if (!GuruDirectorio::moverArchivo(archivo_llegando.ruta_completa_directorio_temporal, archivo_llegando.ruta_completa_directorio_destino))
						throw GuruError().descripcion();
#elif defined __GURULINUX__
					struct stat64 buffer;
					if (stat64(archivo_llegando.ruta_completa_directorio_destino.c_str(), &buffer) == 0) { // el archivo existe
						int resultado_borrado = remove(archivo_llegando.ruta_completa_directorio_destino.c_str());
						if (resultado_borrado) throw string(strerror(errno));
					} else {
					}

					int res_movida = rename(archivo_llegando.ruta_completa_directorio_temporal.c_str(), archivo_llegando.ruta_completa_directorio_destino.c_str());
					if (res_movida) {
						res_movida = errno;
						remove(archivo_llegando.ruta_completa_directorio_temporal.c_str());
						throw string(strerror(res_movida));
					}
#endif
				}

				// y esto?
				archivo_llegando.ruta_completa_directorio_destino = parametrosServidor.directorioDestino + archivo_llegando.nombre;

				//////////////////////////////////////////////////////////////////////
				// Ejecuto el callback para el fin de la copia exitosa.
				//////////////////////////////////////////////////////////////////////
			//		if (parametros.notificacion_copia)  (*parametros.notificacion_copia) (archivo_llegando, mensaje_fin_copia.str(), (extra_parametro_copia_exitosa) ? extra_parametro_copia_exitosa : this);

				Notificar(InformacionArchivoRecibir::recibido);
		}

		InformacionArchivoRecibir		archivo_llegando;
#endif

#ifndef GURUSOCKETARCHIVOS_ENVIAR_NO
		enum	codigos_enviar { bien = 0, no_existe = 1, directorio = 2, error_abriendo = 3, error_leyendo = 4, error_enviando = 5, error_info = 6 };

		class GuruSocketArchivoServidorExcepcionEnviar : public exception {
		public:
			GuruSocketArchivoServidorExcepcionEnviar(codigos_enviar codigo, const string& msg) :_codigo(codigo), msg(msg) {};
			virtual ~GuruSocketArchivoServidorExcepcionEnviar() throw() {};

			virtual const char* what() const throw() { return msg.c_str(); };
			virtual const codigos_enviar codigo() const throw() { return _codigo; };
		private:
			string			msg;
			codigos_enviar	_codigo;
		};
		//
		// Codigos de error:
		// 1 = Archivo Inexistente
		// 2 = Es un Directorio
		// 3 = Error abriendo el archivo
		// 4 = Error obteniendo info del archivo
		//
		void cmd_enviar(const vector<string>& parametros) {
			try {
				GuruEtiquetaLeerEx	tag(parametros);

				archivo_enviando.limpiar();

				archivo_enviando.nombre = tag["archivo"];

				if (!GuruDirectorio::existe(archivo_enviando.nombre)) throw GuruSocketArchivoServidorExcepcionEnviar(no_existe, string("Archivo NO existe"));

				if (GuruDirectorio::esDirectorio(archivo_enviando.nombre)) throw GuruSocketArchivoServidorExcepcionEnviar(directorio, string("Es un directorio"));

				cargar_informacion_archivo();

				string tmp;

				if (!(tmp = tag["desde"]).empty())
					archivo_enviando.desde = _atoi64(tmp.c_str());

				if (!(tmp = tag["velocidad"]).empty())
					archivo_enviando.velocidad = atol(tmp.c_str());

				std::string respuesta;

				respuesta = "tamanio=" + GuruStringAlgo::itos(archivo_enviando.tamanio);
				respuesta += " fecha=" + GuruStringAlgo::comillas(GuruAlgo::fechaHoraRFC822(archivo_enviando.fecha));

				Notificar(InformacionArchivoEnviar::enviar);

				BIEN(respuesta);

				enviar_contenido();

				respuesta.clear();

				if (!archivo_enviando.sha1.empty())
					respuesta += "SHA1=" + archivo_enviando.sha1 + " ";

				respuesta += "Codigo=0";

				BIEN(respuesta);
			} catch (const GuruSocketArchivoServidorExcepcionEnviar& e) {
				ostringstream oss;
				oss << "Codigo=" << e.codigo() << " Msg=" << GuruStringAlgo::comillas(e.what());
				MAL(oss.str());
			}
		}

		void cmd_comparar(const vector<string>& parametros) {
			try {
				if (parametros.size() <= 4) throw string("Parametros Incorrectos");

				GuruEtiquetaLeer	tag(parametros);

				string tmp(tag["archivo"]);

				if (tmp.empty()) throw string("Falta la etiqueta archivo=");

				struct stat64 buffer;
				if (stat64(tmp.c_str(), &buffer) != 0) throw string("Archivo Inexistente.");

				if (!(tmp = tag["tamanio"]).empty())
					if (_atoi64(tmp.c_str()) != buffer.st_size) throw string("Archivo Inexistente.");

				if (!(tmp = tag["fecha"]).empty())
					if (buffer.st_mtime != GuruAlgo::RFC822FechaHora(tmp)) throw string("Archivo Inexistente.");

				if ((tmp = tag["sha1"]).compare("1") == 0) {
					tmp = "SHA1=" + GuruSHA1::archivo(tag["archivo"]);
				} else tmp.clear();

				BIEN(tmp);
			} catch (string e) {
				MAL(e);
			}
		}

		void cmd_existe(const vector<string>& parametros) {
			try {
				if (parametros.size() != 2) throw string("Parametros Incorrectos.");

				if (!GuruDirectorio::existe(parametros.at(1))) throw(string("Archivo Inexistente."));

				BIEN();
			} catch (string& e) {
				MAL(e);
			}
		}

		// informacion c:\temp\nada.fbp
		void cmd_informacion(const vector<string>& parametros) {
			try {
				if (parametros.size() != 2) throw string("Parametros Incorrectos");

				struct stat64 buffer;
				if (stat64(parametros.at(1).c_str(), &buffer) != 0) throw string("Archivo Inexistente.");

				ostringstream oss;

				oss << "fecha=\"" << GuruAlgo::fechaHoraRFC822(buffer.st_mtime) << "\" ";
				oss << "tamanio=" << buffer.st_size;

				BIEN(oss.str());
			} catch (string e) {
				MAL(e);
			}
		}

		// sha1 c:\temp\nada.fbp
		void cmd_sha1(const vector<string>& parametros) {
			try {
				if (parametros.size() != 2) throw string("Parametros Incorrectos.");

				if (!GuruDirectorio::existe(parametros.at(1))) throw(string("Archivo Inexistente."));

				BIEN(GuruSHA1::archivo(parametros.at(1)));
			} catch (string& e) {
				MAL(e);
			}
		}

		// dump c:\temp\nada.fbp
		void cmd_dump(const vector<string>& parametros) {
			try {
				if (parametros.size() != 2) throw string("Parametros Incorrectos.");

				if (!GuruDirectorio::existe(parametros.at(1))) throw(string("Archivo Inexistente."));

				GuruDirectorio::GuruArchivo a(GuruDirectorio::propiedades(parametros.at(1)));

				uint64_t tamanio_archivo = a.tamanio();

				BIEN(GuruStringAlgo::itos(tamanio_archivo));

				ifstream archivo(parametros.at(1).c_str(), ios::in | ios::binary);

				if (!archivo) throw GuruSocketArchivoServidorExcepcionEnviar(error_abriendo, GuruError().descripcion(errno));

				uint32_t	tamanio_buffer = BufferSalida(recibir.interfaz()->sacar_socket().handle());

				GuruBufferChar	buffer_archivo(tamanio_buffer + 2);

				uint64_t	cont_enviados = 0, enviados = 0;

				do {
					archivo.read(buffer_archivo.buf(), tamanio_buffer);

					if (archivo.bad()) throw GuruSocketArchivoServidorExcepcionEnviar(error_leyendo, GuruError().descripcion(errno));

					enviados = send(recibir.interfaz()->sacar_socket().handle(), buffer_archivo.const_buf(), static_cast<int>(archivo.gcount()), 0);

					if (enviados == SOCKET_ERROR) {
						int a  = LastError();
						recibir.interfaz()->sacar_socket().cerrar();
						throw GuruSocketArchivoServidorExcepcionEnviar(error_enviando, GuruError().descripcion(a));
					}

					cont_enviados += enviados;

				} while (cont_enviados < tamanio_archivo);

				archivo.close();
			} catch (string& e) {
				MAL(e);
			}
		}

#ifdef __GURUWIN32__
		std::string sacarRutaShare(const std::string& variable) {
			PSHARE_INFO_502	BufPtr = { 0 };
			// sacado de aqui:
			// http://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte
			int size_needed = MultiByteToWideChar(CP_UTF8, 0, variable.c_str(), -1, NULL, 0);
			std::wstring wstrTo(size_needed, 0);
			MultiByteToWideChar(CP_UTF8, 0, variable.c_str(), -1, &wstrTo[0], size_needed);

			NET_API_STATUS salida;
			if ((salida = NetShareGetInfo(NULL, &wstrTo[0], 502, (LPBYTE *)&BufPtr)) != ERROR_SUCCESS)
				throw GuruError().descripcion(salida);

			size_needed = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)BufPtr->shi502_path, -1, NULL, 0, NULL, NULL);
			std::string strTo(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)BufPtr->shi502_path, -1, &strTo[0], size_needed, NULL, NULL);

			if ((salida = NetApiBufferFree(BufPtr)) != ERROR_SUCCESS)
				throw GuruError().descripcion(salida);

			return strTo;
		}

		// share SYSVOL
		void cmd_share(const vector<string>& parametros) {
			try {
				if (parametros.size() != 2) throw string("Parametros Incorrectos.");

				BIEN(sacarRutaShare(parametros.at(1)));
			} catch (string& e) {
				MAL(e);
			}
		}
#endif

		void	enviar_contenido() {

			ifstream archivo(archivo_enviando.nombre.c_str(), ios::in | ios::binary);

			if (!archivo) throw GuruSocketArchivoServidorExcepcionEnviar(error_abriendo, GuruError().descripcion(errno));

			uint32_t	tamanio_buffer = BufferSalida(recibir.interfaz()->sacar_socket().handle());

			// calculo el tamanio optimo del buffer de salida para cuando se limita la velocidad
			if (archivo_enviando.velocidad) {
				uint32_t tmp_vel = archivo_enviando.velocidad;
				while (tmp_vel > tamanio_buffer) tmp_vel = tmp_vel / 2;

				tamanio_buffer = tmp_vel;
			}

			GuruBufferChar	buffer_archivo(tamanio_buffer + 2);
			sha1_context	sha1ctx;

			uint64_t	cont_enviados = 0, enviados = 0;
			uint64_t	inicio_envio = 0, fin_envio = 0, total_bytes_enviados_seg = 0;
			uint32_t	total_tiempo = 0;

			if (archivo_enviando.desde) {
				archivo.seekg(static_cast<std::streamoff>(archivo_enviando.desde));
				cont_enviados = archivo.tellg();
			}

			if (archivo_enviando.calcular_sha1) sha1_starts(&sha1ctx);

			if (archivo_enviando.velocidad)
				inicio_envio = GetTickCount();

			do {
				archivo.read(buffer_archivo.buf(), tamanio_buffer);

				if (archivo.bad()) throw GuruSocketArchivoServidorExcepcionEnviar(error_leyendo, GuruError().descripcion(errno));

				enviados = send(recibir.interfaz()->sacar_socket().handle(), buffer_archivo.const_buf(), static_cast<int>(archivo.gcount()), 0);

//				GURULOG_TRACE("time out en Socket: " + GuruStringAlgo::itos(recibir.interfaz()->sacar_socket().timeout()));
				
//				Sleep(120000);

				if (enviados == SOCKET_ERROR) {
					int a = LastError();
					recibir.interfaz()->sacar_socket().cerrar();
					throw GuruSocketArchivoServidorExcepcionEnviar(error_enviando, GuruError().descripcion(a));
				}

				cont_enviados += enviados;

				if (archivo_enviando.velocidad) {
					total_bytes_enviados_seg += enviados;

					if (total_bytes_enviados_seg >= archivo_enviando.velocidad) {
						fin_envio = GetTickCount();
						total_tiempo = static_cast<uint32_t>(fin_envio - inicio_envio);
						if (total_tiempo <= 1000) {
							archivo_enviando.ms_espera = total_tiempo;
							// cout << "Espera en ms:" << total_tiempo << endl;
							if (static_cast<uint32_t>(1000) != total_tiempo) {
								Sleep((static_cast<uint32_t>(1000) - total_tiempo));
							} else {
								Sleep(1000);
							}
						}
						total_bytes_enviados_seg = 0;
						inicio_envio = GetTickCount();
					}
				}

				if (archivo_enviando.calcular_sha1) sha1_update(&sha1ctx, (uint8_t*)buffer_archivo.buf(), archivo.gcount());

				Notificar(InformacionArchivoEnviar::enviando);

			} while (cont_enviados < archivo_enviando.tamanio);

			archivo.close();

			if (archivo_enviando.calcular_sha1) {
				uint8_t checksum[42]; // en realidad son 40 bytes
				sha1_finish(&sha1ctx, checksum);
				archivo_enviando.sha1 = GuruSHA1::checksum_a_string(checksum);
			}

			Notificar(InformacionArchivoEnviar::enviado);
		}

		void	cargar_informacion_archivo() {
			struct stat64 buffer;

			if (stat64(archivo_enviando.nombre.c_str(), &buffer) == 0) {
				archivo_enviando.tamanio = buffer.st_size;
				archivo_enviando.fecha = buffer.st_mtime;
			} else
				throw GuruSocketArchivoServidorExcepcionEnviar(error_info, GuruError().descripcion(errno));
		}


		static void	Notificacion(MensajesEnviar* notificaciones) {
			ConfiguracionServidorArchivos::instancia()->Notificacion(notificaciones);
		};

		void Notificar(InformacionArchivoEnviar::estado_enviar estado) {
			if (parametrosServidor.MensajesEnviarArchivos) {
				archivo_enviando.estado = estado;
				parametrosServidor.MensajesEnviarArchivos->Notificar(archivo_enviando);
			}
		};

		enum	opciones_enviar {
			compactado = 0x01,		// envia el contenido compatado por zlib.
			limitar_velocidad = 0x02,	// limita la velocidad de envio.
			no_sha1 = 0x04, 			// no calcula el sha1 mientras se envia el contenido.
			enviar_sha1 = 0x08,		// envia sha1 del archivo antes de transmitir el archivo.
		};

		uint32_t			GuruSocketArchivosEnviar_Opciones;
		InformacionArchivoEnviar	archivo_enviando;
#endif

#ifndef GURUSOCKETARCHIVOSNAV_NO
		typedef enum { nombre = 0x01, bytes = 0x02, fecha = 0x04 } columna;
		typedef enum { sub = 0x01, nosub = 0x02, epoc = 0x04, noepoc = 0x08, ruta = 0x10, noruta = 0x20 } opcion;

		ostringstream& armar_registro(ostringstream& oss, const string& directorio, GuruDirectorio::GuruArchivo& info, uint16_t opt_columnas, uint16_t opt_opciones) {

			oss.str("");

			if (opt_columnas & static_cast<uint16_t>(GuruSocketArchivosServidor::nombre)) {
				oss << " nombre=\"";
				if (opt_opciones & static_cast<uint16_t>(GuruSocketArchivosServidor::noruta)) {
					if (opt_opciones & static_cast<uint16_t>(GuruSocketArchivosServidor::sub)) oss << directorio;
					oss << info.nombre() << "\"";
				} else if (opt_opciones & static_cast<uint16_t>(GuruSocketArchivosServidor::ruta))
					oss << directorio << info.nombre() << "\"";
			}

			if (opt_columnas & static_cast<uint16_t>(GuruSocketArchivosServidor::fecha)) {
				oss << " fecha=\"";
				if (opt_opciones & static_cast<uint16_t>(GuruSocketArchivosServidor::epoc))
					oss << info.fecha() << "\"";
				else if (opt_opciones & static_cast<uint16_t>(GuruSocketArchivosServidor::noepoc))
					oss << GuruAlgo::fechaHoraRFC822(info.fecha()) << "\"";
			}

			if (opt_columnas & static_cast<uint16_t>(GuruSocketArchivosServidor::bytes)) {
				oss << " bytes=";
				if (info.EsArchivo())
					oss << info.tamanio();
				else if (info.EsDirectorio())
					oss << "-1";
				else oss << "0";
			}

			return oss;
		}

		// request
		// dir directorio=/opt/cosas opciones=nosub|epoc|noruta columnas=nombre|bytes|fecha filtro=*.*
		// opciones=
		//		sub|nosub - epoc|noepoc - ruta|noruta
		// columnas=
		//		nombre - bytes - fecha
		// filtro=
		//		expresion regular
		//////////////////////////////////////////////////////////////////////
		// response
		// nombre=aaa bytes=bbb fecha=ccc
		void dir(const vector<string>& parametros) {
			GuruEtiquetaLeerEx et(parametros);

			try {
				string directorio = et["directorio"];
				string opciones = et["opciones"];
				string columnas = et["columnas"];
				string filtro = et["filtro"];

				if (directorio.empty()) throw string("Falta etiqueta \"directorio=\"");

				directorio = GuruDirectorio::comoDirectorio(directorio);

				uint16_t	opt_columnas = static_cast<uint16_t>(GuruSocketArchivosServidor::nombre | GuruSocketArchivosServidor::fecha | GuruSocketArchivosServidor::bytes);
				uint16_t	opt_opciones = static_cast<uint16_t>(GuruSocketArchivosServidor::nosub | GuruSocketArchivosServidor::epoc | GuruSocketArchivosServidor::noruta);

				if (!columnas.empty()) opt_columnas = sacar_columnas(columnas);

				if (!opciones.empty()) opt_opciones = sacar_opciones(opciones);

				if (filtro.empty()) filtro = "*.*";

				ostringstream	oss;
				GuruDirectorio::GuruArchivo		info;

				if (opt_opciones & static_cast<uint16_t>(GuruSocketArchivosServidor::sub)) {
					string ruta;
					GuruDirectorio::iterator_recursivo itfinal;
					for (GuruDirectorio::iterator_recursivo it(directorio, filtro); it != itfinal; ++it) {
						info = *it;
						ruta = (opt_opciones & static_cast<uint16_t>(GuruSocketArchivosServidor::ruta)) ? directorio + it.dir() : it.dir();
						BIEN(armar_registro(oss, ruta, info, opt_columnas, opt_opciones).str());
					}
				} else {
					GuruDirectorio::iterator itfinal;
					for (GuruDirectorio::iterator it(directorio, filtro); it != itfinal; ++it) {
						info = *it;
						BIEN(armar_registro(oss, directorio, info, opt_columnas, opt_opciones).str());
					}
				}

				MAL();
			} catch (const string& e) {
				MAL(e);
			} catch (const std::exception& e) {
				MAL(e.what());
			} catch (...) {
				cerr << "Error !!! ..." << endl;
			}
		}

		// columnas=
		//		nombre - bytes - fecha
		uint16_t sacar_columnas(const string& columnas) {
			GuruToken tk("|");
			vector<string> tokens;
			uint16_t opt = 0;

			if (tk.extraer(columnas, tokens).size() != 0) {
				for (vector<string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
					transform(it->begin(), it->end(), it->begin(), ::tolower);
					if (it->compare("nombre") == 0) opt |= GuruSocketArchivosServidor::nombre;
					else if (it->compare("bytes") == 0) opt |= GuruSocketArchivosServidor::bytes;
					else if (it->compare("fecha") == 0) opt |= GuruSocketArchivosServidor::fecha;
				}
			}

			if (!(opt & static_cast<uint16_t>(GuruSocketArchivosServidor::nombre)))
				opt |= static_cast<uint16_t>(GuruSocketArchivosServidor::nombre);

			//		if (!(opt & static_cast<uint16_t>(GuruSocketArchivosServidor::bytes)))
			//			opt |= static_cast<uint16_t>(GuruSocketArchivosServidor::bytes);

			return opt;
		}

		// opciones=
		//		sub|nosub - epoc|noepoc - ruta|noruta
		uint16_t sacar_opciones(const string& opciones) {
			GuruToken tk("|");
			vector<string> tokens;
			uint16_t opt = 0;

			if (tk.extraer(opciones, tokens).size() != 0) {
				for (vector<string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
					transform(it->begin(), it->end(), it->begin(), ::tolower);
					if (it->compare("sub") == 0) opt |= GuruSocketArchivosServidor::sub;
					else if (it->compare("nosub") == 0) opt |= GuruSocketArchivosServidor::nosub;
					else if (it->compare("epoc") == 0) opt |= GuruSocketArchivosServidor::epoc;
					else if (it->compare("noepoc") == 0) opt |= GuruSocketArchivosServidor::noepoc;
					else if (it->compare("ruta") == 0) opt |= GuruSocketArchivosServidor::ruta;
					else if (it->compare("noruta") == 0) opt |= GuruSocketArchivosServidor::noruta;
				}
			}

			if (!(opt & static_cast<uint16_t>(GuruSocketArchivosServidor::epoc)) && !(opt & static_cast<uint16_t>(GuruSocketArchivosServidor::noepoc)))
				opt |= static_cast<uint16_t>(GuruSocketArchivosServidor::epoc);

			if (!(opt & static_cast<uint16_t>(GuruSocketArchivosServidor::sub)) && !(opt & static_cast<uint16_t>(GuruSocketArchivosServidor::nosub)))
				opt |= static_cast<uint16_t>(GuruSocketArchivosServidor::sub);

			if (!(opt & static_cast<uint16_t>(GuruSocketArchivosServidor::ruta)) && !(opt & static_cast<uint16_t>(GuruSocketArchivosServidor::noruta)))
				opt |= static_cast<uint16_t>(GuruSocketArchivosServidor::noruta);

			return opt;
		}


		//	uint16_t	opt_columnas;
		//	uint16_t	opt_opciones;
#endif
	};
}
// typedef basic_GuruSocketArchivosServidor<GuruSocketLeer, GuruSocketEscribir> GuruSocketArchivosServidor;

#endif
