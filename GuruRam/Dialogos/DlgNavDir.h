// DlgNavDir.h: interface for the DlgNavDir class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGNAVDIR_H__1FC9A94E_126C_4AF4_84BE_29853F372D91__INCLUDED_)
#define AFX_DLGNAVDIR_H__1FC9A94E_126C_4AF4_84BE_29853F372D91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\GuruRamDialogo.h"
#include "..\..\GuruProtocoloCliente.h"
#include "ConexionMultiprotocolo.h"
#include "..\..\GuruLista.h"
#include "..\..\GuruClntNavegarDir.h"

// para los comandos
#include "..\..\GuruSocketNavegarDir.h"

class DlgNavDir : public GuruRamDialogo  
{
public:
	DlgNavDir();
	virtual ~DlgNavDir();

	void configurar(char* conf_servidor_catapulta, int conf_puerto, char* conf_protocolo);

private:
	GuruBuffer		servidor;
	int				puerto;
	GuruBuffer		protocolo;

	GuruRamLista*	archivos;

	bool		inicializar();
	static bool opciones_lista(GuruRamLista& lista_actual, DlgNavDir* esto);
	void		cargar_lista();

	GuruClntNavegarDir		cliente;
};

#endif // !defined(AFX_DLGNAVDIR_H__1FC9A94E_126C_4AF4_84BE_29853F372D91__INCLUDED_)
