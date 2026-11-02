#include "GuruGruposNT.h"

GuruGruposNT::GuruGruposNT()
{

}

GuruGruposNT::~GuruGruposNT()
{

}

DWORD GuruGruposNT::ls_lista_grupos(char *servidor)
{
	DWORD	cantidad=0;								// almacena la cantidad de usuarios cargados.
	WCHAR	wservidor[GURUGRUPOS_MAX_SERVER_LEN];	// almacena el nombre del servidor en unicode
	DWORD	cant_entradas_leidas=0;					// almacena la cantidad de entradas leidas por la API
	DWORD	salida;									// almacena el resultado de la ejecucion de la API
	void	*info;									// almacena la lista retornada por la API
	NET_DISPLAY_GROUP *data;						// temporal para la data retornada por la API
	grupo	reg_grupo;								// registro donde almaceno la data del usuario

	DWORD	cant_grupos = 0;
	cant_grupos--;

	if (!data_grupos.EstaVacia()) data_grupos.vaciar_lista();

	if (servidor)
		MultiByteToWideChar(CP_ACP, 0, (const char *)servidor, strlen(servidor)+1, wservidor, sizeof(wservidor));
	else
		wservidor[0] = (WCHAR)0;

	salida = NetQueryDisplayInformation(wservidor,
										3,
										0,
										(DWORD)-1,
										(DWORD)-1,
										&cant_entradas_leidas,
										&info);

	if (salida == ERROR_SUCCESS)
		{
		data = (NET_DISPLAY_GROUP*)info;

		for (;cant_entradas_leidas>0;cant_entradas_leidas--)
			{
			wideachar(data->grpi3_name,reg_grupo.nombre,sizeof(reg_grupo.nombre));
			wideachar(data->grpi3_comment,reg_grupo.comentario,sizeof(reg_grupo.comentario));

			data_grupos.agregar(&reg_grupo);

			data ++;
			cantidad++;
			}
		}

	NetApiBufferFree(info);

	return cantidad;
}

DWORD GuruGruposNT::ls_lista_grupos_local(char *servidor)
{
	DWORD	cantidad=0;								// almacena la cantidad de usuarios cargados.
	WCHAR	wservidor[GURUGRUPOS_MAX_SERVER_LEN];	// almacena el nombre del servidor en unicode
	DWORD	cant_entradas_leidas=0;					// almacena la cantidad de entradas leidas por la API
	DWORD	salida;									// almacena el resultado de la ejecucion de la API
	void	*info;									// almacena la lista retornada por la API
	LOCALGROUP_INFO_1 *data;						// temporal para la data retornada por la API
	grupo	reg_grupo;								// registro donde almaceno la data del usuario

	DWORD	cant_grupos = 0;
	cant_grupos--;

	if (!data_grupos.EstaVacia()) data_grupos.vaciar_lista();

	if (servidor)
		MultiByteToWideChar(CP_ACP, 0, (const char *)servidor, strlen(servidor)+1, wservidor, sizeof(wservidor));
	else
		wservidor[0] = (WCHAR)0;

	salida = NetLocalGroupEnum(wservidor,
								1,
								(unsigned char **)&info,
								(DWORD)-1,
							&cant_entradas_leidas,
								&cant_grupos,
								NULL);

/*
	salida = NetQueryDisplayInformation(wservidor,
										3,
										0,
										(DWORD)-1,
										(DWORD)-1,
										&cant_entradas_leidas,
										&info);
*/
	if (salida == ERROR_SUCCESS)
		{
		data = (LOCALGROUP_INFO_1*)info;

		for (;cant_entradas_leidas>0;cant_entradas_leidas--)
			{
			wideachar(data->lgrpi1_name,reg_grupo.nombre,sizeof(reg_grupo.nombre));
			wideachar(data->lgrpi1_comment,reg_grupo.comentario,sizeof(reg_grupo.comentario));

			data_grupos.agregar(&reg_grupo);

			data ++;
			cantidad++;
			}
		}

	NetApiBufferFree(info);

	return cantidad;
}

