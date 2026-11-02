#if !defined GURUSVC_H__
#define GURUSVC_H__

//////////////////////////////////////////////////////////////////////
//	Link: Advapi32.lib
//////////////////////////////////////////////////////////////////////

#include "GuruCompat.h"

#ifndef VERSION_GURUSVC
	#define VERSION_GURUSVC		"Compilacion: [" __DATE__ " - " __TIME__ "]"
	#pragma once
#endif

//////////////////////////////////////////////////////////////////////////////////////
//	SERVICE_STOPPED							The service is not running.
//	SERVICE_START_PENDING					The service is starting. 
//	SERVICE_STOP_PENDING					The service is stopping. 
//	SERVICE_RUNNING							The service is running. 
//	SERVICE_CONTINUE_PENDING				The service continue is pending. 
//	SERVICE_PAUSE_PENDING					The service pause is pending. 
//	SERVICE_PAUSED							The service is paused. 
//////////////////////////////////////////////////////////////////////////////////////

#include <GuruError.hpp>
#include <GuruLog.h>

#ifdef __GURUWIN32__
#include <GuruDirectorio.hpp>
#include "GuruRegistry.hpp"
#include "GuruSvcMsg.h"
#include <Windows.h>
#include <Winsvc.h>
#elif defined __GURULINUX__
#include <GuruString_algo.hpp>
#include <signal.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <pwd.h>
#include <stdlib.h>
#endif

#include <iostream>
#include <string>

using namespace std;

class GURU_API GuruSvc
{
public:
	GuruSvc(const string& nombre_interno,const string& nombre_display, bool msgOnDll=false);
	GuruSvc();
	virtual ~GuruSvc();

	bool	StartService(const std::string& comoUsuario="");
	bool	StartApplication();
	void	ConfiguracionServicio(const string& parametro);
	virtual void ParametrosServicio(int cant,char* parametros[]);
	virtual bool Inicializo();
	void    Detener();
	virtual void EsperarFinalizacion();

#ifdef __GURUWIN32__
	static void	EscribirEvento(WORD tipo, DWORD id, const char* parametros[]);
#else
	static void	EscribirEvento(uint16_t tipo, uint32_t id, const char* parametros[]);
#endif
private:
#ifdef __GURUWIN32__
	void	AbrirScm();
	void	AbrirServicio(const string& nombre_servicio);
	void	CerrarScm();
	void	CerrarServicio();
	void	CerrarHandles();

	void	Instalar();
	void	DesInstalar();
	bool	RegistrarServicioEnVisorEventos(const char* path_ejecutable);
	bool	DesRegistrarServicioEnVisorEventos();

	void	PongoEstadoServicio(DWORD Nuevo_Estado);
	void	PongoAutomatico();
	void	PongoManual();
	void	PongoDeshabilitado();
	void	CambiarTipoInicio(DWORD inicio);
	void	CambiarUsuarioClave(const string& usuario,const string& clave,const string& servicio);

	SC_HANDLE HandleServicio();
	SC_HANDLE HandleScm();

#endif
	virtual bool Control_Consola(uint32_t requerimiento);

	string	nombre_interno;
	string	nombre_display;
private:
#ifdef __GURUWIN32__
	static	void WINAPI ServiceMain(DWORD dwArgc,LPTSTR *lpszArgv);
	static	void WINAPI Handler(DWORD fdwControl);
	static  BOOL WINAPI Rutina_Control_Consola(DWORD dwCtrlType);
#elif defined __GURULINUX__
	static void seniales (int nose);
#endif
	void    PreStart();

	virtual void Start() = 0;
	virtual void Stop() {};// SERVICE_CONTROL_STOP
	virtual void Pause() {};// SERVICE_CONTROL_PAUSE
	virtual void Continue() {};// SERVICE_CONTROL_CONTINUE
	virtual void Interrogate() {};// SERVICE_CONTROL_INTERROGATE
	virtual void ShutDown() {};// SERVICE_CONTROL_SHUTDOWN

#ifdef __GURUWIN32__
	virtual void UserDefined(DWORD requerimiento) {};// definido por el usuario
#endif

protected:

#ifdef __GURUWIN32__
// =================== para el control del servicio ========================================
	DWORD	tipo_servicio; 
//	valores posibles: 
//	SERVICE_WIN32_OWN_PROCESS		Specifies a service application that runs in its own process. 
//	SERVICE_WIN32_SHARE_PROCESS		Specifies a service application that shares a process with other services. 
//	SERVICE_KERNEL_DRIVER			Specifies a driver service. 
//	SERVICE_FILE_SYSTEM_DRIVER		Specifies a file system driver service.

