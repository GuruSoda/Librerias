// GuruProtocoloServicios.cpp: implementation of the GuruProtocoloServicios class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuruProtocoloServicios.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuruProtocoloServicios::GuruProtocoloServicios(SOCKET socket):GuruSocketProtocolo(socket)
{
	host					= NULL;
	Servicio_Seleccionado	= NULL;
}

GuruProtocoloServicios::~GuruProtocoloServicios()
{

}

void GuruProtocoloServicios::protocolo()
{
	cargar_comando(GSPS_CMD_SERVICIOS, (funcion_comando *)cmd_servicios, "Lista los servicios instalados.");
	cargar_comando(GSPS_CMD_ACTIVOS, (funcion_comando *)cmd_activos, "Lista los servicios actualmente activos.");
	cargar_comando(GSPS_CMD_DETENIDOS, (funcion_comando *)cmd_detenidos, "Lista los servicios actualmente detenidos.");
	cargar_comando(GSPS_CMD_PAUSADOS, (funcion_comando *)cmd_pausados, "Lista los servicios actualmente pausados.");
	cargar_comando(GSPS_CMD_ACTIVAR, (funcion_comando *)cmd_activar, "Activa un servicio especificado.");
	cargar_comando(GSPS_CMD_DETENER, (funcion_comando *)cmd_detener, "Detiene un servicio especificado.");
	cargar_comando(GSPS_CMD_PAUSAR, (funcion_comando *)cmd_pausar, "Pone en pausa un servicio especificado.");
	cargar_comando(GSPS_CMD_CONTINUAR, (funcion_comando *)cmd_continuar, "Continua un servicio previamente pausado.");
	cargar_comando(GSPS_CMD_BORRAR, (funcion_comando *)cmd_borrar, "Borra un servicio.");
	cargar_comando(GSPS_CMD_AGREGAR, (funcion_comando *)cmd_agregar, "Agrega un servicio.");
	cargar_comando(GSPS_CMD_CAMBIAR, (funcion_comando *)cmd_cambiar, "Cambia la configuracion de un servicio especificado.");
	cargar_comando(GSPS_CMD_NOMBRE_DISPLAY, (funcion_comando *)cmd_nombre_display, "Informa el nombre externo o display de un servicio.");
	cargar_comando(GSPS_CMD_NOMBRE_INTERNO, (funcion_comando *)cmd_nombre_interno, "Informa el nombre interno de un servicio.");
	cargar_comando(GSPS_CMD_TIPO, (funcion_comando *)cmd_tipo, "Informa el tipo de servicio (File System | Kernel | Own Process | Share).");
	cargar_comando(GSPS_CMD_ESTADO, (funcion_comando *)cmd_estado, "Informa el estado actual del servicio (Iniciado | Detenido | Pausado | Deshabilitado).");
	cargar_comando(GSPS_CMD_DEPENDENCIAS, (funcion_comando *)cmd_dependencias, "Lista las dependencias de un servicio especificado.");
	cargar_comando(GSPS_CMD_INICIO, (funcion_comando *)cmd_inicio, "Informa el tipo de inicio del un servicio especificado (Automatico | Boot | Por Demanda | Deshabilitado | Sistema).");
	cargar_comando(GSPS_CMD_CONTROL_ERROR, (funcion_comando *)cmd_control_error, "Informa el tipo de control de error de un servicio especificado (Ignorar | Normal | Severo | Critico).");
	cargar_comando(GSPS_CMD_ARCHIVO, (funcion_comando *)cmd_archivo, "Informa la ruta del archivo de un servicio especificado.");
	cargar_comando(GSPS_CMD_INTERACTIVO, (funcion_comando *)cmd_interactivo, "Informa si el servicio especificado esta marcado como interactivo.");	
	cargar_comando(GSPS_CMD_USUARIO, (funcion_comando *)cmd_usuario, "Informa el usuario que inicio el servicio especificado.");	
	cargar_comando(GSPS_CMD_SELECCIONAR, (funcion_comando *)cmd_seleccionar, "Selecciona un servicio para las subsecuentes operaciones.");
	cargar_comando(GSPS_CMD_LIBERAR, (funcion_comando *)cmd_liberar, "Libera el servicio tomado por el comando Seleccionar.");

	cargar_comando(GSPS_CMD_SERVIDOR, (funcion_comando *)cmd_servidor, "Cambia el servidor al cual se haran las consultas.");
	cargar_comando(GSPS_CMD_BASE_LOCAL, (funcion_comando *)cmd_baselocal, "Cambia al servidor local las consultas.");

/*
	cargar_no_comando((funcion_comando*)funcion_de_no_comando);
	cargar_comando_salida(GSND_TERMINAR, (funcion_comando*)salida);
	cargar_comando_bienvenida((funcion_comando*)bienvenida);
*/
}

