// GuruUsuariosNt.h: interface for the GuruUsuariosNt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GURUUSUARIOSNT_H__616FEF53_B851_4AFF_A07A_D10CF9EA4D22__INCLUDED_)
#define AFX_GURUUSUARIOSNT_H__616FEF53_B851_4AFF_A07A_D10CF9EA4D22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
//
//  Linkear con Netapi32.lib Advapi32.lib
//
//////////////////////////////////////////////////////////////////////


#include "GuruError.h"
#include "GuruListaGenerica.h"

#include "GuruNet.h"

#define			GURUUSUARIOS_MAX_USR_LEN			UNLEN
#define			GURUUSUARIOS_MAX_SERVER_LEN			64
#define			GURUUSUARIOS_MAX_FULL_USER_LEN		256

#define			GURUUSUARIOS_USUARIO_NO_BLOQUEADO	"El usuario no se encuentra bloqueado"

typedef	struct
		{
		char	nombre[GURUUSUARIOS_MAX_USR_LEN];
		char	nombre_completo[GURUUSUARIOS_MAX_FULL_USER_LEN];
		char	descripcion[MAXCOMMENTSZ+1];
		dword	flags;
		}reg_usuario;

typedef void (listado_usuarios) (reg_usuario * data_usuario, void *extra_parametro);

class GuruUsuariosNT : public GuruNet
{
public:
	GuruUsuariosNT();
	virtual ~GuruUsuariosNT();

	dword	ls_lista_usuarios(char *servidor = NULL);
	char *	ls_nombre_usuario();
	char *	ls_nombre_completo_usuario();
	char *	ls_descripcion_usuario();
	void	ls_proximo_usuario();
	void	ls_autoincrementar_usuario(bool inc=TRUE);
	void	ls_reset_usuario();
	bool	ls_cuenta_deshabilitada_usuario();
	bool	ls_cuenta_bloqueada_usuario();
	bool	ls_desbloquear_cuenta_usuario();
	bool	ls_poner_unico_usuario(char *nombre_usuario, char *servidor = NULL);

	void	cargar_extra_parametro(void *extra_parametro);
	void	cargar_notificacion_listado(listado_usuarios *);

	char *	sacar_SID(char *usuario, char **str_SID);

	int		info_usuario_3(char *usuario,USER_INFO_3 *info_3,char *servidor);

	static char* Sid_a_str(SID *sid, char *buffer_destino, dword *tamanio_buffer);

private:
	GuruListaGenerica <reg_usuario> data_usuarios;

	listado_usuarios *	notificar_listado;

	void *				extra_parametro;
	char				servidor_cuentas[UNCLEN + 1]; // nombre donde se encontro la cuenta (PDC ?)
};

#endif // !defined(AFX_GURUUSUARIOSNT_H__616FEF53_B851_4AFF_A07A_D10CF9EA4D22__INCLUDED_)
