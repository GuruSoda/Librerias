#include "GuruCorrer.h"

GuruCorrer::GuruCorrer()
{

}

GuruCorrer::~GuruCorrer()
{

}

//////////////////////////////////////////////////////////////////////
// Tarea:		Lanzar un ejecutable
// Parametros:
//				ejecutable	= Path completo del archivo a correr (requerido)
//				espera		= si tiene que esperar a que termine la aplicacion o no
//				ventana		= como tiene que crearce la ventana de la aplicaion
//
// Tipos de Ventana:
//		SW_FORCEMINIMIZE | SW_HIDE | SW_MAXIMIZE | SW_MINIMIZE | SW_RESTORE
//		SW_SHOW | SW_SHOWDEFAULT | SW_SHOWMAXIMIZED | SW_SHOWMINIMIZED
//		SW_SHOWMINNOACTIVE | SW_SHOWNA | SW_SHOWNOACTIVATE | SW_SHOWNORMAL
//////////////////////////////////////////////////////////////////////
bool GuruCorrer::ejecutar(char *ejecutable, bool espera, word ventana)
{
	BOOL				salida_create;
	STARTUPINFO			si;
	PROCESS_INFORMATION	pi;
	SECURITY_ATTRIBUTES	sa;

	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));


	si.cb = sizeof(STARTUPINFO);

	si.dwFlags		= STARTF_USESHOWWINDOW;
    si.wShowWindow	= ventana;

	cargar_SD_generico(&sa);

	salida_create = CreateProcess(NULL,			// name of executable module
								ejecutable,		// command line string
									&sa,		// SD
									&sa,		// SD
									TRUE,		// handle inheritance option
	NORMAL_PRIORITY_CLASS | CREATE_DEFAULT_ERROR_MODE,	// creation flags
									NULL,		// new environment block
									NULL,		// current directory name
									&si,		// startup information
									&pi			// process information
									);

	if (salida_create)
		{
		DWORD salida_apps;

		GetExitCodeProcess(pi.hProcess, &salida_apps);

		if (espera)
			WaitForSingleObject(pi.hProcess,INFINITE);
		}
	else
		GuruCorrer_error.descripcion();

	return (salida_create == TRUE) ? true : false;
}

SECURITY_ATTRIBUTES * GuruCorrer::cargar_SD_generico(SECURITY_ATTRIBUTES *seguridad)
{
	seguridad->bInheritHandle = TRUE;
	seguridad->nLength = sizeof(SECURITY_ATTRIBUTES);
	seguridad->lpSecurityDescriptor = NULL;

	return seguridad;
}

const char* GuruCorrer::mensaje_error()
{
	return GuruCorrer_error.mensaje();
}
