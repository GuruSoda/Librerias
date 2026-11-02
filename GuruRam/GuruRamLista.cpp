// GuruRamLista.cpp: implementation of the GuruRamLista class.
//
//////////////////////////////////////////////////////////////////////

#include "GuruRamLista.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuruRamLista::GuruRamLista(struct LISTA *lista)
{
	notificar_cambio_lista	= NULL;
	notificar_enter_lista	= NULL;
	notificar_tecla_lista	= NULL;

	lista_extra_not_enter	= NULL;
	lista_extra_not_cambio	= NULL;
	lista_extra_not_tecla	= NULL;

	items_lista.autoincrementar(false);
	info_item.autoincrementar(false);

	lugar_seleccion			= 0;		// indica el primer item seleccionado

	if (lista)
		data_lista = *lista;
}

GuruRamLista::~GuruRamLista()
{

}

void GuruRamLista::notificar_enter(lista_notificacion *notificar, void *extra)
{
	notificar_enter_lista = notificar;
	lista_extra_not_enter  = extra;
}

void GuruRamLista::notificar_cambio(lista_notificacion *notificar, void *extra)
{
	notificar_cambio_lista = notificar;
	lista_extra_not_cambio = extra;
}

void GuruRamLista::notificar_tecla(lista_notificacion *notificar, void *extra)
{
	notificar_tecla_lista = notificar;
	lista_extra_not_tecla = extra;
}

