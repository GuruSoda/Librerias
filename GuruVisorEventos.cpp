#include "GuruVisorEventos.h"

GuruVisorEventos::GuruVisorEventos()
{
	registro		= NULL;

	tamanio_buffer_info		= GURUVISOREVENTOS_TAMANIO_BUFFER;
	buffer_info				= new char [tamanio_buffer_info + 1];

	hl				= NULL;
	tipo_lectura	= EVENTLOG_SEQUENTIAL_READ | EVENTLOG_FORWARDS_READ;
}

GuruVisorEventos::~GuruVisorEventos()
{
	if (hl)
		CloseEventLog(hl);

	if (registro)
		delete registro;

	if (buffer_info)
		delete buffer_info;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
bool GuruVisorEventos::abrir(char *evento_requerido)
{
	strcpy(evento,evento_requerido);

	hl = OpenEventLog("",evento);

	if (!hl)
		{
		GuruVisorEventos_error.descripcion();
		return false;
		}
	else
		return true;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Abrir un Visor de eventos 
// Parametros:	Nombre de la pc, y el tipo de evento
//
//				Nota: El tipo de evento pude ser:
//					Application | Security | System 
//////////////////////////////////////////////////////////////////////
bool GuruVisorEventos::abrir(char *nombre_pc,char *evento_requerido)
{
	strcpy(evento,evento_requerido);
	strcpy(pc,nombre_pc);

	hl = OpenEventLog(pc,evento);

	if (hl)
		return true;
	else
		{
		GuruVisorEventos_error.descripcion();
		return false;
		}
}

HANDLE GuruVisorEventos::handle_visor_eventos()
{
	return hl;
}

void GuruVisorEventos::lectua_desde_el_primero(bool forma)
{
	if (forma)
		tipo_lectura = EVENTLOG_SEQUENTIAL_READ | EVENTLOG_FORWARDS_READ;
	else
		tipo_lectura = EVENTLOG_SEQUENTIAL_READ | EVENTLOG_BACKWARDS_READ;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
dword GuruVisorEventos::cantidad_entradas()
{
	dword cantidad_eventos;

	if (GetNumberOfEventLogRecords(hl, &cantidad_eventos))
		return cantidad_eventos;
	else
		return 0;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
bool GuruVisorEventos::sacar_linea()
{
	BOOL	salida		= TRUE;
	dword	tamanio_buffer_registro = 0;
	EVENTLOGRECORD	temp_buffer	= {0};
	dword	leidos = 0;
	dword	aleer  = 0;

	// siempre tiene que dar insuficiente buffer
	salida = ReadEventLog(hl, tipo_lectura, 0, (EVENTLOGRECORD *) &temp_buffer, 0, &leidos, &aleer);

	if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
		if (registro)
			delete registro;

		tamanio_buffer_registro = aleer + 1;

		registro = (EVENTLOGRECORD *) new char [tamanio_buffer_registro + 4];

		memset(registro, 0, tamanio_buffer_registro);

		salida = ReadEventLog(hl, tipo_lectura, 0, registro, tamanio_buffer_registro, &leidos, &aleer);
		}
	else
		{
//		if (error == ERROR_HANDLE_EOF)
		GuruVisorEventos_error.descripcion();
		salida = FALSE;
		}

	return (salida == TRUE) ? true : false;
}

/*
BOOL GuruVisorEventos::sacar_linea()
{
	BOOL salida;
	DWORD num_entradas=0;

	if (leidos == 0)
		{
		registro = (EVENTLOGRECORD *) &buffer_registros;
		salida = ReadEventLog(hl,
					EVENTLOG_SEQUENTIAL_READ | EVENTLOG_FORWARDS_READ ,
					1,				// number of first record
					registro,       // address of buffer for read data
					BUFFER_SIZE,	// number of bytes to read
					&leidos,        // number of bytes read
					&aleer);		// number of bytes required for the next log entry
		}
	else
		{
		leidos -= registro->Length; 
		registro = (EVENTLOGRECORD*)((LPBYTE)registro + registro->Length);
		if (!leidos)
			sacar_linea();
		}

	return salida;

}
*/

//
//	Then follow: 
// 
//	TCHAR	SourceName[] 
//	TCHAR	Computername[] 
//	SID		UserSid 
//	TCHAR	Strings[] 
//	BYTE	Data[] 
//	CHAR	Pad[] 
//	DWORD	Length; 
// 

//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
char *GuruVisorEventos::origen()
{
//	return cargar_buffer_info((char *)((LPBYTE)registro + sizeof(EVENTLOGRECORD)));
	return (char *)((LPBYTE)registro + sizeof(EVENTLOGRECORD));
}

//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
char *GuruVisorEventos::nombre_pc()
{
	/* saco la longitud de "Source" */
	dword largo_source = (dword)strlen((char *)((LPBYTE)registro + sizeof(EVENTLOGRECORD)));

//	return cargar_buffer_info((char *)((LPBYTE)registro + sizeof(EVENTLOGRECORD) + largo_source+1));
	return (char *)((LPBYTE)registro + sizeof(EVENTLOGRECORD) + largo_source+1);
}

//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
char *GuruVisorEventos::tipo(char *tipo)
{
	switch (registro->EventType)
		{
	case EVENTLOG_ERROR_TYPE:		strcpy(tipo, "Error"); break;
	case EVENTLOG_WARNING_TYPE :	strcpy(tipo, "Warning"); break;
	case EVENTLOG_INFORMATION_TYPE: strcpy(tipo, "Information"); break;
	case EVENTLOG_AUDIT_SUCCESS :	strcpy(tipo, "Success Audit"); break;
	case EVENTLOG_AUDIT_FAILURE :	strcpy(tipo, "Failure Audit"); break;
		}

	return tipo;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
tipo_registro GuruVisorEventos::tipo()
{
	switch (registro->EventType)
		{
	case EVENTLOG_ERROR_TYPE:		return Error; break;
	case EVENTLOG_WARNING_TYPE:		return Warning; break;
	case EVENTLOG_INFORMATION_TYPE: return Information; break;
	case EVENTLOG_AUDIT_SUCCESS:	return Success_Audit; break;
	case EVENTLOG_AUDIT_FAILURE:	return Failure_Audit; break;
	default:						return Ninguno;
		}
}

//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
char *GuruVisorEventos::categoria()
{
	return _itoa(registro->EventCategory, buffer_info, 10);
}

//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
char *GuruVisorEventos::id()
{
	static	char num_suceso[10];

// El id se encuentra en el Byte superior del DWORD (No documentado!!!)

	return _ltoa((WORD)registro->EventID, num_suceso, 10);
//	sprintf(event_id,"%lu",(WORD)registro->EventID);
}

//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
char *GuruVisorEventos::suceso()
{
	static	char num_suceso[10];

// El id se encuentra en el Byte superior del DWORD (No documentado!!!)
	return _ltoa((WORD)registro->EventID, num_suceso, 10);
//	sprintf(event_id,"%lu",(WORD)registro->EventID);
}


//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
char *GuruVisorEventos::usuario()
{
	PSID ptr_Sid = (PSID)((LPBYTE) registro + registro->UserSidOffset);
	char cuenta[256];
	char dominio[256];
	dword tamanio_cuenta = sizeof(cuenta);
	dword tamanio_dominio = sizeof(dominio);
	SID_NAME_USE nombre;

	if (registro->UserSidLength)
		{

		int salida = LookupAccountSid(NULL,				// name of local or remote computer
									ptr_Sid,			// security identifier
									cuenta,				// account name buffer
									&tamanio_cuenta,	// size of account name buffer
									dominio,			// domain name
									&tamanio_dominio,	// size of domain name buffer
									&nombre);			// SID type
		if (!salida)
			GuruVisorEventos_error.descripcion();

		sprintf(buffer_info,"%s",cuenta);
	 	
		}
	else
		sprintf(buffer_info,"%s","N/A");

	return buffer_info;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
char *GuruVisorEventos::descripcion()
{
	char *		comienzo;
	char **		parametros = NULL;
	int			i=0;
		
	parametros = new char * [registro->NumStrings];

	comienzo = (char *)((LPBYTE)registro + (registro->StringOffset/* + registro->UserSidLength*/));

	for (i=0;i<registro->NumStrings;i++)
		{
		if (comienzo[0] == '%' && comienzo[1] == '%')
			{
			GuruVisorEventos_error.descripcion(atoi(&comienzo[2]));

			if (GuruVisorEventos_error.mensaje())
				{
				parametros[i] = new char [strlen(GuruVisorEventos_error.mensaje()) + 1];
				strcpy(parametros[i], GuruVisorEventos_error.mensaje());
				}
			else
				{
				parametros[i] = new char [strlen(comienzo) + 1];
				strcpy(parametros[i],comienzo);
				}
			}
		else
			{
			parametros[i] = new char [strlen(comienzo) + 1];
			strcpy(parametros[i],comienzo);
			}

		comienzo += strlen(comienzo)+1;
		}

	cargar_buffer_info((char*)sacar_descripcion_reg(parametros, (DWORD)registro->EventID));

	delete [] parametros;

	return buffer_info;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
const char *GuruVisorEventos::sacar_descripcion_reg(char *parametros[], dword id)
{
	GuruLista		cad;
	GuruRegistry	reg;
	char			buffer[512];

	sprintf(buffer,"SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s\\", evento, origen());
	if (reg.leer("HKLM", buffer, "EventMessageFile", buffer))
		{

		cad.separadores(";");

		dword cant = cad.dividir_cadena(buffer);

		if (cant)
			{
			if (cad[0][0] == '%')
				reemplazo_entorno(cad[0]);

			GuruVisorEventos_error.descripcion(id, cad[0], parametros);
			}
		}

	return GuruVisorEventos_error.mensaje();
}

//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
char *GuruVisorEventos::hora()
{
	struct	tm *desde;
	static	char buffer_hora[12];

	desde = localtime((const time_t *)&registro->TimeWritten);

	sprintf(buffer_hora,"%02d:%02d:%02d",desde->tm_hour,desde->tm_min,desde->tm_sec);

	return buffer_hora;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
char *GuruVisorEventos::fecha()
{
	struct	tm *desde;
	static	char buffer_fecha[16];

	desde = localtime((const time_t *)&registro->TimeWritten);

	sprintf(buffer_fecha,"%02d/%02d/%04d",desde->tm_mday,desde->tm_mon+1,desde->tm_year+1900);

	return buffer_fecha;
}

const char* GuruVisorEventos::mensaje_error()
{
	return GuruVisorEventos_error.mensaje();
}

dword GuruVisorEventos::numero_error()
{
	return GuruVisorEventos_error.numero();
}

//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
char *GuruVisorEventos::reemplazo_entorno(char *source)
{
	char buffer[_MAX_PATH];
	char valor[_MAX_PATH + 1];
	word i;

	strcpy(buffer,"");

	for (i=1;i<strlen(source);i++)
		{
		if (source[i] == '%') break;

		buffer[i-1] = source[i];
		}

	buffer[i-1] = '\0';
	
	GetEnvironmentVariable(buffer,valor,_MAX_PATH);
	strcat(valor,&source[i+1]);

	strcpy(source,valor);

	return source;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		
// Parametros:	
//////////////////////////////////////////////////////////////////////
char *GuruVisorEventos::cargar_buffer_info(char *nueva_info)
{
	if (tamanio_buffer_info < strlen(nueva_info))
		{
		delete buffer_info;

		buffer_info = new char [strlen(nueva_info) + 1];
		tamanio_buffer_info = (dword)strlen(nueva_info) + 1;
		}

	return strcpy(buffer_info, nueva_info);
}
