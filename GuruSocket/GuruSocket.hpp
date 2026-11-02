#ifndef _GURUSOCKET_H_
#define _GURUSOCKET_H_

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

//	#include <ws2tcpip.h> // sacar --> solo para probar el nuevo connect

// sacado de la definicion existente en winsock2.h
// #define SO_EXCLUSIVEADDRUSE ((int)(~SO_REUSEADDR)) /* disallow local address reuse */

#elif defined __GURULINUX__
   #include <sys/types.h>
   #include <sys/socket.h>
   #include <netinet/in.h>
   #include <netinet/tcp.h>
   #include <netdb.h>
   #include <arpa/inet.h>
   #include <unistd.h>
#endif

#include <iostream>
#include <string>
#include <exception>

#include <GuruError.hpp>
#include <GuruBuffer.hpp>
#include <GuruToken.hpp>
#include <GuruLog.hpp>

using namespace std;

inline bool inicializado_GuruSocket();
inline void inicializar_GuruSocket();
inline void limpiar_GuruSocket() {
			#if defined __GURUWIN32__
				WSACleanup();
			#endif
			};

template <typename T> inline void set_exclusive(const T& _socket);
template <typename T> inline void set_reusable(const T& _socket);
template <typename T> inline void set_linger(const T& _socket);
template <typename T> inline void set_timeout(const T& _socket, uint32_t);
template <typename T> inline uint32_t get_timeout(const T& _socket);
template <typename T> inline void set_tcp_nodelay(const T& _socket);
template <typename T> inline void set_info_par(T& _socket);
template <typename T> inline uint32_t BufferSalida(const T& elsocket);
template <typename T> inline uint32_t BufferEntrada(const T& elsocket);
template <typename T> inline T& interfaces(const T& listado);

#ifdef __GURUWIN32__
	#define LastError() WSAGetLastError()
#elif defined __GURULINUX__
	#define LastError() errno
#endif

#define default_time_out 60

class GuruSocketException: public std::exception
{
public:
	GuruSocketException(const string& texto):mensaje(texto) {};
	GuruSocketException() {	mensaje = GuruError().descripcion();};
	GuruSocketException(int nro_error) { mensaje = GuruError().descripcion(nro_error);};
	virtual ~GuruSocketException() throw() {};

	virtual const char* what() const throw() { return mensaje.c_str();};

private:
	string mensaje;
};

class GuruSocket {
public:
	GuruSocket() :_socket(INVALID_SOCKET), _puerto_local(0), _puerto_remoto(0), _interfaz_remota_binaria(0), _timeout(default_time_out) {};
	GuruSocket(SOCKET _socket) :_socket(_socket), _puerto_local(0), _puerto_remoto(0), _interfaz_remota_binaria(0), _timeout(default_time_out) { };

	GuruSocket(const GuruSocket& copia) {
		this->_socket = copia._socket;
		this->_servidor = copia._servidor;
		this->_puerto_local = copia._puerto_local;
		this->_puerto_remoto = copia._puerto_remoto;
		this->_interfaz_local = copia._interfaz_local;
		this->_interfaz_remota =copia._interfaz_remota;
		this->_interfaz_remota_binaria=copia._interfaz_remota_binaria;
	};

	// contructor para FabricaConectar
	GuruSocket(SOCKET elsocket, string interfaz_remota, unsigned int puerto_remoto, string interfaz_local, unsigned int puerto_local){
		this->_socket=elsocket;
		this->_puerto_local=puerto_local;
		this->_puerto_remoto=puerto_remoto;
		this->_interfaz_remota=interfaz_remota;
		this->_interfaz_local=interfaz_local;
	};

	// contructor para FabricaEscuchar
	GuruSocket(SOCKET elsocket, string interfaz_local, unsigned int puerto_local){
		this->_socket=elsocket;
		this->_puerto_local=puerto_local;		
		this->_interfaz_local=interfaz_local;

		this->_puerto_remoto=0;
	};

