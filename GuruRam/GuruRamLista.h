// GuruRamLista.h: interface for the GuruRamLista class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GURURAMLISTA_H__733F45AE_5114_4CE2_9F1C_6C9BBD2C9417__INCLUDED_)
#define AFX_GURURAMLISTA_H__733F45AE_5114_4CE2_9F1C_6C9BBD2C9417__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GuruRam.h"
#include "GuruRamIngreso.h"
#include "..\GuruLista.h"
#include "..\GuruBuffer.h"
#include "..\GuruListaGenerica.h"

typedef struct
	{
	void* data_item_lista;
	}data_item;

typedef struct LISTA
	{
	unsigned int id;

	int  x;               // Posicion x
	int  y;               // Posicion y

	int  Ancho;           // Cantidad de columnas
	int  Alto;            // Cantidad de filas

	byte ColorFrente;     // Color del caracter
	byte ColorFondo;      // Color del fondo

	char *item_vacio;	  // Linea de relleno para los items que no existen
	}LISTA;

class GuruRamLista : public GuruRam  
{
public:
	GuruRamLista(struct LISTA *data_lista = NULL);
	virtual ~GuruRamLista();

	// true  cuando debe continuar con el ciclo de la lista
	// false cuando debe cortar el ciclo de la lista
typedef bool (lista_notificacion) (GuruRamLista& lista_actual, GuruRam* esto);

//////////////////////////////////////////////////////////////////////
// Para el control tipo Lista
//////////////////////////////////////////////////////////////////////
	word	lista(struct LISTA *forma_lista = NULL, word teclas_salida[] = NULL);
	word	listaEx(struct LISTA *forma_lista = NULL, word teclas_salida[] = NULL, struct VENTANA *borde = NULL);
	void	agregar_item_lista(char *items_lista, data_item* info_item_lista = NULL);
	void	borrar_item_lista(dword posicion);
	void	borrar_lista();

	dword   lista_pos_actual();
	dword   lista_total_items();
	bool    lista_seleccionar(dword item);
	char*   lista_item(dword posicion = 0); // comienza en 1
	word	tecla_presionada();
	data_item*	ptr_asociado();
	void	refrescar();

	dword	alto_control();
	dword	ancho_control();

	void    notificar_enter(lista_notificacion *notificar, void *extra);
	void    notificar_cambio(lista_notificacion *notificar, void *extra);
	void    notificar_tecla(lista_notificacion *notificar, void *extra);

private:
	void	barra_desplazamiento_vertical(dword marca);
	void	escribir_en_lista (int x, int y, byte ColorFrente, byte ColorFondo, const char *item, int ancho);
//////////////////////////////////////////////////////////////////////
// Atributos para el manejo de la Lista
//////////////////////////////////////////////////////////////////////
	lista_notificacion *notificar_enter_lista;	// notifica a una funcion cuando presionan enter en un item de la lista
	lista_notificacion *notificar_cambio_lista; // notifica cuando se produce un cambio en una lista
	lista_notificacion *notificar_tecla_lista;  // notifica cualquier tecla presionada en la lista

	void *      lista_extra_not_enter;
	void *      lista_extra_not_cambio;
	void *      lista_extra_not_tecla;

	GuruLista			items_lista;
	GuruBuffer			item_lista;
	GuruListaGenerica	<data_item> info_item;

	GuruRamIngreso		ingreso;

//////////////////////////////////////////////////////////////////////
// Atributos para vista de la Lista
//////////////////////////////////////////////////////////////////////
	struct LISTA	data_lista;
	word			tecla;				// almacena la ultima tecla presionada;
	int				lugar_seleccion;	// almacena el numero de linea seleccionada
};

#endif // !defined(AFX_GURURAMLISTA_H__733F45AE_5114_4CE2_9F1C_6C9BBD2C9417__INCLUDED_)
