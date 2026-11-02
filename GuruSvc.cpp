#include "GuruSvc.h"

GuruSvc *GuruSvc::Ptr_Svc = NULL;

#ifdef __GURUWIN32__

//contructor utilizado en aplicaciones del tipo servicios
GuruSvc::GuruSvc(const string& nombre_interno,const string& nombre_display, bool msgOnDll)
{
	Ptr_Svc = this;

	this->nombre_interno = nombre_interno;
	this->nombre_display = nombre_display;

	tipo_servicio	= SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
	comienzo		= SERVICE_AUTO_START;
	control_error	= SERVICE_ERROR_NORMAL;
	acceso			= SERVICE_ALL_ACCESS;
	aceptar			= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_CONTROL_INTERROGATE;

	Ejecutando		= false;
	Corriendo       = NULL;

	scm				= NULL;
	servicio		= NULL;

	MsgDll			= msgOnDll;
}

// contructor utilizado solamente para usar las funcion es de servicios
// No Utilizado  como parte de un servicio
GuruSvc::GuruSvc()
{
	Ejecutando		= false;
	scm				= NULL;
	servicio		= NULL;
}

GuruSvc::~GuruSvc()
{
	if (scm)
		CloseServiceHandle(scm);

	if (servicio)
		CloseServiceHandle(servicio);

	if (Corriendo != NULL)
		CloseHandle(Corriendo);
}

void GuruSvc::AbrirScm()
{
	scm = OpenSCManager(NULL,SERVICES_ACTIVE_DATABASE,SC_MANAGER_ALL_ACCESS);

	if (!scm) throw GuruError().descripcion();
}

void GuruSvc::AbrirServicio(const string& nombre_servicio)
{
	AbrirScm(); // primero abro el service control manager

	// despues abro el servicio
	servicio = OpenService(	scm,  // handle to service control manager database
							nombre_servicio.c_str(), // pointer to name of service to start
							SERVICE_ALL_ACCESS // type of access to service
							);
	if (!servicio) {
		string leyenda(GuruError().descripcion());
		CerrarScm();
		throw leyenda;
	}
}

void GuruSvc::CerrarScm()
{
	if (!CloseServiceHandle(scm)) throw GuruError().descripcion();
}

void GuruSvc::CerrarServicio()
{
	if (!CloseServiceHandle(servicio)) throw GuruError().descripcion();
}

void GuruSvc::CerrarHandles()
{
	CerrarScm();

	CerrarServicio();
}

void GuruSvc::Instalar()
{
// primero abro el service control manager
	AbrirScm();

// saco el nombre del propio ejecutable
	char path_ejecutable[MAX_PATH];
	if (!GetModuleFileName(	NULL,					// handle to module to find filename for
							path_ejecutable,		// pointer to buffer to receive module path
							sizeof(path_ejecutable) // size of buffer, in characters
							)) {
		string leyenda(GuruError().descripcion());
		CerrarScm();

		throw leyenda;
	}

	// creo el servicio
	SC_HANDLE handle_svc;
	handle_svc = CreateService(HandleScm(),				// handle to service control manager database
								nombre_interno.c_str(),	// pointer to name of service to start
								nombre_display.c_str(),	// pointer to display name
								acceso,						// type of access to service
								tipo_servicio,				// type of service
								comienzo,					// when to start service
								control_error,				// severity if service fails to start
								path_ejecutable,			// pointer to name of binary file
								NULL,	// pointer to name of load ordering group
								NULL,	// pointer to variable to get tag identifier
								NULL,	// pointer to array of dependency names
								NULL,	// pointer to account name of service
								NULL    // pointer to password for service account
								);

	if (!handle_svc){
		string leyenda(GuruError().descripcion());
		CerrarScm();
		throw leyenda;
	}

	if (!RegistrarServicioEnVisorEventos(path_ejecutable)) throw GuruError().descripcion();

	const char *parametros[] = {nombre_display.c_str(), VERSION_GURUSVC, NULL};

	EscribirEvento(EVENTLOG_INFORMATION_TYPE, GURUSVC_INSTALADO, parametros);

	CerrarScm();
}