	virtual ~GuruSocket() {}; // No Agregar cerrar() ... en una copia de GuruSocket se cierra!!!

	bool conectado() {return (_socket == INVALID_SOCKET) ? false : true;};
	void cerrar() {

		if (_socket == INVALID_SOCKET) return;

	#ifdef __GURUWIN32__
		shutdown(_socket, SD_BOTH);
	#elif defined __GURULINUX__
		shutdown(_socket, SHUT_RDWR);
	#endif

		GURULOG_TRACE(GuruLog::str_id() + " Cerrando Socket " + GuruStringAlgo::itos(_socket));
		closesocket(_socket);
		_socket=INVALID_SOCKET;
	};

	inline SOCKET			handle() {return _socket;};

	inline string			servidor() {return _servidor;};
	inline unsigned int		puerto_local() {return _puerto_local;};
	inline string			interfaz_local() {return _interfaz_local;};
	inline string			interfaz_remota() {
		if (_interfaz_remota.empty()) set_info_par(*this);
		return _interfaz_remota;
	};

	inline unsigned int		puerto_remoto() {
		if (_puerto_remoto == 0) set_info_par(*this);
		return _puerto_remoto;
	};

	inline uint32_t			interfaz_remota_binaria() {
		if (_interfaz_remota_binaria == 0) set_info_par(*this);
		return _interfaz_remota_binaria;
	};

	// para la lectura y escritura en un socket el time out es el mismo.
	inline void timeout(uint32_t seg) { set_timeout(_socket, seg); }
	inline uint32_t timeout() { return get_timeout(_socket); }

	inline GuruSocket& operator = (const GuruSocket& gurusocket2){
		this->_socket = gurusocket2._socket;
		this->_servidor = gurusocket2._servidor;
		this->_puerto_local = gurusocket2._puerto_local;
		this->_puerto_remoto = gurusocket2._puerto_remoto;
		this->_interfaz_local = gurusocket2._interfaz_local;
		this->_interfaz_remota =gurusocket2._interfaz_remota;
		this->_interfaz_remota_binaria=gurusocket2._interfaz_remota_binaria;

		return *this;
	}

protected:
	SOCKET		_socket;
	string		_servidor;
	string		_interfaz_local;
	string		_interfaz_remota;
	uint16_t	_puerto_local;
	uint16_t	_puerto_remoto;
	uint32_t	_interfaz_remota_binaria;
	uint32_t	_timeout; // tiempo de espera maximo para esperar datos en una recepcion o envio.

	template <typename T> friend inline void set_info_par(T& _socket);
};

class GuruSocketTxt: public GuruSocket {
public:
	GuruSocketTxt(): GuruSocket() {};
	GuruSocketTxt(SOCKET _socket):GuruSocket(_socket) {};
	GuruSocketTxt(const GuruSocket& enchufe):GuruSocket(enchufe) {};
	virtual ~GuruSocketTxt() {};

	GuruSocketTxt& operator << (const char* buffer){
		int salida_send = send (_socket, buffer, (int) strlen(buffer), 0);

		if (salida_send == SOCKET_ERROR) {
			int a = LastError();
			closesocket(_socket);
			throw GuruSocketException(a);
		}

		return *this;
	}
 
	GuruSocketTxt& operator << (const string& str) {

		int salida_send = send (_socket, str.c_str(), static_cast<int>(str.length()), 0);

		if (salida_send == SOCKET_ERROR) {
			int a = LastError();
			closesocket(_socket);
			throw GuruSocketException(a);
		}

		return *this;
	}

	GuruSocketTxt& operator >> (string& str) {

		recibir(str);

		return *this;
	}

