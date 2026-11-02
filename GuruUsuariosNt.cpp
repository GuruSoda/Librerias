// GuruUsuariosNt.cpp: implementation of the GuruUsuariosNt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuruUsuariosNt.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuruUsuariosNT::GuruUsuariosNT()
{
	notificar_listado	= NULL;
	extra_parametro		= NULL;
	servidor_cuentas[0]	= '\0';
}

GuruUsuariosNT::~GuruUsuariosNT()
{

}

void GuruUsuariosNT::cargar_notificacion_listado(listado_usuarios *nueva_notificacion)
{
	notificar_listado = nueva_notificacion;
}

void GuruUsuariosNT::cargar_extra_parametro(void *nuevo_extra_parametro)
{
	extra_parametro = nuevo_extra_parametro;
}

dword GuruUsuariosNT::ls_lista_usuarios(char *servidor)
{
	DWORD	cantidad=0;								// almacena la cantidad de usuarios cargados.
	WCHAR	wservidor[GURUUSUARIOS_MAX_SERVER_LEN];	// almacena el nombre del servidor en unicode
	DWORD	cant_entradas_leidas=0;					// almacena la cantidad de entradas leidas por la API
	DWORD	salida;									// almacena el resultado de la ejecucion de la API
	void	*info;									// almacena la lista retornada por la API
	NET_DISPLAY_USER *data;							// temporal para la data retornada por la API
	reg_usuario	temp_reg_usuario;					// registro donde almaceno la data del usuario
	dword	indice = 0;

	if (!data_usuarios.EstaVacia()) data_usuarios.vaciar_lista();

	if (servidor)
		MultiByteToWideChar(CP_ACP, 0, (const char *)servidor, strlen(servidor)+1, wservidor, sizeof(wservidor));
	else
		wservidor[0] = (WCHAR)0;

	do{
		salida = NetQueryDisplayInformation(wservidor,
											1,
											indice,
											(DWORD)-1,
											MAX_PREFERRED_LENGTH,
											&cant_entradas_leidas,
											&info);

		if ((salida == ERROR_SUCCESS) || (salida == ERROR_MORE_DATA))
			{
			data = (NET_DISPLAY_USER*)info;

			for (;cant_entradas_leidas>0;cant_entradas_leidas--)
				{
				WideCharToMultiByte( CP_ACP, 0, data->usri1_name, -1, temp_reg_usuario.nombre, GURUUSUARIOS_MAX_USR_LEN, NULL, NULL);

				WideCharToMultiByte( CP_ACP, 0, data->usri1_full_name, -1, temp_reg_usuario.nombre_completo, GURUUSUARIOS_MAX_FULL_USER_LEN, NULL, NULL);

				WideCharToMultiByte( CP_ACP, 0, data->usri1_comment, -1, temp_reg_usuario.descripcion, MAXCOMMENTSZ, NULL, NULL);


				temp_reg_usuario.flags = data->usri1_flags;

				data_usuarios.agregar(&temp_reg_usuario);

				if (notificar_listado)
					(*notificar_listado) (&temp_reg_usuario, extra_parametro);

				indice = data->usri1_next_index;
				data ++;
				cantidad++;
				}

			NetApiBufferFree(info);
			}
		else
			GuruNet_Error.descripcion();

	} while (salida == ERROR_MORE_DATA);

	if (servidor)
		strcpy(servidor_cuentas, servidor);

	return cantidad;
}

char *GuruUsuariosNT::ls_nombre_usuario()
{
	return data_usuarios.item()->nombre;
}

char *GuruUsuariosNT::ls_nombre_completo_usuario()
{
	return data_usuarios.item()->nombre_completo;
}

char *GuruUsuariosNT::ls_descripcion_usuario()
{
	return data_usuarios.item()->descripcion;
}

void GuruUsuariosNT::ls_autoincrementar_usuario(bool inc)
{
	data_usuarios.autoincrementar(inc);
}

void GuruUsuariosNT::ls_proximo_usuario()
{
	data_usuarios++;
}

void GuruUsuariosNT::ls_reset_usuario()
{
	data_usuarios.ir_a(0);
}

bool GuruUsuariosNT::ls_cuenta_deshabilitada_usuario()
{
	if (data_usuarios.item()->flags & UF_ACCOUNTDISABLE)
		return true;
	else
		return false;
}

bool GuruUsuariosNT::ls_cuenta_bloqueada_usuario()
{
	if (data_usuarios.item()->flags & UF_LOCKOUT)
		return true;
	else
		return false;
}

