// GuruProtocoloWTS.h: interface for the GuruProtocoloWTS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GURUPROTOCOLOWTS_H__54D5F786_9CE5_4D20_8BF8_71DB8FBDDB4B__INCLUDED_)
#define AFX_GURUPROTOCOLOWTS_H__54D5F786_9CE5_4D20_8BF8_71DB8FBDDB4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GuruSocketProtocolo.h"
#include "GuruWTS.h"
#include "GuruBuffer.h"

//////////////////////////////////////////////////////////////////////
// Comandos
//////////////////////////////////////////////////////////////////////
const char			CMD_TOTAL_SESIONES[]	= "Total-Sesiones";
const char			CMD_SERVIDOR[]			= "Servidor";
const char			CMD_USUARIO[]			= "Usuario";
const char			CMD_ESTADO_USUARIOS[]	= "Estado-Usuarios";

//////////////////////////////////////////////////////////////////////
// Mensajes en general
//////////////////////////////////////////////////////////////////////
#define		MSG_SIN_PARAMETROS		"Comando Sin Parametros."
#define		MSG_SIN_AYUDA			"Ayuda no disponible"
const char  MSG_SERVIDOR_LOCAL[]  = "LocalHost";


//////////////////////////////////////////////////////////////////////
// Mensajes de ayuda para los comandos
//////////////////////////////////////////////////////////////////////
const char HLP_USUARIOS[]       = MSG_SIN_PARAMETROS;
const char HLP_TOTAL_SESIONES[] = MSG_SIN_AYUDA;
const char HLP_SERVIDOR[]       = "Servidor <Nombre_Servidor>";
const char HLP_USUARIO[]        = "Usuario";

class GuruProtocoloWTS : public GuruSocketProtocolo  
{
public:
	GuruProtocoloWTS(SOCKET socket);
	virtual ~GuruProtocoloWTS();

	bool	inicializar();

	void	protocolo();

	static void cmd_estado_usuarios(int cant_param, char *parametros[], GuruProtocoloWTS * esto);
	static void cmd_total_sesiones(int cant_param, char *parametros[], GuruProtocoloWTS * esto);
	static void cmd_servidor(int cant_param, char *parametros[], GuruProtocoloWTS * esto);
	static void cmd_usuario(int cant_param, char *parametros[], GuruProtocoloWTS * esto);

private:

	GuruWTS wts;
	GuruBuffer nombre_servidor;
};

#endif // !defined(AFX_GURUPROTOCOLOWTS_H__54D5F786_9CE5_4D20_8BF8_71DB8FBDDB4B__INCLUDED_)