DWORD GuruGruposNT::ls_lista_grupos_usuario(char *usuario, char *servidor)
{
	DWORD	cantidad=0;								// almacena la cantidad de usuarios cargados.
	WCHAR	wservidor[GURUGRUPOS_MAX_SERVER_LEN];	// almacena el nombre del servidor en unicode
	WCHAR	wuser[GURUGRUPOS_MAX_SERVER_LEN];		// almacena el nombre del usuario en unicode
	DWORD	cant_entradas_leidas=0;					// almacena la cantidad de entradas leidas por la API
	DWORD	cant_total_entradas=0;					// almacena el total de entrada que tendria que leer
	DWORD	salida;									// almacena el resultado de la ejecucion de la API
	unsigned char *info;									// almacena la lista retornada por la API
	GROUP_USERS_INFO_0 *data;						// temporal para la data retornada por la API
	grupo	reg_grupo;								// registro donde almaceno la data del usuario

	DWORD	cant_grupos = 0;
	cant_grupos--;

	if (!data_grupos.EstaVacia()) data_grupos.vaciar_lista();

	if (servidor)
		MultiByteToWideChar(CP_ACP, 0, (const char *)servidor, strlen(servidor)+1, wservidor, sizeof(wservidor));
	else
		wservidor[0] = (WCHAR)0;

	MultiByteToWideChar(CP_ACP, 0, (const char *)usuario, strlen(usuario)+1, wuser, sizeof(wuser));

	salida = NetUserGetGroups(	wservidor,
								wuser,
								0,
								&info,
								MAX_PREFERRED_LENGTH,
								&cant_entradas_leidas,
								&cant_total_entradas);
/*
	salida = NetQueryDisplayInformation(wservidor,
										3,
										0,
										(DWORD)-1,
										(DWORD)-1,
										&cant_entradas_leidas,
										&info);
*/
	if (salida == ERROR_SUCCESS)
		{
		data = (GROUP_USERS_INFO_0*)info;

		for (;cant_entradas_leidas>0;cant_entradas_leidas--)
			{
			wideachar(data->grui0_name, reg_grupo.nombre, sizeof(reg_grupo.nombre));

			data_grupos.agregar(&reg_grupo);

			data ++;
			cantidad++;
			}
		}

	NetApiBufferFree(info);

	return cantidad;
}

DWORD GuruGruposNT::ls_lista_miembros_grupo(char *servidor,char *grupo)
{
	DWORD	cantidad=0;								// almacena la cantidad de maquinas cargados.
	WCHAR	wservidor[GURUGRUPOS_MAX_SERVER_LEN];		// almacena el nombre del servidor en unicode
	WCHAR	wgrupo[GURUGRUPOS_MAX_SERVER_LEN];			// almacena el nombre del servidor en unicode
	DWORD	cant_entradas_leidas=0;					// almacena la cantidad de entradas leidas por la API
	DWORD	salida;									// almacena el resultado de la ejecucion de la API
	LPBYTE	info = {0};									// almacena la lista retornada por la API
	GROUP_USERS_INFO_0 *data;						// temporal para la data retornada por la API
	miembro_grupo	reg_miembro;							// registro donde almaceno la data del usuario
	DWORD	total_entradas=0;
	DWORD	handle=0;

	DWORD cant_usrs = 0;
	cant_usrs--;

	if (!data_miembros_grupo.EstaVacia()) data_miembros_grupo.vaciar_lista();

	if (grupo)
		MultiByteToWideChar(CP_ACP, 0, (const char *)grupo, strlen(grupo)+1, wgrupo, sizeof(wgrupo));
	else
		return 0;

	if (servidor)
		MultiByteToWideChar(CP_ACP, 0, (const char *)servidor, strlen(servidor)+1, wservidor, sizeof(wservidor));
	else
		wservidor[0] = (WCHAR)0;

//	salida = NetGroupGetUsers(wservidor, wgrupo, 0, &info, 0, &cant_entradas_leidas, &total_entradas, &handle);

	salida = NetGroupGetUsers(wservidor,
								wgrupo,
								0,
								&info,
								(DWORD)-1,
								&cant_entradas_leidas,
								&total_entradas,
								&handle);

	if (salida == ERROR_SUCCESS)
		{
		data = (GROUP_USERS_INFO_0*)info;

		for (;cant_entradas_leidas>0;cant_entradas_leidas--)
			{
			wideachar(data->grui0_name,reg_miembro.miembro,sizeof(reg_miembro.miembro));

			data_miembros_grupo.agregar(&reg_miembro);

			cantidad++;
			data ++;
			}
		}
	else
		GuruNet_Error.descripcion(salida);

	NetApiBufferFree(info);

	return cantidad;
}

