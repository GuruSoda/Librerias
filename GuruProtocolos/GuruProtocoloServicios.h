// GuruProtocoloServicios.h: interface for the GuruProtocoloServicios class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GURUPROTOCOLOSERVICIOS_H__BCE4EB2A_5C11_4643_9583_45069AAA23D5__INCLUDED_)
#define AFX_GURUPROTOCOLOSERVICIOS_H__BCE4EB2A_5C11_4643_9583_45069AAA23D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GuruSocketProtocolo.h"
#include "GuruServicios.h"

// enum GSPS_estados 

// typedef struct STR_ESTADO

#define	GSPS_SEPARADOR ";"

//////////////////////////////////////////////////////////////////////
// Comandos
//////////////////////////////////////////////////////////////////////
const char GSPS_CMD_SERVICIOS[] = "Servicios";
const char GSPS_CMD_ACTIVOS[] = "Activos";
const char GSPS_CMD_DETENIDOS[] = "Detenidos";
const char GSPS_CMD_PAUSADOS[] = "Pausados";

const char GSPS_CMD_ACTIVAR[] = "Activar";
const char GSPS_CMD_DETENER[] = "Detener";
const char GSPS_CMD_PAUSAR[] = "Pausar";
const char GSPS_CMD_CONTINUAR[] = "Continuar";

const char GSPS_CMD_BORRAR[] = "Borrar";
const char GSPS_CMD_AGREGAR[] = "Agregar";
const char GSPS_CMD_CAMBIAR[] = "Cambiar";


const char GSPS_CMD_NOMBRE_DISPLAY[] = "Display";
const char GSPS_CMD_NOMBRE_INTERNO[] = "Interno";

const char GSPS_CMD_TIPO[] = "Tipo";
const char GSPS_CMD_ESTADO[] = "Estado";
const char GSPS_CMD_DEPENDENCIAS[] = "Dependencias";
const char GSPS_CMD_INICIO[] = "Inicio";
const char GSPS_CMD_CONTROL_ERROR[] = "ControlError";
const char GSPS_CMD_ARCHIVO[] = "Archivo";
const char GSPS_CMD_INTERACTIVO[] = "Interactivo";
const char GSPS_CMD_USUARIO[] = "Usuario";
const char GSPS_CMD_OPC_CLAVE[] = "Clave";

const char GSPS_CMD_SELECCIONAR[] = "Seleccionar";
const char GSPS_CMD_LIBERAR[] = "Liberar";

const char GSPS_CMD_SERVIDOR[] = "Servidor";
const char GSPS_CMD_BASE_LOCAL[] = "BaseLocal";

const char GSPS_CMD_OPC_LIMPIAR[] = "Limpiar";
const char GSPS_CMD_OPC_DETALLADO[] = "Detallado";

#define	GSPS_CMD_OPCIONES_AGREGAR {GSPS_CMD_NOMBRE_DISPLAY, GSPS_CMD_NOMBRE_INTERNO, GSPS_CMD_TIPO, GSPS_CMD_DEPENDENCIAS, GSPS_CMD_INICIO, GSPS_CMD_CONTROL_ERROR, GSPS_CMD_ARCHIVO, GSPS_CMD_INTERACTIVO, GSPS_CMD_USUARIO, GSPS_CMD_OPC_CLAVE}
#define	GSPS_CMD_OPCIONES_AGREGAR {GSPS_CMD_NOMBRE_DISPLAY, GSPS_CMD_NOMBRE_INTERNO, GSPS_CMD_TIPO, GSPS_CMD_DEPENDENCIAS, GSPS_CMD_INICIO, GSPS_CMD_CONTROL_ERROR, GSPS_CMD_ARCHIVO, GSPS_CMD_INTERACTIVO, GSPS_CMD_USUARIO, GSPS_CMD_OPC_CLAVE}
#define	GSPS_CMD_OPCIONES_CAMBIAR {GSPS_CMD_NOMBRE_DISPLAY, GSPS_CMD_TIPO, GSPS_CMD_DEPENDENCIAS, GSPS_CMD_INICIO, GSPS_CMD_CONTROL_ERROR, GSPS_CMD_ARCHIVO, GSPS_CMD_INTERACTIVO, GSPS_CMD_OPC_CLAVE, GSPS_CMD_USUARIO}


//////////////////////////////////////////////////////////////////////
// Mensajes
//////////////////////////////////////////////////////////////////////
const char GSPS_MSG_SIN_SELECCION[] = "Servicio no seleccionado";
const char GSPS_MSG_PARAMETRO_DESCONOCIDO[] = "Parametro desconocido";
const char GSPS_MSG_HABILITADO[] = "Habilitado";

