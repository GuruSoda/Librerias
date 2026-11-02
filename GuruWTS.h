#if !defined __GURUWTS_H__
#define __GURUWTS_H__

//////////////////////////////////////////////////////////////////////
// Link con Wtsapi32.lib
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <WtsApi32.h>

#include "GuruCompat.h"
#include <GuruError.hpp>

#include <vector>
#include <string>
#include <iostream>

using namespace std;

#define					WTS_ESTADO_ACTIVE				"Activo"
#define					WTS_ESTADO_CONNECTED			"Conectado"
#define					WTS_ESTADO_CONNECTQUERY			"Conectando"
#define					WTS_ESTADO_SHADOW				"Shadow"
#define					WTS_ESTADO_DISCONNECTED			"Desconectado"
#define					WTS_ESTADO_IDLE					"Inactivo"
#define					WTS_ESTADO_LISTEN				"Escuchando"
#define					WTS_ESTADO_RESET				"Reset"
#define					WTS_ESTADO_DOWN					"Caido"
#define					WTS_ESTADO_INIT					"Iniciando"
#define					WTS_ESTADO_DESCONOCIDO			"Desconocido"

class data_session {
public:
		string	nombre_usuario;
		ULONG	session_id;
		int		estado;
		string	nombre_cliente; // nombre de la ws del cliente
		string  nombre_winstation;
		string	dominio;
		string	aplicacion;
};

class GuruWTS
{
public:
	class proceso {
		friend class GuruWTS;

		ULONG	session_id;
		DWORD	proceso_id;
		string	proceso_nombre;
		string	usuario_sid;
		string	usuario_nombre;
		string	dominio_nombre;

	public:
		DWORD	id_proceso() { return proceso_id; }
		ULONG	id_session() { return session_id; }
		string	nombre_proceso() { return proceso_nombre; }
		string	nombre_usuario() { return usuario_nombre; }
		string	nombre_dominio() { return dominio_nombre; }
	};

	class sesion {
		friend class GuruWTS;

		string	_WTSApplicationName;	// Pointer to a null-terminated string containing the published name of the application the session is running. If the function is called from the Terminal Services console, ppBuffer returns a NULL pointer.
		string	_WTSClientAddress;		// Pointer to a WTS_CLIENT_ADDRESS structure containing the network type and network address of the client. If the function is called from the Terminal Services console, ppBuffer returns a NULL pointer. Note that the first byte of the IP address returned in the ppBuffer parameter will be located at an offset of two bytes from the first location of the buffer.
		string	_WTSClientBuildNumber;	// Pointer to a ULONG variable. If the function is called from the Terminal Services console, ppBuffer returns a NULL pointer.
		string	_WTSClientDirectory;	// Pointer to a null-terminated string indicating the directory in which the client is installed. If the function is called from the Terminal Services console, ppBuffer returns a NULL pointer.
		std::pair<DWORD, DWORD>	_WTSClientDisplay;	// Pointer to a WTS_CLIENT_DISPLAY structure containing information about the client's display. If the function is called from the Terminal Services console, ppBuffer returns a NULL pointer.
		string	_WTSClientHardwareId;	// Pointer to a ULONG variable containing a client-specific hardware identifier. If the function is called from the Terminal Services console, ppBuffer returns a NULL pointer.
		string	_WTSClientName;			// Pointer to a null-terminated string containing the name of the client. If the function is called from the Terminal Services console, ppBuffer returns a NULL pointer.
		string	_WTSClientProductId;	// Pointer to a USHORT variable containing a client-specific product identifier. If the function is called from the Terminal Services console, ppBuffer returns a NULL pointer.
		string	_WTSClientProtocolType;	// Pointer to a USHORT variable containing the protocol type. This is one of the following values: WTS_PROTOCOL_TYPE_CONSOLE WTS_PROTOCOL_TYPE_ICA WTS_PROTOCOL_TYPE_RDP
		WTS_CONNECTSTATE_CLASS	_WTSConnectState;	// Pointer to an INT variable that contains a value from the WTS_CONNECTSTATE_CLASS enumeration type to indicate the session's current connection state.
		string	_WTSDomainName;			// Pointer to a null-terminated string that names the domain of the logged-on user.
		string	_WTSInitialProgram;		// Pointer to a null-terminated string containing the name of the initial program that Terminal Services runs when the user logs on.
		string	_WTSOEMId;				// Pointer to a null-terminated string that contains an OEM identifier; however, this value is not currently used.
		ULONG 	_WTSSessionId;			// Pointer to a ULONG variable containing the session identifier.
		string	_WTSUserName;			// Pointer to a null-terminated string containing the name of the user associated with the session.
		string	_WTSWinStationName;		// Pointer to a null-terminated string containing the name of the specified session. Note  Despite its name, specifying this type does not return the window station name. Rather, it returns the name of the Terminal Services session. Each Terminal Services session is associated with an interactive window station. Currently, since the only supported window station name for an interactive window station is "WinSta0", each session is associated with its own "WinSta0" window station. For more information, see Window Stations.
		string	_WTSWorkingDirectory;	// Pointer to a null-terminated string containing the default directory used when launching the initial program.