//////////////////////////////////////////////////////////////////////
// lista
// Tarea: Genera una lista en pantalla y permite recorrerla usando las
//        teclas comunes.
// Nota:
//        Uso de cada variable:
//        * lugar_seleccion = Se usa para saber cual es la posicion en 
//          pantalla de la seleccion (de 0 hasta data_lista->Alto)
//////////////////////////////////////////////////////////////////////
word GuruRamLista::lista(struct LISTA *forma_lista, word teclas_salida[])
{
	word        teclas_salida_default[] = {GuruRam::kbEsc, GuruRam::kbTab, /*GuruRam::kbEnter, */NULL};
	bool        cambio = false;
	bool		continuar = true; // indica si tiene que continuar en el ciclo que mantiene la lista.

	if (forma_lista)
		data_lista = *forma_lista;
	else if (data_lista.x == 0)
		return GuruRam::kbEsc;

//	region_video region = {0};

//	salvar(data_lista.x-1, data_lista.y-1, data_lista.Ancho + 3, data_lista.Alto + 3, &region);

	if (!teclas_salida)
		teclas_salida =  teclas_salida_default;

	item_lista.reservar(data_lista.Ancho + 1);

// le saco un lugar por la barra de desplazamiento
// a refrescar
	refrescar();
// a refrescar

	if (notificar_tecla_lista)
		continuar = (*notificar_tecla_lista) (*this, (GuruRam*)lista_extra_not_tecla);

	if (continuar)
		tecla = ingreso.leer_tecla();
	else
		return GuruRam::kbEsc;

	while (!ingreso.existe_tecla(tecla, teclas_salida))
		{
		switch (tecla)
			{
		case GuruRam::kbUp:
				if (items_lista.lugar_actual() == 0) break;

				if (lugar_seleccion) // desplazamiento normal
					{
					escribir_en_lista(data_lista.x, data_lista.y+lugar_seleccion, data_lista.ColorFrente, data_lista.ColorFondo, (const char *)items_lista.item(), data_lista.Ancho);
					items_lista--;
					info_item--;
					lugar_seleccion--;
					escribir_en_lista(data_lista.x, data_lista.y+lugar_seleccion, data_lista.ColorFondo, data_lista.ColorFrente, (const char *)items_lista.item(), data_lista.Ancho);
					}
				else
					if (lugar_seleccion == 0) // desplazo la lista un lugar para abajo
						{
						items_lista--;
						info_item--;
						for (int i=0;(i<data_lista.Alto) && items_lista.item();i++)
							{
							escribir_en_lista(data_lista.x, data_lista.y+i, data_lista.ColorFrente, data_lista.ColorFondo, items_lista.item(), data_lista.Ancho);
							items_lista++;
							info_item++;
							}

						// pregunto si lista se lleno por completo
						if (items_lista.item()) // llena por completo
							{
							items_lista.retroceder(i);
							info_item.retroceder(i);
							}
						else // quedo con items en blanco
							{
							items_lista.al_final(i-1);
							info_item.al_final(i-1);
							}

						escribir_en_lista(data_lista.x, data_lista.y, data_lista.ColorFondo, data_lista.ColorFrente, items_lista.item(), data_lista.Ancho);
						}

				cambio = true;

				break;

		case GuruRam::kbDown:
				if (items_lista.total_elementos() == (items_lista.lugar_actual()+1)) break;

				// desplazamiento normal por la ventana
				if (lugar_seleccion+1 < data_lista.Alto)
					{
					escribir_en_lista(data_lista.x, data_lista.y+lugar_seleccion, data_lista.ColorFrente, data_lista.ColorFondo, (const char *)items_lista.item(), data_lista.Ancho);
					items_lista++;
					info_item++;
					lugar_seleccion++;
					escribir_en_lista(data_lista.x, data_lista.y+lugar_seleccion, data_lista.ColorFondo, data_lista.ColorFrente, (const char *)items_lista.item(), data_lista.Ancho);
					}
				else // tengo que desplazar los items de la panatalla
					if ((items_lista.total_elementos() != (items_lista.lugar_actual()+1)) && (lugar_seleccion+1 == data_lista.Alto))
						{
						// retrocedo hasta la primer prosicion de lo que se muestra en pantalla
						items_lista.retroceder(data_lista.Alto - 2);
						info_item.retroceder(data_lista.Alto - 2);

						for (int i = 0;i<data_lista.Alto-1;i++)
							{
							escribir_en_lista(data_lista.x, data_lista.y+i, data_lista.ColorFrente, data_lista.ColorFondo, items_lista.item(), data_lista.Ancho);
							items_lista++;
							info_item++;
							}

						escribir_en_lista(data_lista.x, data_lista.y+lugar_seleccion, data_lista.ColorFondo, data_lista.ColorFrente, items_lista.item(), data_lista.Ancho);
						}

				cambio = true;
				break;

		case GuruRam::kbPgUp:
					if (items_lista.lugar_actual()+1 > (dword)data_lista.Alto)
						{
						items_lista.retroceder(data_lista.Alto);
						info_item.retroceder(data_lista.Alto);

						for (int i = 0;i<data_lista.Alto;i++)
							{
							escribir_en_lista(data_lista.x, data_lista.y+i, data_lista.ColorFrente, data_lista.ColorFondo, items_lista.item(), data_lista.Ancho);
							items_lista++;
							info_item++;							
							}

						// eje_y_actual = data_lista->y;
						lugar_seleccion = 0;
						items_lista.retroceder(data_lista.Alto);
						info_item.retroceder(data_lista.Alto);
						escribir_en_lista(data_lista.x, data_lista.y+lugar_seleccion, data_lista.ColorFondo, data_lista.ColorFrente, items_lista.item(), data_lista.Ancho);						
						}
					else // no puedo retroceder menos ya que no tenga tanta cantidad de items
						if (items_lista.lugar_actual()+1 <= (dword)data_lista.Alto)
							{
							if (items_lista.lugar_actual() == 0) break;

							lugar_seleccion = 0;

							items_lista.ir_a(0);
							info_item.ir_a(0);

							for (int i = 0;i<data_lista.Alto;i++)
								{
								escribir_en_lista(data_lista.x, data_lista.y+i, data_lista.ColorFrente, data_lista.ColorFondo, items_lista.item(), data_lista.Ancho);
								items_lista++;
								info_item++;
								}

							items_lista.ir_a(0);
							info_item.ir_a(0);
							escribir_en_lista(data_lista.x, data_lista.y+lugar_seleccion, data_lista.ColorFondo, data_lista.ColorFrente, items_lista.item(), data_lista.Ancho);
							}

					cambio = true;
					break;

		case GuruRam::kbPgDn:
					// estoy en el ultimo lugar?, no hago nada
					if (items_lista.total_elementos() == (items_lista.lugar_actual()+1)) break;

					// un page down normal (que avanza una pantalla completa)
					if (((items_lista.lugar_actual()+1) + data_lista.Alto) < items_lista.total_elementos())
						{
						items_lista++;
						info_item++;

						for (int i = 0;i<data_lista.Alto-1;i++)
							{
							escribir_en_lista(data_lista.x, data_lista.y+i, data_lista.ColorFrente, data_lista.ColorFondo, items_lista.item(), data_lista.Ancho);
							items_lista++;
							info_item++;
							}
						lugar_seleccion = data_lista.Alto-1;
						}
					else
						{
						// lo primero que hago es despintar lo marcado
						escribir_en_lista(data_lista.x, data_lista.y+lugar_seleccion, data_lista.ColorFrente, data_lista.ColorFondo, items_lista.item(), data_lista.Ancho);

						// pregunto si la lista a mostrar es menor que lo que ocupa la pantalla
						if (items_lista.total_elementos() < (dword)data_lista.Alto)
							{
							items_lista.al_final();
							info_item.al_final();
							lugar_seleccion = items_lista.total_elementos() - 1;
							}
						else // si no puedo avanzar una pantalla completa (y tengo que ir al final de la lista)
							if (((items_lista.lugar_actual()+1) + data_lista.Alto) >= items_lista.total_elementos())
								{
								items_lista.al_final(data_lista.Alto-1); // el -1 agregado el dia 23/07/2004
								info_item.al_final(data_lista.Alto-1);
								lugar_seleccion = data_lista.Alto-1;

								for (int i = 0;i<data_lista.Alto-1;i++)
									{
									escribir_en_lista(data_lista.x, data_lista.y+i, data_lista.ColorFrente, data_lista.ColorFondo, items_lista.item(), data_lista.Ancho);
									items_lista++;
									info_item++;									
									}
								}
						}

					// pinto lo marcado
					escribir_en_lista(data_lista.x, data_lista.y+lugar_seleccion, data_lista.ColorFondo, data_lista.ColorFrente, items_lista.item(), data_lista.Ancho);

					cambio = true;
					break;

		case GuruRam::kbHome:
					if (items_lista.lugar_actual() == 0) break;

					// lo primero que hago es despintar lo marcado
					escribir_en_lista(data_lista.x, data_lista.y+lugar_seleccion, data_lista.ColorFrente, data_lista.ColorFondo, items_lista.item(), data_lista.Ancho);

					items_lista.ir_a(0);
					info_item.ir_a(0);

					lugar_seleccion = 0;

					// pregunto si la lista a mostrar es menor que lo que ocupa la pantalla
					if (items_lista.total_elementos() < (dword)data_lista.Alto)
						{
						// pinto el item resaltado
						escribir_en_lista(data_lista.x, data_lista.y+lugar_seleccion, data_lista.ColorFondo, data_lista.ColorFrente, items_lista.item(), data_lista.Ancho);
						}
					else
						// if (items_lista.total_elementos() >= (dword)data_lista->Alto)
						{
						for (int w=0;w<data_lista.Alto;w++)
							{
							escribir_en_lista(data_lista.x, data_lista.y+w, data_lista.ColorFrente, data_lista.ColorFondo, items_lista.item(), data_lista.Ancho);
							items_lista++;
							info_item++;
							}

						items_lista.ir_a(0);
						info_item.ir_a(0);

						escribir_en_lista(data_lista.x, data_lista.y+lugar_seleccion, data_lista.ColorFondo, data_lista.ColorFrente, items_lista.item(), data_lista.Ancho);
						}

					cambio = true;
					break;

		case GuruRam::kbEnd:
					// pregunto si la lista a mostrar es menor que lo que ocupa la pantalla
					if (items_lista.total_elementos() < (dword)data_lista.Alto)
						{
						escribir_en_lista(data_lista.x, data_lista.y, data_lista.ColorFrente, data_lista.ColorFondo, items_lista.item(), data_lista.Ancho);
						items_lista.al_final();
						info_item.al_final();
						lugar_seleccion = items_lista.total_elementos() - 1;
						escribir_en_lista(data_lista.x, data_lista.y+lugar_seleccion, data_lista.ColorFondo, data_lista.ColorFrente, items_lista.item(), data_lista.Ancho);
						}
					else
						{
						items_lista.al_final(data_lista.Alto - 1);
						info_item.al_final(data_lista.Alto - 1);

						for (int j = 0; j < data_lista.Alto -1 && items_lista.item(); j++)
							{
							escribir_en_lista(data_lista.x, data_lista.y+j, data_lista.ColorFrente, data_lista.ColorFondo, items_lista.item(), data_lista.Ancho);
							items_lista++;
							info_item++;
							}

						items_lista.al_final();
						info_item.al_final();
						lugar_seleccion = data_lista.Alto - 1; // la seleccion internamente empiza en 0
						escribir_en_lista(data_lista.x, data_lista.y+lugar_seleccion, data_lista.ColorFondo, data_lista.ColorFrente, items_lista.item(), data_lista.Ancho);
						}

					cambio = true;
					break;
			}

		if (items_lista.total_elementos() > (dword)data_lista.Alto)
			barra_desplazamiento_vertical((((((items_lista.lugar_actual()+1) * 100) / items_lista.total_elementos()) * (data_lista.Alto-1)) / 100));

		if (notificar_tecla_lista)
			continuar = (*notificar_tecla_lista) (*this, (GuruRam*)lista_extra_not_tecla);

		if (cambio)
			{
			if (notificar_cambio_lista)
				continuar = (*notificar_cambio_lista) (*this, (GuruRam*)lista_extra_not_tecla);

			cambio = false;
			}

		if (continuar)
			tecla = ingreso.leer_tecla();
		else
			break;
		}

	if (tecla == GuruRam::kbEnter && notificar_enter_lista)
		(*notificar_enter_lista) (*this, (GuruRam*)lista_extra_not_tecla);

//	recuperar(&region);

	return tecla;
}

