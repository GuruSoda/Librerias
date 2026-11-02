// GuruPipes.cpp: implementation of the GuruPipes class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "GuruPipes.h"
#include <Aclapi.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuruPipes::GuruPipes()
{
	pipe			=	INVALID_HANDLE_VALUE;
	modo_apertura	=	PIPE_ACCESS_DUPLEX | FILE_FLAG_WRITE_THROUGH;
	modo_pipe		=	PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT;
	tiempo_espera	=	NMPWAIT_USE_DEFAULT_WAIT; // ESPERA_PIPES;

	escuchar_entradas			= TRUE;
	handle_escuchar_entradas	= NULL;
	soy_servidor_pipe			= FALSE;

	nombre_pipe		=	new char [254];
	msg_error		=	new char [254];
}

GuruPipes::~GuruPipes()
{
	DWORD salida=0;

	if (nombre_pipe)
		delete nombre_pipe;

	if (msg_error)
		delete msg_error;


	if (handle_escuchar_entradas)
		{
		if (!GetExitCodeThread(handle_escuchar_entradas,&salida))
			{
			if (escuchar_entradas)
				{
				escuchar_entradas = FALSE;
				Sleep(INTERVALO_SCAN_PIPES*2);
				}

			if (pipe != INVALID_HANDLE_VALUE)
				{
				DisconnectNamedPipe(pipe);
				}
			}
		}
	else
		if (pipe != INVALID_HANDLE_VALUE)
			DisconnectNamedPipe(pipe);
}

BOOL GuruPipes::abrir(const char *nombre)
{
	BOOL		salida;
	GuruError	error;
	SECURITY_ATTRIBUTES seguridad;
	SECURITY_DESCRIPTOR	*sd;

	strcpy(nombre_pipe,nombre);

	sd = (SECURITY_DESCRIPTOR *) LocalAlloc(LPTR,SECURITY_DESCRIPTOR_MIN_LENGTH);

	// inicializo el security descriptor
	InitializeSecurityDescriptor(sd,SECURITY_DESCRIPTOR_REVISION);

	// agrega un null al descriptor ACL
	SetSecurityDescriptorDacl(sd,TRUE,(PACL)NULL,FALSE);

	seguridad.nLength = sizeof(SECURITY_ATTRIBUTES);
	seguridad.bInheritHandle = TRUE;
	seguridad.lpSecurityDescriptor = sd;

	pipe = CreateFile(
			nombre_pipe,			// file name
		GENERIC_READ | GENERIC_WRITE,			// access mode
		FILE_SHARE_WRITE | FILE_SHARE_READ,						// share mode
			&seguridad,				// SD
			OPEN_EXISTING,			// how to create
FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,	// file attributes
			NULL					// handle to template file
			);

	if (pipe == INVALID_HANDLE_VALUE)
		{
		salida = FALSE;
		strcpy(msg_error,error.descripcion());
		}
	else
		{
		salida = TRUE;
		soy_servidor_pipe = FALSE; // soy un cliente de pipes
		}

	return salida;
}


BOOL GuruPipes::crear(const char *nombre)
{
	BOOL		salida=TRUE;
	SECURITY_ATTRIBUTES seguridad;
	SECURITY_DESCRIPTOR	*sd;

	strcpy(nombre_pipe,nombre);

	sd = (SECURITY_DESCRIPTOR *) LocalAlloc(LPTR,SECURITY_DESCRIPTOR_MIN_LENGTH);

	// inicializo el security descriptor
	salida = InitializeSecurityDescriptor(sd,SECURITY_DESCRIPTOR_REVISION);

	// agrega un null al descriptor ACL
	salida = SetSecurityDescriptorDacl(sd,TRUE,(PACL)NULL,FALSE);


	seguridad.nLength = sizeof(SECURITY_ATTRIBUTES);
	seguridad.bInheritHandle = TRUE;
	seguridad.lpSecurityDescriptor = sd;

	pipe = CreateNamedPipe(
				nombre_pipe,		// pipe name
				modo_apertura,		// pipe open mode
				modo_pipe,			// pipe-specific modes
				PIPE_UNLIMITED_INSTANCES,	// maximum number of instances
				BUFFER_SALIDA,				// output buffer size
				BUFFER_ENTRADA,				// input buffer size
				tiempo_espera,				// time-out interval
				&seguridad						// SD
				);

	if (pipe == INVALID_HANDLE_VALUE)
		{
		salida = FALSE;
		strcpy(msg_error,error.descripcion());
		}
	else
		{
/*		if (!acceso("Administrators"))
			strcpy(msg_error,error.descripcion());
*/
		salida = TRUE;
		soy_servidor_pipe = TRUE; // soy un servidor que escucha pipes
		}

	return salida;
}

BOOL GuruPipes::leer(void *buffer,DWORD a_leer)
{
	BOOL salida;

	if (pipe == INVALID_HANDLE_VALUE)
		return FALSE;

	salida = ReadFile(
				pipe,				// handle to file
				buffer,				// data buffer
				a_leer,				// number of bytes to read
				&a_leer,			// number of bytes read
				NULL				// overlapped buffer
				);

	if (!salida)	strcpy(msg_error,error.descripcion());

	return salida;
}

