// GuruRamDialogo.cpp: implementation of the GuruRamDialogo class.
//
//////////////////////////////////////////////////////////////////////

#include "GuruRamDialogo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuruRamDialogo::GuruRamDialogo(byte x_dialogo, byte y_dialogo, byte ancho_dialogo, byte alto_dialogo)
{
	ancho    = ancho_dialogo;
	alto     = ancho_dialogo;

	dialogo_x = x_dialogo;
	dialogo_y = y_dialogo;
	dialogo_ancho = ancho_dialogo;
	dialogo_alto  = alto_dialogo;

	cant_controles = 1;

	region_dialogo.celdas_salvadas = NULL;

	controles.autoincrementar(false);
	estaticos.autoincrementar(false);
}

GuruRamDialogo::~GuruRamDialogo()
{
	controles.al_inicio();

	while (!controles.fin())
		{
		delete controles.item()->GuruControl;
		controles++;
		}

	controles.al_inicio();

	if (region_dialogo.celdas_salvadas)
		recuperar(&region_dialogo);
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
// bool GuruRamDialogo::inicializar()
// {
//	return true;
// }

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void GuruRamDialogo::crear_marco()
{
	salvar(data_ventana.x, data_ventana.y, data_ventana.Ancho + (data_ventana.Sombra?1:0), data_ventana.Alto + (data_ventana.Sombra?1:0), &region_dialogo);

	ventana(&data_ventana);
}

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void GuruRamDialogo::mostrar_controles()
{
	controles.al_inicio();

	while (!controles.fin())
		{
		switch (controles.item()->tipo_control)
			{
		case control_menu: 
							((GuruRamMenu*)(controles.item()->GuruControl))->refrescar();
						break;
		case control_lista: 
							((GuruRamLista*)(controles.item()->GuruControl))->refrescar();
						break;
		case control_ingreso: 
							((GuruRamIngreso*)(controles.item()->GuruControl))->refrescar();
						break;
			}

		controles++;
		}

	controles.al_inicio();
}

void GuruRamDialogo::mostrar_controles_estaticos()
{
	estaticos.al_inicio();

	while (!estaticos.fin())
		{
		switch (estaticos.item()->tipo_control)
			{
		case control_texto:
						GuruRam::escribir(estaticos.item()->x,estaticos.item()->y, estaticos.item()->ColorFrente, estaticos.item()->ColorFondo, estaticos.item()->texto.contenido());
							break;
			}
		estaticos++;
		}

	estaticos.al_inicio();
}

void GuruRamDialogo::limites(byte x_dialogo, byte y_dialogo, byte ancho_dialogo, byte alto_dialogo)
{
	ancho = ancho_dialogo;
	alto  = alto_dialogo;

	dialogo_x = x_dialogo;
	dialogo_y = y_dialogo;
	dialogo_ancho = ancho_dialogo;
	dialogo_alto  = alto_dialogo;

//////////////////////////////////////////////////////////////////////
// Para el marco por defecto.
//////////////////////////////////////////////////////////////////////
	data_ventana.id = 0;

	data_ventana.x = dialogo_x;						// Posicion x
	data_ventana.y = dialogo_y;						// Posicion y
	data_ventana.Ancho = dialogo_ancho;				// Cantidad de columnas
	data_ventana.Alto = dialogo_alto;				// Cantidad de filas
	data_ventana.ColorFrente = white;				// Color del caracter
	data_ventana.ColorFondo = blue;					// Color del fondo
	data_ventana.Titulo = NULL;						// Titulo de la ventana
	data_ventana.LugarTitulo = arriba_derecha;		// Posicion del titulo
	data_ventana.tipo_borde = simple_simple;		// Tipo de borde de ventana
	data_ventana.atras = NULL;						// lo que hay atras de la ventana
	data_ventana.Sombra = true;						// Con o sin sombra
}


void GuruRamDialogo::marco(struct VENTANA *forma_ventana)
{
	data_ventana = *forma_ventana;
}

void GuruRamDialogo::texto (struct ESCRIBIR *forma_escribir)
{
	estatico temp_reg;

	switch (forma_escribir->x)
		{
	case alineado_centrado: temp_reg.x = ((ancho/2) - (strlen(forma_escribir->Frase)/2)) + dialogo_x; break;
	case alineado_derecha: temp_reg.x = dialogo_x + ancho - strlen(forma_escribir->Frase) - 1; break;
	case alineado_izquierda: temp_reg.x = dialogo_x + 1; break;
	default: temp_reg.x = forma_escribir->x + dialogo_x;
		}

	switch (forma_escribir->y)
		{
	case alineado_centrado: temp_reg.y = dialogo_y + (alto/2); break;
	case alineado_arriba: temp_reg.y = dialogo_y; break;
	case alineado_abajo: temp_reg.y = dialogo_y + alto; break;
	default: temp_reg.y = forma_escribir->y + dialogo_y - 1;
		}

	temp_reg.id_control = cant_controles++;
	temp_reg.tipo_control = control_texto;

	temp_reg.ColorFrente = forma_escribir->ColorFrente;
	temp_reg.ColorFondo  = forma_escribir->ColorFondo;

	temp_reg.ancho = strlen(forma_escribir->Frase);
	temp_reg.alto  = 1;

	temp_reg.texto = (char *)forma_escribir->Frase;

	estaticos.agregar(&temp_reg);
}

void GuruRamDialogo::texto (int x, int y, byte ColorFrente, byte ColorFondo, const char *Frase)
{
	estatico temp_reg;

	switch (x)
		{
	case alineado_centrado: temp_reg.x = ((ancho/2) - (strlen(Frase)/2)) + dialogo_x; break;
	case alineado_derecha: temp_reg.x = dialogo_x + ancho - strlen(Frase) - 1; break;
	case alineado_izquierda: temp_reg.x = dialogo_x + 1; break;
	default: temp_reg.x = x-1 + dialogo_x;
		}

	switch (y)
		{
	case alineado_centrado: temp_reg.y = dialogo_y + (alto/2); break;
	case alineado_arriba: temp_reg.y = dialogo_y; break;
	case alineado_abajo: temp_reg.y = dialogo_y + alto - 1; break;
	default: temp_reg.y = y-1 + dialogo_y;
		}


	temp_reg.id_control = cant_controles++;
	temp_reg.tipo_control = control_texto;

	temp_reg.ColorFrente = ColorFrente;
	temp_reg.ColorFondo  = ColorFondo;

//	temp_reg.x = x + dialogo_x;
//	temp_reg.y = y + dialogo_y;

	temp_reg.ancho = strlen(Frase);
	temp_reg.alto  = 1;

	temp_reg.texto = (char *)Frase;

	estaticos.agregar(&temp_reg);
}

void GuruRamDialogo::texto (int x, int y, char caracter)
{
	estatico temp_reg;

	switch (x)
		{
	case alineado_centrado: temp_reg.x = (ancho/2) + dialogo_x; break;
	case alineado_derecha: temp_reg.x = dialogo_x + ancho - 1; break;
	case alineado_izquierda: temp_reg.x = dialogo_x + 1; break;
	default: temp_reg.x = x-1 + dialogo_x;
		}

	switch (y)
		{
	case alineado_centrado: temp_reg.y = dialogo_y + (alto/2); break;
	case alineado_arriba: temp_reg.y = dialogo_y; break;
	case alineado_abajo: temp_reg.y = dialogo_y + alto; break;
	default: temp_reg.y = y-1 + dialogo_y;
		}


	temp_reg.id_control = cant_controles++;
	temp_reg.tipo_control = control_texto;

//	temp_reg.ColorFrente;
//	temp_reg.ColorFondo;

//	temp_reg.x = x + dialogo_x;
//	temp_reg.y = y + dialogo_y;

	temp_reg.ancho = 1;
	temp_reg.alto  = 1;

	temp_reg.texto.reservar(3);
	temp_reg.texto.rellenar('\0');
	temp_reg.texto.insertar(1, caracter, 1);

	estaticos.agregar(&temp_reg);
}

GuruRamMenu* GuruRamDialogo::menu(struct MENU *forma_menu, word teclas_salida[])
{
	control temp_reg = {0};

	temp_reg.tipo_control = control_menu;
	temp_reg.id_control = cant_controles++;
	temp_reg.GuruControl = new GuruRamMenu;

	controles.agregar(&temp_reg);

	return (GuruRamMenu*) temp_reg.GuruControl;
}

GuruRamLista* GuruRamDialogo::lista(struct LISTA *forma_lista, word teclas_salida[])
{
	struct LISTA temp_forma_lista = *forma_lista;

	temp_forma_lista.x += dialogo_x;
	temp_forma_lista.y += dialogo_y;

	control temp_reg = {0};

	temp_reg.tipo_control = control_lista;
	temp_reg.id_control = cant_controles++;
	temp_reg.GuruControl = new GuruRamLista(&temp_forma_lista);

	controles.agregar(&temp_reg);

	return (GuruRamLista*) temp_reg.GuruControl;
}

GuruRamIngreso* GuruRamDialogo::ingresar(struct INGRESAR *forma_ingreso, word teclas_salida[])
{
	struct INGRESAR temp_forma_ingreso = *forma_ingreso;

	switch (forma_ingreso->x)
		{
	case alineado_centrado: temp_forma_ingreso.x = ((ancho/2) - (forma_ingreso->largo_control/2)) + dialogo_x; break;
	case alineado_derecha: temp_forma_ingreso.x = dialogo_x + ancho - forma_ingreso->largo_control - 1; break;
	case alineado_izquierda: temp_forma_ingreso.x = dialogo_x + 1; break;
	default: temp_forma_ingreso.x = forma_ingreso->x-1 + dialogo_x;
		}

	switch (forma_ingreso->y)
		{
	case alineado_centrado: temp_forma_ingreso.y = dialogo_y + (alto/2); break;
	case alineado_arriba: temp_forma_ingreso.y = dialogo_y; break;
	case alineado_abajo: temp_forma_ingreso.y = dialogo_y + alto; break;
	default: temp_forma_ingreso.y = forma_ingreso->y-1 + dialogo_y;
		}


//	temp_forma_ingreso.x += dialogo_x;
//	temp_forma_ingreso.y += dialogo_y;

	control temp_reg = {0};

	temp_reg.tipo_control = control_ingreso;
	temp_reg.id_control = cant_controles++;
	temp_reg.GuruControl = new GuruRamIngreso(&temp_forma_ingreso);

	controles.agregar(&temp_reg);

	return (GuruRamIngreso*)temp_reg.GuruControl;
}

//////////////////////////////////////////////////////////////////////
// 
// Tareas:
//        1) Llamar a inicializar();
//        2) Llamar a crear_marco()
//        3) Llamar a post_crear_marco()
//        4) Llamar a mostrar_controles_estaticos()
//        5) Llamar a mostrar_controles()
//        6) Llamar a pre_ejecutar()
//        
//////////////////////////////////////////////////////////////////////
word GuruRamDialogo::ejecutar()
{
// Paso 1 = Inicializar
	if (!inicializar()) return 1;

// Paso 2 = Crear lo que seria el marco del dialogo
	crear_marco();

// Paso 3 = Llamar a una funcion Post creacion del marco
	post_crear_marco();

// Paso 4 = Mostrar en la ventana todos los controles estaticos
	mostrar_controles_estaticos();

// Paso 5 = Mostrar en la ventana todos los controles
	mostrar_controles();

// Paso 6 = Llamar a una funcion antes de empezar a ejecutar el dialogo.
	word corrida = pre_ejecutar();

// pregunto si continuo la ejecucion del dialogo o se corta aca.
	if (corrida == cortar_ejecucion_dialogo)
		return GuruRam::kbMinus;

// le paso el control al primero control (ID: 1)
	word tecla_salida_control;
	bool continar = true; // indica si continua con el ciclo que recorre los controles o no

	controles.al_inicio();

	do{
		switch (controles.item()->tipo_control)
			{
		case control_menu: 
							tecla_salida_control = ((GuruRamMenu*)(controles.item()->GuruControl))->menu();
						break;
		case control_lista: 
							tecla_salida_control = ((GuruRamLista*)(controles.item()->GuruControl))->lista();
						break;
		case control_ingreso: 
							cursor(true);
							tecla_salida_control = ((GuruRamIngreso*)(controles.item()->GuruControl))->ingresar();
							cursor(false);
						break;
			}

		if (tecla_salida_control == GuruRam::kbEnter)
			continar = CuandoEnter();

		if (tecla_salida_control == GuruRam::kbEsc)
			continar = CuandoEscape();

		controles++;

		if (controles.fin())
			controles.al_inicio();

	}while (continar);

	controles.al_inicio();

	return tecla_salida_control;
}
