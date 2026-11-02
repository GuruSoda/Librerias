#include "GuruCmd.h"

void GuruCmd::cargar_cmd()
{
// This class is build from the following MSDN article.
// Q190351 HOWTO: Spawn Console Processes with Redirected Standard Handles.

	HANDLE proceso_actual = ::GetCurrentProcess();

// Set up the security attributes struct.
	SECURITY_ATTRIBUTES sa;
	memset(&sa, 0,sizeof(SECURITY_ATTRIBUTES));

	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	HANDLE salidaTmp, entradaTmp, errorTmp;

// Creo un pipe de entrada y no me interesa el de salida (entradaTmp).
	CreatePipe(&entrada_hijo, &entradaTmp, &sa, 0);

// Creo un pipe de salida y no me interesa el de entrada (salidaTmp).
	CreatePipe(&salidaTmp, &salida_hijo, &sa, 0);

// Creo un pipe de error y no me interesa el de escritura de error (errorTmp).
	CreatePipe(&errorTmp, &error_hijo, &sa, 0);

// Create new stdin write, stdout and stderr read handles.
// Set the properties to FALSE. Otherwise, the child inherits the
// properties and, as a result, non-closeable handles to the pipes
// are created.

	BOOL salida;

	salida = DuplicateHandle(proceso_actual, entradaTmp, proceso_actual, &Entrada_Std, 0, FALSE, DUPLICATE_SAME_ACCESS);

	salida = DuplicateHandle(proceso_actual, salidaTmp, proceso_actual, &Salida_Std, 0, FALSE, DUPLICATE_SAME_ACCESS);

	salida = DuplicateHandle(proceso_actual, errorTmp, proceso_actual, &Error_Std, 0, FALSE, DUPLICATE_SAME_ACCESS);

// Close inheritable copies of the handles you do not want to be inherited.

	CloseHandle(salidaTmp);
	CloseHandle(entradaTmp);
	CloseHandle(errorTmp);

// Start child process with redirected stdout, stdin & stderr
	ejecutar_cmd();

	Ejecutando_Cmd = true;

	DWORD tid=0;
// thread que controla la salida del interprete de comandos
	thread_salida = CreateThread(&sa, 0, (LPTHREAD_START_ROUTINE)leer_salida, this, 0, &tid);

	thread_error = CreateThread(&sa, 0, (LPTHREAD_START_ROUTINE)leer_salida_error, this, 0, &tid);

	thread_control = CreateThread(&sa, 0, (LPTHREAD_START_ROUTINE)controlar_cmd, this, 0, &tid);

	handle_evento = CreateEvent(NULL, TRUE, FALSE, NULL);

	//  leer_entrada(this);

	//  terminar_cmd();
}

HANDLE GuruCmd::ejecutar_cmd()
{
	char				aplicacion[_MAX_PATH];
	STARTUPINFO			info_inicio;
	PROCESS_INFORMATION	info_proceso;

	strcpy(aplicacion,nombre_interprete_comandos(aplicacion));
	strcat(aplicacion," /A ");

	memset(&info_inicio,0,sizeof(STARTUPINFO));
	memset(&info_proceso,0,sizeof(PROCESS_INFORMATION));

	info_inicio.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

	info_inicio.hStdInput = entrada_hijo;
	info_inicio.hStdOutput = salida_hijo;
	info_inicio.hStdError = error_hijo;

	info_inicio.wShowWindow = SW_HIDE;

	info_inicio.cb = sizeof(STARTUPINFO);

// Set up the security attributes struct.
	SECURITY_ATTRIBUTES sa;
	memset(&sa, 0,sizeof(SECURITY_ATTRIBUTES));

	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	CreateProcess(NULL,      // name of executable module
			  aplicacion,      // command line string
				&sa,      // SD
				NULL,      // SD
				TRUE,      // handle inheritance option
			CREATE_NEW_CONSOLE,    // creation flags
				NULL,      // new environment block
				NULL,      // current directory name
			&info_inicio,      // startup information
			&info_proceso      // process information
			  );

	CloseHandle(info_proceso.hThread);

	handle_cmd = info_proceso.hProcess;

	return info_proceso.hProcess;
}