	string& recibir(string& cadena, int total_a_leer=0) {
		fd_set	read;

		struct	timeval struc_tiempo_espera = {0};

		int		cant_sockets = 0;
		char	caracter = 0;
		char	caracter_anterior = 0;
		int		leidos = 0;
		int		cant_caracteres=0;

		cadena.clear();

		do {
			struc_tiempo_espera.tv_sec = 0;
			struc_tiempo_espera.tv_usec = 0;

			FD_ZERO(&read);

			FD_SET(_socket, &read);

			cant_sockets = select ( (int)_socket + 1, &read, NULL, NULL, NULL);

			// pregunto por time out o por error en el socket
			if (cant_sockets == 0 || cant_sockets == SOCKET_ERROR) {
				int a = LastError();
				closesocket(_socket);
				throw GuruSocketException(a);
			}

			caracter_anterior = caracter;
			leidos = recv (_socket, &caracter, 1, 0); // saco de a un byte

			if (leidos == 0 || leidos == SOCKET_ERROR) {
				int a = LastError();
				closesocket(_socket);
				throw GuruSocketException(a);
			}

			if (caracter == '\r') continue;

			if (caracter == '\n' && caracter_anterior == '\r') {
				cadena.erase(cant_caracteres); // borro el CRLF
				break;
			}

			if (caracter == 0x08) { // es backspace
				if (cant_caracteres) {
					cant_caracteres--;
					cadena.erase(cant_caracteres);
				}
				continue;
			}

			cadena.push_back(caracter);
			cant_caracteres++;

			if (total_a_leer == cant_caracteres) break;

		} while (true);

		return cadena;
	}

};

class GuruSocketBin: public GuruSocket {
public:
	GuruSocketBin(): GuruSocket() {};
	GuruSocketBin(SOCKET enchufe):GuruSocket(enchufe) {};
	GuruSocketBin(const GuruSocket& enchufe):GuruSocket(enchufe) {};
	virtual ~GuruSocketBin() {};

	GuruSocketBin& operator >> (GuruBufferChar& buffer){
		buffer.clear_data();

		uint32_t buf_entrada = BufferEntrada(_socket);

		int leidos = recv (_socket, buffer.buf(), buf_entrada, 0);

		if (leidos == 0 || leidos == SOCKET_ERROR) {
			int a  = WSAGetLastError();
			closesocket(_socket);
			throw GuruSocketException(a);
		}

		return *this;
	}
 
	GuruSocketBin& operator << (GuruBufferChar& buffer) {
		int salida_send = send (_socket, buffer.buf(), static_cast<int>(buffer.cantidad_data()), 0);

		if (salida_send == SOCKET_ERROR) {
			int a  = WSAGetLastError();
			closesocket(_socket);
			throw GuruSocketException(a);
		}

		return *this;
	}

};

class auto_GuruSocket: public GuruSocket {
public:
	auto_GuruSocket() {};
	auto_GuruSocket(SOCKET enchufe): GuruSocket(enchufe) {};
	auto_GuruSocket(const GuruSocket& enchufe):GuruSocket(enchufe) {};
	virtual ~auto_GuruSocket() { cerrar(); };
};

// Esta funcion verifica si es una direccion IP hecha por numeros y puntos.
static bool esIP(const string& addrr) {
	GuruTokenEx a(".");

	if (a.extraer(addrr) == 4) {
		for (int i = 0; i < 4; i++) {
			if (!GuruStringAlgo::numerico(a[i])) return false;

			int octeto = atoi(a[i].c_str());

			if (octeto >= 0 && octeto <= 255) continue;

			return false;
		}
	} else return false;

	return true;
}