void GuruSvc::DesInstalar()
{
	AbrirServicio(nombre_interno);

//	CerrarScm();

	if (!DeleteService(HandleServicio())){
		string leyenda(GuruError().descripcion());
		CerrarHandles();
		throw leyenda;
	}

	const char *parametros[] = {nombre_display.c_str(), VERSION_GURUSVC, NULL};

	EscribirEvento(EVENTLOG_INFORMATION_TYPE, GURUSVC_DESINSTALADO, parametros);

	DesRegistrarServicioEnVisorEventos();

	CerrarHandles();
}

bool GuruSvc::RegistrarServicioEnVisorEventos(const char* path_ejecutable)
{
	GuruRegistry	GuruRegedt32;
	std::string		rama_servicio;
	std::string		ruta_mensajes;

	rama_servicio = "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\" + nombre_display;

	if (MsgDll) {
		GuruDirectorio::GuruSplitPath split(path_ejecutable);

		ruta_mensajes = split.drive() + split.dir() + split.nombre() + "Msg.dll";
	} else 
		ruta_mensajes = path_ejecutable;

	GuruRegedt32.escribir("HKLM", rama_servicio.c_str(), "EventMessageFile", ruta_mensajes);
	GuruRegedt32.escribir("HKLM", rama_servicio.c_str(), "CategoryMessageFile", ruta_mensajes);
	GuruRegedt32.escribir("HKLM", rama_servicio.c_str(), "ParameterMessageFile", ruta_mensajes);

	GuruRegedt32.escribir("HKLM", rama_servicio.c_str(), "TypesSupported",  EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE);

	return true;
}

bool GuruSvc::DesRegistrarServicioEnVisorEventos()
{
	bool	salida = true;

	return	salida;
}

void GuruSvc::PongoAutomatico()
{
	CambiarTipoInicio(SERVICE_AUTO_START);
}

void GuruSvc::PongoManual()
{
	CambiarTipoInicio(SERVICE_DEMAND_START);
}

void GuruSvc::PongoDeshabilitado()
{
	CambiarTipoInicio(SERVICE_DISABLED);
}

void GuruSvc::CambiarUsuarioClave(const string& usuario,const string& clave,const string& servicio)
{
	AbrirServicio(servicio);

	if (!ChangeServiceConfig(
					HandleServicio(),			// handle to service
					SERVICE_NO_CHANGE,			// type of service
					SERVICE_NO_CHANGE,			// when to start service
					SERVICE_NO_CHANGE,			// severity of start failure
					NULL,						// service binary file name
					NULL,						// load ordering group name
					NULL,						// tag identifier
					NULL,						// array of dependency names
					usuario.c_str(),			// account name
					clave.c_str(),				// account password
					NULL						// display name
					))
		throw GuruError().descripcion();
}

void GuruSvc::CambiarTipoInicio(DWORD inicio)
{
	AbrirServicio(nombre_interno);

	if (!ChangeServiceConfig(HandleServicio(),
							SERVICE_NO_CHANGE,		// type of service
							inicio,					// when to start service
							SERVICE_NO_CHANGE,		// severity of start failure
							NULL,					// service binary file name
							NULL,					// load ordering group name
							NULL,					// tag identifier
							NULL,					// array of dependency names
							NULL,					// account name
							NULL,					// account password
							NULL					// display name
							)) {
		throw GuruError().descripcion();
	}

	CerrarHandles();
}

SC_HANDLE GuruSvc::HandleServicio()
{
	return servicio;
}

SC_HANDLE GuruSvc::HandleScm()
{
	return scm;
}

