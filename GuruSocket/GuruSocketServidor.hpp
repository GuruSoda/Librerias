#ifndef __GURUSOCKETSERVIDOR_H__
#define __GURUSOCKETSERVIDOR_H__

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
//////////////////////////////////////////////////////////////////////
//	Link: pthread
//////////////////////////////////////////////////////////////////////
	#include <pthread.h>
	#include <semaphore.h>
	#include <signal.h>
#endif

#include "GuruSocket.hpp"
#include "GuruSocketBase.hpp"
#include "GuruSocketLeer.hpp"
#include "GuruSocketEscribir.hpp"

#include <GuruError.hpp>
#include <GuruAcceso.hpp>
#include <GuruPool.hpp>
#include <GuruQueue.hpp>
#include <GuruLog.hpp>

class GuruSocketServidor;

class GuruSocketServidorException: public std::exception
{
public:
	GuruSocketServidorException(const string& texto):mensaje(texto) {};
	GuruSocketServidorException() {	mensaje = GuruError().descripcion();};
	GuruSocketServidorException(int nro_error) { mensaje = GuruError().descripcion(nro_error);};
	virtual ~GuruSocketServidorException() throw() {};

	virtual const char* what() const throw() { return mensaje.c_str();};

private:
	string mensaje;
};

class GuruSocketServidorEjecutandoException: public GuruSocketServidorException
{
public:
	GuruSocketServidorEjecutandoException(const string& texto):GuruSocketServidorException(texto) {};
};

class InfoConexion {
public:
	InfoConexion(){};
	InfoConexion(const InfoConexion& copia){
		_socket = copia._socket;
		_id = copia._id;
		_ptr=copia._ptr;
	};

	virtual ~InfoConexion() {};

	inline uint64_t id(){return _id;};

	inline void socket(GuruSocket& _socket) { this->_socket=_socket;};
	inline GuruSocket& socket() {return _socket;}

	inline void  extra_ptr(void *ptr) {_ptr=ptr;};
	inline void* extra_ptr() {return _ptr;};

private:
	GuruSocket			_socket;
	uint64_t 			_id;
	void*				_ptr;
};

class GuruSocketServicio {
public:
	GuruSocketServicio() {};
	virtual ~GuruSocketServicio() {};

	virtual void main () = 0;

	inline IRecibir& 	lectura() 				{ return recibir; };
	inline void			lectura(IRecibir& a) 	{ recibir = a;};

	inline IEnviar&		escritura()				{ return enviar; };
	inline void			escritura(IEnviar& a) 	{ enviar = a;};

	inline void info(InfoConexion* a) 	{ _info=a;};
	inline InfoConexion* info() 		{ return _info;};

protected:
	inline GuruSocket&		SocketEntrada()	{ return enviar.interfaz()->sacar_socket(); };
	inline GuruSocket&		SocketSalida()	{ return recibir.interfaz()->sacar_socket(); };

	IRecibir		recibir;
	IEnviar			enviar;

	InfoConexion* 	_info;
};

template <class T>
class GuruSocketServicioEx:public GuruSocketServicio {
public:
	GuruSocketServicioEx(const T& data):data(data) {};
	virtual ~GuruSocketServicioEx() {};

private:
	T&	data;
};

class GuruSocketServidor {
	bool		_escuchando;
	void*		_ptr;
	size_t		_aceptadas;
	size_t		_error_listen;
	size_t		_error_accept;
	size_t		_error_servicio;
	time_t		_hora_inicio;

	// configuracion de todas las placas y puertos que escucha el servidor.
	vector<std::pair<uint16_t, string> >	_escuchas;
	vector<GuruSocket>						_sockets;

	GuruQueue_threadsafe<SOCKET>	cola_entrada;

