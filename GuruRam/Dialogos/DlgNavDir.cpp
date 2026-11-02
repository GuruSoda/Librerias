// DlgNavDir.cpp: implementation of the DlgNavDir class.
//
//////////////////////////////////////////////////////////////////////

#include "DlgNavDir.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DlgNavDir::DlgNavDir()
{
	poner_handle(GuruRam::instancia()->sacar_handle());

	archivos = NULL;
}

DlgNavDir::~DlgNavDir()
{

}

void DlgNavDir::configurar(char* conf_servidor, int conf_puerto, char* conf_protocolo)
{
	servidor	= conf_servidor;
	puerto		= conf_puerto;
	protocolo	= conf_protocolo;
}

bool DlgNavDir::inicializar()
{
	limites(2, 2, 45, 20);

	struct LISTA forma_lista = {0, 1, 1, 42, 16, black, white, NULL};

	archivos = lista(&forma_lista);

	archivos->notificar_tecla((GuruRamLista::lista_notificacion*)opciones_lista, this);

	GuruProtocoloCliente*	cli;
	ConexionMultiprotocolo	conexion;

	if ((cli = conexion.conectar(servidor.contenido(), puerto, protocolo.contenido(), true)) == NULL) return false;

	cliente.cargar_socket(cli->handle_socket());

	cargar_lista();

	return true;
}

void DlgNavDir::cargar_lista()
{
	cliente.cargar_info();

	archivos->borrar_lista();

//	RegNavDir& entradas_archivos = cliente.primera_entrada();

	GuruBuffer	linea_lista(80);

	linea_lista.insertar(1, '\0', 80);

	for (dword i=1;i<=cliente.total_entradas();i++)
		{
		RegNavDir& entradas_archivos = cliente.entrada(i);

		linea_lista.borrar();
		linea_lista.insertar(1, ' ', 80);

		linea_lista.modificar(1, entradas_archivos.nombre().contenido());
		if (entradas_archivos.EsDirectorio())
			linea_lista.modificar(30, "<DIR>");
		else
			if (entradas_archivos.EsArchivo())
				linea_lista.modificar(30, entradas_archivos.tamanio().contenido());

		archivos->agregar_item_lista(linea_lista.contenido(), NULL);

//		if (cliente.total_entradas() != i)
//			entradas_archivos = cliente.siguiente_entrada();
		}

	cliente.entrada(1);
}

bool DlgNavDir::opciones_lista(GuruRamLista& lista_actual, DlgNavDir* esto)
{
	RegNavDir& entrada = esto->cliente.entrada(lista_actual.lista_pos_actual());

	switch (lista_actual.tecla_presionada())
		{
	case GuruRam::kbEnter:
				if (entrada.EsDirectorio())
					{
					if (strcmp(entrada.nombre().contenido(),"..") == 0)
						{
						esto->cliente.subir_nivel();
						}
					else
						esto->cliente.cambiar_directorio(entrada.nombre().contenido());

					esto->cargar_lista();
					esto->archivos->refrescar();

					return true;
					}
				break;
	case GuruRam::kbF5:
				break;
	case GuruRam::kbF2:
				break;
		}

/*
	GuruBuffer linea;
	linea.insertar(1, ' ', 80);
	char temp[15];

	linea.modificar(1, entrada.nombre().contenido());
	ltoa(lista_actual.lista_pos_actual(), temp, 10);
	linea.modificar(70, temp);

	esto->escribir(1, 25, black, white, linea.contenido());
*/
	return true;
}
