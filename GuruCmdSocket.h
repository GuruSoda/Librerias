// GuruCmdSocket.h: interface for the GuruCmdSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GURUCMDSOCKET_H__16A72DF1_03A9_11D5_B618_006097D8B488__INCLUDED_)
#define AFX_GURUCMDSOCKET_H__16A72DF1_03A9_11D5_B618_006097D8B488__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GuruCmd.h"
#include <winsock.h>


class GuruCmdSocket : public GuruCmd  
{
public:
	GuruCmdSocket();
	virtual ~GuruCmdSocket();

	bool interprete_comandos(SOCKET Guru_Socket);

	virtual	void	on_salida_cmd(char *texto);
	virtual	void	on_salida_error_cmd(char *texto);
	virtual	void	on_terminar_cmd();

private:

	SOCKET Guru_Cmd_Socket;

};

#endif // !defined(AFX_GURUCMDSOCKET_H__16A72DF1_03A9_11D5_B618_006097D8B488__INCLUDED_)