	class itemPool;
	GuruPool_threadsafe<itemPool>	pool;

#ifdef __GURUWIN32__
	HANDLE	hsem;
	HANDLE	handle_thread_conexiones;
	HANDLE	handle_lanza_servicio;
#elif defined __GURULINUX__
	pthread_t	handle_thread_conexiones;
	pthread_t	handle_lanza_servicio;
	sem_t	hsem;
#endif

public:
	struct estadisticas {
		// tamaño del pool.
		size_t	capacidad_pool;
		// tamaño actual del pool.
		size_t	size_pool;
		// cuantos thread estan en estado ejecutando.
		size_t	threads_ejecutando;
		// cuando pedidos hubo desde que comenzo a escuchar el puerto.
		size_t	pedidos;
		// cuantos servicios se estan ejecutando.
		size_t	servicios_ejecutando;
		// cuantos thread, cuando terminen el servicio dejaran de ejecutarse.
		size_t	esperando_terminar;
		// tamaño de la cola de socket a procesar.
		size_t	cola_socket;
		// segundos desde el inicio del servidor.
		size_t	tiempo_ejecucion;
		// fecha hora de inicio del servidor.
		size_t	inicio_ejecucion;
		// cuantos thread estan esperando a ser ejecutados.
		size_t	esperas;
		// cuantos errores hubo en el comando select del socket que esta en escucha.
		size_t	errores_socket_listen;
		// cuantos errores hubo en cada nuevo socket.
		size_t	errores_socket_accept;
		// cuantos errores hubo en los servicios que llama el servidor de sockets.
		size_t	errores_servicio; // no implementado.
	};

public:
	GuruSocketServidor(uint16_t puerto, const string& interfaz = "0.0.0.0"):_ptr(NULL),_escuchando(false) {
		_escuchas.push_back(make_pair(puerto, interfaz));
	};

	GuruSocketServidor():_ptr(NULL), _escuchando(false) {
	};

	virtual ~GuruSocketServidor() {
		detener();
	};

	void clear() {
//		detener();

		_ptr = NULL;
		_aceptadas = 0;
		_error_listen = 0;
		_error_accept = 0;
		_error_servicio = 0;
		_hora_inicio = 0;

		_escuchas.clear();
		_sockets.clear();

//		GuruQueue_threadsafe<SOCKET>	cola_entrada;

//		GuruPool_threadsafe<itemPool>	pool;

	}

	// Agrega a la configuracion otro puerto he interfaz a escuchar el servidor
	void agregar_escucha(uint16_t puerto, const string& interfaz = "0.0.0.0") {
		_escuchas.push_back(make_pair(puerto, interfaz));
	}

	void eliminar_escucha(uint16_t puerto, const string& interfaz = "0.0.0.0") {
		_escuchas.push_back(make_pair(puerto, interfaz));

		for (vector<std::pair<uint16_t, string> >::iterator it = _escuchas.begin(), itend = _escuchas.end(); it != itend; ++it) {
			if (it->first == puerto && it->second.compare(interfaz) == 0) {
				_escuchas.erase(it);
			}
		}
	}

	// tamanio maximo del pool de threads que mantendra en ejecucion
	void conf_pool(size_t capacidad) {
		pool.capacidad(capacidad);
	}

	// retorno el tamanio maximo del pool de threads que mantiene en ejecucion
	size_t conf_pool() {
		return pool.capacidad();
	}

	void  extra_ptr(void* ptr) {_ptr=ptr;};
	void* extra_ptr() {return _ptr;};

	void detener() { 
		if (!_escuchando) return ;

		_escuchando = false;

		for (vector<GuruSocket>::iterator it=_sockets.begin();it!=_sockets.end();++it)
			it->cerrar();
		
	#ifdef __GURUWIN32__
		ReleaseSemaphore(hsem, 1, NULL);

		WaitForSingleObject(handle_thread_conexiones, INFINITE);
		CloseHandle(handle_thread_conexiones);
		WaitForSingleObject(handle_lanza_servicio, INFINITE);
		CloseHandle(handle_lanza_servicio);

		CloseHandle(hsem);
	#elif defined __GURULINUX__
		sem_post(&hsem);
		if (pthread_equal(handle_thread_conexiones, pthread_self()) != 0) {
			pthread_join(handle_thread_conexiones, NULL);
			handle_thread_conexiones=pthread_self();
		}

		if (pthread_equal(handle_lanza_servicio, pthread_self()) != 0) {
			pthread_join(handle_lanza_servicio, NULL);
			handle_lanza_servicio=pthread_self();
		}

		sem_destroy(&hsem);
	#endif
	};

