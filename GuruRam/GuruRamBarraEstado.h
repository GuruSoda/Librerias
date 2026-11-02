// GuruRamBarraEstado.h: interface for the GuruRamBarraEstado class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GURURAMBARRAESTADO_H__E638A4A7_B625_4B99_9667_AA53249B88FD__INCLUDED_)
#define AFX_GURURAMBARRAESTADO_H__E638A4A7_B625_4B99_9667_AA53249B88FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GuruRam.h"
#include "..\GuruBuffer.h"

class GuruRamBarraEstado : public GuruRam  
{
	enum lugar {izquierda, centro, derecha};
	enum lugar_barra {abajo, arriba};

public:
	GuruRamBarraEstado();
	virtual ~GuruRamBarraEstado();

	void	escribir(char *frase, enum lugar donde = izquierda);

private:
	GuruBuffer linea;
};

#endif // !defined(AFX_GURURAMBARRAESTADO_H__E638A4A7_B625_4B99_9667_AA53249B88FD__INCLUDED_)
