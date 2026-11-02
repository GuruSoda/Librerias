// GuruRamMenu.h: interface for the GuruRamMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GURURAMMENU_H__D9FE558D_9FFD_4CFF_92BB_1219C8B48E69__INCLUDED_)
#define AFX_GURURAMMENU_H__D9FE558D_9FFD_4CFF_92BB_1219C8B48E69__INCLUDED_

#include <stdio.h>
#include "GuruRam.h"
#include "GuruRamIngreso.h"

typedef struct MENU
	{
	unsigned int id;

	int  x;               // Posicion x
	int  y;               // Posicion y

	struct ITEMS_MENU *items;

	byte ColorFrente;     // Color del caracter
	byte ColorFondo;      // Color del fondo

	byte espacios;		  // cantidad de espacion es blanco que se deja entre cada item
	}MENU;

typedef struct ITEMS_MENU
	{
	char *item;
	void (*funcion_item) (dword item, char *texto_item);
	}ITEMS_MENU;

class GuruRamMenu : public GuruRam
{
public:
	GuruRamMenu(struct MENU *data_menu = NULL);
	virtual ~GuruRamMenu();

//////////////////////////////////////////////////////////////////////
// Para el control tipo Menu
//////////////////////////////////////////////////////////////////////
	word	menu (struct MENU *data_menu = NULL, word teclas_salida[] = NULL);
	word	menuEx(struct MENU *data_menu = NULL, word teclas_salida[] = NULL);

	word	menu_horizontal(struct MENU *data_menu = NULL, word teclas_salida[] = NULL);

	void	refrescar();

	dword	item_seleccionado ();

private:
	GuruRamIngreso ingreso;

	struct MENU estructura_menu;

	dword	item_actual;
	dword	items_total;
};

#endif // !defined(AFX_GURURAMMENU_H__D9FE558D_9FFD_4CFF_92BB_1219C8B48E69__INCLUDED_)