void GuruCmd::terminar_cmd()
{
	SetEvent(handle_evento);

	Ejecutando_Cmd = false;

	WaitForSingleObject(thread_control, INFINITE);

// cierro los handles de la apliacion
	CloseHandle(entrada_hijo);
	CloseHandle(salida_hijo);
	CloseHandle(error_hijo);

// cierro los handles del programa
	CloseHandle(Entrada_Std);
	CloseHandle(Salida_Std);
	CloseHandle(Error_Std);

// cierro los handles del los thread
	if (WaitForSingleObject(thread_salida, 1) == WAIT_TIMEOUT)
		{
		//    logeo.escribir_log("thread salida existente, matando. ...");
		if (WaitForSingleObject(thread_salida, 1000) == WAIT_TIMEOUT)
			TerminateThread(thread_salida, 1);
		}

	if (WaitForSingleObject(thread_error, 1) == WAIT_TIMEOUT)
		{
		//    logeo.escribir_log("thread error existente, matando. ...");

		if (WaitForSingleObject(thread_error, 1000) == WAIT_TIMEOUT)
			TerminateThread(thread_error, 1);
		}

	if (WaitForSingleObject(thread_control, 1) == WAIT_TIMEOUT)
		{
		//    logeo.escribir_log("thread control existente, matando. ...");

		if (WaitForSingleObject(thread_control, 1000) == WAIT_TIMEOUT)
			TerminateThread(thread_control, 1);
		}

	if (WaitForSingleObject(handle_cmd, 500) == WAIT_TIMEOUT)
		TerminateProcess(handle_cmd, 1);
}

bool GuruCmd::enviar_comando(char *comando)
{
	DWORD  leidos = 0;

	if (!WriteFile(Entrada_Std, comando, (DWORD)strlen(comando), &leidos, NULL)) {
		mensaje = error.descripcion();
		return false;
	}

	return true;
}


void GuruCmd::leer_salida(void *yo)
{
	GuruCmd *esto = (GuruCmd *)yo;
	char buffer_salida[4096+1];
	DWORD leidos=0;

	while (esto->Ejecutando_Cmd)
		{
		if (!ReadFile(esto->Salida_Std, buffer_salida, 4096, &leidos, NULL)) {
			esto->mensaje = esto->error.descripcion();
			// esto->writeln(esto->mensaje);
			break;
		}

		buffer_salida[leidos] = '\0';
		esto->on_salida_cmd(buffer_salida);

		memset(buffer_salida,0,4096);
		}

	//  esto->logeo.escribir_log("thread Saliendo de salida. ...");
	ExitThread(0);
}

void GuruCmd::leer_salida_error(void *yo)
{
	GuruCmd *esto = (GuruCmd *)yo;
	char buffer_salida[4096+1];
	DWORD leidos=0;

	while (esto->Ejecutando_Cmd)
		{
		if (!ReadFile(esto->Error_Std, buffer_salida, 4096, &leidos, NULL)) {
			esto->mensaje = esto->error.descripcion();
			break;
		}

		buffer_salida[leidos] = '\0';
		esto->on_salida_error_cmd(buffer_salida);
		}

//  esto->logeo.escribir_log("thread Saliendo de error. ...");
	ExitThread(0);
}

void GuruCmd::controlar_cmd(void *yo)
{
	GuruCmd *esto = (GuruCmd *)yo;
	HANDLE  monitoreo[2];
	DWORD tid=0;

	monitoreo[0] = esto->handle_evento;
	monitoreo[1] = esto->handle_cmd;

	do {

		switch (::WaitForMultipleObjects(2,monitoreo, FALSE, 1))
		  {
		case WAIT_OBJECT_0 + 0 : // cambio el evento que solo se cambia desde terminar_cmd
					  break;

		case WAIT_OBJECT_0 + 1:  //n se terminao el cmd (comando exit desde el cmd?)
					  esto->Ejecutando_Cmd = FALSE;
					  CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)enviar_senial, yo, 0, &tid);
					  break;
		  }

	}while (esto->Ejecutando_Cmd);

	//  esto->logeo.escribir_log("thread Saliendo de control. ...");
	ExitThread(0);
}

void GuruCmd::enviar_senial(void *yo)
{
	GuruCmd *esto = (GuruCmd *)yo;

	esto->terminar_cmd();
}

char *GuruCmd::nombre_interprete_comandos(char *aplicacion)
{
	char buffer[_MAX_PATH+2];

	strcpy(buffer,"");

	GetEnvironmentVariable("ComSpec",buffer,sizeof(buffer));

	if (stricmp(buffer,"") != 0)
		strcpy(aplicacion,buffer);
	else
		strcpy(aplicacion,"");

	return aplicacion;
}

void GuruCmd::on_salida_cmd(char *texto)
{

}

void GuruCmd::on_salida_error_cmd(char *texto)
{

}

void GuruCmd::on_terminar_cmd()
{

}