BOOL GuruPipes::leer_cadena(char *buffer,DWORD a_leer)
{
	BOOL salida;

	if (pipe == INVALID_HANDLE_VALUE)
		{
		strcpy(msg_error,error.descripcion());
		return FALSE;
		}

	salida = ReadFile(
				pipe,				// handle to file
				buffer,				// data buffer
				a_leer,				// number of bytes to read
				&a_leer,			// number of bytes read
				NULL				// overlapped buffer
				);

	if (!salida)
		strcpy(msg_error,error.descripcion());
	else
		{
		buffer[a_leer] = '\0';
		}
	return salida;
}



BOOL GuruPipes::escribir(void *buffer,DWORD a_escribir)
{
	BOOL		salida;

	if (pipe == INVALID_HANDLE_VALUE)
		return FALSE;

	salida = WriteFile(
					pipe,			// handle to file
					buffer,			// data buffer
					a_escribir,		// number of bytes to write
					&a_escribir,	// number of bytes written
					NULL			// overlapped buffer
					);

	if (!salida)	strcpy(msg_error,error.descripcion());

	return salida;
}

BOOL GuruPipes::escribir_cadena(const char *buffer)
{
	BOOL		salida;
	DWORD		a_escribir=0;

	if (pipe == INVALID_HANDLE_VALUE)
		return FALSE;

	salida = WriteFile(
					pipe,			// handle to file
					buffer,			// data buffer
					strlen(buffer),		// number of bytes to write
					&a_escribir,	// number of bytes written
					NULL			// overlapped buffer
					);

	if (!salida)	strcpy(msg_error,error.descripcion());

	return salida;
}

BOOL GuruPipes::esperar(DWORD milisegundos)
{
	BOOL	salida;

	if (milisegundos == 1) /* quien va a poner 1 milisegundo?*/
		milisegundos = tiempo_espera;

	salida = WaitNamedPipe(
						nombre_pipe,		// pipe name
						milisegundos		// time-out interval
						);

	if (!salida)	strcpy(msg_error,error.descripcion());

	return salida;
}

// este metodo hace que se detenga la ejecucion del programa hasta
// que un cliente se conecte a ese pipe creado
BOOL GuruPipes::conectar()
{
	BOOL		salida;

	salida = ConnectNamedPipe(pipe,NULL);

//	salida = WaitForSingleObject(pipe,INFINITE);
	/*
	do {
		if (!escuchar_entradas) return FALSE;

		salida = WaitForSingleObject(pipe,INTERVALO_SCAN_PIPES);

	} while(salida == WAIT_TIMEOUT);*/

//	while ((salida = WaitForSingleObject(pipe,INTERVALO_SCAN_PIPES)) == WAIT_TIMEOUT || escuchar_entradas)
//				;

//	MessageBox(NULL, "Despues de connect", "Cuidado", MB_OK | MB_ICONQUESTION | MB_APPLMODAL);


	if (!salida || GetLastError() == ERROR_PIPE_CONNECTED)
		{
		salida = FALSE;
		strcpy(msg_error,error.descripcion());
		}

	return salida;
}

char * GuruPipes::mensaje_error()
{
	return msg_error;
}


void GuruPipes::cortar()
{
	if (soy_servidor_pipe)
		DisconnectNamedPipe(pipe);
	else // soy un cliente
		CloseHandle(pipe);


	pipe = INVALID_HANDLE_VALUE;
}

// este metodo cambia el handle del pipe por el pasado por parametro y retorna el antiguo
HANDLE GuruPipes::nuevo_handle(HANDLE nuevo)
{
	HANDLE viejo = handle_pipe();

	pipe = nuevo;

	return viejo;
}

void GuruPipes::liberar()
{
	FlushFileBuffers(pipe);
}

HANDLE GuruPipes::handle_pipe()
{
	return pipe;
}


