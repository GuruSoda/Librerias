// GuruRamMenu.cpp: implementation of the GuruRamMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "GuruRamMenu.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuruRamMenu::GuruRamMenu(struct MENU *data_menu)
{
	if (data_menu)
		estructura_menu = *data_menu;

	item_actual = 1;
	items_total = 0;
}

GuruRamMenu::~GuruRamMenu()
{
}

word GuruRamMenu::menuEx (struct MENU *data_menu, word teclas_salida[])
{
	int ancho = strlen(data_menu->items[0].item);
	int alto = 0;

	while (data_menu->items[alto++].item)
		;

	alto--;

	struct VENTANA para_el_menu = {0, data_menu->x-1, data_menu->y-1, ancho + 2, alto + 2, data_menu->ColorFrente, data_menu->ColorFondo, NULL, abajo_izquierda, simple_simple, NULL, true};

	region_video region = {0};

	salvar(data_menu->x-1, data_menu->y-1, strlen(data_menu->items->item) + 3, alto + 3, &region);

	ventana(&para_el_menu);

	word salida_menu = menu(data_menu, teclas_salida);

	recuperar(&region);

	return salida_menu;
}

word GuruRamMenu::menu (struct MENU *data_menu, word teclas_salida[])
{
	int		eje_y_actual = data_menu->y;
	bool	cortar_menu = false;
	word	teclas_salida_default[] = {GuruRam::kbEsc, GuruRam::kbTab, /*GuruRam32::kbEnter, */NULL};

	if (data_menu)
		estructura_menu = *data_menu;
	else if (estructura_menu.y == 0)
		return GuruRam::kbEsc;

	refrescar();

	eje_y_actual = estructura_menu.y + item_actual - 1;

	region_video region = {0};

	salvar(data_menu->x, data_menu->y, strlen(data_menu->items->item), 3, &region);

	if (!teclas_salida)
		teclas_salida =  teclas_salida_default;

	word tecla;

	do {
		tecla = ingreso.leer_tecla();

		switch (tecla)
			{
		case GuruRam::kbUp:
				// si esta arriba de todo y quiero seguir subiendo
				if (item_actual == 1)
					{
					// despinto la barra
					escribir(data_menu->x, eje_y_actual, data_menu->ColorFrente, data_menu->ColorFondo, data_menu->items[item_actual-1].item);
					eje_y_actual += items_total-1;
					item_actual = items_total;
					escribir(data_menu->x, eje_y_actual, data_menu->ColorFondo, data_menu->ColorFrente, data_menu->items[item_actual-1].item);
					}
				else // desplazamiento normal
					{
					escribir(data_menu->x, eje_y_actual, data_menu->ColorFrente, data_menu->ColorFondo, data_menu->items[item_actual-1].item);
					eje_y_actual--;
					item_actual--;
					escribir(data_menu->x, eje_y_actual, data_menu->ColorFondo, data_menu->ColorFrente, data_menu->items[item_actual-1].item);
					}
				break;

		case GuruRam::kbDown:
				if (item_actual == items_total)
					{
					// despinto la barra
					escribir(data_menu->x, eje_y_actual, data_menu->ColorFrente, data_menu->ColorFondo, data_menu->items[item_actual-1].item);
					eje_y_actual = data_menu->y;
					item_actual = 1;
					escribir(data_menu->x, eje_y_actual, data_menu->ColorFondo, data_menu->ColorFrente, data_menu->items[item_actual-1].item);
					}
				else // desplazamiento normal
					{
					escribir(data_menu->x, eje_y_actual, data_menu->ColorFrente, data_menu->ColorFondo, data_menu->items[item_actual-1].item);
					eje_y_actual++;
					item_actual++;
					escribir(data_menu->x, eje_y_actual, data_menu->ColorFondo, data_menu->ColorFrente, data_menu->items[item_actual-1].item);
					}
				break;

		case GuruRam::kbEnter:
				if (data_menu->items[item_actual-1].funcion_item)
					(*data_menu->items[item_actual-1].funcion_item)(item_actual, data_menu->items->item);
				else
					cortar_menu = true;
				break;
			}

		if (cortar_menu) break;

		}while (!ingreso.existe_tecla(tecla, teclas_salida));

	recuperar(&region);

	return tecla;
}

word GuruRamMenu::menu_horizontal(struct MENU *data_menu, word teclas_salida[])
{
	return 0;
}

dword GuruRamMenu::item_seleccionado()
{
	return item_actual;
}

void GuruRamMenu::refrescar()
{
	// guardo el inicio de la lista de items
	struct ITEMS_MENU *opciones = estructura_menu.items;

	items_total = 0;

	// pinto el menu y cuento la cantidad de items ...
	while (estructura_menu.items->item)
		{
		escribir(estructura_menu.x, estructura_menu.y+items_total, estructura_menu.ColorFrente, estructura_menu.ColorFondo, estructura_menu.items->item);
		items_total++;
		estructura_menu.items++;
		}

	// data_menu->items -= items_total;
	estructura_menu.items = opciones;

	// pinto el primer item marcado
	escribir(estructura_menu.x, estructura_menu.y + item_actual - 1, estructura_menu.ColorFondo, estructura_menu.ColorFrente, estructura_menu.items[item_actual - 1].item);
}