// Tarea: Llenar un vector con informacion de los siguientes campos.
/*			La informacion a retornar es filtrada por el parametro estado.
			El estado puede ser una de las siguiente constantes.
			SERVICE_ACTIVE		Enumerates services that are in the following states:
								SERVICE_START_PENDING, SERVICE_STOP_PENDING, SERVICE_RUNNING,
								SERVICE_CONTINUE_PENDING, SERVICE_PAUSE_PENDING, and SERVICE_PAUSED.

			SERVICE_INACTIVE	Enumerates services that are in the SERVICE_STOPPED state.
			SERVICE_STATE_ALL	Combines the following states: SERVICE_ACTIVE and SERVICE_INACTIVE.  */

/* Retorna : (Estructura)
			typedef struct _ENUM_SERVICE_STATUS {
					LPTSTR lpServiceName;
					LPTSTR lpDisplayName;
					SERVICE_STATUS ServiceStatus;
					} ENUM_SERVICE_STATUS, *LPENUM_SERVICE_STATUS; */


// Tarea:  controla si el parametro es instalar / desinstalar / Automatico / manual / deshabilitado.
void GuruSvc::ConfiguracionServicio(const string& parametro)
{
// ===================== para la ayuda del servicio ===========================
	if (parametro.compare("-?") == 0 || parametro.compare("/?") == 0 || stricmp(parametro.c_str(), "/ayuda") == 0 || parametro.compare("?") == 0)
		{
		cout << "Parametros Aceptados:" << endl << endl;
		cout << "Instalar      : Instala el servicio" << endl;
		cout << "DesInstalar   : Desinstala el servicio" << endl;
		cout << "Manual        : Tipo de inicio Manual" << endl;
		cout << "Automatico    : Tipo de inicio Automatico" << endl;
		cout << "Deshabilitado : Deshabilita el servicio" << endl;
		return ;
		}

	try {
// ===================== para instalar el servicio actual =====================
		if (stricmp(parametro.c_str(), "/Instalar") == 0) {
			Instalar();
			cout << "El servicio se ha instalado correctamente ..." << endl;
		}

// ===================== para desinstalarlo =================================
		if (stricmp(parametro.c_str(), "/DesInstalar") == 0) {
			DesInstalar();
			cout << "El servicio se ha desinstalado correctamente ..." << endl;
		}

// ================== para cambiarlo a "Manual" =============================
		if (stricmp(parametro.c_str(), "/Manual") == 0) {
			PongoManual();
			cout << "El servicio se a cambiado a Manual correctamente ..." << endl;
		}

// ================== para cambiarlo a "Automatico" =============================
		if (stricmp(parametro.c_str(), "/Automatico") == 0) {
			PongoAutomatico();
			cout << "Error al cambiar la configuracion a Automatico ..." << endl;
		}

// ================== para cambiarlo a "Deshabilitado" =============================
		if (stricmp(parametro.c_str(), "/Deshabilitado") == 0) {
			PongoDeshabilitado();
			cout << "El servicio se a cambiado a Deshabilitado correctamente ..." << endl;
		}

// =================== para cambiar el usuario =================
/*
		if (strnicmp(parametro,"/Usuario:",sizeof("/Usuario:")-1) == 0) {
			if (EstaInstalado()) {
				if (!PongoUsuario(&parametro[sizeof("/Usuario:")-1])) {
					printf("Error al cambiar el Usuario (%s)...\n",GuruSvc_Error.mensaje());
					return OK_PARAMETROS_ERROR;
				} else {
					printf("El servicio ha cambiado de Usuario correctamente ... \n");
					return OK_PARAMETROS;
				}
			}
		else // el servicio esta instalado
			{
				printf("El servicio No se encuentra instalado ...\n");
				return OK_PARAMETROS_ERROR;
				}
			}

// =================== para cambiar la clave =================
		if (strnicmp(parametro,"/Clave:",sizeof("/Clave:")-1) == 0)	{
			if (EstaInstalado()){
				if (!PongoClave(&parametro[sizeof("/Clave:")-1])){
					printf("Error al cambiar la clave ...\n");
					return OK_PARAMETROS_ERROR;
				} else {
					printf("El servicio ha cambiado de Clave correctamente ... \n");
					return OK_PARAMETROS;
				}
			}
		else // el servicio esta instalado
			{
				printf("El servicio No se encuentra instalado ...\n");
				return OK_PARAMETROS_ERROR;
			}
		}
*/
	}catch(string& e) { cout << e << endl;};
}

