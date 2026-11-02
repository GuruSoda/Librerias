// GuruMaquinasNT.cpp: implementation of the GuruMaquinasNT class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuruMaquinasNT.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuruMaquinasNT::GuruMaquinasNT()
{

}

GuruMaquinasNT::~GuruMaquinasNT()
{

}

dword GuruMaquinasNT::ls_lista_maquinas(char *servidor)
{
	dword	cantidad=0;								// almacena la cantidad de maquinas cargados.
	WCHAR	wservidor[GURUMAQUINAS_MAX_SERVER_LEN];		// almacena el nombre del servidor en unicode
	DWORD	cant_entradas_leidas=0;					// almacena la cantidad de entradas leidas por la API
	DWORD	salida;									// almacena el resultado de la ejecucion de la API
	void	*info;									// almacena la lista retornada por la API
	NET_DISPLAY_MACHINE *data;						// temporal para la data retornada por la API
	maquina	reg_maquina;							// registro donde almaceno la data del usuario
	dword	index=0;

	if (!data_maquinas.EstaVacia()) data_maquinas.vaciar_lista();

	if (servidor)
		MultiByteToWideChar(CP_ACP, 0, (const char *)servidor, strlen(servidor)+1, wservidor, sizeof(wservidor));
	else
		wservidor[0] = (WCHAR)0;

	do{
		salida = NetQueryDisplayInformation(wservidor,
											2,
											index,
											(DWORD)-1,  /* 1000 */
											(DWORD)-1,
											&cant_entradas_leidas,
											&info);

		if((salida == ERROR_SUCCESS) || (salida==ERROR_MORE_DATA))
			{
			data = (NET_DISPLAY_MACHINE*)info;

			while (cant_entradas_leidas--)
	//		for (;cant_entradas_leidas>0;cant_entradas_leidas--)
				{
				wideachar(data->usri2_name, reg_maquina.nombre, sizeof(reg_maquina.nombre));
				wideachar(data->usri2_comment, reg_maquina.comentario, sizeof(reg_maquina.comentario));

				reg_maquina.nombre[strlen(reg_maquina.nombre)-1] = '\0';

				reg_maquina.tipo = data->usri2_flags;

				data_maquinas.agregar(&reg_maquina);

				cantidad++;
				index = data->usri2_next_index;
				data ++;
				}
			}

		NetApiBufferFree(info);

	}while (salida==ERROR_MORE_DATA);

	if (servidor)
		strcpy(actual_servidor, servidor);

	return cantidad;
}

char *GuruMaquinasNT::ls_nombre_maquina()
{
	return data_maquinas.item()->nombre;
}

char *GuruMaquinasNT::ls_comentario_maquina()
{
	return data_maquinas.item()->comentario;
}

dword GuruMaquinasNT::ls_tipo_maquina()
{
	return data_maquinas.item()->tipo;
}

bool GuruMaquinasNT::ls_es_BDC_maquina()
{
	USER_MODALS_INFO_1	*info_1;
	WCHAR                wservidor[64];

	MultiByteToWideChar(CP_ACP, 0, (const char *) actual_servidor, strlen(actual_servidor)+1, wservidor, sizeof(wservidor));

	if (NetUserModalsGet(wservidor, 1, (LPBYTE *)&info_1) == NERR_Success)
		{
		if (info_1->usrmod1_role & UAS_ROLE_BACKUP)
			return true;
		else
            return false;
		}
	else
		{
        GuruNet_Error.descripcion();
		return false;
		}
}

bool GuruMaquinasNT::ls_es_PDC_maquina()
{
	USER_MODALS_INFO_1	*info_1;
	WCHAR               wservidor[64];

	MultiByteToWideChar(CP_ACP, 0, (const char *) actual_servidor, strlen(actual_servidor)+1, wservidor, sizeof(wservidor));

	if (NetUserModalsGet(wservidor, 1, (LPBYTE *)&info_1)  == NERR_Success)
		{
		if (info_1->usrmod1_role & UAS_ROLE_PRIMARY)
			return true;
		else
			return false;
		}
	else
		{
        GuruNet_Error.descripcion();
		return false;
		}
}

bool GuruMaquinasNT::ls_es_Server_maquina()
{
	USER_MODALS_INFO_1	*info_1;
	WCHAR               wservidor[64];

	MultiByteToWideChar(CP_ACP, 0, (const char *) actual_servidor, strlen(actual_servidor)+1, wservidor, sizeof(wservidor));

	if (NetUserModalsGet(wservidor, 1, (LPBYTE *)&info_1) == NERR_Success)
		{
		if (info_1->usrmod1_role & UAS_ROLE_MEMBER)
			return true;
		else
			return false;
		}
	else
		{
        GuruNet_Error.descripcion();
		return false;
		}
}

bool GuruMaquinasNT::ls_es_Estacion_maquina()
{
	if (data_maquinas.item()->tipo & UF_WORKSTATION_TRUST_ACCOUNT)
		return true;
	else
		return false;
}

void GuruMaquinasNT::ls_autoincrementar_maquina(bool inc)
{
	data_maquinas.autoincrementar(inc);
}

void GuruMaquinasNT::ls_proximo_maquina()
{
	data_maquinas++;
}

void GuruMaquinasNT::ls_reset_maquina()
{
	data_maquinas.ir_a(0);
}
