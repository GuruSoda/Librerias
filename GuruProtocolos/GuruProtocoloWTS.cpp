// GuruProtocoloWTS.cpp: implementation of the GuruProtocoloWTS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuruProtocoloWTS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GuruProtocoloWTS::GuruProtocoloWTS(SOCKET socket):GuruSocketProtocolo(socket)
{
	wts.sesiones.autoincrementar(false);
	wts.procesos.autoincrementar(false);
}

GuruProtocoloWTS::~GuruProtocoloWTS()
{

}

bool GuruProtocoloWTS::inicializar()
{
	return wts.abrir(NULL);
}

void GuruProtocoloWTS::protocolo()
{
	cargar_comando(CMD_ESTADO_USUARIOS, (funcion_comando *)cmd_estado_usuarios, "Lista el total de usuarios conectados.");
	cargar_comando(CMD_TOTAL_SESIONES, (funcion_comando *)cmd_total_sesiones, "Muestra la cantidad de usuarios conectados.");
	cargar_comando(CMD_SERVIDOR, (funcion_comando *)cmd_servidor, "Se conecta a otro sevidor via WTS.");
	cargar_comando(CMD_USUARIO, (funcion_comando *)cmd_usuario, "Permite verificar la existensia de un usuario.");
}

void GuruProtocoloWTS::cmd_estado_usuarios(int cant_param, char *parametros[], GuruProtocoloWTS * esto)
{
	dword cargados = 0;

	int campos[] = {WTSUserName, WTSConnectState};

	if (!esto->wts.ls_sessiones(campos, 2, &cargados))
		{
		esto->enviar_error(esto->wts.mensaje_error());
		return ;
		}

	GuruBuffer linea;

	while (cargados--)
		{
		linea = esto->wts.sesiones.item()->nombre_usuario.contenido();
		linea += " ";
		linea += esto->wts.estado_a_str(esto->wts.sesiones.item()->estado);

		esto->enviar_no_error(linea.contenido());
		esto->wts.sesiones++;
		}

	esto->enviar_error();
}

void GuruProtocoloWTS::cmd_total_sesiones(int cant_param, char *parametros[], GuruProtocoloWTS * esto)
{
	dword cargados = 0;
	int campos[] = {WTSUserName, WTSConnectState};
	if (!esto->wts.ls_sessiones(campos, 2, &cargados))
		{
		esto->enviar_error(esto->wts.mensaje_error());
		return ;
		}

	char numero[15];
	esto->enviar_no_error(ltoa(cargados, numero, 10));
}

void GuruProtocoloWTS::cmd_servidor(int cant_param, char *parametros[], GuruProtocoloWTS * esto)
{
	switch (cant_param)
		{
	case 1:
			if (esto->nombre_servidor.longitud_data() == 0)
				esto->enviar_no_error(MSG_SERVIDOR_LOCAL);
			else
				esto->enviar_no_error(esto->nombre_servidor.contenido());

			break;
	case 2:
			if (esto->wts.abrir(parametros[1]))
				{
				esto->nombre_servidor = parametros[1];
				esto->enviar_no_error();
				}
			else
				esto->enviar_error(esto->wts.mensaje_error());

			break;

	default:
		esto->enviar_error(HLP_SERVIDOR);
		}
}

void GuruProtocoloWTS::cmd_usuario(int cant_param, char *parametros[], GuruProtocoloWTS * esto)
{
	if (cant_param == 2)
		{
		dword cargados = 0;
		int campos[] = {WTSUserName, WTSConnectState};
		if (!esto->wts.ls_sessiones(campos, 2, &cargados))
			{
			esto->enviar_error(esto->wts.mensaje_error());
			return ;
			}

		while (cargados--)
			{
			if (stricmp(parametros[1], esto->wts.sesiones.item()->nombre_usuario.contenido()) == 0)
				esto->enviar_no_error(esto->wts.estado_a_str(esto->wts.sesiones.item()->estado));

			esto->wts.sesiones++;
			}

		esto->enviar_error();
		}
	else
		esto->enviar_error(HLP_USUARIO);
}