// =========================================================================================================
// Desde aqui: Los metodos son para realizar aplicacion de tipo servicio.
// =========================================================================================================
//

bool GuruSvc::StartService(const string&)
{
	SERVICE_TABLE_ENTRY tabla_servicios[] = {
						{(char*)nombre_interno.c_str(),ServiceMain},
						{NULL,NULL}};

	BOOL salida = StartServiceCtrlDispatcher(tabla_servicios);

	return (salida) ? true : false;
}

void GuruSvc::PreStart()
{
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)Rutina_Control_Consola, true);

// Con este flag mantengo al servicio ejecutandose
	Ejecutando = true; // el servicio esta corriendo

	Corriendo = CreateEvent(NULL, true, false, nombre_interno.c_str());
}

bool GuruSvc::StartApplication()
{
	GURULOG_INFO("Servicio Iniciado como aplicacion!");

	if (Inicializo())
		{
		PreStart();
		Start();

		return true;
		}
	else
		return false;
}

// Estos parametros vienen desde el SCM o del comando "Net stat SERVICO parametros"
void WINAPI GuruSvc::ServiceMain(DWORD dwArgc,LPTSTR* lpszArgv)
{
	GuruSvc *Ptr = Ptr_Svc;

	Ptr->status = RegisterServiceCtrlHandler(Ptr->nombre_interno.c_str(), // address of name of service
									(LPHANDLER_FUNCTION)Handler // address of handler function
									);

	if (!Ptr->status) return ;

// ======================================= pongo el estado que va a leer el SCM ======================
	Ptr->estado_servicio.dwServiceType = Ptr->tipo_servicio;
	Ptr->estado_servicio.dwControlsAccepted = Ptr->aceptar;
	Ptr->estado_servicio.dwWin32ExitCode = NO_ERROR;
	Ptr->estado_servicio.dwServiceSpecificExitCode = 0;
	Ptr->estado_servicio.dwCheckPoint = 0;
	Ptr->estado_servicio.dwWaitHint = 0;

// ============================ verifico los parametros ==================================
/*
	if (dwArgc == 2)
		if	(Ptr->ParametrosServicio(lpszArgv[1]) == OK_PARAMETROS)
			{
			Ptr->PongoEstadoServicio(SERVICE_START_PENDING);
			Ptr->PongoEstadoServicio(SERVICE_RUNNING);
			Ptr->PongoEstadoServicio(SERVICE_STOPPED);
			return ;
			}
*/

	Ptr->ParametrosServicio(dwArgc, lpszArgv);

// ===== Le digo al SCM que se esta levantando el servicio =========

	Ptr->PongoEstadoServicio(SERVICE_START_PENDING);

// ==== llamo a la funcion que inicializa el servicio ================
	if (Ptr->Inicializo() == false) {
		Ptr->PongoEstadoServicio(SERVICE_STOPPED);
		return ;
	}

// Confirugo una variable para mantener el servicio corriendo
// Tambien configuro un evento para mantenerlo corriendo

	Ptr->PreStart();

/////////////////////////////////////////////////////////////////////////////////////////////
//				Parte principal del servicio

	Ptr->PongoEstadoServicio(SERVICE_RUNNING);

	const char* parametros_inicio[] = {Ptr->nombre_display.c_str(), VERSION_GURUSVC, NULL};
	Ptr->EscribirEvento(EVENTLOG_INFORMATION_TYPE, GURUSVC_INICIADO, parametros_inicio);

	Ptr->Start();			// ejecuto la funcion principal del servicio

	const char* parametros_fin[] = {Ptr->nombre_display.c_str(), VERSION_GURUSVC, NULL};
	Ptr->EscribirEvento(EVENTLOG_INFORMATION_TYPE, GURUSVC_DETENIDO, parametros_fin);

// Cuando sale de la funcion, el servicio termino
	Ptr->PongoEstadoServicio(SERVICE_STOPPED);

//				Fin de la parte principal del servicio
/////////////////////////////////////////////////////////////////////////////////////////////
}
					// requested control code
