// GuruCmdSocket.cpp: implementation of the GuruCmdSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuruCmdSocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuruCmdSocket::GuruCmdSocket()
{
	Guru_Cmd_Socket = INVALID_SOCKET;
}

GuruCmdSocket::~GuruCmdSocket()
{

}

void GuruCmdSocket::on_salida_cmd(char *texto)
{
	int cant_caracteres;

	cant_caracteres = send(Guru_Cmd_Socket, texto, (int)strlen(texto), 0);
}

void GuruCmdSocket::on_salida_error_cmd(char *texto)
{
	int cant_caracteres;

	cant_caracteres = send(Guru_Cmd_Socket, texto, (int)strlen(texto), 0);
}

void GuruCmdSocket::on_terminar_cmd()
{

}

bool GuruCmdSocket::interprete_comandos(SOCKET Guru_Socket)
{
	Guru_Cmd_Socket = Guru_Socket;
	char	buffer_teclado[256];
	int		cant_caracteres;

	fd_set	set;
	struct	timeval espera;

	cargar_cmd();

	do{
		FD_ZERO(&set);

		memset(&espera,0,sizeof(struct timeval));

		espera.tv_sec = 1; // 1.000.000 de microsegundos == 1 segundo

//		espera.tv_usec = 500000; // medio segundo

		FD_SET(Guru_Socket,&set);

		cant_caracteres = select(0,&set,0,0,&espera);

		if (cant_caracteres == 0) continue;

		cant_caracteres = recv(Guru_Socket, buffer_teclado, 256, 0);

//		DebugBreak();

		if (cant_caracteres == 0 || cant_caracteres == SOCKET_ERROR)
			{
			terminar_cmd();
			break;
			}

		buffer_teclado[cant_caracteres] = '\0';

		if (!enviar_comando(buffer_teclado))
			{
			Ejecutando_Cmd = FALSE;
			break;
			}

	}while (Ejecutando_Cmd);

	return true;
}
