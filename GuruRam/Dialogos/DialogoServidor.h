// DialogoServidor.h: interface for the DialogoServidor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined DIALOGOSERVIDOR_H__
#define DIALOGOSERVIDOR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\GuruRamDialogo.h"

class DialogoServidor : public GuruRamDialogo  
{
public:
	DialogoServidor();
	virtual ~DialogoServidor();

	void	nombre_servidor(char* nombre_servidor);
	char*	nombre_servidor();

private:
	GuruRamIngreso* servidor;
};

#endif // !defined DIALOGOSERVIDOR_H__