void GuruRamLista::escribir_en_lista (int x, int y, byte ColorFrente, byte ColorFondo, const char *item, int ancho)
{
	if (item)
		{
		item_lista.poner((char *)item);

		if (item_lista.longitud_data() >= (dword)ancho)
			item_lista.borrar(ancho, item_lista.longitud_data() - ancho);
		else
			{
			dword total_caracteres = (dword)ancho - item_lista.longitud_data();
			for (dword i=0;i<total_caracteres;i++)
				item_lista.concatenar(' ');
			}

		struct ESCRIBIR info_escribir = {0xffff, x, y, ColorFrente, ColorFondo, item_lista.contenido()};

		escribir(&info_escribir);
		}
}

void GuruRamLista::barra_desplazamiento_vertical(dword marca)
{
// data_lista->ColorFondo;

//	CHAR_INFO *desplazamiento = new CHAR_INFO [1 * data_lista->Alto];

	// '²'

	for (int i=0; i<data_lista.Alto;i++)
		escribir(data_lista.x+data_lista.Ancho, data_lista.y+i, data_lista.ColorFrente, data_lista.ColorFondo, '²');

	escribir(data_lista.x+data_lista.Ancho, data_lista.y+marca, data_lista.ColorFrente, data_lista.ColorFondo, (unsigned char)254);
}

