#include "GuruVisorEventosEscribir.h"

GuruVisorEventosEscribir::GuruVisorEventosEscribir()
{

}

GuruVisorEventosEscribir::GuruVisorEventosEscribir(const char *nombre_reg_servicio)
{
	strcpy(nombre_servicio,	nombre_reg_servicio);
}

GuruVisorEventosEscribir::~GuruVisorEventosEscribir()
{

}

void GuruVisorEventosEscribir::registrar(int lugar, char *nombre_registracion)
{
	char tipo[25];

	switch (lugar)
		{
	case APLICACION: strcpy(tipo, "Application");
					break;
	case SEGURIDAD: strcpy(tipo, "Security");
					break;
	case SISTEMA:	strcpy(tipo, "System");
					break;
		}

	char entrada[1024];
	char path_ejecutable[MAX_PATH];

	if (GetModuleFileName(NULL, path_ejecutable, sizeof(path_ejecutable)))
		{
		GuruRegistry	GuruRegedt32;

		sprintf(entrada, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s", tipo, nombre_servicio);

		GuruRegedt32.escribir("HKLM", entrada, "EventMessageFile", path_ejecutable);

		GuruRegedt32.escribir("HKLM", entrada, "TypesSupported",  EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE);
		}
}

void GuruVisorEventosEscribir::registrar_aplicacion(char *nombre_registracion)
{
	registrar(APLICACION, nombre_registracion);
}

void GuruVisorEventosEscribir::registrar_seguridad(char *nombre_registracion)
{
	registrar(SEGURIDAD, nombre_registracion);
}

void GuruVisorEventosEscribir::registrar_sistema(char *nombre_registracion)
{
	registrar(SISTEMA, nombre_registracion);
}

//////////////////////////////////////////////////////////////////////
// Tarea: Escribir en el Event Viewer un evento
// Parametros:
//				tipo = solo uno:
//					1)	EVENTLOG_ERROR_TYPE
//					2)	EVENTLOG_WARNING_TYPE
//					3)	EVENTLOG_INFORMATION_TYPE
//////////////////////////////////////////////////////////////////////
void GuruVisorEventosEscribir::EscribirEvento(WORD tipo, DWORD id, const char *parametros[])
{
	WORD	cant_parametros = 0;

	while (parametros[cant_parametros++])
		;

	cant_parametros--;

	HANDLE hregistroev = RegisterEventSource(NULL, nombre_servicio);


	ReportEvent(hregistroev,	// handle to event log
				tipo,			// event type
				(WORD)0,		// event category
				id,				// event identifier
				NULL,			// user security identifier
		cant_parametros,		// number of strings to merge
			(DWORD)0,			// size of binary data
			parametros,			// array of strings to merge
				NULL			// binary data buffer
					);

	DeregisterEventSource(hregistroev);
}
