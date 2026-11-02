#include "GuruWTS.h"

GuruWTS::GuruWTS()
{
	handle_server = NULL;

	campos_habilitados	= WTSSessionId | WTSUserName | WTSConnectState | WTSClientName;

	handle_server = WTS_CURRENT_SERVER_HANDLE;

#if defined __CON_GURUWTS__
	WTSOpenServer	= NULL;
	Wtsapi32_dll	= NULL;
#endif
}

GuruWTS::~GuruWTS()
{
#if defined __CON_GURUWTS__
	if (Wtsapi32_dll)
		FreeLibrary(Wtsapi32_dll);
#endif
}

bool GuruWTS::cargar_libreria()
{
#if defined __CON_GURUWTS__

	Wtsapi32_dll = LoadLibrary("Wtsapi32.dll");

	if (Wtsapi32_dll)
		{
		WTSOpenServer				= (Ptr_WTSOpenServer)				GetProcAddress(Wtsapi32_dll, "WTSOpenServerA");
		WTSCloseServer				= (Ptr_WTSCloseServer)				GetProcAddress(Wtsapi32_dll, "WTSCloseServer");
		WTSEnumerateSessions		= (Ptr_WTSEnumerateSessions)		GetProcAddress(Wtsapi32_dll, "WTSEnumerateSessionsA");
		WTSFreeMemory				= (Ptr_WTSFreeMemory)				GetProcAddress(Wtsapi32_dll, "WTSFreeMemory");
		WTSQuerySessionInformation	= (Ptr_WTSQuerySessionInformation)	GetProcAddress(Wtsapi32_dll, "WTSQuerySessionInformationA");
		WTSEnumerateProcesses		= (Ptr_WTSEnumerateProcesses)		GetProcAddress(Wtsapi32_dll, "WTSEnumerateProcessesA");
		WTSTerminateProcess			= (Ptr_WTSTerminateProcess)			GetProcAddress(Wtsapi32_dll, "WTSTerminateProcess");
		WTSLogoffSession			= (Ptr_WTSLogoffSession)			GetProcAddress(Wtsapi32_dll, "WTSLogoffSession");
		WTSSendMessage				= (Ptr_WTSSendMessage)				GetProcAddress(Wtsapi32_dll, "WTSSendMessageA");
		WTSDisconnectSession		= (Ptr_WTSDisconnectSession)		GetProcAddress(Wtsapi32_dll, "WTSDisconnectSession");
		WTSShutdownSystem			= (Ptr_WTSShutdownSystem)			GetProcAddress(Wtsapi32_dll, "WTSShutdownSystem");

		return true;
		}
	else
		{
		GuruWTS_Error.descripcion();
		return false;
		}
#else
	return true;
#endif
}


// WTS_CURRENT_SERVER_HANDLE 
bool GuruWTS::abrir(const string& nombre_servidor)
{
#if defined __CON_GURUWTS__
	if (WTSOpenServer == NULL)
		if (!cargar_libreria())
			return false;
#endif

	if (handle_server) cerrar();

	handle_server = WTSOpenServer((char*)nombre_servidor.c_str());

	if (handle_server)
		return true;
	else {
		GuruError().descripcion();
		return false;
	}
}

void GuruWTS::cerrar()
{
	WTSCloseServer(handle_server);

	handle_server = NULL;
}

void GuruWTS::campos(int campos_a_cargar)
{
	campos_habilitados = campos_a_cargar;
}

