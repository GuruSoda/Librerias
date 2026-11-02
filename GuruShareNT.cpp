// GuruShareNT.cpp: implementation of the GuruShareNT class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuruShareNT.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuruShareNT::GuruShareNT()
{

}

GuruShareNT::~GuruShareNT()
{

}

BOOL GuruShareNT::info_share(char *share, data_share *estructura)
{
	SHARE_INFO_502 *data;
	int            salida;
	char           shareado[MAX_SHARE_NAME];
	WCHAR          wshareado[MAX_SHARE_NAME];
	char           buffer[254];

	strcpy(shareado,share);

	MultiByteToWideChar(CP_ACP, 0, shareado, sizeof(shareado), wshareado, sizeof(wshareado));

	salida = NetShareGetInfo(NULL, (LPWSTR)wshareado, 502, (LPBYTE *)&data);

	if (salida == NERR_Success)
		{
		WideCharToMultiByte(CP_ACP, 0,(LPCWSTR) data->shi502_netname, -1,buffer,sizeof(buffer),NULL,NULL);
		strcpy(estructura->nombre_share,buffer);

		WideCharToMultiByte(CP_ACP, 0,(LPCWSTR) data->shi502_remark, -1,buffer,sizeof(buffer),NULL,NULL);
		strcpy(estructura->comentario,buffer);

		WideCharToMultiByte(CP_ACP, 0,(LPCWSTR) data->shi502_path, -1,buffer,sizeof(buffer),NULL,NULL);
		strcpy(estructura->ruta,buffer);

		WideCharToMultiByte(CP_ACP, 0,(LPCWSTR) data->shi502_passwd, -1,buffer,sizeof(buffer),NULL,NULL);
		strcpy(estructura->clave,buffer);

		estructura->tipo = data->shi502_type;
		estructura->permisos = data->shi502_permissions;
		estructura->max_cant_usuarios = data->shi502_max_uses;
		estructura->actuales_usuarios = data->shi502_current_uses;
		estructura->reservado = data->shi502_reserved;

		NetApiBufferFree(data);
		return TRUE;
		}
	else
		return FALSE;
}