void GuruPipes::ver_cmd()
{
#define			BUFFER_OUTPUT			4096

//    HANDLE leer;
	HANDLE salida_actual;
	HANDLE entrada_actual;
	HANDLE error_actual;

	// salvar en standard output
	if ((salida_actual = GetStdHandle(STD_OUTPUT_HANDLE)) == INVALID_HANDLE_VALUE)
		return ;

	// salvar en standard input
	if ((entrada_actual = GetStdHandle(STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE)
		return ;

	// salvar en standard error
	if ((error_actual = GetStdHandle(STD_ERROR_HANDLE)) == INVALID_HANDLE_VALUE)
		return ;

/*
	BOOL DuplicateHandle(
						::GetCurrentProcess(),  // handle to source process
						handle_pipe(),         // handle to duplicate
  HANDLE hTargetProcessHandle,  // handle to target process
  LPHANDLE lpTargetHandle,      // duplicate handle
  DWORD dwDesiredAccess,        // requested access
  BOOL bInheritHandle,          // handle inheritance option
  DWORD dwOptions               // optional actions
);
*/

	if (!SetStdHandle(STD_OUTPUT_HANDLE,handle_pipe()))
		return ;

	if (!SetStdHandle(STD_INPUT_HANDLE,handle_pipe()))
		return ;

	if (!SetStdHandle(STD_ERROR_HANDLE,handle_pipe()))
		return ;

//	CloseHandle(leer);
}

BOOL GuruPipes::acceso(char *grupo)
{
	SID_NAME_USE use;
	DWORD		tamanio_grupo=0;
	DWORD		tamanio_dominio=0;
	char		buffer[50];
	void *		ptr_sid = NULL;
	BOOL		salida;
	SE_OBJECT_TYPE tipo = SE_FILE_OBJECT;

	if (!LookupAccountName(NULL,grupo,NULL, &tamanio_grupo, NULL, &tamanio_dominio, &use))
		{
		ptr_sid = (SID *)new char [tamanio_grupo];
		if (!ptr_sid) return FALSE;

		if (LookupAccountName(NULL,grupo,ptr_sid, &tamanio_grupo, buffer, &tamanio_dominio, &use))
			{
			if (SetSecurityInfo(
								pipe,			// handle to object
								tipo,			// object type
								GROUP_SECURITY_INFORMATION, // buffer
								NULL,			// new owner SID
								ptr_sid,			// new primary group SID
								NULL,			// new DACL
								NULL			// new SACL
								) != ERROR_SUCCESS)
				salida = TRUE;
			else
				{
				salida = FALSE;
				strcpy(msg_error,error.descripcion());
				}

			}
		else
			{
			salida = FALSE;
			strcpy(msg_error,error.descripcion());
			}
		}
	else
		{
		salida = FALSE;
		strcpy(msg_error,error.descripcion());
		}

	FreeSid(ptr_sid);

	return salida ;
}


// ============================== funciones para escuchar las entradas de los clientes ==========

void GuruPipes::escuchar(char *nuevo_pipe)
{
	SECURITY_ATTRIBUTES seguridad;
	GuruPipes *esto = this;
	DWORD threadid;

	seguridad.bInheritHandle	= TRUE;
	seguridad.nLength			= sizeof(SECURITY_ATTRIBUTES);
	seguridad.lpSecurityDescriptor = NULL;

	strcpy(nombre_pipe,nuevo_pipe);

	handle_escuchar_entradas = CreateThread(&seguridad,		// SD
								0,			// initial stack size
				(LPTHREAD_START_ROUTINE) tomar_entradas_pipes,    // thread function
								esto,		// thread argument
								0,			// creation option
								&threadid		// thread identifier
								);
}

void GuruPipes::tomar_entradas_pipes(GuruPipes *esto)
{

	while (esto->escuchar_entradas)
		{
		if (!esto->crear(esto->nombre_pipe))
			{
			strcpy(esto->msg_error,esto->error.descripcion());
			break;
			}
		else
			strcpy(esto->msg_error,esto->error.descripcion()); // esto->logeo.escribir_log("Creando nueva instancia %s",NOMBRE_PIPE);

		if (esto->conectar())// se ha realizado alguna conexion?
			{

			if (esto->aceptar_entrada())
				{

				}
			}
		else // conectar
			{
			strcpy(esto->msg_error,esto->error.descripcion());
			esto->cortar();
			}
		}

	ExitThread(0);
}


BOOL GuruPipes::aceptar_entrada()
{

// ejemplo de lo que deberia hacer una clase derivada con este metodo.

/*
	Inicio_Pipe *nueva_conexion;

	nueva_conexion = new Inicio_Pipe(llamador);

	if (nueva_conexion)
		aceptada(nueva_conexion,this);
	else
		return FALSE;

	return TRUE;

*/
	return TRUE;
}

void GuruPipes::aceptada(GuruPipes *nueva,GuruPipes *actual)
{
	SECURITY_ATTRIBUTES seguridad;

	seguridad.bInheritHandle	= TRUE;
	seguridad.nLength			= sizeof(SECURITY_ATTRIBUTES);
	seguridad.lpSecurityDescriptor = NULL;

	DWORD threadid;

	nueva->pipe = actual->pipe;

	if (!CreateThread(&seguridad,0,(LPTHREAD_START_ROUTINE) nueva->nueva_conexion, nueva, 0, &threadid))
		{
		strcpy(actual->msg_error,actual->error.descripcion()); // esto->logeo.escribir_log("Error creando requerimiento = %s",pipes->mensaje_error());
		actual->cortar();
		// break;			// tal vez
		}
}

void GuruPipes::nueva_conexion(GuruPipes *llamador)
{
	llamador->main_pipes();

	FlushFileBuffers(llamador->pipe);

	llamador->cortar();

	delete llamador;

	ExitThread(0);
}

void GuruPipes::main_pipes()
{

}