// Posibles campos:
// 	WTSApplicationName;
//	WTSClientAddress;
// 	WTSClientBuildNumber;
// 	WTSClientDirectory;
//	WTSClientDisplay;
// 	WTSClientHardwareId;
// 	WTSClientName;
// 	WTSClientProductId;
// 	WTSClientProtocolType;
// 	WTSConnectState;
// 	WTSDomainName;
// 	WTSInitialProgram;
// 	WTSOEMId;
// 	WTSSessionId;
// 	WTSUserName;
// 	WTSWinStationName;
// 	WTSWorkingDirectory;
vector<GuruWTS::sesion>& GuruWTS::sesiones(vector<GuruWTS::sesion>& info, const string& computadora, int campos[])
{
	info.clear();

	try {
		if (!abrir(computadora)) throw GuruError().mensaje();

		WTS_SESSION_INFO*	info_session;
		WTS_SESSION_INFO*	inicio;
		DWORD			cant_sesiones;
		DWORD			cant_sesiones_correctas = 0;

		if (!WTSEnumerateSessions(handle_server, 0, 1, &info_session, &cant_sesiones)) throw GuruError().descripcion();

		inicio = info_session;

		for (DWORD i=0;i<cant_sesiones;i++) {
			GuruWTS::sesion	temp_sesion;

			cant_sesiones_correctas++;

			int numero_campo = 0;
			while (campos[numero_campo] != NULL) {
				LPSTR   buffer = NULL;
				DWORD	tamanio_buffer;

				switch (campos[numero_campo]) {
				case WTSUserName:
							if (WTSQuerySessionInformation (handle_server, info_session->SessionId, WTSUserName, &buffer, &tamanio_buffer))
								temp_sesion._WTSUserName = (tamanio_buffer) ? buffer : "";
							break;

				case WTSConnectState:
							if (WTSQuerySessionInformation (handle_server, info_session->SessionId, WTSConnectState, &buffer, &tamanio_buffer))
								temp_sesion._WTSConnectState = (tamanio_buffer) ? *((WTS_CONNECTSTATE_CLASS*)buffer) : WTSInit;
							break;

				case WTSSessionId:
							if (WTSQuerySessionInformation (handle_server, info_session->SessionId, WTSSessionId, &buffer, &tamanio_buffer))
								temp_sesion._WTSSessionId = (tamanio_buffer) ? *((ULONG*)buffer) : 0;
							break;

				case WTSClientAddress:
							if (WTSQuerySessionInformation(handle_server, info_session->SessionId, WTSClientAddress, &buffer, &tamanio_buffer))
								temp_sesion._WTSClientAddress = (tamanio_buffer) ? (const char*)(((WTS_CLIENT_ADDRESS*)(buffer))->Address) : "";
							//	temp_sesion._WTSClientAddress = (tamanio_buffer) ? ((WTS_CLIENT_ADDRESS*)(buffer))->Address : "";
							break;

				case WTSClientDisplay:
					if (WTSQuerySessionInformation(handle_server, info_session->SessionId, WTSClientDisplay, &buffer, &tamanio_buffer)) {
						WTS_CLIENT_DISPLAY* tmp_display = { 0 };

						if (tamanio_buffer) tmp_display = (WTS_CLIENT_DISPLAY*)(buffer);

						temp_sesion._WTSClientDisplay = std::make_pair(tmp_display->HorizontalResolution, tmp_display->VerticalResolution);
					}
					break;

				case WTSClientName:
							if (WTSQuerySessionInformation (handle_server, info_session->SessionId, WTSClientName, &buffer, &tamanio_buffer))
								temp_sesion._WTSClientName = (tamanio_buffer) ? buffer : "";
							break;

				case WTSWinStationName:						
							if (WTSQuerySessionInformation (handle_server, info_session->SessionId, WTSWinStationName, &buffer, &tamanio_buffer))
								temp_sesion._WTSWinStationName = (tamanio_buffer) ? buffer : "";
							break;

				case WTSApplicationName:						
							if (WTSQuerySessionInformation (handle_server, info_session->SessionId, WTSApplicationName, &buffer, &tamanio_buffer))
								temp_sesion._WTSApplicationName = (tamanio_buffer) ? buffer : "";
							break;

				case WTSWorkingDirectory:
							if (WTSQuerySessionInformation (handle_server, info_session->SessionId, WTSWorkingDirectory, &buffer, &tamanio_buffer))
								temp_sesion._WTSWorkingDirectory = (tamanio_buffer) ? buffer : "";
							break;

				case WTSInitialProgram:
							if (WTSQuerySessionInformation (handle_server, info_session->SessionId, WTSInitialProgram, &buffer, &tamanio_buffer))
								temp_sesion._WTSInitialProgram= (tamanio_buffer) ? buffer : "";
							break;

				case WTSOEMId:
							if (WTSQuerySessionInformation (handle_server, info_session->SessionId, WTSOEMId, &buffer, &tamanio_buffer))
								temp_sesion._WTSOEMId= (tamanio_buffer) ? buffer : "";
							break;

				case WTSDomainName:
							if (WTSQuerySessionInformation (handle_server, info_session->SessionId, WTSDomainName, &buffer, &tamanio_buffer))
								temp_sesion._WTSDomainName= (tamanio_buffer) ? buffer : "";
							break;

				case WTSClientDirectory:
							if (WTSQuerySessionInformation (handle_server, info_session->SessionId, WTSClientDirectory, &buffer, &tamanio_buffer))
								temp_sesion._WTSClientDirectory= (tamanio_buffer) ? buffer : "";
							break;
					} // case

				if (buffer)
					WTSFreeMemory(buffer);

				numero_campo++;
				} // while

			info.push_back(temp_sesion);

		info_session++;

		} // for de sessiones

		WTSFreeMemory (inicio);

	}catch (const string& e) {cerr << "Error:" << e;};

	cerrar();

	return info;
}