	bool escuchando() {return _escuchando;};

	estadisticas& stats (estadisticas& info) {
		info.capacidad_pool = pool.capacidad();
		info.size_pool = pool.size();

		size_t total=0;
		size_t ejecutando=0;
		size_t esperando_terminar=0;
		int	esperas=0;
// Segun C++11
/*
		std::for_each(pool.begin(), pool.end(), [&total,&ejecutando](pair<itemPool*, GuruPool<itemPool>::estado>& i) {
			if (i.second == GuruPool<itemPool>::tomado) total++;
			if (i.first->servicio_ejecutando()) ejecutando++;
		});
*/
// Segun C++03
		for (GuruPool<itemPool>::iterator it=pool.begin();it!=pool.end();++it) {
			if (it->second == GuruPool<itemPool>::tomado) total++;
			if (it->first->servicio_ejecutando()) ejecutando++;
			if (it->first->por_finalizar()) esperando_terminar++;
			if (it->first->esperas() > 0) esperas++;
		}

		info.esperas=esperas;
		info.esperando_terminar = esperando_terminar;
		info.threads_ejecutando = total;
		info.servicios_ejecutando = ejecutando;

		info.pedidos=_aceptadas;
		info.cola_socket=cola_entrada.size();
		info.errores_socket_listen=_error_listen;
		info.errores_socket_accept=_error_accept;
		info.errores_servicio=_error_servicio;

		info.tiempo_ejecucion=static_cast<size_t>(time(NULL) - _hora_inicio);
		info.inicio_ejecucion = static_cast<size_t>(_hora_inicio);

		return info;		
	};

	void capacidad_pool (size_t capacidad) {
		pool.capacidad(capacidad);
	};

	bool ejecutandose () {

	#ifdef __GURUWIN32__
		DWORD status_conexiones=0, status_servicio=0;
		if (GetExitCodeThread(handle_thread_conexiones, &status_conexiones)) {
			if (status_conexiones == STILL_ACTIVE) 
				return true;
		}

		if (GetExitCodeThread(handle_lanza_servicio, &status_servicio)) {
			if (status_servicio == STILL_ACTIVE) 
				return true;
		}
	#elif defined __GURULINUX__
		if (pthread_equal(handle_thread_conexiones, pthread_self()) == 0) return false;

		if (pthread_equal(handle_lanza_servicio, pthread_self()) == 0) return false;
	#endif

		return false;
	}

	//////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////
	void iniciar() {
		if (_escuchas.empty()) throw GuruSocketServidorException("Puerto/interfaz no configurado.");

//////////////////////////////////////////////////////////////////////
// Primero verifico si esta corriendo o no.
//////////////////////////////////////////////////////////////////////
		if (ejecutandose()) throw GuruSocketServidorEjecutandoException("Servidor Ejecutandose.");

//////////////////////////////////////////////////////////////////////
// Inicio el GuruSocketServidor.
//////////////////////////////////////////////////////////////////////

		if (!inicializado_GuruSocket()) inicializar_GuruSocket();

		inicializar();

		for (vector<std::pair<uint16_t, string> >::iterator it = _escuchas.begin(); it != _escuchas.end(); ++it) {
			try {
				_sockets.push_back(FabricaSocketEscuchar(it->first, it->second));
				GURULOG_DEBUG(GuruLog::str_id() + "Escuchando en " + it->second + ":" + GuruStringAlgo::itos(it->first));
			} catch (GuruSocketException&) {
				throw;
			};
		}

	#ifdef __GURUWIN32__
		hsem=CreateSemaphore(NULL, 0, 50, NULL);

		handle_thread_conexiones = CreateThread(NULL, 0, recibe_conexiones, static_cast<void*>(this), 0, NULL);
		if (handle_thread_conexiones == NULL) throw GuruSocketServidorException();

		handle_lanza_servicio = CreateThread(NULL, 0, lanza_servicioV2, static_cast<void*>(this), 0, NULL);
		if (handle_lanza_servicio == NULL) throw GuruSocketServidorException();
	#elif defined __GURULINUX__
		sem_init(&hsem, 0, 0);

		if (pthread_create(&handle_thread_conexiones, NULL, recibe_conexiones, static_cast<void*>(this)) != 0)
			throw GuruSocketServidorException();

		if (pthread_create(&handle_lanza_servicio, NULL, lanza_servicioV2, static_cast<void*>(this)) != 0)
			throw GuruSocketServidorException();
	#endif
	}

private:
	virtual void inicializar() {
#ifdef __GURUWIN32__
		hsem = NULL;
		handle_thread_conexiones = NULL;
		handle_lanza_servicio = NULL;
#endif
		_escuchando = 0;
		_aceptadas = 0;
		_error_listen = 0;
		_error_accept = 0;
		_hora_inicio = 0;
		pool.vaciar();
#if defined __GURULINUX__
		handle_thread_conexiones = pthread_self();
		handle_lanza_servicio = pthread_self();
#endif
	};