	DWORD	comienzo;
//	valores posibles:
//	SERVICE_BOOT_START				Specifies a device driver started by the system loader. This value is valid only for driver services. 
//	SERVICE_SYSTEM_START			Specifies a device driver started by the IoInitSystem function. This value is valid only for driver services. 
//	SERVICE_AUTO_START				Specifies a service to be started automatically by the service control manager during system startup. 
//	SERVICE_DEMAND_START			Specifies a service to be started by the service control manager when a process calls the StartService function. 
//	SERVICE_DISABLED				Specifies a service that can no longer be started. 

	DWORD	control_error;
//	valores	posibles:
//	SERVICE_ERROR_IGNORE			The startup program logs the error but continues the startup operation. 
//	SERVICE_ERROR_NORMAL			The startup program logs the error and puts up a message box pop-up but continues the startup operation. 
//	SERVICE_ERROR_SEVERE			The startup program logs the error. If the last-known-good configuration is being started, the startup operation continues. Otherwise, the system is restarted with the last-known-good configuration. 
//	SERVICE_ERROR_CRITICAL			The startup program logs the error, if possible. If the last-known-good configuration is being started, the startup operation fails. Otherwise, the system is restarted with the last-known good configuration. 

	DWORD	acceso;
//	valores posibles:
//	ACCESS_SYSTEM_SECURITY			Enables calling of the QueryServiceObjectSecurity or SetServiceObjectSecurity function to access the SACL. The proper way to obtain this access is to enable the SE_SECURITY_NAME privilege in the caller's current access token, open the handle for ACCESS_SYSTEM_SECURITY access, and then disable the privilege.  
//	DELETE							Enables calling of the DeleteService function to delete the service. 
//	READ_CONTROL					Enables calling of the QueryServiceObjectSecurity function to query the security descriptor of the service object. 
//	WRITE_DAC						Enables calling of the SetServiceObjectSecurity function to modify the Dacl member of the service object's security descriptor. 
//	WRITE_OWNER						Enables calling of the SetServiceObjectSecurity function to modify the Owner and Group members of the service object's security descriptor. 

//	You can specify any or all of the following service object access types. Object access Description 
//	SERVICE_ALL_ACCESS				Includes STANDARD_RIGHTS_REQUIRED in addition to all of the access types listed in this table. 
//	SERVICE_CHANGE_CONFIG			Enables calling of the ChangeServiceConfig function to change the service configuration. 
//	SERVICE_ENUMERATE_DEPENDENTS	Enables calling of the EnumDependentServices function to enumerate all the services dependent on the service. 
//	SERVICE_INTERROGATE				Enables calling of the ControlService function to ask the service to report its status immediately. 
//	SERVICE_PAUSE_CONTINUE			Enables calling of the ControlService function to pause or continue the service. 
//	SERVICE_QUERY_CONFIG			Enables calling of the QueryServiceConfig function to query the service configuration. 
//	SERVICE_QUERY_STATUS			Enables calling of the QueryServiceStatus function to ask the service control manager about the status of the service. 
//	SERVICE_START					Enables calling of the StartService function to start the service. 
//	SERVICE_STOP					Enables calling of the ControlService function to stop the service. 
//	SERVICE_USER_DEFINED_CONTROL	Enables calling of the ControlService function to specify a user-defined control code. 


//	You can specify any of the following generic access types. Generic access Service access 
//	GENERIC_READ					Combines the following access types: STANDARD_RIGHTS_READ, SERVICE_QUERY_CONFIG, SERVICE_QUERY_STATUS, SERVICE_INTERROGATE, and SERVICE_ENUMERATE_DEPENDENTS. 
//	GENERIC_WRITE					Combines the following access types: STANDARD_RIGHTS_WRITE and SERVICE_CHANGE_CONFIG. 
//	GENERIC_EXECUTE					Combines the following access types: STANDARD_RIGHTS_EXECUTE, SERVICE_START, SERVICE_STOP, SERVICE_PAUSE_CONTINUE, and SERVICE_USER_DEFINED_CONTROL. 