vector<GuruWTS::proceso>& GuruWTS::procesos(vector<GuruWTS::proceso>& info, const string& computadora, DWORD sessionid, const string& username, DWORD processid)
{
	info.clear();

	try {
		if (!abrir(computadora)) throw GuruError().mensaje();

		WTS_PROCESS_INFO*	buffer = NULL;
		WTS_PROCESS_INFO*	inicio_buffer = NULL;
		DWORD				cantidad = 0;
		char				nombre_usuario[64];
		DWORD				tamanio_nombre_usuario = 64;
		char				nombre_dominio[64];
		DWORD				tamanio_nombre_dominio = 64;
		SID_NAME_USE		tipo_cuenta;

		if (!WTSEnumerateProcesses(handle_server, 0, 1, &buffer,  &cantidad)) throw GuruError().descripcion();

		inicio_buffer = buffer;

		for (DWORD i = 0; i < cantidad; i++, buffer++) {
			GuruWTS::proceso temp_proceso;

			temp_proceso.proceso_nombre = buffer->pProcessName;

			temp_proceso.proceso_id = buffer->ProcessId;
			temp_proceso.session_id = buffer->SessionId;

			if (IsValidSid(buffer->pUserSid)) {
				std::memset(nombre_usuario, 0, sizeof(nombre_usuario));
				std::memset(nombre_dominio, 0, sizeof(nombre_dominio));

				if (LookupAccountSid(NULL, buffer->pUserSid, nombre_usuario, &tamanio_nombre_usuario, nombre_dominio, &tamanio_nombre_dominio, &tipo_cuenta)) {
					temp_proceso.usuario_nombre = nombre_usuario;
					temp_proceso.dominio_nombre = nombre_dominio;
				}
			} 

			if (sessionid != -1 && sessionid != buffer->SessionId) continue;
			
			if (processid != -1 && processid != buffer->ProcessId) continue;

			if (!username.empty() && !GuruStringAlgo::stringequals(username, string(nombre_usuario))) continue;

			info.push_back(temp_proceso);
		}

		WTSFreeMemory(inicio_buffer);
	}catch (const string& e) {cerr << "Error:" << e;};

	cerrar();

	return info;
}