	public:
		string	WTSApplicationName() { return _WTSApplicationName; };
		string	WTSClientAddress() { return _WTSClientAddress; };
		string	WTSClientBuildNumber() { return _WTSClientBuildNumber; };
		string	WTSClientDirectory() { return _WTSClientDirectory; };
		std::pair<DWORD, DWORD>	WTSClientDisplay() { return _WTSClientDisplay; };
		string	WTSClientHardwareId() { return _WTSClientHardwareId; };
		string	WTSClientName() { return _WTSClientName; };
		string	WTSClientProductId() { return _WTSClientProductId; };
		string	WTSClientProtocolType() { return _WTSClientProtocolType; };
		DWORD	WTSConnectState() { return _WTSConnectState; };
		string	WTSDomainName() { return _WTSDomainName; };
		string	WTSInitialProgram() { return _WTSInitialProgram; };
		string	WTSOEMId() { return _WTSOEMId; };
		DWORD	WTSSessionId() { return _WTSSessionId; };
		string	WTSUserName() { return _WTSUserName; };
		string	WTSWinStationName() { return _WTSWinStationName; };
		string	WTSWorkingDirectory() { return _WTSWorkingDirectory; };
	};

public:
	GuruWTS();
	virtual ~GuruWTS();

	bool	abrir(const string& servidor);

	bool	cargar_libreria();

	// WTSUserName | WTSConnectState | WTSSessionId | WTSClientName | WTSWinStationName | WTSApplicationName

	vector<GuruWTS::sesion>& sesiones(vector<GuruWTS::sesion>&, const string& computadora, int campos[]);
	vector<GuruWTS::proceso>& procesos(vector<GuruWTS::proceso>&, const string& computadora="", DWORD sessionid=-1, const string& username="", DWORD processid=-1);

	vector<WTSINFOEX_LEVEL1>& info_level1(string&, vector<WTSINFOEX_LEVEL1>&);
	vector<WTSCLIENT>& info_cliente(string&, vector<WTSCLIENT>&);

	bool	terminar_proceso(DWORD id_proceso, DWORD codigo_salida = 0);
	bool	terminar_proceso (string& servidor, DWORD id_proceso, DWORD codigo_salida = 0);

	bool	cerrar_session(DWORD sid, bool esperar);
	bool	cerrar_session(string& servidor, DWORD sid, bool esperar);

	bool	desconectar_session (DWORD sid, bool esperar);
	bool	desconectar_session (string& servidor, DWORD sid, bool esperar);

	bool	reiniciar_servidor ();
	bool	reiniciar_servidor (string& servidor);

	bool	apagar_servidor ();
	bool	apagar_servidor (string& servidor);

	bool	enviar_mensaje(DWORD sid, const char *mensaje, const char *titulo, bool esperar);
	bool	enviar_mensaje(string& servidor, DWORD sid, const char *mensaje, const char *titulo, bool esperar);

	void	campos(int campos_a_cargar);
	const char*	estado_a_str(int estado);
	int		str_a_estado(char *estado);

	bool	sid_a_nombre_usuario(PSID sid, char *nombre_usuario);

	HANDLE	handle();

//	vector<info_sesionWTS>	_sesiones;
//	vector<proceso>		_procesos;
	vector<WTSCLIENT>		_cliente;
	vector<WTSINFOEX_LEVEL1>	_info_level1;
private:

	void	cerrar();

protected:
	HANDLE		handle_server;

	DWORD		campos_habilitados;

	string		servidor;
};

#endif // !defined __GURUWTS_H__