	DWORD	aceptar;
//	posibles valores:
//	SERVICE_ACCEPT_STOP				The service can be stopped. This flag allows the service to receive the SERVICE_CONTROL_STOP value. 
//	SERVICE_ACCEPT_PAUSE_CONTINUE	The service can be paused and continued. This flag allows the service to receive the SERVICE_CONTROL_PAUSE and SERVICE_CONTROL_CONTINUE values. 
//	SERVICE_ACCEPT_SHUTDOWN			The service is notified when system shutdown occurs. This flag allows the service to receive the SERVICE_CONTROL_SHUTDOWN value. 
//	Note that ControlService cannot send this control code; only the system can send SERVICE_CONTROL_SHUTDOWN.
 
//	SERVICE_ACCEPT_PARAMCHANGE		Windows 2000: The service can reread its startup parameters without being stopped and restarted. This flag allows the service to receive the SERVICE_CONTROL_PARAMCHANGE value. 
//	SERVICE_ACCEPT_NETBINDCHANGE	Windows 2000: The service is a network component that can accept changes in its binding without being stopped and restarted. This flag allows the service to receive the SERVICE_CONTROL_NETBINDADD, SERVICE_CONTROL_NETBINDREMOVE, SERVICE_CONTROL_NETBINDENABLE, and SERVICE_CONTROL_NETBINDDISABLE values. 
//	SERVICE_ACCEPT_HARDWAREPROFILECHANGE Windows 2000: The service is notified when the computer's hardware profile has changed. This enables the system to send a SERVICE_CONTROL_HARDWAREPROFILECHANGE value to the service. The service receives this value only if it has called the RegisterServiceCtrlHandlerEx function. The ControlService function cannot send this control code. 
//	SERVICE_ACCEPT_POWEREVENT		Windows 2000: The service is notified when the computer's power status has changed. This enables the system to send a SERVICE_CONTROL_POWEREVENT value to the service. The service receives this value only if it has called the RegisterServiceCtrlHandlerEx function. The ControlService function cannot send this control code. 
#endif
// datos 
protected:
	static	GuruSvc*		Ptr_Svc;
	bool					MsgDll;
	bool					Ejecutando;
	
#ifdef __GURUWIN32__
	SERVICE_STATUS_HANDLE	status;
	SERVICE_STATUS			estado_servicio;

	SC_HANDLE				scm;
	SC_HANDLE				servicio;
	
	HANDLE					Corriendo;
#endif
};

#endif // !defined GURUSVC_H__

