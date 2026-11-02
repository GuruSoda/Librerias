#if !defined _GURUGRUPOSNT_H__
#define _GURUGRUPOSNT_H__

#include <GuruListaGenerica.h>
#include <GuruUsuariosNT.h>
#include <GuruWTS.h>
#include <GuruNet.h>

const int GURUGRUPOS_MAX_USR_LEN			= GNLEN;
const int GURUGRUPOS_MAX_SERVER_LEN			= 64;
const int GURUGRUPOS_MAX_FULL_USER_LEN		= 256;

typedef	struct
		{
		char	nombre[GURUGRUPOS_MAX_USR_LEN];
		char	comentario[GURUGRUPOS_MAX_FULL_USER_LEN];
		}grupo;

typedef	struct
		{
		char	miembro[UNLEN];
		}miembro_grupo;

class GuruGruposNT : public GuruNet
{
public:
	GuruGruposNT();
	virtual ~GuruGruposNT();

	dword	ls_lista_grupos(char *servidor = NULL);
	dword	ls_lista_grupos_local(char *servidor = NULL);
	DWORD	ls_lista_grupos_usuario(char *usuario, char *servidor = NULL);

	char *	ls_nombre_grupo();
	char *	ls_comentario_grupo();
	void	ls_autoincrementar_grupo(bool inc=true);
	void	ls_proximo_grupo();
	void	ls_reset_grupo();
	bool	ls_existe_grupo(char *grupo);

	dword	ls_lista_miembros_grupo(char *servidor,char *grupo);
	dword	ls_lista_miembros_grupo_local(char *servidor,char *grupo);
	bool	ls_existe_usuario_en_grupo(char *nombre_usuario);
	char *	ls_nombre_miembro_grupo();
	void	ls_autoincrementar_miembro_grupo(bool inc);
	void	ls_proximo_miembro_grupo();
	void	ls_reset_miembro_grupo();

protected:
	GuruListaGenerica <grupo> data_grupos;
	GuruListaGenerica <miembro_grupo> data_miembros_grupo;
};

#endif