vector<WTSINFOEX_LEVEL1>& GuruWTS::info_level1(string& computadora, vector<WTSINFOEX_LEVEL1>& info1)
{
	info1.clear();

	try { 
		if (!abrir(computadora)) throw GuruError().mensaje();

		WTS_SESSION_INFO*	info_session;
		WTS_SESSION_INFO*	inicio;
		DWORD				cant_sesiones = 0;

		if (!WTSEnumerateSessions(handle_server, 0, 1, &info_session, &cant_sesiones)) throw GuruError().descripcion();

		inicio = info_session;

		for (DWORD i = 0; i < cant_sesiones; i++) {
			char*	buffer = NULL;
			DWORD	tamanio_buffer = 0;

			if (WTSQuerySessionInformation(handle_server, info_session->SessionId, WTSSessionInfoEx, &buffer, &tamanio_buffer)) {
				WTSINFOEX_LEVEL1 tmp;

				tmp = *(WTSINFOEX_LEVEL1*)buffer;

				info1.push_back(tmp);

				if (buffer)
					WTSFreeMemory(buffer);

			} else {
				cout << GuruError().descripcion() << endl;
			}

			info_session++;
		}

		WTSFreeMemory(inicio);

	} catch (const string& e) { cerr << "Error:" << e; };

	cerrar();

	return info1;
}

vector<WTSCLIENT>& GuruWTS::info_cliente(string& computadora, vector<WTSCLIENT>& info) {

	info.clear();

	return info;
}

bool GuruWTS::terminar_proceso(DWORD id_proceso, DWORD codigo_salida)
{
	return terminar_proceso (servidor, id_proceso, codigo_salida);
}

bool GuruWTS::terminar_proceso (string& servidor, DWORD id_proceso, DWORD codigo_salida)
{
	if (abrir(servidor)) {
		if (WTSTerminateProcess(handle_server, id_proceso, codigo_salida))
			return true;
		else {
			GuruError().descripcion();
			return false;
		}
	} else
		return false;
}

bool GuruWTS::cerrar_session(DWORD sid, bool esperar)
{
	return cerrar_session(servidor, sid, esperar);
}

bool GuruWTS::cerrar_session(string& servidor, DWORD sid, bool esperar)
{
	bool salida = true;;
	try{
		if (!abrir(servidor)) throw GuruError().mensaje();

		if (!WTSLogoffSession(handle_server, sid, esperar)) GuruError().descripcion();

	}catch (const string&) {salida = false;}

	cerrar();
	return salida;
}

bool GuruWTS::desconectar_session (DWORD sid, bool esperar)
{
	return desconectar_session (servidor, sid, esperar);
}

bool GuruWTS::desconectar_session (string& servidor, DWORD sid, bool esperar)
{
	if (abrir(servidor)) {
		if (WTSDisconnectSession(handle_server, sid, esperar))
			return true;
		else {
			GuruError().descripcion();
			return false;
		}
	} else
		return false;
}

//////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////
bool GuruWTS::apagar_servidor ()
{
	return apagar_servidor (servidor);
}

//////////////////////////////////////////////////////////////////////
//
// forma :
//		    WTS_WSD_LOGOFF
//			WTS_WSD_POWEROFF
//			WTS_WSD_REBOOT
//			WTS_WSD_SHUTDOWN
//////////////////////////////////////////////////////////////////////
bool GuruWTS::apagar_servidor (string& servidor)
{
	if (abrir(servidor)) {
		if (WTSShutdownSystem(handle_server, WTS_WSD_POWEROFF))
			return true;
		else {
			GuruError().descripcion();
			return false;
		}
	} else
		return false;

}

//////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////
bool GuruWTS::reiniciar_servidor ()
{
	return apagar_servidor (servidor);
}

//////////////////////////////////////////////////////////////////////
//
// forma :
//		    WTS_WSD_LOGOFF
//			WTS_WSD_POWEROFF
//			WTS_WSD_REBOOT
//			WTS_WSD_SHUTDOWN
//////////////////////////////////////////////////////////////////////
bool GuruWTS::reiniciar_servidor (string& servidor)
{
	if (abrir(servidor)) {
		if (WTSShutdownSystem(handle_server, WTS_WSD_REBOOT))
			return true;
		else {
			GuruError().descripcion();
			return false;
		}
	} else
		return false;
}

bool GuruWTS::enviar_mensaje(DWORD sid, const char *mensaje, const char *titulo, bool esperar)
{
	return enviar_mensaje(servidor, sid, mensaje, titulo, esperar);
}

