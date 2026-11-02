// net.h: interface for the net class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NET_H__55193383_9207_11D3_A619_00104B0256E3__INCLUDED_)
#define AFX_NET_H__55193383_9207_11D3_A619_00104B0256E3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AUTOR_H
	#define		__AUTOR_H
//	static char autor[] = "Autor: Fabricio D. Rossi (El Guru)";
#endif

//////////////////////////////////////////////////////////////////////
//
//  Linkear con Netapi32.lib
//
//////////////////////////////////////////////////////////////////////

#include "GuruError.h"
#include "GuruListaGenerica.h"

#include <Lm.h>
#include <time.h>

class GuruNet
{
public:
	GuruNet();
	virtual ~GuruNet();

	char *	nombre_pdc(char **nombre);
	char *	pc_a_dominio(char *nombre_pc);
	char *	dominio_a_pc(char *nombre_dominio);


	WCHAR * charawide(char *cadena,WCHAR *widecadena,DWORD tamanio);
	char  * wideachar(WCHAR *widecadena,char *cadena,DWORD tamanio);

	const char*	mensaje_error();

	GuruError	GuruNet_Error;

	char *	fecha(char *buffer,const long tiempo);
	char *	hora(char *buffer,const long tiempo);
};

#endif // !defined(AFX_NET_H__55193383_9207_11D3_A619_00104B0256E3__INCLUDED_)