bool GuruUsuariosNT::ls_desbloquear_cuenta_usuario()
{
	WCHAR		wcuenta[50];
	USER_INFO_1 *buffer = NULL;
	bool		salida = false;
	WCHAR		wserver[50];
	char		nombre_cuenta[UNLEN];
	WCHAR		*servidor = NULL;


	if (data_usuarios.EstaVacia()) return false;

	strcpy(nombre_cuenta, ls_nombre_usuario());

	if (MultiByteToWideChar(CP_ACP, 0, (const char *)nombre_cuenta, strlen(nombre_cuenta) + 1, wcuenta, 50) == 0)
		{
		GuruNet_Error.descripcion();
		return false;
		}

	if (servidor_cuentas[0] != '\0')
		{
		if (MultiByteToWideChar(CP_ACP, 0, (const char *)servidor_cuentas, strlen(servidor_cuentas) + 1, wserver, 50) == 0)
			{
			GuruNet_Error.descripcion();
			return false;
			}
		else
			servidor = wserver;
		}

	NET_API_STATUS ssss = NetUserGetInfo(servidor, wcuenta, 1, (BYTE **)&buffer);
	if (ssss == NERR_Success)
		{

		if (buffer->usri1_flags & UF_LOCKOUT)
			buffer->usri1_flags = buffer->usri1_flags ^ UF_LOCKOUT;

		DWORD index_error = 0;
		ssss = NetUserSetInfo(servidor, wcuenta, 1, (BYTE *)buffer, &index_error);
		if ( ssss == NERR_Success)
			{
			// verifico que se desbloqueo
			NetApiBufferFree(buffer);
			salida = true;

			ssss = NetUserGetInfo(servidor, wcuenta, 1, (BYTE **)&buffer);
			if ( ssss == NERR_Success)
				{
				if ((buffer->usri1_flags & UF_LOCKOUT) == FALSE)
					salida = TRUE;

				NetApiBufferFree(buffer);
				}
			else
				GuruNet_Error.descripcion(ssss);
			}
		else
			GuruNet_Error.descripcion(ssss);
		}
	else
		GuruNet_Error.descripcion(ssss);

	return salida;
}

bool GuruUsuariosNT::ls_poner_unico_usuario(char *nombre_usuario, char *servidor)
{
	reg_usuario	temp_reg_usuario;
	USER_INFO_2	*buffer;
	WCHAR		wusuario[GURUUSUARIOS_MAX_USR_LEN];
	WCHAR		wservidor[GURUUSUARIOS_MAX_USR_LEN];
	bool		salida = false;

	if (!data_usuarios.EstaVacia()) data_usuarios.vaciar_lista();

	if (!nombre_usuario) return false;

	memset(&temp_reg_usuario, 0, sizeof(reg_usuario));

	if (servidor)
		MultiByteToWideChar(CP_ACP, 0, (const char *)servidor, strlen(servidor)+1, wservidor, sizeof(wservidor));
	else
		{
		wservidor[0] = (WCHAR)NULL;
		servidor_cuentas[0] = '\0';
		}


	MultiByteToWideChar(CP_ACP, 0, (const char *)nombre_usuario, strlen(nombre_usuario)+1, wusuario, sizeof(wusuario));


	if (NetUserGetInfo(wservidor, wusuario, 2, (unsigned char **)&buffer) == NERR_Success)
		{
		WideCharToMultiByte( CP_ACP, 0, buffer->usri2_full_name, -1, temp_reg_usuario.nombre_completo, GURUUSUARIOS_MAX_USR_LEN, NULL, NULL);
		WideCharToMultiByte( CP_ACP, 0, buffer->usri2_name, -1, temp_reg_usuario.nombre, GURUUSUARIOS_MAX_USR_LEN, NULL, NULL);
		WideCharToMultiByte( CP_ACP, 0, buffer->usri2_comment, -1, temp_reg_usuario.descripcion, MAXCOMMENTSZ, NULL, NULL);

		NetApiBufferFree(buffer);
		data_usuarios.agregar(&temp_reg_usuario);
		salida = true;
		}
	else
		GuruNet_Error.descripcion();

	return salida;
}

//////////////////////////////////////////////////////////////////////
// Tarea: Sacar el SID de un usuario
// Parametros:
//				usuario = nombre del usuario a sacar el SID
//				str_SID = donde se almacenara el SID
//////////////////////////////////////////////////////////////////////
char *GuruUsuariosNT::sacar_SID(char *usuario, char **str_SID)
{
	SID_NAME_USE	name_use;
	DWORD			tamanio_sid = 0;
	void *			ptr_sid;
	DWORD			tamanio_dominio=0;
	char *			nombre_dominio;

//////////////////////////////////////////////////////////////////////
// primero saco la cantidad de bytes que mide el SID
//////////////////////////////////////////////////////////////////////
	if (!LookupAccountName(	NULL,			// system name
							usuario,		// account name
							NULL,			// security identifier
							&tamanio_sid,	// size of security identifier
							NULL,			// domain name
							&tamanio_dominio,			// size of domain name
							&name_use		// SID-type indicator
							))
		{

		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
//////////////////////////////////////////////////////////////////////
// pido memoria para el SID y el nombre del dominio
//////////////////////////////////////////////////////////////////////
			ptr_sid = (SID *) new char [tamanio_sid];

			if (!ptr_sid) return FALSE;

			nombre_dominio = new char [tamanio_dominio + 1];

//////////////////////////////////////////////////////////////////////
// despues saco el SID
//////////////////////////////////////////////////////////////////////
			if (LookupAccountName(NULL,				// system name
								usuario,			// account name
								ptr_sid,			// security identifier
								&tamanio_sid,		// size of security identifier
								nombre_dominio,		// domain name
								&tamanio_dominio,	// size of domain name
								&name_use			// SID-type indicator
								))
				{
				DWORD	temp_tamanio_buffer = 0;

//////////////////////////////////////////////////////////////////////
// saco la cantidad de bytes que mide el SID en texto
//////////////////////////////////////////////////////////////////////
				Sid_a_str((SID *)ptr_sid, NULL, &temp_tamanio_buffer);

				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
					{
					char *texto_sid = new char [temp_tamanio_buffer + 1];

					Sid_a_str((SID *)ptr_sid, texto_sid, &temp_tamanio_buffer);

					*str_SID = new char [temp_tamanio_buffer + 1];
					strcpy(*str_SID, texto_sid);

					delete texto_sid;
					}
				else
					GuruNet_Error.descripcion();
				}
			else
				GuruNet_Error.descripcion();

			delete ptr_sid;
			delete nombre_dominio;
			}
		else
			GuruNet_Error.descripcion();
		}

	return *str_SID;
}

