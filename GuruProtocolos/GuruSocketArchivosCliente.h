#ifndef __GURUSOCKETARCHIVOSCLIENTE_H__
#define __GURUSOCKETARCHIVOSCLIENTE_H__

#include <GuruCompat.h>
#include <GuruEtiquetaLeer.hpp>
#include <GuruSHA1.hpp>
#include <GuruError.hpp>
#include <GuruAlgo.hpp>
#include <GuruSocket/GuruProtocoloCliente.hpp>
#include <GuruBuffer.hpp>
#include <GuruDirectorio.hpp>
#include <GuruLog.hpp>

#include <polarssl/sha1.h>

#include <stdlib.h>
#include <string>
#include <sstream>
#include <fstream>
#include <istream>
#include <iostream>
#include <exception>

// para _stat64
#include <sys/types.h>
#include <sys/stat.h>

#if defined __GURULINUX__
#include <sys/types.h>
#include <utime.h>
#include <unistd.h>
#include <GuruCompat/gettickcount.h>
#elif defined __GURUWIN32__
#include <stdlib.h>
#include <sys\utime.h>
#endif 

#ifndef GURUSOCKETARCHIVOSTRAER_NO
class InformacionArchivoTraer
{
public:
	InformacionArchivoTraer() {limpiar();};

	enum estado_traer {inconsistente=0,traer,trayendo,traido};

	string		nombre;
	uint64_t	tamanio;
	time_t		fecha;
	string		sha1;
	bool		reemplazar;
	uint64_t	agregar;
	uint64_t	bytes_recibidos;
	bool		calcular_sha1;

	string		archivo_traer;
	string		ruta_archivo_local;

	uint32_t	velocidad_transmision;	// Cuanto es que se le dijo al servidor que tiene que transmitir
	uint32_t	bytes_seg;				// Cuantos bytes recibio en el ultimo segundo
	uint32_t	promedio_seg;			// Cuanto es el promedio desde que se empezo la transmision

	estado_traer	estado;

	void limpiar() {
		tamanio=0;
		fecha=0;
		agregar=0;
		reemplazar=0;
		bytes_recibidos=0;
		velocidad_transmision=0;
		bytes_seg=0;
		promedio_seg=0;
		calcular_sha1=true;
		estado=inconsistente;
	};
};

class MensajesTraer
{
public:
	MensajesTraer() {};

	virtual bool Notificar(const InformacionArchivoTraer& info){return true;};
};

template <class Clase>
class MensajesTraerArchivo: public MensajesTraer
{
	friend class GuruSocketArchivosTraer;
	typedef bool (Clase::*Metodo)(const InformacionArchivoTraer& info);

public:
	MensajesTraerArchivo(Clase* laclase, Metodo elmetodo) {
		instancia = laclase;
		metodo    = elmetodo;
	};

	bool Notificar(const InformacionArchivoTraer& info){
		return (instancia->*metodo)(info);
	};

private:
	Clase*  instancia;
	Metodo  metodo;
};

#endif

#ifndef GURUSOCKETARCHIVOSENVIAR_NO
class InformacionArchivoEnviar
{
public:
	InformacionArchivoEnviar() {limpiar();};

	enum estado_enviar {inconsistente=0,enviar,enviando,enviado};

	string		nombre;
	uint64_t	tamanio;
	time_t		fecha;
	string		sha1;
	uint64_t	agregar;
	bool		reemplazar;
	string		directorio_destino;
	string		ruta_archivo_local;
	uint64_t	bytes_enviados;
	uint32_t	velocidad_transmision;			// 
	uint64_t	tiempo_transmision;				// Cantidad de Milisegundos que tardo la transmision
	bool		calcular_sha1;

	estado_enviar	estado;
	void limpiar() {
		tamanio=0;
		fecha=0;
		agregar=0;
		reemplazar=0;
		bytes_enviados=0;
		velocidad_transmision=0;
		tiempo_transmision=0;
		calcular_sha1=true;
		estado=inconsistente;

		nombre.clear();
		sha1.clear();
		directorio_destino.clear();
		ruta_archivo_local.clear();
	};
};