void GuruProtocoloServicios::cmd_servicios(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	GuruServicios	svc;
	bool			detallado = false;
	char			linea_detallado[256 + 256 + 25 + 2];


	switch (cant_param)
		{
	case 2: 
			if (stricmp(parametros[1], GSPS_CMD_OPC_DETALLADO) == 0) 
				detallado = true;
			
	case 1: 
			dword cantidad = svc.ls_servicios(esto->host);

			if (cantidad)
				{
				while (!svc.ls_fin_servicios())
					{
					if (detallado)
						{
						//Contro de Control Guru;ccg;Activo;
						sprintf(linea_detallado, "%s"GSPS_SEPARADOR"%s"GSPS_SEPARADOR"%s",svc.ls_nombre_display(), svc.ls_nombre_interno());
						esto->enviar_no_error(linea_detallado);
						}
					else
						esto->enviar_no_error(svc.ls_nombre_display());
					}

				esto->enviar_error();
				}
			else
				esto->enviar_error(svc.mensaje_error());
			
			break;
		}	

}

void GuruProtocoloServicios::cmd_activos(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	GuruServicios	svc;

	svc.ls_servicios(esto->host, SERVICE_ACTIVE);

	svc.ls_servicios_autoincrementar(false);

	while (!svc.ls_fin_servicios())
		{
		if (svc.ls_esta_activo())
			esto->enviar_no_error(svc.ls_nombre_display());

		svc.proximo_servicio();
		}

	esto->enviar_error();
}

void GuruProtocoloServicios::cmd_detenidos(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	GuruServicios	svc;

	svc.ls_servicios(esto->host, SERVICE_INACTIVE);

	while (!svc.ls_fin_servicios())
		esto->enviar_no_error(svc.ls_nombre_display());

	esto->enviar_error();
}

void GuruProtocoloServicios::cmd_pausados(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	GuruServicios	svc;

	svc.ls_servicios(esto->host, SERVICE_ACTIVE);

	svc.ls_servicios_autoincrementar(false);

	while (!svc.ls_fin_servicios())
		{
		if (svc.ls_esta_pausa())
			esto->enviar_no_error(svc.ls_nombre_display());

		svc.proximo_servicio();
		}

	esto->enviar_error();
}

void GuruProtocoloServicios::cmd_activar(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	char			*servicio_activar = NULL;

	if (cant_param == 1 || cant_param == 2)
		{
		if (!esto->parametro_valido(cant_param, parametros, &servicio_activar))
			return ;			
		}
	else
		{
		esto->enviar_error(GSPS_HLP_ACTIVAR);
		return ;
		}

	if (servicio_activar)
		{
		GuruServicios	svc;

		if (!svc.activar(servicio_activar, esto->host))
			esto->enviar_error(svc.mensaje_error());
		else
			esto->enviar_no_error();

		delete servicio_activar;
		}
}

void GuruProtocoloServicios::cmd_detener(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	char			*servicio_detener = NULL;

	if (cant_param == 1 || cant_param == 2)
		{
		if (!esto->parametro_valido(cant_param, parametros, &servicio_detener))
			return ;			
		}
	else
		{
		esto->enviar_error(GSPS_HLP_DETENER);
		return ;
		}

	if (servicio_detener)
		{
		GuruServicios	svc;

		if (!svc.detener(servicio_detener, esto->host))
			esto->enviar_error(svc.mensaje_error());
		else
			esto->enviar_no_error();

		delete servicio_detener;
		}
}

