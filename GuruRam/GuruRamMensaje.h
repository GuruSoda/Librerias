// GuruRamMensaje.h: interface for the GuruRamMensaje class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GURURAMMENSAJE_H__7341C89F_917D_4640_8039_4134ED47C283__INCLUDED_)
#define AFX_GURURAMMENSAJE_H__7341C89F_917D_4640_8039_4134ED47C283__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GuruRam.h"

class GuruRamMensaje : public GuruRam  
{
public:
	GuruRamMensaje();
	virtual ~GuruRamMensaje();

	void	mostrar(char* frase, char* titulo = NULL, dword opciones = 0, struct VENTANA* ventana = NULL);
	void	destruir();
	void	ocultar();

private:
	region_video	zona_salvada;
};

#endif // !defined(AFX_GURURAMMENSAJE_H__7341C89F_917D_4640_8039_4134ED47C283__INCLUDED_)