VOID WINAPI GuruSvc::Handler(DWORD requerimiento)
{

	switch(requerimiento)
		{
	case SERVICE_CONTROL_STOP :
			Ptr_Svc->PongoEstadoServicio(SERVICE_STOP_PENDING);

			Ptr_Svc->Stop();

			Ptr_Svc->Ejecutando = false;
			SetEvent(Ptr_Svc->Corriendo);

			break;

	case SERVICE_CONTROL_PAUSE :		
			Ptr_Svc->PongoEstadoServicio(SERVICE_PAUSE_PENDING);

			Ptr_Svc->Pause();

			Ptr_Svc->PongoEstadoServicio(SERVICE_PAUSED);
			break;

	case SERVICE_CONTROL_CONTINUE :
			Ptr_Svc->PongoEstadoServicio(SERVICE_CONTINUE_PENDING);

			Ptr_Svc->Continue();
			break;

	case SERVICE_CONTROL_INTERROGATE :
			Ptr_Svc->Interrogate();
			break;

	case SERVICE_CONTROL_SHUTDOWN :
			Ptr_Svc->ShutDown();
			break;

			/* de 128 a 255 = es definido para el usuario */
	default :
			Ptr_Svc->UserDefined(requerimiento);
			break; // SERVICE_USER_DEFINED_CONTROL
		}
}

////////////////////////////////////////////////////////////////////////////////////////
//		SERVICE_STOPPED				The service is not running.				(1)
//		SERVICE_START_PENDING		The service is starting.				(2)
//		SERVICE_STOP_PENDING		The service is stopping.				(3)
//		SERVICE_RUNNING				The service is running.					(4)
//		SERVICE_CONTINUE_PENDING	The service continue is pending.		(5)
//		SERVICE_PAUSE_PENDING		The service pause is pending.			(6)
//		SERVICE_PAUSED				The service is paused.					(7)
////////////////////////////////////////////////////////////////////////////////////////

void GuruSvc::PongoEstadoServicio(DWORD Nuevo_Estado)
{
	estado_servicio.dwCurrentState = Nuevo_Estado;
	SetServiceStatus(status,&estado_servicio);
}

bool GuruSvc::Inicializo()
{
	return true;
}

void GuruSvc::Detener()
{
	Ejecutando = false;
	SetEvent(Corriendo);
}

void GuruSvc::Start()
{
	while (Ejecutando)
		Sleep(5000);
}

void GuruSvc::EsperarFinalizacion() {
	WaitForSingleObject(Corriendo, INFINITE);
}


bool GuruSvc::Control_Consola(uint32_t requerimiento)
{
	return false;
}

void GuruSvc::ParametrosServicio(int cant,char* parametros[])
{

}

// Rutina que esta registrada en modo aplicacion y se ejecuta
// cuando se produce un control C o Control Break
// Si la funcion retorna true, el operativo NO mata al proceso
// Si la funcion retorna false, el operativo mata al proceso
BOOL WINAPI GuruSvc::Rutina_Control_Consola(DWORD dwCtrlType)
{
	return Ptr_Svc->Control_Consola(dwCtrlType);
}

