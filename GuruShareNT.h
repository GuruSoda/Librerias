// GuruShareNT.h: interface for the GuruShareNT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GURUSHARENT_H__14D854F0_1834_4AE0_8475_8B7D703804E8__INCLUDED_)
#define AFX_GURUSHARENT_H__14D854F0_1834_4AE0_8475_8B7D703804E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GuruNet.h"

#define			MAX_SHARE_NAME			80
#define			MAX_COMMENT_LEN			256

typedef struct 
		{
		char	nombre_share[MAX_SHARE_NAME+1];
		DWORD	tipo;
		char	comentario[MAX_COMMENT_LEN+1];
		DWORD	permisos;
		DWORD	max_cant_usuarios;
		DWORD	actuales_usuarios;
		char	ruta[_MAX_PATH];
		char	clave[MAX_PASSWD_LEN];
		DWORD	reservado;
		}data_share;

class GuruShareNT : public GuruNet  
{
public:
	GuruShareNT();
	virtual ~GuruShareNT();

	BOOL	info_share(char *share, data_share *estructura);
};

#endif // !defined(AFX_GURUSHARENT_H__14D854F0_1834_4AE0_8475_8B7D703804E8__INCLUDED_)