	void liberar_item_pool (itemPool* item) {
		pool.liberar(item);
	};

	//////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////
	#ifdef __GURUWIN32__
	static DWORD WINAPI recibe_conexiones(LPVOID llamador) {
	#elif defined __GURULINUX__
	static void* recibe_conexiones(void* llamador) {
	#endif
		GuruSocketServidor* esto = static_cast<GuruSocketServidor*>(llamador);

		int total_sockets = 0;
		fd_set lectura, activo;
		struct sockaddr sock_in;

		esto->_escuchando = true;

		esto->_hora_inicio = time(NULL);

			FD_ZERO(&lectura);

			for (vector<GuruSocket>::iterator it = esto->_sockets.begin(); it != esto->_sockets.end(); ++it)
				FD_SET(it->handle(), &lectura);

			int alto_socket = 0;

			// en linux, la funcion select necesita el socket mas alto mas uno.
			for (vector<GuruSocket>::iterator it = esto->_sockets.begin(); it != esto->_sockets.end(); ++it)
				if (it->handle() > (SOCKET)alto_socket) alto_socket = static_cast<int>(it->handle());

		GURULOG_DEBUG(GuruLog::str_id() + "Comenzando a recibir...");
		while (esto->_escuchando) {
			activo = lectura;
			total_sockets = select(alto_socket + 1, &activo, NULL, NULL, NULL);

			if (total_sockets > 0) { // algun socket a cambiado de estado
			#ifdef __GURUWIN32__
				int tamanio_sock = sizeof(struct sockaddr);
			#elif defined __GURULINUX__
				socklen_t tamanio_sock = sizeof(struct sockaddr);
			#endif

			#ifdef __GURUWIN32__
				for (int i = 0; i < total_sockets; i++) {
					SOCKET nueva_conexion = 0;
					nueva_conexion = accept(activo.fd_array[i], (struct sockaddr*)&sock_in, &tamanio_sock);

					if (nueva_conexion == INVALID_SOCKET) { 
						GURULOG_DEBUG(GuruLog::str_id() + "Error accept:" + GuruStringAlgo::itos(nueva_conexion) + " - " + GuruError().descripcion());
						esto->_error_accept++; 
						continue; 
					}

//					set_reusable(nueva_conexion);
//					set_linger(nueva_conexion);
//					set_tcp_nodelay(nueva_conexion);
//					set_reusable(nueva_conexion);

					GURULOG_DEBUG(GuruLog::str_id() + "Conexion Aceptada:" + GuruStringAlgo::itos(nueva_conexion) + "/" + GuruStringAlgo::itos(lectura.fd_array[i]));
					esto->cola_entrada.push(nueva_conexion);

					esto->_aceptadas++;

				#ifdef __GURUWIN32__
					ReleaseSemaphore(esto->hsem, 1, NULL);
				#elif defined __GURULINUX__
					sem_post(&esto->hsem);
				#endif
				}
			#elif defined __GURULINUX__
				for (vector<GuruSocket>::iterator it = esto->_sockets.begin(); it != esto->_sockets.end(); ++it) {
					if (FD_ISSET(it->handle(), &lectura)) {
						SOCKET nueva_conexion;
						nueva_conexion = accept(it->handle(), (struct sockaddr*)&sock_in, &tamanio_sock);

						if (nueva_conexion == INVALID_SOCKET) { esto->_error_accept++; continue; }

						esto->cola_entrada.push(nueva_conexion);

						esto->_aceptadas++;

				#ifdef __GURUWIN32__
						ReleaseSemaphore(esto->hsem, 1, NULL);
				#elif defined __GURULINUX__
						sem_post(&esto->hsem);
				#endif
					}
				}
			#endif
			} else if (total_sockets < 0) { // -1 == ERROR == SOCKET_ERROR
				esto->_error_listen++;
				GURULOG_DEBUG("Error en GuruSocketServidor:" + GuruError().descripcion(WSAGetLastError()));
				continue;
			}
		}

		for (vector<GuruSocket>::iterator it = esto->_sockets.begin(); it != esto->_sockets.end(); ++it)
			it->cerrar();

	#ifdef __GURUWIN32__
		ExitThread(0);
	#elif defined __GURULINUX__
		pthread_exit(0);
	#endif

		return 0;
	}

	//////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////
	#ifdef __GURUWIN32__
	static DWORD WINAPI lanza_servicioV2(LPVOID llamador) {
	#elif defined __GURULINUX__
	static void* lanza_servicioV2(void* llamador) {
	#endif
		GuruSocketServidor* esto = static_cast<GuruSocketServidor*>(llamador);
		SOCKET nueva_conexion;

		for (int i=1;i<=5;i++) esto->pool.agregar(new itemPool);

		do {
		#ifdef __GURUWIN32__
			WaitForSingleObject(esto->hsem, INFINITE);
		#elif defined __GURULINUX__
			sem_wait(&esto->hsem);
		#endif

			if (!esto->_escuchando) 
				break;

			nueva_conexion = esto->cola_entrada.front();
			esto->cola_entrada.pop();

			itemPool* item;
			do {
				item = esto->pool.obtener();

				if (item) break;

				Sleep(250);
			} while (item == NULL);

			item->Socket(nueva_conexion);
			item->SocketServidor(esto);
			item->item(item);
			item->extra_ptr(esto->extra_ptr());

			item->iniciar();
		} while(8);

		for (GuruPool<itemPool>::iterator it=esto->pool.begin();it!=esto->pool.end();++it) {
			if (it->second == GuruPool<itemPool>::tomado)
				it->first->Socket().cerrar();

			it->first->finalizar();
			while (it->first->ejecutando()) Sleep(100);
		}

		esto->pool.vaciar();

	#ifdef __GURUWIN32__
		ExitThread(0);
	#elif defined __GURULINUX__
		pthread_exit(0);
	#endif

		return 0;
	}

	class itemPool {
		GuruSocket			_socket;
		GuruSocketServidor*	_socketservidor;
		itemPool*			_item;
		bool				terminar;
		bool				ejecutando_servicio;
		InfoConexion		info;
		int					_esperas;
	#ifdef __GURUWIN32__
		DWORD	tid;
		HANDLE	handle_thread;
		HANDLE	ejecutar_servicio;
	#elif defined __GURULINUX__
		pthread_t	handle_thread;
		sem_t	ejecutar_servicio;
	#endif

	public:
		itemPool() { inicializar(); };
		virtual ~itemPool() {
		#ifdef __GURUWIN32__
			CloseHandle(handle_thread);
			CloseHandle(ejecutar_servicio);
		#elif defined __GURULINUX__
			sem_destroy(&ejecutar_servicio);
		#endif
		};

		inline void Socket(GuruSocket a) 	{_socket=a;info.socket(a);};
		inline GuruSocket& Socket() 		{return _socket;};

		inline void  extra_ptr(void* ptr) {info.extra_ptr(ptr);};
		inline void* extra_ptr() {return info.extra_ptr();};

		inline void SocketServidor(GuruSocketServidor* a) {_socketservidor=a;};
		inline GuruSocketServidor* SocketServidor() {return _socketservidor;};

		inline void  item(itemPool* a) {_item=a;};
		inline itemPool* item() {return _item;};

		inline bool servicio_ejecutando() {return ejecutando_servicio;}

		inline void liberar() {
			if (_item) {
				_socketservidor->liberar_item_pool (_item);
				_item=NULL;
			}
		};

		inline int esperas() {return _esperas;};

		void inicializar() {
			_esperas=0;
			_item=NULL;
			_socketservidor=NULL;
			terminar=false;ejecutando_servicio=false;
		#ifdef __GURUWIN32__
			handle_thread=0;tid=0;
			ejecutar_servicio = CreateEvent(NULL, FALSE, FALSE, NULL);
			handle_thread = CreateThread(NULL, 0, ejecutar, static_cast<void*>(this), 0, &tid);
			if (handle_thread == NULL) throw GuruSocketServidorException();
		#elif defined __GURULINUX__
			sem_init(&ejecutar_servicio, 0, 0);
			if (pthread_create(&handle_thread,  NULL, ejecutar, static_cast<void*>(this)) != 0)
				throw GuruSocketServidorException();
		#endif
		};

		bool ejecutando() {
		#ifdef __GURUWIN32__
			DWORD status=0;
			if (GetExitCodeThread(handle_thread, &status)) {
				return (status == STILL_ACTIVE) ? true : false;
			} else return false;
		#elif defined __GURULINUX__
			return (pthread_kill(handle_thread,0) == ESRCH) ? false : true;
		#endif
		};

		inline void iniciar() {
	#ifdef __GURUWIN32__
		SetEvent(ejecutar_servicio);
	#elif defined __GURULINUX__
		sem_post(&ejecutar_servicio);
	#endif
		};

		void finalizar() {
			terminar=true;
	#ifdef __GURUWIN32__
			SetEvent(ejecutar_servicio);
	#elif defined __GURULINUX__
			sem_post(&ejecutar_servicio);
	#endif
		};

		bool por_finalizar() {
			return terminar;
		};

	#ifdef __GURUWIN32__
		static DWORD WINAPI ejecutar(LPVOID llamador) {
	#elif defined __GURULINUX__
		static void* ejecutar(void* llamador) {
	#endif
		itemPool* esto = static_cast<itemPool*>(llamador);

		do {
			esto->_esperas++;
		#ifdef __GURUWIN32__
			WaitForSingleObject(esto->ejecutar_servicio, INFINITE);
		#elif defined __GURULINUX__
			sem_wait(&esto->ejecutar_servicio);
		#endif

			if (!esto->terminar) {

				esto->ejecutando_servicio=true;

				try{
//					GURULOG_DEBUG(GuruLog::str_id() + "Pre-Servicio:" + GuruStringAlgo::itos(esto->Socket().handle()));
					esto->SocketServidor()->servicio(&esto->info);
//					Sleep(5000);
//					GURULOG_DEBUG(GuruLog::str_id() + "Pos-Servicio:" + GuruStringAlgo::itos(esto->Socket().handle()));
				}catch (GuruSocketException& e) {
					GURULOG_DEBUG("GuruSocketException:" + string(e.what()));
				}catch (std::exception& e) {
					GURULOG_DEBUG("Excepcion atrapada en SocketServidor:" + string(e.what()));
					#if defined __GURULINUX__
					GuruError::stacktrace();
					#endif
				}catch (...) {
					// Con esto evito que de un error el thread, atrapa todas las excepciones
					// no atrapadas dentro del main()
					GURULOG_DEBUG("Error Desconocido en GuruSocketServidor.");
				}

				esto->ejecutando_servicio=false;
			}

//			GURULOG_DEBUG(GuruLog::str_id() + "Liberando:" + GuruStringAlgo::itos(esto->Socket().handle()));
			esto->Socket().cerrar();
			esto->liberar();
			esto->_esperas--;
		} while (!esto->terminar);

	#ifdef __GURUWIN32__
		ExitThread(0);
	#elif defined __GURULINUX__
		pthread_exit (0);
	#endif
		return 0;
		}

	};

	//////////////////////////////////////////////////////////////////
	// Este metodo abstracto deber ser creada para cada situacion.
	// Actualmente hay creados los siguientes:
	// * GuruServidorTCP
	// * GuruServidorTCPProtocolo
	//////////////////////////////////////////////////////////////////
	virtual void servicio (InfoConexion* info) = 0;
};

#endif