//////////////////////////////////////////////////////////////////////
// Tarea: Escribir en el Event Viewer un evento
// Parametros:
//				tipo = solo uno:
//					1)	EVENTLOG_ERROR_TYPE
//					2)	EVENTLOG_WARNING_TYPE
//					3)	EVENTLOG_INFORMATION_TYPE
//////////////////////////////////////////////////////////////////////
void GuruSvc::EscribirEvento(WORD tipo, DWORD id, const char *parametros[])
{
	WORD	cant_parametros = 0;

	while (parametros[cant_parametros++])
		;

	cant_parametros--;

	HANDLE hregistroev = RegisterEventSource(NULL, Ptr_Svc->nombre_display.c_str());

	BOOL salida = ReportEvent(hregistroev,	// handle to event log
		tipo,			// event type
		(WORD)0,		// event category
		id,				// event identifier
		NULL,		// user security identifier
		cant_parametros,    // number of strings to merge
		(DWORD)0,    // size of binary data
		parametros,  // array of strings to merge
		NULL     // binary data buffer
	);

	DWORD error = GetLastError();

	DeregisterEventSource(hregistroev);

	if (salida == 0)
		throw GuruError().descripcion(error);
}

#elif defined __GURULINUX__
GuruSvc::GuruSvc(const string& nombre_interno, const string& nombre_display, bool msgOnDll)
{
	Ptr_Svc = this;

	this->nombre_interno = nombre_interno;
	this->nombre_display = nombre_display;
	
	Ejecutando		= false;

	MsgDll			= msgOnDll;
}

GuruSvc::GuruSvc()
{
		
}

GuruSvc::~GuruSvc()
{
		
}

void GuruSvc::EsperarFinalizacion() {
	while (Ejecutando) sleep(1);
}

void GuruSvc::seniales (int nose)
{
	std::ostringstream oss;
	
	oss << "Llegando señal "; 

   switch(nose) {
    case SIGALRM: oss << "SIGALRM";break;
    case SIGUSR1: oss << "SIGUSR1";exit(EXIT_SUCCESS); break;
    case SIGCHLD: oss << "SIGCHLD";break;

    case SIGHUP: oss << "SIGHUP";break;
	case SIGINT: oss << "SIGINT";break;
	case SIGTERM: oss << "SIGTERM";break;
	case SIGQUIT: oss << "SIGQUIT";break;
	case SIGPIPE: oss << "SIGPIPE";break;
	default:
			oss << "Valor:" << nose;
    }

	oss << endl;

	syslog(LOG_INFO, "%s\n", oss.str().c_str());

	Ptr_Svc->Ejecutando = false;
}

