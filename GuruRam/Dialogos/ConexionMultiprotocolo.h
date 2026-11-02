// ConexionMultiprotocolo.h: interface for the ConexionMultiprotocolo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONEXIONMULTIPROTOCOLO_H__DC513115_D666_4A80_8D03_0D3F086A1E12__INCLUDED_)
#define AFX_CONEXIONMULTIPROTOCOLO_H__DC513115_D666_4A80_8D03_0D3F086A1E12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\GuruRam.h"
#include "..\GuruRamMensaje.h"
#include "..\GuruRamIngreso.h"
#include "..\..\GuruProtocoloCliente.h"

class ConexionMultiprotocolo : public GuruRam  
{
public:
	ConexionMultiprotocolo();
	virtual ~ConexionMultiprotocolo();

	GuruProtocoloCliente* conectar(char* servidor, int puerto, char* protocolo, bool con_bienvenida = false);

private:
	GuruProtocoloCliente* cliente;
};

#endif // !defined(AFX_CONEXIONMULTIPROTOCOLO_H__DC513115_D666_4A80_8D03_0D3F086A1E12__INCLUDED_)