#define				MENSAJES_V1

#if defined MENSAJES_V1
	////////////////////// estado del servicio ///////////////////////////
	const char GSPS_MSG_ACTIVO[] = "Activo";
	const char GSPS_MSG_DETENIDO[] = "Detenido";
	const char GSPS_MSG_PAUSA[] = "Pausa";
	const char GSPS_MSG_ACTIVANDO[] = "Activando";
	const char GSPS_MSG_DETENIENDO[] = "Deteniendo";
	const char GSPS_MSG_INICIANDO_PAUSA[] = "Iniciando Pausa";
	const char GSPS_MSG_INICIANDO_CONTINUACION[] = "Iniciando Continuacion";

	////////////////////// inicio del servicio ///////////////////////////
	const char GSPS_MSG_MANUAL[] = "Manual";
	const char GSPS_MSG_AUTOMATICO[] = "Automatico";
	const char GSPS_MSG_DESHABILITADO[] = "Deshabilitado";
	const char GSPS_MSG_BOOT[] = "Boot";
	const char GSPS_MSG_SISTEMA[] = "Sistema";
	const char GSPS_MSG_DESCONOCIDO[] = "Desconocido";

	////////////////////// control de error del servicio /////////////////
	const char GSPS_MSG_IGNORAR[] = "Ignorar";
	const char GSPS_MSG_NORMAL[] = "Normal";
	const char GSPS_MSG_SEVERO[] = "Severo";
	const char GSPS_MSG_CRITICO[] = "Critico";

	////////////////////// tipo de servicio //////////////////////////////
	const char GSPS_MSG_DRIVER_FILE_SYSTEM[] = "DriverSistemaArchivos";
	const char GSPS_MSG_DRIVER_KERNEL[] = "DriverKernel";
	const char GSPS_MSG_PROCESO_WIN32[] = "ServicioPropietario";
	const char GSPS_MSG_PROCESO_SHARE_WIN32[] = "ServicioCompartido";

#elif MENSAJES_V2
	////////////////////// estado del servicio ///////////////////////////
	const char GSPS_MSG_ACTIVO[] = "SERVICE_RUNNING";
	const char GSPS_MSG_DETENIDO[] = "SERVICE_STOPPED";
	const char GSPS_MSG_PAUSA[] = "SERVICE_PAUSED";
	const char GSPS_MSG_ACTIVANDO[] = "SERVICE_START_PENDING";
	const char GSPS_MSG_DETENIENDO[] = "SERVICE_STOP_PENDING";
	const char GSPS_MSG_INICIANDO_PAUSA[] = "SERVICE_PAUSE_PENDING";
	const char GSPS_MSG_INICIANDO_CONTINUACION[] = "SERVICE_CONTINUE_PENDING";

	////////////////////// inicio del servicio ///////////////////////////
	const char GSPS_MSG_MANUAL[] = "SERVICE_DEMAND_START";
	const char GSPS_MSG_AUTOMATICO[] = "SERVICE_AUTO_START";
	const char GSPS_MSG_DESHABILITADO[] = "SERVICE_DISABLED";
	const char GSPS_MSG_BOOT[] = "SERVICE_BOOT_START";
	const char GSPS_MSG_SISTEMA[] = "SERVICE_SYSTEM_START";
	const char GSPS_MSG_DESCONOCIDO[] = "Desconocido";

	////////////////////// control de error del servicio /////////////////
	const char GSPS_MSG_IGNORAR[] = "SERVICE_ERROR_IGNORE";
	const char GSPS_MSG_NORMAL[] = "SERVICE_ERROR_NORMAL";
	const char GSPS_MSG_SEVERO[] = "SERVICE_ERROR_SEVERE";
	const char GSPS_MSG_CRITICO[] = "SERVICE_ERROR_CRITICAL";

	////////////////////// tipo de servicio //////////////////////////////
	const char GSPS_MSG_DRIVER_FILE_SYSTEM[] = "SERVICE_FILE_SYSTEM_DRIVER";
	const char GSPS_MSG_DRIVER_KERNEL[] = "SERVICE_KERNEL_DRIVER";
	const char GSPS_MSG_PROCESO_WIN32[] = "SERVICE_WIN32_OWN_PROCESS";
	const char GSPS_MSG_PROCESO_SHARE_WIN32[] = "SERVICE_WIN32_SHARE_PROCESS";
#endif