template <typename T, typename Y>
inline GuruSocket FabricaSocketConectar(const T& servidor, const Y puerto, const string& interfaz_origen="0.0.0.0", int puerto_origen=0)
{
//	cout << GuruLog::_nivel << endl;
//	cout << GuruLog::def_level::level() << endl;
	GURULOG_DEBUG(GuruLog::str_id() + " FabricaSocketConectar: " + servidor + ":" + GuruStringAlgo::itos(puerto) + " - " + interfaz_origen + ":" + GuruStringAlgo::itos(puerto_origen));
	SOCKET	_socket;

#if defined __GURUWIN32__
	struct sockaddr_in	data_connect = { 0 };
	struct hostent*		hosts;

//	Verificar si la variable servidor tiene un nombre
	if ((hosts = gethostbyname(servidor.c_str())))
		memcpy((char *)&data_connect.sin_addr, hosts->h_addr_list[0], hosts->h_length); // copio la direccion ip del servidor
	else
		// Verificar si es una notacion de numeros y puntos
		if ((data_connect.sin_addr.s_addr = inet_addr(servidor.c_str())) == INADDR_NONE) {
			int errorsocket = LastError();
			GURULOG_DEBUG(GuruLog::str_id() + " Error en FabricaSocketConectar(): " + GuruError().descripcion(errorsocket));
			throw GuruSocketException(LastError());
		}

	data_connect.sin_family = AF_INET;
	data_connect.sin_port = (unsigned int)htons(puerto);

	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	if (_socket == INVALID_SOCKET) {
		int errorsocket = LastError();
		GURULOG_DEBUG(GuruLog::str_id() + " Error en FabricaSocketConectar(socket): " + GuruError().descripcion(errorsocket));
		throw GuruSocketException(LastError());
	}

	if (connect(_socket, (struct sockaddr*)&data_connect, sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
		int errorsocket = LastError();
		GURULOG_DEBUG(GuruLog::str_id() + " Error en FabricaSocketConectar(connect): " + GuruError().descripcion(errorsocket));
		closesocket(_socket);
		throw GuruSocketException(errorsocket);
	}

	set_timeout(_socket, default_time_out);
	GURULOG_DEBUG(GuruLog::str_id() + "Poniendo Timeout: " + GuruStringAlgo::itos(get_timeout(_socket)) + "Seg.");

#elif defined __GURULINUX__

	struct addrinfo* res;

	if (int s = getaddrinfo(servidor.c_str(), GuruStringAlgo::itos(puerto).c_str(), NULL, &res)) {
		string a(string(gai_strerror(s)));
		GURULOG_DEBUG(GuruLog::str_id() + " Error en FabricaSocketConectar(getaddrinfo): " + a);
		throw GuruSocketException(a);
	}

	_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	
	if (_socket == SOCKET_ERROR) {
		int errorsocket = errno;
		GURULOG_DEBUG(GuruLog::str_id() + " Error en FabricaSocketConectar(socket): " + GuruError().descripcion(errorsocket));
		throw GuruSocketException(errorsocket);
	}

	if (connect(_socket, res->ai_addr, res->ai_addrlen) == SOCKET_ERROR) {
		int errorsocket = errno;
		freeaddrinfo(res);
		closesocket(_socket);
		GURULOG_DEBUG(GuruLog::str_id() + " Error en FabricaSocketConectar(connect): " + GuruError().descripcion(errorsocket));
		throw GuruSocketException(errorsocket);
	}

	freeaddrinfo(res);

	set_timeout(_socket, default_time_out);
	GURULOG_DEBUG(GuruLog::str_id() + " Poniendo Timeout: " + GuruStringAlgo::itos(get_timeout(_socket)) + "Seg.");
#endif

	GURULOG_DEBUG(GuruLog::str_id() + " FabricaSocketConectar -> Conectado -> Socket:" + GuruStringAlgo::itos(_socket));

	return GuruSocket(_socket, servidor, puerto, interfaz_origen, puerto_origen);
}

template <typename T>
inline GuruSocket FabricaSocketEscuchar(T puerto, const string& interfaz = "0.0.0.0")
{
	SOCKET _socket = socket (AF_INET, SOCK_STREAM, IPPROTO_IP);

	if (_socket == INVALID_SOCKET)
		throw GuruSocketException(LastError());

#ifdef __GURUWIN32__
	set_exclusive(_socket); // permite escuchar un socket que ya esta siendo usado.
#elif defined __GURULINUX__
	set_reusable(_socket); // permite escuchar un socket que ya esta siendo usado (en estado TIME_WAIT).
#endif

	set_linger(_socket);
	set_tcp_nodelay(_socket);
	set_timeout(_socket, default_time_out);

	struct sockaddr_in sock_in = {0};

	sock_in.sin_family = AF_INET; // IPv4

	if (interfaz.compare("0.0.0.0") == 0)
		sock_in.sin_addr.s_addr = htonl(INADDR_ANY); // todas las interfaces de red
	else
		sock_in.sin_addr.s_addr = inet_addr(interfaz.c_str());

	sock_in.sin_port = htons((u_short)puerto);

	int tamanio_var = sizeof(sock_in);
	if (::bind (_socket,(struct sockaddr*)&sock_in, tamanio_var) != 0)
		throw GuruSocketException(LastError());

	if (listen (_socket, SOMAXCONN) != 0)
		throw GuruSocketException(LastError());

	return GuruSocket(_socket, interfaz, puerto);
}

inline bool inicializado_GuruSocket() {
#if defined __GURUWIN32__
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (s == INVALID_SOCKET && WSAGetLastError() == WSANOTINITIALISED) {
		return false;
	}

	closesocket(s);
#endif
	return true;
}

inline void inicializar_GuruSocket()
{
	GURULOG_INFO("Inicializando...");

#if defined __GURUWIN32__
	WSADATA Data = { 0 };

	if (WSAStartup(MAKEWORD(2, 2), &Data)) {
		GURULOG_DEBUG("Error en inicializar_GuruSocket:" + GuruError().descripcion(WSAGetLastError()));
		throw GuruSocketException(WSAGetLastError()); 
	}
#endif
}

#if defined __GURUWIN32__
template <typename T>
inline void set_exclusive(const T& _socket)
{
	int    salida=sizeof(int);
	int    parametro=1;

	if (setsockopt(_socket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char *)&parametro, salida) != 0) {
		GURULOG_DEBUG("Error en set_exclusive:" + GuruError().descripcion(LastError()));
	}
}
#endif