class MensajesEnvio
{
public:
	MensajesEnvio() {};

	virtual bool Notificar(const InformacionArchivoEnviar& info){return true;};
};

template <class Clase>
class MensajesEnvioArchivo: public MensajesEnvio
{
	friend class GuruSocketArchivosEnviar;
	typedef bool (Clase::*Metodo)(const InformacionArchivoEnviar& info);

public:
	MensajesEnvioArchivo(Clase* laclase, Metodo elmetodo) {
		instancia = laclase;
		metodo    = elmetodo;
	};

	bool Notificar(const InformacionArchivoEnviar& info){
		return (instancia->*metodo)(info);
	};

private:
	Clase*  instancia;
	Metodo  metodo;
};

#endif

class archivo_remoto {
public:
	archivo_remoto () {};

	string nombre() {return etiquetas["nombre"];};
	string directorio() {return etiquetas["directorio"];};
	string bytes(){return etiquetas["bytes"];};
	string ruta_completa(){return etiquetas["ruta_completa"];};
	string respuesta() {return etiquetas.sacar_registro();}

	void cargar_archivo(const string& registro) { etiquetas.cargar(registro);};

protected:
	GuruEtiquetaLeer etiquetas;
};

class GuruSocketArchivosCliente
{
public:
//	GuruSocketArchivosCliente(const string& servidor="", int puerto=0, string protocolo=""):servidor(servidor),puerto(puerto),nombreProtocolo(protocolo) {
	GuruSocketArchivosCliente() {
		#ifndef GURUSOCKETARCHIVOSTRAER_NO
			GuruSocketArchivosTraer_Opciones=0;
			MsjsTraer=NULL;
		#endif
		#ifndef GURUSOCKETARCHIVOSENVIAR_NO
			GuruSocketArchivosEnviar_Opciones=0;
			MsjsEnvio=NULL;
		#endif

		cancelar_operacion = false;
		timeout(0);
	};
	virtual ~GuruSocketArchivosCliente(void) { desconectar(); };

	class GuruSocketArchivosClienteException : public exception {
	public:
		GuruSocketArchivosClienteException(const string& msg):_msg(msg) {};
		virtual ~GuruSocketArchivosClienteException() throw() {};

		virtual const char* what() const throw() { return _msg.c_str(); };
	private:
		std::string _msg;
	};

	class transferencia {
	public:
		transferencia():_opciones(0),_velocidad(0) {}
		virtual ~transferencia() {}

		inline void nombreLocal(const string& a) { _nombreLocal = a; }
		inline std::string& nombreLocal() { return _nombreLocal; }

		inline void nombreRemoto(const string& a) { _nombreRemoto = a; }
		std::string nombreRemoto() {
			if (_nombreRemoto.empty()) {
				GuruDirectorio::GuruSplitPath a(rutaCompletaLocal());

				return a.nombre_ext();
			}

			return _nombreRemoto; 
		}

		inline void directorioLocal(const string& a) { _directorioLocal = a; }
		inline std::string& directorioLocal() { return _directorioLocal; }

		inline void directorioRemoto(const string& a) { _directorioRemoto = a; }
		inline std::string& directorioRemoto() { return _directorioRemoto; }

		inline void opciones(uint32_t a) { _opciones = a; }
		inline uint32_t opciones() { return _opciones; };

		inline void velocidad(uint32_t a) { _velocidad = a; }
		inline uint32_t velocidad() { return _velocidad; }

		inline std::string rutaCompletaRemota() { return GuruDirectorio::comoDirectorio(directorioRemoto()) + nombreRemoto(); }
		inline std::string rutaCompletaLocal() { return GuruDirectorio::comoDirectorio(directorioLocal()) + nombreLocal(); }
	private:
		std::string _nombreLocal;
		std::string _directorioLocal;
		std::string _nombreRemoto;
		std::string _directorioRemoto;
		uint32_t	_opciones;
		uint32_t	_velocidad;
	};