//////////////////////////////////////////////////////////////////////
// listaEx: crea la lista y ademas le agrega una ventana
// Nota:
//       * En la estructura borde los miembros x | y | Ancho | Alto 
//         son ignorados.
//////////////////////////////////////////////////////////////////////
word GuruRamLista::listaEx(struct LISTA *data_lista, word teclas_salida[], struct VENTANA *borde)
{
	region_video region = {0};

	salvar(data_lista->x-1, data_lista->y-1, data_lista->Ancho + 3, data_lista->Alto + 3, &region);

	if (borde)
		{
		borde->Alto  = data_lista->Alto + 2;
		borde->Ancho = data_lista->Ancho + 2;
		borde->x     = data_lista->x-1;
		borde->y     = data_lista->y-1;

		if (borde->ColorFondo == 255)
			borde->ColorFondo = data_lista->ColorFondo;

		if (borde->ColorFrente == 255)
			borde->ColorFrente = data_lista->ColorFrente;

		ventana(borde);
		}
	else
		{
		struct VENTANA para_la_lista = {0, data_lista->x-1, data_lista->y-1, data_lista->Ancho + 2, data_lista->Alto + 2, data_lista->ColorFrente, data_lista->ColorFondo, NULL, arriba_centro, doble_doble, NULL, true};
		ventana(&para_la_lista);
		}


	word retorno =lista(data_lista, teclas_salida);

	recuperar(&region);

	return retorno;
}