template <typename T>
inline void set_timeout(const T& _socket, uint32_t seg)
{
#if defined __GURUWIN32__
	DWORD t = seg * 1000;

	// time en cada socket que recibe en modo escucha

	if (setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&t, sizeof(DWORD)) != 0) {
		GURULOG_DEBUG("Error en set_timeout (SO_RCVTIMEO):" + GuruError().descripcion(LastError()));
	}

	if (setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&t, sizeof(DWORD)) != 0) {
		GURULOG_DEBUG("Error en set_timeout (SO_SNDTIMEO):" + GuruError().descripcion(LastError()));
	}
#elif defined __GURULINUX__
	struct timeval struc_time;

	struc_time.tv_sec = seg;
	struc_time.tv_usec = 0;

	if (setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&struc_time, sizeof(struct timeval)) != 0) {
		GURULOG_DEBUG("Error en set_timeout (SO_RCVTIMEO):" + GuruError().descripcion(LastError()));
	}

	if (setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&struc_time, sizeof(struct timeval)) != 0) {
		GURULOG_DEBUG("Error en set_timeout (SO_SNDTIMEO):" + GuruError().descripcion(LastError()));
	}
#endif
}

template <typename T>
inline uint32_t get_timeout(const T& _socket)
{
	uint32_t seg = 0;

#if defined __GURUWIN32__
	int opt_len = sizeof(int);

	// time en cada socket que recibe en modo escucha

	getsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&seg, &opt_len);
	getsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&seg, &opt_len);
#elif defined __GURULINUX__
	socklen_t opt_len = sizeof(socklen_t);

	struct timeval struc_time;

	struc_time.tv_sec = seg;
	struc_time.tv_usec = 0;

	getsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (void*)&seg, &opt_len);
	getsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO, (void*)&seg, &opt_len);
#endif

	return seg/1000; // get y set son en Segundos.
}