bool GuruWTS::enviar_mensaje(string& servidor, DWORD sid, const char *mensaje, const char *titulo, bool esperar)
{
	if (abrir(servidor))
		{
		DWORD largo_mensaje = 0;
		DWORD largo_titulo = 0;
		DWORD respuesta = 0;

		if (mensaje)
			largo_mensaje = (DWORD)strlen(mensaje);

		if (titulo)
			largo_titulo = (DWORD)strlen(titulo);

		if (WTSSendMessage(handle_server, sid, (char *)titulo, largo_titulo, (char *)mensaje, largo_mensaje, MB_OK, false, &respuesta, esperar))
			return true;
		else
			{
			GuruError().descripcion();
			return false;
			}
		}
	else
		return false;
}

//////////////////////////////////////////////////////////////////////
// Este metodo convierte la constante definida por wtsapi del estado
// de un usuario a un formato de cadena legible.
//////////////////////////////////////////////////////////////////////
const char *GuruWTS::estado_a_str(int estado)
{
	switch (estado)
		{
	case WTSActive:
					return WTS_ESTADO_ACTIVE;
					break;
	case WTSConnected:
					return WTS_ESTADO_CONNECTED;
					break;
	case WTSConnectQuery:
					return WTS_ESTADO_CONNECTQUERY;
					break;
	case WTSShadow:
					return WTS_ESTADO_SHADOW;
					break;
	case WTSDisconnected:
					return WTS_ESTADO_DISCONNECTED;
					break;
	case WTSIdle:
					return WTS_ESTADO_IDLE;
					break;
	case WTSListen:
					return WTS_ESTADO_LISTEN;
					break;
	case WTSReset:
					return WTS_ESTADO_RESET;
					break;
	case WTSDown:
					return WTS_ESTADO_DOWN;
					break;
	case WTSInit:
					return WTS_ESTADO_INIT;
					break;
	default:
					return WTS_ESTADO_DESCONOCIDO;
			break;
		}
}

int GuruWTS::str_a_estado(char *estado)
{
	if (stricmp(estado, WTS_ESTADO_ACTIVE))
		return WTSActive;
	else if (stricmp(estado, WTS_ESTADO_CONNECTED))
		return WTSConnected;
	else if (stricmp(estado, WTS_ESTADO_CONNECTQUERY))
		return WTSConnectQuery;
	else if (stricmp(estado, WTS_ESTADO_SHADOW))
		return WTSShadow;
	else if (stricmp(estado, WTS_ESTADO_DISCONNECTED))
		return WTSDisconnected;
	else if (stricmp(estado, WTS_ESTADO_IDLE))
		return WTSIdle;
	else if (stricmp(estado, WTS_ESTADO_LISTEN))
		return WTSListen;
	else if (stricmp(estado, WTS_ESTADO_RESET))
		return WTSReset;
	else if (stricmp(estado, WTS_ESTADO_DOWN))
		return WTSDown;
	else if (stricmp(estado, WTS_ESTADO_INIT))
		return WTSInit;
	else return WTSActive;
}

bool GuruWTS::sid_a_nombre_usuario(PSID sid, char *nombre_usuario)
{
	char	cuenta[256];
	char	dominio[256];
	DWORD	tamanio_cuenta = sizeof(cuenta);
	DWORD	tamanio_dominio = sizeof(dominio);
	SID_NAME_USE nombre;

	int salida = LookupAccountSid(NULL,				// name of local or remote computer
									sid,				// security identifier
									cuenta,				// account name buffer
									&tamanio_cuenta,	// size of account name buffer
									dominio,			// domain name
									&tamanio_dominio,	// size of domain name buffer
									&nombre);			// SID type
	if (salida)
		sprintf(nombre_usuario,"%s\\%s",dominio, cuenta);
	else
		GuruError().descripcion();

	return (salida != 0) ? true : false;
}

HANDLE GuruWTS::handle()
{
	return handle_server;
}