//////////////////////////////////////////////////////////////////////
//
//	Modo de uso de la clase:
//
//  1) Crear una clase Heredad de GuruSvc.
//	2) Sobrecargar las siguientes funciones:
//		2.0) GuruSvc (Contructor).
//		2.1) Metodos propios del servicio.
//		2.1.1)	Start.
//				Funcion principal del servicio.
//		2.1.2)	Stop.			(Opcional)
//				Llamada cuando desde el Panel de Control -> Servicios
//				presionan el boton Stop.
//		2.1.3)	Pause.			(Opcional)
//				Llamada cuando desde el Panel de Control -> Servicios
//				presionan el boton Pause.
//		2.1.4) 	Continue.		(Opcional)
//				Llamada cuando desde el Panel de Control -> Servicios
//				presionan el boton Continue.
//		2.1.5) 	Interrogate.	(Opcional).
//				Este metodo es llamado cuando el Service Control Manager
//				necesita datos de como se encuentra nuestro servicio.
//		2.1.6) 	ShutDown.		(Opcional)
//				Este metodo es llamado cuando al equipo donde esta corriendo
//				el servicio se esta apagando y reiniciando.
//		2.1.7) 	UserDefined.	(Opcional)
//				Metodo que recibe requerimiento que son definidos por el 
//				usuario (128 - 255).
//		2.1.8)	Inicializo.		(Opcional)
//				Este metodo se ejecuta antes de llamar a la funcion principal
//				(Start), y esta para realizar inicializaciones propias del 
//				nuevo servicio creado.
//
//////////////////////////////////////////////////////////////////////
//
//  Formas de llamar al constructor.
//
//  Forma 1.
//
//  Clase_Heredada::Clase_Heredada(const char *nombre_interno, const char *nombre_externo)
//		:GuruSvc(nombre_interno,nombre_externo)
//
//	Nota:	El "Nombre_interno" es aquel que recibe el servicio para ser marcado en
//			la registry, y no debe contener espacios.
//			El "Nombre_Externo" es aquel que se ve el la lista de servicios, tanto con
//			el comando "net start" o desde el panel de control -> Servicios.
//
//	Foram 2.
//
//	Clase_Heredada::Clase_Heredada()
//		Este contructor sirve para aquellos programas realizan tareas sobre servicios.
//		Nota: Estos programas no deben ser un servicio.
//		Ej: Cantidad de servicios, levantar un servicio, modificar un servicio, etc.
//
//////////////////////////////////////////////////////////////////////
//
//	2.2.1)
//	Ej: Metodo tipico para la funcion principal del servicio.
//
//	opcion 1:
//	void Clase_Heredada::Start()
//	{
//		while (Ejecutando)
//			{
//			Sleep(500); // espero medio segundo.
//			}
//	}
//
//	- o -
//	opcion 2:
//	void Clase_Heredada::Start()
//	{
//		do{
//			//.....;
//		}while(WaitForSingleObject(Corriendo, 5000) == WAIT_TIMEOUT);
//
//	}
//
// - o -
//	opcion 3:
//	void Clase_Heredada::Start()
//	{
//		WaitForSingleObject(Corriendo, INFINITE);
//	}
//
//	Variables disponibles de la clase heredada.
//
//	nombre_interno_servicio = Nombre interno que tiene el servicio.
//	display_nombre_servicio = Nombre externo que tiene el servicio.
//	logeo					= Para escribir archivos de log.
//	mensaje					= ?.
//	error					= Para la manipulacion de errores.
//	Ptr_Svc					= Puntero a la propia clase (this).
//	status					= Estdo actual del servicio.
//  estado_servicio			= Idem.
//
//	Las siguientes funciones son utilizadas para controlar servicios y no para
//	trabajar en un servicio, utilizado para funciones varias.
//	estado, mantener_scm, scm, servicio.
//	
//
//	2.1.8)
//  Ej: Sobrecarga de la funcion de inicializacion (Opcional)
//
//	bool Clase_Heredada::Inicializo()
//		{
//
//		...;
//		...;
//
//		return GuruSvc::Inicializo();
//		}
//
//
//////////////////////////////////////////////////////////////////////
//
//
// Como deberia ser un programa prinpial que llame al servicio:
// 
//	Forma 1.
//
//	int main (int cant, char *parametros[])
//	{
//	Clase_Heredada servicio("Nombre_Interno","Nombre Display");
//
//	if (cant == 2)
//		servicio.ParametrosServicio(parametros[1]);
//	else
//		servicio.StartService();
//
//	return 0;
//	}
//
//	Nota: 
//		El metodo "ParametrosServicio" verifica si el parametro pasado desde la linea
//		de comando es uno de los siguiente:
//		/Instalar o /DesInstalar.
//		En caso que sea "/Instalar" instala el servicio y muestra el estado por pantalla
//		En caso que sea "/DesInstalar" desinstala el servicio y muestra el estado por pantalla.
//		
//		Caso contrario termina el programa.
//
//
//
//	Forma 2.
//
//	int main (int cant, char *parametros[])
//	{
//	Clase_Heredada servicio("EspiaDir_Gw","El Espia de GwOut");
//
//	if (!servicio.EstaInstalado())
//		{
//		if (!servicio.Instalar())
//			{
//			printf("Error creando servicio ...\n");
//			return 1;
//			}
//		else
//			printf("El servicio a sido instalado correctamente \n");
//		}		
//	else
//		servicio.StartService();
//
//	return 0;
//	}
//
//
//	Forma 3.
//
//	int main(int argc, char* argv[])
//	{
//	Clase_Heredada servicio("catapulta","Catapulta a Filiales");
//
//	if (cant == 2)
//		{
//		if (stricmp(parametros[1], "/Apps") == 0)
//			servicio.StartApplication();
//		else
//			servicio.ParametrosServicio(parametros[1]);
//		}
//	else
//		servicio.StartService();
//
//	return 0;
//	}
//
//////////////////////////////////////////////////////////////////////