char *GuruUsuariosNT::Sid_a_str(SID *sid, char *buffer_destino, DWORD *tamanio_buffer)
{
    PSID_IDENTIFIER_AUTHORITY psia;
    DWORD dwSubAuthorities;
    DWORD dwSidRev=SID_REVISION;
    DWORD dwCounter;
    DWORD dwSidSize;

    // Validate the binary SID.

    if(!IsValidSid(sid)) return FALSE;

    // Get the identifier authority value from the SID.

    psia = GetSidIdentifierAuthority(sid);

    // Get the number of subauthorities in the SID.

    dwSubAuthorities = *GetSidSubAuthorityCount(sid);

    // Compute the buffer length.
    // S-SID_REVISION- + IdentifierAuthority- + subauthorities- + NULL

    dwSidSize=(15 + 12 + (12 * dwSubAuthorities) + 1) * sizeof(TCHAR);

    // Check input buffer length.
    // If too small, indicate the proper size and set last error.

    if (*tamanio_buffer < dwSidSize)
		{
        *tamanio_buffer = dwSidSize;
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return NULL;
		}

    // Add 'S' prefix and revision number to the string.

    dwSidSize=sprintf(buffer_destino, "S-%lu-", dwSidRev );

    // Add SID identifier authority to the string.

    if ( (psia->Value[0] != 0) || (psia->Value[1] != 0) )
		{
        dwSidSize+=sprintf(buffer_destino + lstrlen(buffer_destino),
                    "0x%02hx%02hx%02hx%02hx%02hx%02hx",
                    (USHORT)psia->Value[0],
                    (USHORT)psia->Value[1],
                    (USHORT)psia->Value[2],
                    (USHORT)psia->Value[3],
                    (USHORT)psia->Value[4],
                    (USHORT)psia->Value[5]);
		}
    else
		{
        dwSidSize+=sprintf(buffer_destino + lstrlen(buffer_destino),
                    "%lu",
                    (ULONG)(psia->Value[5]      )   +
                    (ULONG)(psia->Value[4] <<  8)   +
                    (ULONG)(psia->Value[3] << 16)   +
                    (ULONG)(psia->Value[2] << 24)   );
		}

    // Add SID subauthorities to the string.
    //
    for (dwCounter=0 ; dwCounter < dwSubAuthorities ; dwCounter++)
		{
        dwSidSize+=sprintf(buffer_destino + dwSidSize, "-%lu",
                    *GetSidSubAuthority(sid, dwCounter) );
		}

	return buffer_destino;
}

int GuruUsuariosNT::info_usuario_3(char *usuario,USER_INFO_3 *info_3,char *servidor)
{
	WCHAR	wservidor[GURUUSUARIOS_MAX_SERVER_LEN+1];
	WCHAR	wusuario[GURUUSUARIOS_MAX_USR_LEN+1];
	char	aux_server[GURUUSUARIOS_MAX_SERVER_LEN+1];
	LPBYTE	buffer;
	int		salida;

	if (servidor)
		{
		if (servidor[0] != '\\')
			{
			strcpy(aux_server,"\\\\");
			strcat(aux_server,servidor);
			MultiByteToWideChar(CP_ACP, 0, (const char *)aux_server, strlen(aux_server)+1, wservidor, sizeof(wservidor));
			}
		}
	else
		wservidor[0] = (WCHAR)0;

	if (usuario)
		MultiByteToWideChar(CP_ACP, 0, (const char *)usuario, strlen(usuario)+1, wusuario, sizeof(wusuario));
	else
		wusuario[0] = (WCHAR)0;


	salida = NetUserGetInfo(wservidor, wusuario, 3, &buffer);

	if (salida != NERR_Success)
		GuruNet_Error.descripcion();
	else
		*info_3 = *((USER_INFO_3*)buffer);

	NetApiBufferFree(buffer);

	return salida;
}