void GuruProtocoloServicios::cmd_pausar(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{

}

void GuruProtocoloServicios::cmd_continuar(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{

}

void GuruProtocoloServicios::cmd_borrar(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	GuruServicios	svc;

	if (cant_param == 2)
		{
		if (svc.borrar(parametros[1], esto->host))
			esto->enviar_no_error();
		else
			esto->enviar_error(svc.mensaje_error());
		}
	else
		esto->enviar_error(GSPS_HLP_BORRAR);
}

//////////////////////////////////////////////////////////////////////
// Verificar si esto anda !!!!!!
// Fue cambiada el 02/04/2004 !!!!! #define por const char .....
//////////////////////////////////////////////////////////////////////
void GuruProtocoloServicios::cmd_agregar(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	const char * cad_opciones[] = GSPS_CMD_OPCIONES_AGREGAR;

	if (esto->existe_parametro(cad_opciones, sizeof(cad_opciones), parametros[1]))
		{
		if (stricmp(parametros[2], GSPS_CMD_NOMBRE_DISPLAY) == 0)
			{
			if (esto->nuevo_servicio.nombre_display)
				delete esto->nuevo_servicio.nombre_display;

			esto->nuevo_servicio.nombre_display = new char [strlen(parametros[3])+1];
			strcpy(esto->nuevo_servicio.nombre_display, parametros[3]);
			}
		else
			if (stricmp(parametros[2], GSPS_CMD_NOMBRE_INTERNO) == 0)
				{
				if (esto->nuevo_servicio.nombre_interno)
					delete esto->nuevo_servicio.nombre_interno;

				esto->nuevo_servicio.nombre_interno = new char [strlen(parametros[3]) + 1];
				strcpy(esto->nuevo_servicio.nombre_interno, parametros[3]);
				}
			else
				if (stricmp(parametros[2], GSPS_CMD_INTERACTIVO) == 0)
					esto->nuevo_servicio.tipo &= SERVICE_INTERACTIVE_PROCESS;
				else
					if (stricmp(parametros[2], GSPS_CMD_ARCHIVO) == 0)
						{
						if (esto->nuevo_servicio.archivo)
							delete esto->nuevo_servicio.archivo;

						esto->nuevo_servicio.archivo = new char [strlen(parametros[3]) + 1];
						strcpy(esto->nuevo_servicio.archivo, parametros[3]);
						}
					else
						if ((stricmp(parametros[2], GSPS_CMD_USUARIO) == 0))
							{
							if (parametros[3] && parametros[4])
								{
							
								if (esto->nuevo_servicio.usuario)
									delete esto->nuevo_servicio.usuario;

								if (esto->nuevo_servicio.clave)
									delete esto->nuevo_servicio.clave;

								esto->nuevo_servicio.usuario = new char [strlen(parametros[3]) + 1];
								esto->nuevo_servicio.clave   = new char [strlen(parametros[4]) + 1];
							
								strcpy(esto->nuevo_servicio.usuario, parametros[3]);
								strcpy(esto->nuevo_servicio.clave, parametros[4]);
								}
							}
						else
							if (stricmp(parametros[2], GSPS_CMD_INICIO) == 0)
								{
								const char *opciones_inicio[] = GSPS_CMD_OPCIONES_AGREGAR_INICIO;

								if (esto->existe_parametro(opciones_inicio, sizeof(opciones_inicio), parametros[3]))
									esto->nuevo_servicio.inicio = esto->cadenas_inicio_a_dword(parametros[3]);
								else
									esto->enviar_error(GSPS_MSG_PARAMETRO_DESCONOCIDO);
								}
							else
								if (stricmp(parametros[2], GSPS_CMD_CONTROL_ERROR) == 0)
									{
									const char *opciones_error[] = GSPS_CMD_OPCIONES_AGREGAR_ERROR;

									if (esto->existe_parametro(opciones_error, sizeof(opciones_error), parametros[3]))
										esto->nuevo_servicio.controlerror = esto->cadenas_error_a_dword(parametros[3]);
									else
										esto->enviar_error(GSPS_MSG_PARAMETRO_DESCONOCIDO);
									}
								else
									if (stricmp(parametros[2], GSPS_CMD_TIPO) == 0)
										{
										const char *opciones_tipo[] = GSPS_CMD_OPCIONES_AGREGAR_TIPO;

										if (esto->existe_parametro(opciones_tipo, sizeof(opciones_tipo), parametros[3]))
											esto->nuevo_servicio.tipo = esto->cadenas_tipo_a_dword(parametros[3]);
										else
											esto->enviar_error(GSPS_MSG_PARAMETRO_DESCONOCIDO);
										}
									else
										esto->enviar_error(GSPS_MSG_PARAMETRO_DESCONOCIDO);
		}
	else
		esto->enviar_error(GSPS_HLP_AGREGAR);
}

void GuruProtocoloServicios::cmd_cambiar(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	const char* cad_opciones[] = GSPS_CMD_OPCIONES_CAMBIAR;
	dword			indice = 1;
	char*			nombre_servicio = NULL;
	GuruServicios	svc;

	if (svc.existe(parametros[1], esto->host))
		{
		nombre_servicio = new char [strlen(parametros[1]) + 1];
		strcpy(nombre_servicio, parametros[1]);
		indice++;
		}
	else
		if (!esto->Servicio_Seleccionado)
			{
			esto->enviar_error(GSPS_MSG_SIN_SELECCION);
			return ;
			}
		else
			{
			nombre_servicio = new char [strlen(esto->Servicio_Seleccionado) + 1];
			strcpy(nombre_servicio, esto->Servicio_Seleccionado);
			}

	if (esto->existe_parametro(cad_opciones, sizeof(cad_opciones), parametros[indice]))
		{
		data_servicio	cambio_servicio = {0};

		cambio_servicio.controlerror = SERVICE_NO_CHANGE;
		cambio_servicio.tipo		 = SERVICE_NO_CHANGE;
		cambio_servicio.inicio		 = SERVICE_NO_CHANGE;

		if (stricmp(parametros[indice], GSPS_CMD_NOMBRE_DISPLAY) == 0)
			{
			cambio_servicio.nombre_display = new char [strlen(parametros[indice+1]) + 1];
			strcpy(cambio_servicio.nombre_display, parametros[indice+1]);
			}
		else
			if (stricmp(parametros[indice], GSPS_CMD_INTERACTIVO) == 0)
				{
				GuruServicios			svc;
				QUERY_SERVICE_CONFIG*	config;

				if (svc.configuracion(nombre_servicio, &config))
					delete (char *)config;						

				cambio_servicio.tipo &= SERVICE_INTERACTIVE_PROCESS;
				}
			else
				if (stricmp(parametros[indice], GSPS_CMD_ARCHIVO) == 0)
					{
					if (cambio_servicio.archivo)
						delete cambio_servicio.archivo;

					cambio_servicio.archivo = new char [strlen(parametros[indice+1]) + 1];

					strcpy(cambio_servicio.archivo, parametros[indice+1]);
					}
				else
					if ((stricmp(parametros[indice], GSPS_CMD_USUARIO) == 0))
						{
						cambio_servicio.usuario = new char [strlen(parametros[indice+1]) + 1];
						cambio_servicio.clave   = new char [strlen(parametros[indice+2]) + 1];

						strcpy(cambio_servicio.usuario, parametros[indice+1]);
						strcpy(cambio_servicio.clave, parametros[indice+2]);
						}
					else
						if (stricmp(parametros[indice], GSPS_CMD_INICIO) == 0)
							{
							const char *opciones_inicio[] = GSPS_CMD_OPCIONES_AGREGAR_INICIO;

							if (esto->existe_parametro(opciones_inicio, sizeof(opciones_inicio), parametros[indice+1]))
								cambio_servicio.inicio = esto->cadenas_inicio_a_dword(parametros[indice+1]);
							else
								esto->enviar_error(GSPS_MSG_PARAMETRO_DESCONOCIDO);
							}
						else
							if (stricmp(parametros[indice], GSPS_CMD_CONTROL_ERROR) == 0)
								{
								const char *opciones_error[] = GSPS_CMD_OPCIONES_AGREGAR_ERROR;

								if (esto->existe_parametro(opciones_error, sizeof(opciones_error), parametros[indice+1]))
									cambio_servicio.controlerror = esto->cadenas_error_a_dword(parametros[indice+1]);
								else
									esto->enviar_error(GSPS_MSG_PARAMETRO_DESCONOCIDO);
								}
							else
								if (stricmp(parametros[indice], GSPS_CMD_TIPO) == 0)
									{
									const char *opciones_tipo[] = GSPS_CMD_OPCIONES_AGREGAR_TIPO;

									if (esto->existe_parametro(opciones_tipo, sizeof(opciones_tipo), parametros[indice+1]))
										cambio_servicio.tipo = esto->cadenas_tipo_a_dword(parametros[indice+1]);
									else
										esto->enviar_error(GSPS_MSG_PARAMETRO_DESCONOCIDO);
									}
								else
									esto->enviar_error(GSPS_MSG_PARAMETRO_DESCONOCIDO);

		if (svc.abrir_servicio(nombre_servicio, esto->host))
			if (ChangeServiceConfig(svc.handle_servicio(), 
									cambio_servicio.tipo,			// type of service
									cambio_servicio.inicio,			// when to start service
									cambio_servicio.controlerror,	// severity of start failure
									cambio_servicio.archivo,		// service binary file name
									NULL,							// load ordering group name
									NULL,							// tag identifier
									NULL,							// array of dependency names
									cambio_servicio.usuario,		// account name
									cambio_servicio.clave,			// account password
									cambio_servicio.nombre_display))	// display name
				esto->enviar_no_error();				
			else
				{
				GuruError error_config;

				esto->enviar_error(error_config.descripcion());
				}

		if (cambio_servicio.nombre_display)
			delete cambio_servicio.nombre_display;

		if (cambio_servicio.archivo)
			delete cambio_servicio.archivo;

		if (cambio_servicio.usuario)
			delete cambio_servicio.usuario;

		if (cambio_servicio.clave)
			delete cambio_servicio.clave;

		}
	else
		esto->enviar_error(GSPS_HLP_AGREGAR);

	delete nombre_servicio;
}

void GuruProtocoloServicios::cmd_nombre_display(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	char			nombre_servicio[255] = {0};
	GuruServicios	svc;

	switch (cant_param)
		{
	case 1: 
			if (esto->Servicio_Seleccionado)
				{
				if (!svc.nombres(esto->Servicio_Seleccionado, NULL, nombre_servicio))
					esto->enviar_error(svc.mensaje_error());
				}
			else
				esto->enviar_error(GSPS_MSG_SIN_SELECCION);
			break;

	case 2: if (!svc.nombres(parametros[1], NULL, nombre_servicio))
				esto->enviar_error(svc.mensaje_error());
			break;

	default:
			esto->enviar_error(GSPS_HLP_NOMBRE_DISPLAY);
		}

	if (nombre_servicio[0])
		esto->enviar_no_error(nombre_servicio);
}

void GuruProtocoloServicios::cmd_nombre_interno(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	char			nombre_servicio[255] = {0};
	GuruServicios	svc;

	switch (cant_param)
		{
	case 1: 
			if (esto->Servicio_Seleccionado)
				{
				if (!svc.nombres(esto->Servicio_Seleccionado, nombre_servicio, NULL))
					esto->enviar_error(svc.mensaje_error());
				}
			else
				esto->enviar_error(GSPS_MSG_SIN_SELECCION);
			break;

	case 2: if (!svc.nombres(parametros[1], nombre_servicio, NULL))
				esto->enviar_error(svc.mensaje_error());
			break;

	default:
			esto->enviar_error(GSPS_HLP_NOMBRE_INTERNO);
		}

	if (nombre_servicio[0])
		esto->enviar_no_error(nombre_servicio);
}

void GuruProtocoloServicios::cmd_tipo(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	char *nombre_servicio;

	if (cant_param == 1 || cant_param == 2)
		{
		if (esto->parametro_valido(cant_param, parametros, &nombre_servicio))
			{
			GuruServicios			svc;
			QUERY_SERVICE_CONFIG*	config;

			if (svc.configuracion(nombre_servicio, &config))
				{
				switch (config->dwServiceType)
					{
				case SERVICE_FILE_SYSTEM_DRIVER: esto->enviar_no_error(GSPS_MSG_DRIVER_FILE_SYSTEM);break;
				case SERVICE_KERNEL_DRIVER: esto->enviar_no_error(GSPS_MSG_DRIVER_KERNEL);break;
				case SERVICE_WIN32_OWN_PROCESS: esto->enviar_no_error(GSPS_MSG_PROCESO_WIN32);break;
				case SERVICE_WIN32_SHARE_PROCESS: esto->enviar_no_error(GSPS_MSG_PROCESO_SHARE_WIN32);break;
				default:
					esto->enviar_no_error(GSPS_MSG_DESCONOCIDO);					
					}

				delete (char *)config;
				}
			else
				esto->enviar_error(svc.mensaje_error());
			}
		else
			return ;
		}
	else
		esto->enviar_error(GSPS_HLP_TIPO);
}

void GuruProtocoloServicios::cmd_estado(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	GuruServicios	svc;
	SERVICE_STATUS	estado; 
	char *			estado_servicio;

	if (cant_param == 1 || cant_param == 2)
		{
		if (esto->parametro_valido(cant_param, parametros, &estado_servicio))
			{
			switch (svc.estado_actual(estado_servicio, &estado)->dwCurrentState)
				{
			case SERVICE_CONTINUE_PENDING: esto->enviar_no_error(GSPS_MSG_INICIANDO_CONTINUACION); break;
			case SERVICE_PAUSE_PENDING: esto->enviar_no_error(GSPS_MSG_INICIANDO_PAUSA); break;
			case SERVICE_PAUSED: esto->enviar_no_error(GSPS_MSG_PAUSA); break;
			case SERVICE_RUNNING: esto->enviar_no_error(GSPS_MSG_ACTIVO); break;
			case SERVICE_START_PENDING: esto->enviar_no_error(GSPS_MSG_ACTIVANDO); break;
			case SERVICE_STOP_PENDING: esto->enviar_no_error(GSPS_MSG_DETENIENDO); break;
			case SERVICE_STOPPED: esto->enviar_no_error(GSPS_MSG_DETENIDO); break;
				}
			}
		else
			return ;
		}
	else
		esto->enviar_error(GSPS_HLP_ESTADO);
}

void GuruProtocoloServicios::cmd_dependencias(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{

}

void GuruProtocoloServicios::cmd_inicio(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	char *nombre_servicio;

	if (cant_param == 1 || cant_param == 2)
		{
		if (esto->parametro_valido(cant_param, parametros, &nombre_servicio))
			{
			GuruServicios			svc;
			QUERY_SERVICE_CONFIG*	config;

			if (svc.configuracion(nombre_servicio, &config))
				{
				switch (config->dwStartType)
					{
				case SERVICE_AUTO_START: esto->enviar_no_error(GSPS_MSG_AUTOMATICO);break;
				case SERVICE_BOOT_START: esto->enviar_no_error(GSPS_MSG_BOOT);break;
				case SERVICE_DEMAND_START: esto->enviar_no_error(GSPS_MSG_MANUAL);break;
				case SERVICE_DISABLED: esto->enviar_no_error(GSPS_MSG_DESHABILITADO);break;
				case SERVICE_SYSTEM_START: esto->enviar_no_error(GSPS_MSG_SISTEMA);break;
				default:
					esto->enviar_no_error(GSPS_MSG_DESCONOCIDO);					
					}

				delete (char *)config;
				}
			else
				esto->enviar_error(svc.mensaje_error());
			}
		else
			esto->enviar_error(GSPS_MSG_PARAMETRO_DESCONOCIDO);
		}
	else
		esto->enviar_error(GSPS_HLP_INICIO);
}

void GuruProtocoloServicios::cmd_control_error(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	char *nombre_servicio;

	if (cant_param == 1 || cant_param == 2)
		{
		if (esto->parametro_valido(cant_param, parametros, &nombre_servicio))
			{
			GuruServicios			svc;
			QUERY_SERVICE_CONFIG*	config;

			if (svc.configuracion(nombre_servicio, &config))
				{
				switch (config->dwErrorControl)
					{
				case SERVICE_ERROR_IGNORE: esto->enviar_no_error(GSPS_MSG_IGNORAR);break;
				case SERVICE_ERROR_NORMAL: esto->enviar_no_error(GSPS_MSG_NORMAL);break;
				case SERVICE_ERROR_SEVERE: esto->enviar_no_error(GSPS_MSG_SEVERO);break;
				case SERVICE_ERROR_CRITICAL: esto->enviar_no_error(GSPS_MSG_CRITICO);break;
				default:
					esto->enviar_no_error(GSPS_MSG_DESCONOCIDO);					
					}

				delete (char *)config;
				}
			else
				esto->enviar_error(svc.mensaje_error());
			}
		else
			esto->enviar_error(GSPS_MSG_PARAMETRO_DESCONOCIDO);
		}
	else
		esto->enviar_error(GSPS_HLP_CONTROL_ERROR);
}

void GuruProtocoloServicios::cmd_archivo(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	char *nombre_servicio;

	if (cant_param == 1 || cant_param == 2)
		{
		if (esto->parametro_valido(cant_param, parametros, &nombre_servicio))
			{
			GuruServicios svc;
			QUERY_SERVICE_CONFIG*	config;

			if (svc.configuracion(nombre_servicio, &config))
				{
				esto->enviar_no_error(config->lpBinaryPathName);

				delete (char *)config;
				}
			else
				esto->enviar_error(svc.mensaje_error());
			}
		else
			esto->enviar_error(GSPS_MSG_PARAMETRO_DESCONOCIDO);
		}
	else
		esto->enviar_error(GSPS_HLP_ARCHIVO);

}

void GuruProtocoloServicios::cmd_interactivo(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	char *nombre_servicio;

	if (cant_param == 1 || cant_param == 2)
		{
		if (esto->parametro_valido(cant_param, parametros, &nombre_servicio))
			{
			GuruServicios			svc;
			QUERY_SERVICE_CONFIG*	config;

			if (svc.configuracion(nombre_servicio, &config))
				{
				if (config->dwServiceType & SERVICE_INTERACTIVE_PROCESS)
					esto->enviar_no_error(GSPS_MSG_HABILITADO);
				else
					esto->enviar_no_error(GSPS_MSG_DESHABILITADO);

				delete (char *)config;
				}
			else
				esto->enviar_error(svc.mensaje_error());
			}
		else
			esto->enviar_error(GSPS_MSG_PARAMETRO_DESCONOCIDO);
		}
	else
		esto->enviar_error(GSPS_HLP_INTERACTIVO);
}

void GuruProtocoloServicios::cmd_usuario(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	char *nombre_servicio;

	if (cant_param == 1 || cant_param == 2)
		{
		if (esto->parametro_valido(cant_param, parametros, &nombre_servicio))
			{
			GuruServicios			svc;
			QUERY_SERVICE_CONFIG*	config;

			if (svc.configuracion(nombre_servicio, &config))
				{
				esto->enviar_no_error(config->lpServiceStartName);

				delete (char *)config;
				}
			else
				esto->enviar_error(svc.mensaje_error());
			}
		else
			esto->enviar_error(GSPS_MSG_PARAMETRO_DESCONOCIDO);
		}
	else
		esto->enviar_error(GSPS_HLP_USUARIO);
}

void GuruProtocoloServicios::cmd_seleccionar(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	if (cant_param == 2)
		{
		GuruServicios svc;

		if (svc.existe(parametros[1], esto->host))
			{
			esto->Servicio_Seleccionado = new char [strlen(parametros[1]) + 1];

			if (esto->Servicio_Seleccionado)
				esto->enviar_no_error(strcpy(esto->Servicio_Seleccionado, parametros[1]));
			else
				esto->enviar_error(GSPS_MSG_SIN_SELECCION);
			}
		else
			esto->enviar_error(svc.mensaje_error());
		}
	else
		esto->enviar_error(GSPS_HLP_SELECCIONAR);
}

void GuruProtocoloServicios::cmd_liberar(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	if (esto->Servicio_Seleccionado)
		{
		esto->enviar_no_error(esto->Servicio_Seleccionado);

		delete esto->Servicio_Seleccionado;

		esto->Servicio_Seleccionado = NULL;
		}
	else
		esto->enviar_no_error(GSPS_MSG_SIN_SELECCION);
}

void GuruProtocoloServicios::cmd_servidor(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	if (cant_param == 2)
		{
		if (esto->host)
			delete esto->host;

		esto->host = new char [strlen(parametros[1]) + 1];

		if (esto->host)
			esto->enviar_no_error(strcpy(esto->host, parametros[1]));
		else
			esto->enviar_error();
		}
	else
		esto->enviar_error(GSPS_HLP_SERVIDOR);
}

void GuruProtocoloServicios::cmd_baselocal(int cant_param, char *parametros[], GuruProtocoloServicios * esto)
{
	if (esto->host)
		{
		esto->enviar_no_error(esto->host);

		delete esto->host;

		esto->host = NULL;
		}
	else
		esto->enviar_error(GSPS_HLP_BASE_LOCAL);
}

bool GuruProtocoloServicios::parametro_valido(int cant_param, char *parametros[], char **buffer)
{
	switch (cant_param)
		{
	case 1: 
			if (Servicio_Seleccionado)
				{
				*buffer = new char [strlen(Servicio_Seleccionado) + 1];
				strcpy(*buffer, Servicio_Seleccionado);
				}
			else
				{
				enviar_error(GSPS_MSG_SIN_SELECCION);
				return false;
				}

			break;

	case 2: 
			*buffer = new char [strlen(parametros[1]) + 1];
			strcpy(*buffer, parametros[1]);
			break;
		}

	return true;
}

bool GuruProtocoloServicios::existe_parametro(const char *lista_parametros[], word tamanio_lista, char *parametro)
{
	bool	existe=false;
	word	cant_opciones=tamanio_lista /sizeof(char *);
	word	i;

	for (i=0;i<cant_opciones;i++)
		if (stricmp(lista_parametros[i], parametro) == 0)
			existe = true;

	return (existe) ? true : false;
}

dword GuruProtocoloServicios::cadenas_inicio_a_dword(char *cadena_inicio)
{
	dword salida = 0;

	if (stricmp(cadena_inicio, GSPS_MSG_MANUAL) == 0)
		salida = SERVICE_DEMAND_START;
	else
		if (stricmp(cadena_inicio, GSPS_MSG_AUTOMATICO) == 0)
			salida = SERVICE_AUTO_START;
		else
			if (stricmp(cadena_inicio, GSPS_MSG_DESHABILITADO) == 0)
				salida = SERVICE_DISABLED;
			else
				if (stricmp(cadena_inicio, GSPS_MSG_BOOT) == 0)
					salida = SERVICE_BOOT_START;
				else
					if (stricmp(cadena_inicio, GSPS_MSG_SISTEMA) == 0)
						salida = SERVICE_SYSTEM_START;

	return salida;
}

dword GuruProtocoloServicios::cadenas_error_a_dword(char *cadena_error)
{
	dword salida = 0;

	if (stricmp(cadena_error, GSPS_MSG_IGNORAR) == 0)
		salida = SERVICE_ERROR_IGNORE;
	else
		if (stricmp(cadena_error, GSPS_MSG_NORMAL) == 0)
			salida = SERVICE_ERROR_NORMAL;
		else
			if (stricmp(cadena_error, GSPS_MSG_SEVERO) == 0)
				salida = SERVICE_ERROR_SEVERE;
			else
				if (stricmp(cadena_error, GSPS_MSG_CRITICO) == 0)
					salida = SERVICE_ERROR_CRITICAL;

	return salida;
}

dword GuruProtocoloServicios::cadenas_tipo_a_dword(char *cadena_tipo)
{
	dword salida = 0;

	if (stricmp(cadena_tipo, GSPS_MSG_DRIVER_FILE_SYSTEM) == 0)
		salida = SERVICE_FILE_SYSTEM_DRIVER;
	else
		if (stricmp(cadena_tipo, GSPS_MSG_DRIVER_KERNEL) == 0)
			salida = SERVICE_KERNEL_DRIVER;
		else
			if (stricmp(cadena_tipo, GSPS_MSG_PROCESO_WIN32) == 0)
				salida = SERVICE_WIN32_OWN_PROCESS;
			else
				if (stricmp(cadena_tipo, GSPS_MSG_PROCESO_SHARE_WIN32) == 0)
					salida = SERVICE_WIN32_SHARE_PROCESS;

	return salida;
}
