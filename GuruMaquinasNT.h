// GuruMaquinasNT.h: interface for the GuruMaquinasNT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GURUMAQUINASNT_H__9B94A30F_9C4B_435B_8DCB_EBC7F8726255__INCLUDED_)
#define AFX_GURUMAQUINASNT_H__9B94A30F_9C4B_435B_8DCB_EBC7F8726255__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GuruError.h"
#include "GuruListaGenerica.h"

#include <Lmcons.h>
#include "GuruNet.h"

#define			GURUMAQUINAS_MAX_USR_LEN				UNLEN
#define			GURUMAQUINAS_MAX_SERVER_LEN				64
#define			GURUMAQUINAS_MAX_FULL_USER_LEN			256

typedef	struct
		{
		char	nombre[GURUMAQUINAS_MAX_USR_LEN];
		char	comentario[GURUMAQUINAS_MAX_FULL_USER_LEN];
		dword	tipo;
		}maquina;

class GuruMaquinasNT : public GuruNet  
{
public:
	GuruMaquinasNT();
	virtual ~GuruMaquinasNT();

	dword	ls_lista_maquinas(char *servidor = NULL);
	char *	ls_nombre_maquina();
	char *	ls_comentario_maquina();
	dword	ls_tipo_maquina();
	bool	ls_es_BDC_maquina();
	bool	ls_es_PDC_maquina();
	bool	ls_es_Estacion_maquina();
	bool	ls_es_Server_maquina();

	void	ls_proximo_maquina();
	void	ls_autoincrementar_maquina(bool inc=true);
	void	ls_reset_maquina();

private:

	GuruListaGenerica <maquina> data_maquinas;

	char	actual_servidor[64];
};

#endif // !defined(AFX_GURUMAQUINASNT_H__9B94A30F_9C4B_435B_8DCB_EBC7F8726255__INCLUDED_)