void GuruRamLista::agregar_item_lista(char *nuevo_item, data_item* info_item_lista)
{
	items_lista.agregar(nuevo_item);

	if (info_item_lista)
		info_item.agregar((data_item*)info_item_lista);
}

void GuruRamLista::borrar_item_lista(dword posicion)
{
	if (posicion <= lista_total_items())
		{
		dword lugar_items = items_lista.lugar_actual();
		dword lugar_info  = info_item.lugar_actual();

		items_lista.al_inicio(posicion - 1);
		info_item.al_inicio(posicion - 1);

		items_lista.borrar();
		info_item.borrar();

		if (posicion == lista_total_items()+1)
			{
			if (lista_total_items() < (dword)data_lista.Alto)
				lugar_seleccion--;

			items_lista.al_final();
			info_item.al_final();
			}
		else
			{
			items_lista.al_inicio(lugar_items);
			info_item.al_inicio(lugar_info);
			}
		}
}

void GuruRamLista::borrar_lista()
{
	lugar_seleccion = 0;
	items_lista.vaciar_lista();
	info_item.vaciar_lista();
}


dword GuruRamLista::lista_pos_actual()
{
	return items_lista.lugar_actual() + 1;
}

dword GuruRamLista::lista_total_items()
{
	return items_lista.total_elementos();
}

bool GuruRamLista::lista_seleccionar(dword item)
{
	if (item < lista_total_items())
		{
		items_lista.al_inicio(item);
		info_item.al_inicio(item);
		return true;
		}
	else
		return false;	
}

word GuruRamLista::tecla_presionada()
{
	return tecla;
}

data_item* GuruRamLista::ptr_asociado()
{
	return info_item.item();
}

dword GuruRamLista::alto_control()
{
	return data_lista.Alto;
}

dword GuruRamLista::ancho_control()
{
	return data_lista.Ancho;
}

char *GuruRamLista::lista_item(dword posicion)
{
	char *item_requerido = NULL;

	if (posicion)
		{
		items_lista.al_inicio(posicion);
		info_item.al_inicio(posicion);

		item_requerido = items_lista.item();
		}
	else
		item_requerido = items_lista.item();

	return item_requerido;
}

void GuruRamLista::refrescar()
{
	GuruBuffer  linea_en_blanco(data_lista.Ancho + 1);
	char *		item = NULL;
	dword		primero_visible=0;

	primero_visible = items_lista.lugar_actual() - lugar_seleccion;

// si no tengo una linea de relleno, uso una en blanco
	if (!data_lista.item_vacio)
		{
		for (int i=0;i<data_lista.Ancho;i++)
			linea_en_blanco.concatenar(' ');

		item = linea_en_blanco.contenido();
		}
	else
		item = data_lista.item_vacio;

	// primero pinto el fondo de la lista
	for (int i=0;i<data_lista.Alto;i++)
		escribir_en_lista(data_lista.x, data_lista.y+i, data_lista.ColorFrente, data_lista.ColorFondo, item, data_lista.Ancho);

	for (i=lugar_seleccion;i > 0;i--)
		items_lista--;

	// pinto la lista y cuento la cantidad de items ...
	for (i=0;(i<data_lista.Alto) && items_lista.item();i++)
		{
		escribir_en_lista(data_lista.x, data_lista.y+i, data_lista.ColorFrente, data_lista.ColorFondo, items_lista.item(), data_lista.Ancho);

		items_lista++;
		}

	if (items_lista.item())
		items_lista.retroceder(data_lista.Alto - lugar_seleccion);
	else
		{
		// me fijo si tengo menos items que lo que me pide la ventana
		// i tiene la cantidad de items que dibujo
		if (i < data_lista.Alto) // no pinto el total items que entran en la ventana
			items_lista.al_final(i-lugar_seleccion-1);
		else
			items_lista.al_final(data_lista.Alto - lugar_seleccion - 1);
		}

	// pinto el primer item marcado
	escribir_en_lista(data_lista.x, data_lista.y + lugar_seleccion, data_lista.ColorFondo, data_lista.ColorFrente, items_lista.item(), data_lista.Ancho);

	if (items_lista.total_elementos() > (dword)data_lista.Alto)
		barra_desplazamiento_vertical((((((items_lista.lugar_actual()+1) * 100) / items_lista.total_elementos()) * (data_lista.Alto-1)) / 100));
}