	void cancelar(bool c=true) {
		cancelar_operacion = c;
	}

	void conectar(const std::string& servidor, int puerto, std::string protocolo = "") {

		try{
			this->servidor = servidor;
			this->puerto = puerto;
			this->nombreProtocolo = protocolo;

			conexion = FabricaSocketConectar(servidor, puerto);

			protocoloCliente.cargar_handle(conexion);

			if (!protocolo.empty()) {
				ProtocoloRespuesta res(protocoloCliente.cmd_linea(protocolo));

				if (res.tipo() != GuruProtocolo::codigo_bien()) {
					throw GuruSocketArchivosClienteException(res.texto());
				}
			}

		} catch (GuruSocketException& e) {
			msg_salida = e.what();
			throw GuruSocketArchivosClienteException(e.what());
		}

	};

	void desconectar() {

		try {
			if (!this->nombreProtocolo.empty()) {
				protocoloCliente.cmd_linea("salir");
			}

			protocoloCliente.cmd_linea("salir");

			conexion.cerrar();
		} catch (GuruSocketException&) {
		}
	}

	void timeout(uint32_t seg) { this->conexion.timeout(seg); }
	uint32_t timeout() { return this->conexion.timeout(); }

	void directorio(const string& dire);

	bool borrar(const string& archivo) {
		try {
			string comando;

			comando += "borrar " + archivo;

			ProtocoloRespuesta respuesta(protocoloCliente.cmd_linea(comando));

			if (respuesta.tipo() != GuruProtocolo::codigo_bien()) throw respuesta.texto();

			return true;
		} catch (const string& e) {
			msg_salida = e;
		} catch (GuruSocketException& e) {
			msg_salida = e.what();
		}

		return false;
	}

	std::string sha1(const string& archivo) {
		try {
			string comando;

			comando += "sha1 " + archivo;

			ProtocoloRespuesta respuesta(protocoloCliente.cmd_linea(comando));

			if (respuesta.tipo() != GuruProtocolo::codigo_bien()) throw respuesta.texto();

			return respuesta.texto();
		} catch (const string& e) {
			msg_salida = e;
		} catch (GuruSocketException& e) {
			msg_salida = e.what();
		}

		return string();
	}

	std::string share(const string& nombre) {
		try {
			string comando;

			comando += "share " + nombre;

			ProtocoloRespuesta respuesta(protocoloCliente.cmd_linea(comando));

			if (respuesta.tipo() != GuruProtocolo::codigo_bien()) throw respuesta.texto();

			return respuesta.texto();
		} catch (const string& e) {
			msg_salida = e;
		} catch (GuruSocketException& e) {
			msg_salida = e.what();
		}

		return string();
	}

#ifndef GURUSOCKETARCHIVOSTRAER_NO
	enum	opciones_traer {
		traer_reemplazar = 0x01,		// reemplaza el archivo si existe en el destino.
		traer_compactado = 0x02,		// envia el contenido compatado por zlib.
		traer_limitar_velocidad = 0x04,	// limita la velocidad de envio.
		traer_no_sha1 = 0x08,			// no calcula el sha1 mientras se recibe el contenido.
		traer_no_mover = 0x10,			// no mueve el archivo del temporal al destino.
		traer_no_terminar = 0x20,		// que no termine la trasmision.
		traer_sha1 = 0x40				// calcula el sha1 mientas de recibe el contenido.
	};

	enum respuesta_traer {
		transferencia_correcta_traer=1,
		archivo_existente,
		archivo_remoto_inexistente,
		error_traer=10,
		error_traer_conectando,
		error_conexion_traer,
		error_protocolo_traer,
		error_creando_archivo,
		error_sha1_traer
	};