DWORD GuruGruposNT::ls_lista_miembros_grupo_local(char *servidor,char *grupo)
{
	DWORD	cantidad=0;								// almacena la cantidad de maquinas cargados.
	WCHAR	wservidor[GURUGRUPOS_MAX_SERVER_LEN];		// almacena el nombre del servidor en unicode
	WCHAR	wgrupo[GURUGRUPOS_MAX_SERVER_LEN];			// almacena el nombre del servidor en unicode
	DWORD	cant_entradas_leidas=0;					// almacena la cantidad de entradas leidas por la API
	DWORD	salida;									// almacena el resultado de la ejecucion de la API
	LPBYTE	info = {0};									// almacena la lista retornada por la API
	LOCALGROUP_MEMBERS_INFO_0 *data;						// temporal para la data retornada por la API
	miembro_grupo	reg_miembro;							// registro donde almaceno la data del usuario
	DWORD	total_entradas=0;
	DWORD	handle=0;

	DWORD cant_usrs = 0;
	cant_usrs--;

	if (!data_miembros_grupo.EstaVacia()) data_miembros_grupo.vaciar_lista();

	if (grupo)
		MultiByteToWideChar(CP_ACP, 0, (const char *)grupo, strlen(grupo)+1, wgrupo, sizeof(wgrupo));
	else
		return 0;

	if (servidor)
		MultiByteToWideChar(CP_ACP, 0, (const char *)servidor, strlen(servidor)+1, wservidor, sizeof(wservidor));
	else
		wservidor[0] = (WCHAR)0;

//	salida = NetGroupGetUsers(wservidor, wgrupo, 0, &info, 0, &cant_entradas_leidas, &total_entradas, &handle);

	salida = NetLocalGroupGetMembers(wservidor,
								wgrupo,
								0,
								&info,
								(DWORD)-1,
								&cant_entradas_leidas,
								&total_entradas,
								&handle);

	if (salida == ERROR_SUCCESS)
		{
		data = (LOCALGROUP_MEMBERS_INFO_0*)info;

		for (;cant_entradas_leidas>0;cant_entradas_leidas--)
			{
//			dword total = UNLEN;
//			GuruUsuariosNT::Sid_a_str((SID *)data->lgrmi0_sid, reg_miembro.miembro, &total);
			GuruWTS gts;
			gts.sid_a_nombre_usuario(data->lgrmi0_sid, reg_miembro.miembro);
//			wideachar(data->lgrmi0_sidnandname,reg_miembro.miembro, sizeof(reg_miembro.miembro));
			data_miembros_grupo.agregar(&reg_miembro);

			cantidad++;
			data ++;
			}
		}
	else
		GuruNet_Error.descripcion(salida);

	NetApiBufferFree(info);

	return cantidad;
}

bool GuruGruposNT::ls_existe_usuario_en_grupo(char *nombre_usuario)
{
	bool salida = false;

	dword cant_usuarios = data_miembros_grupo.total_elementos();

	for (;cant_usuarios>0;cant_usuarios--)
		{
		if (stricmp(nombre_usuario, ls_nombre_miembro_grupo()) == 0)
			{
			salida = true;
			break;
			}
		}

	return salida;
}

char *GuruGruposNT::ls_nombre_grupo()
{
	return data_grupos.item()->nombre;
}

char *GuruGruposNT::ls_comentario_grupo()
{
	return data_grupos.item()->comentario;
}

void GuruGruposNT::ls_autoincrementar_grupo(bool inc)
{
	data_grupos.autoincrementar(inc);
}

void GuruGruposNT::ls_proximo_grupo()
{
	data_grupos++;
}

void GuruGruposNT::ls_reset_grupo()
{
	data_grupos.ir_a(0);
}

bool GuruGruposNT::ls_existe_grupo(char *grupo)
{
	bool	encontrado = false;
	dword actual = data_grupos.posicion_actual();

	ls_reset_grupo();

	while (!data_grupos.fin())
		{
		if (stricmp(grupo, ls_nombre_grupo()) == 0)
			{
			encontrado = true;
			break;
			}
		}

	data_grupos.ir_a(actual);

	return encontrado;
}

char *GuruGruposNT::ls_nombre_miembro_grupo()
{
	return data_miembros_grupo.item()->miembro;
}

void GuruGruposNT::ls_autoincrementar_miembro_grupo(bool inc)
{
	data_miembros_grupo.autoincrementar(inc);
}

void GuruGruposNT::ls_proximo_miembro_grupo()
{
	data_miembros_grupo++;
}

void GuruGruposNT::ls_reset_miembro_grupo()
{
	data_miembros_grupo.ir_a(0);
}