template <typename T>
inline void set_reusable(const T& _socket)
{
	int    salida=sizeof(int);
	int    parametro=1;

	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&parametro, salida) != 0) {
		GURULOG_DEBUG("Error en set_reusable:" + GuruError().descripcion(LastError()));
	}
}

template <typename T>
inline void set_linger(const T& _socket)
{
	struct linger lin;

	lin.l_onoff = 1;
	lin.l_linger = 30;

	if (setsockopt(_socket, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(struct linger)) != 0) {
		GURULOG_DEBUG("Error en set_linger:" + GuruError().descripcion(LastError()));
	}
}

template <typename T>
inline void set_tcp_nodelay(const T& _socket)
{
	int opcion_socket = 1;

	if (setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&opcion_socket, sizeof(int)) != 0) {
		GURULOG_DEBUG("Error en set_tcp_nodelay:" + GuruError().descripcion(LastError()));
	}
}

template <typename T>
inline void set_info_par(T& _socket)
{
	struct sockaddr_in data_cliente;
#if defined __GURUWIN32__
	int     tam_data=sizeof(struct sockaddr_in);
#elif defined __GURULINUX__
    socklen_t tam_data=sizeof(struct sockaddr_in);
#endif

	int salida = getpeername(_socket.handle(),(struct sockaddr*)&data_cliente, &tam_data);

	if (!salida) {
		// sacando el valor en forma directa los bytes me aparecen invertidos, no se por que
		// Con ayuda de http://stackoverflow.com/questions/3060950/how-to-get-ip-address-from-sock-structure-in-c
		// Con el &0x000000FF solo retorna los bits prendidos del ultimo octeto/byte
		_socket._interfaz_remota_binaria = data_cliente.sin_addr.s_addr&0x000000FF;
		_socket._interfaz_remota_binaria<<=8;
		_socket._interfaz_remota_binaria+=(data_cliente.sin_addr.s_addr&0x0000FF00)>>8;
		_socket._interfaz_remota_binaria<<=8;
		_socket._interfaz_remota_binaria+=(data_cliente.sin_addr.s_addr&0x00FF0000)>>16;
		_socket._interfaz_remota_binaria<<=8;
		_socket._interfaz_remota_binaria+=(data_cliente.sin_addr.s_addr&0xFF000000)>>24;

		_socket._interfaz_remota = inet_ntoa(data_cliente.sin_addr);
		_socket._puerto_remoto = ntohs(data_cliente.sin_port);
	}
}

template <typename T>
inline uint32_t BufferSalida(const T& elsocket)
{
	int tamanio=0;

	#if defined __GURUWIN32__
		int bytes_tamanio=sizeof(tamanio);
	#elif defined __GURULINUX__
		socklen_t bytes_tamanio=sizeof(tamanio);
	#endif

	getsockopt(elsocket,SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&tamanio), &bytes_tamanio);

	return (tamanio==0) ? 1024 : tamanio ;
}

template <typename T>
inline uint32_t BufferEntrada(const T& elsocket)
{
	int tamanio=0;

	#if defined __GURUWIN32__
		int bytes_tamanio=sizeof(tamanio);
	#elif defined __GURULINUX__
		socklen_t bytes_tamanio=sizeof(tamanio);
	#endif

	getsockopt(elsocket,SOL_SOCKET, SO_RCVBUF, (char*)&tamanio, &bytes_tamanio);

	return (tamanio==0) ? 1024 : tamanio ;
}

template <typename T>
inline T& interfaces(T& listado)
{
	listado.clear();

	struct in_addr sin_addr;
	struct hostent* pHost=gethostbyname(NULL);

	if (pHost == NULL) return listado;

	while (*pHost->h_addr_list) {
		memcpy(&sin_addr, *pHost->h_addr_list, pHost->h_length);
		listado.push_back(inet_ntoa(sin_addr));
		pHost->h_addr_list++;
	}

	return listado;
}

#endif