	class GuruSocketArchivosExceptionTraer : public exception {
	public:
		GuruSocketArchivosExceptionTraer(enum GuruSocketArchivosCliente::respuesta_traer codigo, const string& msg) :_codigo(codigo), msg(msg) {};
		virtual ~GuruSocketArchivosExceptionTraer() throw() {};

		virtual const char* what() const throw() { return msg.c_str(); };
		virtual const GuruSocketArchivosCliente::respuesta_traer codigo() const throw() { return _codigo; };

	private:
		GuruSocketArchivosCliente::respuesta_traer	_codigo;
		string										msg;
	};

	enum GuruSocketArchivosCliente::respuesta_traer GuruSocketArchivosCliente::traer_archivo(GuruSocketArchivosCliente::transferencia& dataArchivoTraer);
//	enum GuruSocketArchivosCliente::respuesta_traer traer_archivo (const string& nombre_archivo, const string& directorio_local, uint32_t opciones_traer=0, uint32_t velocidad_limite=0);
	string& msg_traer() { return msg_salida; };

	void Notificacion(MensajesTraer* notificaciones) {
		MsjsTraer=notificaciones;
	};
#endif

#ifndef GURUSOCKETARCHIVOSENVIAR_NO
	enum	opciones_enviar {
				enviar_reemplazar = 0x01,		// reemplaza el archivo si existe en el destino.
				enviar_compactado=0x02,			// envia el contenido compatado por zlib.
				enviar_limitar_velocidad=0x04,	// limita la velocidad de envio.
				enviar_no_sha1=0x08,			// no calcula el sha1 mientras se envia el contenido.
				enviar_no_mover=0x10,			// no mueve el archivo del temporal al destino.
				enviar_no_terminar=0x20,		// que no termine la trasmision.
				enviar_sha1=0x40			// envia sha1 del archivo antes de transmitir el archivo.
	};

	enum	respuesta_enviar {
			transferencia_correcta_enviar = 1,
			existente, 
			no_enviado,
			envio_cancelado,
			error_enviar=10,
			falta_servidor,
			falta_archivo,
			error_archivo,
			error_conexion_enviar,
			error_protocolo_enviar,
			error_sha1_enviar,
			error_desconocido
			
	};

	class GuruSocketArchivosExceptionEnviar : public exception {
	public:
		GuruSocketArchivosExceptionEnviar(enum GuruSocketArchivosCliente::respuesta_enviar codigo, const string& msg):_codigo(codigo), msg(msg) {};
		virtual ~GuruSocketArchivosExceptionEnviar() throw() {};

		virtual const char* what() const throw() { return msg.c_str(); };
		virtual const GuruSocketArchivosCliente::respuesta_enviar codigo() const throw() { return _codigo; };
	private:
		GuruSocketArchivosCliente::respuesta_enviar	_codigo;
		string										msg;
	};

//	GuruSocketArchivosCliente::respuesta_enviar enviar_archivo(const string& nombre_archivo_local, const string& directorio_destino = "", uint32_t opciones_envio = 0, uint32_t velocidad_limite = 0);
	GuruSocketArchivosCliente::respuesta_enviar GuruSocketArchivosCliente::enviar_archivo(GuruSocketArchivosCliente::transferencia& dataArchivoEnviar);
	string& msg_enviar() { return msg_salida; };