#define		GSPS_CMD_OPCIONES_AGREGAR_INICIO {GSPS_MSG_MANUAL, GSPS_MSG_AUTOMATICO, GSPS_MSG_DESHABILITADO, GSPS_MSG_BOOT, GSPS_MSG_SISTEMA}
#define		GSPS_CMD_OPCIONES_AGREGAR_ERROR  {GSPS_MSG_IGNORAR, GSPS_MSG_NORMAL, GSPS_MSG_SEVERO, GSPS_MSG_CRITICO}
#define		GSPS_CMD_OPCIONES_AGREGAR_TIPO   {GSPS_MSG_DRIVER_FILE_SYSTEM, GSPS_MSG_DRIVER_KERNEL, GSPS_MSG_PROCESO_WIN32, GSPS_MSG_PROCESO_SHARE_WIN32}


//////////////////////////////////////////////////////////////////////
// Mensajes de ayuda
//////////////////////////////////////////////////////////////////////
const char GSPS_HLP_SERVICIOS[] = "Ayuda No definida";
const char GSPS_HLP_ACTIVOS[] = "Ayuda No definida";
const char GSPS_HLP_DETENIDOS[] = "Ayuda No definida";
const char GSPS_HLP_PAUSADOS[] = "Ayuda No definida";

const char GSPS_HLP_ACTIVAR[] = "Ayuda No definida";
const char GSPS_HLP_DETENER[] = "Ayuda No definida";
const char GSPS_HLP_PAUSAR[] = "Ayuda No definida";
const char GSPS_HLP_CONTINUAR[] = "Ayuda No definida";

const char GSPS_HLP_BORRAR[] = "Ayuda No definida";
const char GSPS_HLP_AGREGAR[] = "Ayuda No definida";

const char GSPS_HLP_NOMBRE_DISPLAY[] = "Ayuda No definida";
const char GSPS_HLP_NOMBRE_INTERNO[] = "Ayuda No definida";

const char GSPS_HLP_TIPO[] = "Ayuda No definida";
const char GSPS_HLP_ESTADO[] = "Ayuda No definida";
const char GSPS_HLP_DEPENDENCIAS[] = "Ayuda No definida";
const char GSPS_HLP_INICIO[] = "Ayuda No definida";
const char GSPS_HLP_CONTROL_ERROR[] = "Ayuda No definida";
const char GSPS_HLP_ARCHIVO[] = "Ayuda No definida";
const char GSPS_HLP_INTERACTIVO[] = "Ayuda No definida";
const char GSPS_HLP_USUARIO[] = "Ayuda No definida";

const char GSPS_HLP_SELECCIONAR[] = "Ayuda No definida";
const char GSPS_HLP_LIBERAR[] = "Ayuda No definida";

const char GSPS_HLP_SERVIDOR[] = "Ayuda No definida";
const char GSPS_HLP_BASE_LOCAL[] = "Ayuda No definida";

typedef	struct
		{
		char*	nombre_interno;
		char*	nombre_display;
		dword	acceso;
		dword	tipo;
		dword	inicio;
		dword	controlerror;
		char*	archivo;
		char**	dependencias;
		char*	usuario;
		char*	clave;
		}data_servicio;

class GuruProtocoloServicios : public GuruSocketProtocolo
{
public:
	GuruProtocoloServicios(SOCKET socket);
	virtual ~GuruProtocoloServicios();

	void protocolo();

protected:

	static void cmd_servicios(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_activos(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_detenidos(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_pausados(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_activar(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_detener(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_pausar(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_continuar(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_borrar(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_agregar(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_cambiar(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_nombre_display(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_nombre_interno(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_tipo(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_estado(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_dependencias(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_inicio(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_control_error(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_archivo(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_interactivo(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_usuario(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_seleccionar(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_liberar(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_servidor(int cant_param, char *parametros[], GuruProtocoloServicios * esto);
	static void cmd_baselocal(int cant_param, char *parametros[], GuruProtocoloServicios * esto);

	bool		parametro_valido(int cant_param, char *parametros[], char **buffer);
	bool		existe_parametro(const char *lista_parametros[], word tamanio_lista, char *parametro);

	dword		cadenas_inicio_a_dword(char *cadena_inicio);
	dword		cadenas_error_a_dword(char *cadena_error);
	dword		cadenas_tipo_a_dword(char *cadena_tipo);


protected:

	char *	host;
	char *  Servicio_Seleccionado;

	data_servicio	nuevo_servicio;
};

#endif // !defined(AFX_GURUPROTOCOLOSERVICIOS_H__BCE4EB2A_5C11_4643_9583_45069AAA23D5__INCLUDED_)