bool GuruSvc::StartService(const std::string& comoUsuario)
{
	char	dir_actual[MAX_PATH];

	if (getcwd(dir_actual, MAX_PATH) == NULL)
		strcpy(dir_actual, ".");

	pid_t pid, sid;

	pid = fork();

	if (pid < 0) {
		syslog(LOG_ERR, "%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (pid > 0)
		exit(EXIT_SUCCESS);

	if (pid == 0) { // ya estoy en el hijo
	   // verifico si es root, para poder crear el archivo pid
    	if ( getuid() == 0 || geteuid() == 0 ) {
			openlog(nombre_display.c_str(), LOG_PID, LOG_DAEMON);

			string nombre_pid;

			nombre_pid = "/var/run/" + nombre_interno + ".pid";

			pid_t elpid=getpid();

			FILE* archivo_pid=fopen(nombre_pid.c_str(), "w");
		
			if (archivo_pid) {
				std::string strpid;
				strpid = GuruStringAlgo::itos(elpid);
				fwrite(&strpid[0], sizeof(char), strpid.length(), archivo_pid);
				fclose(archivo_pid);
			} else { 
				cout << nombre_pid << ": NO pudo ser creado (" << strerror(errno) << ")" << endl;
				syslog(LOG_ERR, "%s\n", "No pudo ser creado el archivo de pid");
			}

			if (!comoUsuario.empty()) {
	        		struct passwd* pw = getpwnam(comoUsuario.c_str());
		        	if ( pw ) {
    	        			syslog( LOG_NOTICE, "setting user");
	        		    	if (setuid( pw->pw_uid ))
    	        				syslog( LOG_ERR, "setuid user");
	        		} else {
						syslog(LOG_ERR, "%s\n", "No existe el usuario personificar");
				}
	    		}
		}

		// First, start a new session
		if((sid = setsid()) < 0) {
			syslog(LOG_ERR, "%s\n", "setsid");
//			exit(EXIT_FAILURE);
			return false;
		}

		if((chdir(dir_actual)) < 0) {
			syslog(LOG_ERR, "%s\n", "chdir");
//			exit(EXIT_FAILURE);
			return false;
		}

		// Reset the file mode
		umask(027);

		//Close unneeded file descriptors
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
/*
		FILE* f;
  		f = freopen("/dev/null", "r", stdin);
  		f = freopen("/dev/null", "w", stdout);
  		f = freopen("/dev/null", "w", stderr);
*/ 		
		signal(SIGHUP, Ptr_Svc->seniales);
		signal(SIGINT, Ptr_Svc->seniales);
		signal(SIGTERM, Ptr_Svc->seniales);
		signal(SIGQUIT, Ptr_Svc->seniales);
		signal(SIGPIPE, SIG_IGN);
		signal(SIGCHLD, SIG_IGN);

		Ejecutando = true;
/*
		syslog(LOG_INFO, "%s\n", "Iniciando el Servicio.");
		FILE* a;
		a=fopen("acaestoy.log", "w");
		if (a != NULL) {
			int b=fwrite("OK", sizeof(char), strlen("OK"), a);
			if (b==0 || b != sizeof(char)*strlen("ok")) cout << strerror(errno)  << endl;

			fclose(a);
		} else cout << strerror(errno)  << endl;

*/
//		kill(getppid(),SIGUSR1); // mato el padre

		syslog(LOG_INFO, "%s\n", "Iniciando el Servicio.");
		
		if (!Inicializo()) {
			syslog(LOG_INFO, "%s\n", "Error Inicializando el Servicio.");
			return false;
		}
		
		Start();

		syslog(LOG_INFO, "%s\n", "Deteniendo el Servicio.");
		}

	return true;
}

bool GuruSvc::StartApplication()
{
	if (Inicializo())
		{
		PreStart();

		Start();

		return true;
		}
	else
		return false;
}

void GuruSvc::ConfiguracionServicio(const string& parametro)
{

}

bool GuruSvc::Inicializo()
{
	return true;		
}

void GuruSvc::Detener()
{
		
}

void GuruSvc::PreStart()
{
	signal(SIGHUP, Ptr_Svc->seniales);
	signal(SIGINT, Ptr_Svc->seniales);
	signal(SIGTERM, Ptr_Svc->seniales);
	signal(SIGQUIT, Ptr_Svc->seniales);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);

/*
   // Cancel certain signals 
    signal(SIGCHLD,SIG_DFL); // A child process dies
    signal(SIGTSTP,SIG_IGN); // Various TTY signals
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP, SIG_IGN); // Ignore hangup signal
    signal(SIGTERM,SIG_DFL); // Die on SIGTERM 

*/
	Ejecutando = true;
}

bool GuruSvc::Control_Consola(uint32_t requerimiento)
{
	return false;		
}

void GuruSvc::ParametrosServicio(int cant,char* parametros[])
{
		
}
#ifdef __GURUWIN32__
void	GuruSvc::EscribirEvento(WORD tipo, DWORD id, const char* parametros[])
#else
void	GuruSvc::EscribirEvento(uint16_t tipo, uint32_t id, const char* parametros[])
#endif

{
}

#endif