	void Notificacion(MensajesEnvio* notificaciones) { MsjsEnvio=notificaciones;};
	
#endif
#ifndef GURUSOCKETARCHIVOSNAV_NO
	class iterator_directorio {
	public:
		iterator_directorio(GuruSocketArchivosCliente& cliente):cliente(cliente),itprot(GuruProtocolo::codigo_mal()) {};
		iterator_directorio(GuruSocketArchivosCliente& cliente, const string& directorio):cliente(cliente),itprot(GuruProtocolo::codigo_mal()) {
			try {
/*
				cliente.conexion=FabricaSocketConectar(cliente.servidor, cliente.puerto);
				ProtocoloRespuesta 	b;

				a.cargar_handle(cliente.conexion);
				if (!cliente.protocolo.empty())
					b = a.cmd_linea(cliente.protocolo);
				else b = a.respuesta();

				if (b.tipo() != GuruProtocolo::codigo_bien()) throw b.texto();
*/
				std::string cmd;

				cmd = "dir";
				cmd += " directorio=" + GuruStringAlgo::comillas(directorio) + "\r\n";

				itprot = a.begin(cmd);
				if (itprot->tipo() != GuruProtocolo::codigo_mal())
					_archivo_remoto.cargar_archivo(itprot->texto());
			}catch (string e) {
				throw e;
			}catch (GuruSocketException& e) {
				throw string(e.what());
			}
		};

		virtual ~iterator_directorio () {};

		void operator++() {
			++itprot;
			_archivo_remoto.cargar_archivo(itprot->texto());
		}

		bool operator!=(const iterator_directorio& it2) const {
			if (itprot->tipo() != it2.itprot->tipo()) return true;
			else return false;
		}

		archivo_remoto* operator->() const {
			return (archivo_remoto*)&_archivo_remoto;
		}

	protected:
		GuruSocketArchivosCliente& 		cliente;
		archivo_remoto					_archivo_remoto;
		GuruProtocoloCliente::iterator	itprot;
		GuruProtocoloCliente 			a;
	};

	iterator_directorio begin(const string& directorio) {return iterator_directorio(*this, directorio);};
	iterator_directorio end() {return iterator_directorio(*this);};
#endif
protected:

#ifndef GURUSOCKETARCHIVOSTRAER_NO
	void Notificar(InformacionArchivoTraer::estado_traer estado){
		if (MsjsTraer) {
			archivo_trayendo.estado = estado;
			MsjsTraer->Notificar(archivo_trayendo);
		}
	};

	bool	existe_archivo (const string& nombre, time_t fecha=0, uint64_t tamanio=0);
	bool	existe_archivo();
	void	traer_contenido ();
	void	crear_nombre_archivo_local(const string& directorio_local);

	uint32_t				GuruSocketArchivosTraer_Opciones;
	InformacionArchivoTraer	archivo_trayendo;
	MensajesTraer*			MsjsTraer;
#endif

#ifndef GURUSOCKETARCHIVOSENVIAR_NO
	void Notificar(InformacionArchivoEnviar::estado_enviar estado){
		if (MsjsEnvio) {
			archivo_enviando.estado = estado;
			MsjsEnvio->Notificar(archivo_enviando);
		}
	};

	void	enviar_contenido();
	bool	cargar_data_archivo(const string& nombre_archivo_local);
	bool	comparar_sha1(const string& sha1, const string& nombre_archivo);

	InformacionArchivoEnviar	archivo_enviando;
	uint32_t					GuruSocketArchivosEnviar_Opciones;
	MensajesEnvio*				MsjsEnvio;
#if defined __GURUWIN32__ && __CONTENIDO_COMPACTADO__
	void	env_contenido_compactado(const string& nombre_archivo_local);
	static	void	hilo_lectura(thread_carga* info);
	static	void	carga_dir_virtual (char *ruta_completa, WIN32_FIND_DATA *data, void *esto);
	HANDLE	evento_entrada_salida;	// evento que indica si tiene que leer o escribir el archivo
	HANDLE	evento_cargado;			// evento que indica que tiene que escribir o leer
	HANDLE	evento_terminar;		// evento que indica que tiene que temrinar el thread
#endif

#endif

	string			servidor;
	int				puerto;
	GuruSocket		conexion;
	string			nombreProtocolo;
	string			msg_salida;
	bool			cancelar_operacion;

	GuruProtocoloCliente	protocoloCliente;
//	ProtocoloRespuesta		respuesta;

};

#endif
