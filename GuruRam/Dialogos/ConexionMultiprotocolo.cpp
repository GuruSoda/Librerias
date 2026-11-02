// ConexionMultiprotocolo.cpp: implementation of the ConexionMultiprotocolo class.
//
//////////////////////////////////////////////////////////////////////

#include "ConexionMultiprotocolo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ConexionMultiprotocolo::ConexionMultiprotocolo()
{
	poner_handle(GuruRam::instancia()->sacar_handle());

	cliente = NULL;
}

ConexionMultiprotocolo::~ConexionMultiprotocolo()
{

}

GuruProtocoloCliente* ConexionMultiprotocolo::conectar(char* servidor, int puerto, char* protocolo, bool con_bienvenida)
{
	bool			conectado = false;
	GuruBuffer		linea_estado;
	GuruRamMensaje	msg;

	linea_estado = "Conectando a ";
	linea_estado += servidor;

	msg.mostrar(linea_estado.contenido());

	cliente = new GuruProtocoloCliente;

	if (cliente == NULL) return cliente;

	if (cliente->conectar(puerto, servidor)) 
		{
		if (protocolo)
			{
			if (cliente->enviar(protocolo))
				{
				if (con_bienvenida)
					{
					if (cliente->readline(1024))
						conectado = true;
					else
						{
						linea_estado = "No se pudo leer la bienvenida del protocolo.";
						conectado = false;
						}
					}
				else
					conectado = true;
				}
			else
				linea_estado = "Error al acceder al protocolo";
			}
		else
			if (con_bienvenida)
				{
				if (cliente->readline(1024))
					conectado = true;
				else
					{
					linea_estado = "No se pudo leer la bienvenida del protocolo.";
					conectado = false;
					}
				}
			else
				conectado = true;
		}
	else
		{
		linea_estado = "Error al conectarse con ";
		linea_estado += servidor;
		}


	msg.ocultar();

	if (!conectado)
		{
		msg.mostrar(linea_estado.contenido());

		GuruRamIngreso teclado;
		teclado.leer_tecla();

		msg.ocultar();

		delete cliente;
		cliente = NULL;
		}

	return cliente;
}
