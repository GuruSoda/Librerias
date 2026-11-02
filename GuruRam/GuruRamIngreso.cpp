// GuruRamIngreso.cpp: implementation of the GuruRamIngreso class.
//
//////////////////////////////////////////////////////////////////////

#include "GuruRamIngreso.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuruRamIngreso::GuruRamIngreso(struct INGRESAR *ingreso)
{
//	poner_handle(GuruRam::instancia()->sacar_handle());

	if (ingreso)
		{
		data_ingreso = *ingreso;

		pos_cursor	= data_ingreso.x;

		ingreso_x	= data_ingreso.x;
		ingreso_y	= data_ingreso.y;
		}
	else
		{
		ingreso_x = 0;
		ingreso_y = 0;
		pos_cursor = 0;
		}


}

GuruRamIngreso::~GuruRamIngreso()
{

}

//////////////////////////////////////////////////////////////////////
// Parametros:
// x, y: eje x/y de la pantalla.
// char *cadena = buffer que contendra la cadena.
// int cant = cantidad de caracteres que puede tener la cadena.
// int tipo = tipo de caracter a ingresar.
// word teclas_salida = con que teclas se va del ingreso.
// word GuruRamIngreso::ingresar(int x, int y,char *cadena, int cant, int tipo, word teclas_salida[])
//////////////////////////////////////////////////////////////////////
word GuruRamIngreso::ingresar(struct INGRESAR *forma_ingreso, word teclas_salida[])
{
	word	tecla;
	byte	insertado=TRUE;
	int		cont_caract=0;
	word	teclas_salida_default[] = {GuruRam::kbEsc, GuruRam::kbEnter, GuruRam::kbTab, NULL};

	if (!teclas_salida)
		teclas_salida =  teclas_salida_default;

	if (forma_ingreso)
		{
		data_ingreso = *forma_ingreso;

		pos_cursor	= data_ingreso.x;
		ingreso_x	= data_ingreso.x;
		ingreso_y	= data_ingreso.y;
		}
	else if (data_ingreso.x == 0)
		return GuruRam::kbEsc;

//	char *aux_cadena = data_ingreso.cadena;

//	texto = data_ingreso.cadena;

	refrescar();

	int cant = data_ingreso.largo_control; // puede ser largo_buffer ....
	cant--; // tengo que dejar un lugar libre para mover las letras

	cursor(data_ingreso.x, data_ingreso.y);

	cont_caract = texto.longitud_data();

//	data_ingreso.cadena = aux_cadena;

	if (cont_caract >= data_ingreso.largo_control)
		cont_caract--;

	pos_cursor=data_ingreso.x+cont_caract;
	cursor(pos_cursor, data_ingreso.y);
	tecla = leer_tecla();

	while (!existe_tecla(tecla, teclas_salida))
		{
		switch (tecla)
			{
		case GuruRam::kbRight:
					if (pos_cursor != cont_caract+data_ingreso.x)
						{
						pos_cursor++;
						cursor(pos_cursor, data_ingreso.y);
						}

					break;

		case GuruRam::kbLeft:
					if (pos_cursor == data_ingreso.x)  break;

					pos_cursor--;
					cursor(pos_cursor, data_ingreso.y);

					break;

		case GuruRam::kbHome:
					pos_cursor = data_ingreso.x;
					cursor(data_ingreso.x, data_ingreso.y);

					break;

		case GuruRam::kbEnd:
					pos_cursor = cont_caract+data_ingreso.x;
					cursor(cont_caract+data_ingreso.x, data_ingreso.y);

					break;

		case GuruRam::kbDelete:
					if (data_ingreso.x+cont_caract != pos_cursor) // en el medio o principio
						{
						texto.borrar(pos_cursor-data_ingreso.x+1, 1);
						escribir(data_ingreso.x, data_ingreso.y, data_ingreso.ColorFrente, data_ingreso.ColorFondo, texto.contenido());
						cont_caract--;
						escribir(data_ingreso.x+cont_caract, data_ingreso.y, data_ingreso.ColorFrente, data_ingreso.ColorFondo, ' ');
						}

					break;

		case GuruRam::kbInsert:
					if (insertado == TRUE)
						insertado = FALSE;
					else
						insertado = TRUE;

					break;

		case GuruRam::kbBackSpace:
					if (pos_cursor > data_ingreso.x) // "no" estoy en el primer lugar
						{
						texto.borrar(pos_cursor-data_ingreso.x,1);
						if (pos_cursor == data_ingreso.x+cont_caract) // estoy al final?
							escribir(pos_cursor-1, data_ingreso.y, data_ingreso.ColorFrente, data_ingreso.ColorFondo, ' ');
						else // en el medio del texto
							{
							escribir(data_ingreso.x, data_ingreso.y, data_ingreso.ColorFrente, data_ingreso.ColorFondo, texto.contenido());
							escribir(data_ingreso.x+cont_caract - 1, data_ingreso.y, data_ingreso.ColorFrente, data_ingreso.ColorFondo, ' ');
							}

						cont_caract--;
						pos_cursor--;
						cursor(pos_cursor, data_ingreso.y);	// retrocedo cursor
						}

					break;
		case GuruRam::kbSpace:
					Lo(tecla) = 0;
		default:
				if (Lo(tecla) != 0) break;

				if (data_ingreso.tipo == numeros)
					if (tecla < '0' || tecla > '9')
						break;

				if (insertado == FALSE && pos_cursor <= cant+data_ingreso.x)
					{
					texto.contenido()[pos_cursor-data_ingreso.x] = tecla;
					cursor(pos_cursor, data_ingreso.y);
					escribir(pos_cursor, data_ingreso.y, data_ingreso.ColorFrente, data_ingreso.ColorFondo, tecla);
					cont_caract++;
					pos_cursor++;
					texto.contenido()[data_ingreso.x+cont_caract] = '\0';
					}
				else
					if (insertado == TRUE)
						if (pos_cursor != data_ingreso.x+cont_caract) // en el medio o al inicio
							{
							if (cont_caract == cant)
								{
								// cadena[cont_caract] = '\0';
								// cprintf("%s", insert(cadena,tecla,pos_cursor-x));
								// pos_cursor++;
								cursor(pos_cursor, data_ingreso.y);
								}
							else
								{
								cont_caract++;
								texto.insertar(pos_cursor-data_ingreso.x+1, tecla);
								escribir(data_ingreso.x, data_ingreso.y, data_ingreso.ColorFrente, data_ingreso.ColorFondo, texto.contenido());
								pos_cursor++;
								cursor(pos_cursor, data_ingreso.y);
								}
							}
						else // al final
							{
							if (pos_cursor != data_ingreso.x+cant)
								{
								texto.concatenar(tecla);
								escribir(pos_cursor, data_ingreso.y, data_ingreso.ColorFrente, data_ingreso.ColorFondo, tecla);

								cont_caract++;
								pos_cursor++;
								cursor(pos_cursor, data_ingreso.y);
								}
							}
			} // switch
// otra:
/*
	char chartecla[120];
	sprintf(chartecla, "pos_cursor  = %02lu", pos_cursor);
	escribir(1, 25, GuruRam::red, GuruRam::lightgray, chartecla);

	sprintf(chartecla, "cont_caract = %02lu", cont_caract);
	escribir(1, 24, GuruRam::red, GuruRam::lightgray, chartecla);

	sprintf(chartecla, "largo texto = %02lu", texto.longitud_data());
	escribir(1, 23, GuruRam::red, GuruRam::lightgray, chartecla);

	sprintf(chartecla, "texto = %-78s", texto.contenido());
	escribir(1, 22, GuruRam::red, GuruRam::lightgray, chartecla);
*/
//	char chartecla[120];
//	sprintf(chartecla, "%-50s", ingresado());
//	escribir(1, 24, data_ingreso.ColorFrente, data_ingreso.ColorFondo, chartecla);
	tecla = leer_tecla();
	} // while

//	data_ingreso.cadena = aux_cadena;
//	data_ingreso.cadena[cont_caract+1] = '\0';

	return tecla;
}

word GuruRamIngreso::leer_tecla()
{
	INPUT_RECORD reg_entrada[128];
	dword        leidas = 0;
	bool         salir = false;
	word         tecla_presionada;

	while (!salir)
		{
		WaitForSingleObject(entrada, INFINITE); // espero a que pase algo

		if (ReadConsoleInput(entrada, reg_entrada, 128, &leidas))
			{
			switch (reg_entrada[0].EventType)
				{
			case KEY_EVENT:
							if (reg_entrada[0].Event.KeyEvent.bKeyDown)
								{
								tecla_presionada = 0;

								switch (reg_entrada[0].Event.KeyEvent.uChar.AsciiChar)
									{
								case 0x08: // Back Space
								case 0x09: // Tabulador
								case 0x0D: // Enter
								case 0x1B: // Escape
								case 0x20: // Space
										tecla_presionada = (byte)reg_entrada[0].Event.KeyEvent.uChar.AsciiChar;
										Lo(tecla_presionada) = (byte)reg_entrada[0].Event.KeyEvent.wVirtualScanCode;
										break;
									}

								if (tecla_presionada) return tecla_presionada;

								// primero pregunto si es una tecla especial
								if (reg_entrada[0].Event.KeyEvent.uChar.AsciiChar == 0)
									{
									Lo(tecla_presionada) = (byte)reg_entrada[0].Event.KeyEvent.wVirtualKeyCode;
									// tecla_presionada = reg_entrada[0].Event.KeyEvent.wVirtualKeyCode;
									}
								else
									tecla_presionada = (byte)reg_entrada[0].Event.KeyEvent.uChar.AsciiChar;

								return tecla_presionada;
								}

							break;
			case MOUSE_EVENT:
							// break;
			case WINDOW_BUFFER_SIZE_EVENT:
								//break;
			case MENU_EVENT:
							// break;
			case FOCUS_EVENT:
							//break;
							continue;
				}
			}
	}

	return 0;
}

void GuruRamIngreso::escribir_tecla(word tecla)
{
	INPUT_RECORD reg_entrada[128];
	dword        escritos = 0;

	reg_entrada[0].EventType = KEY_EVENT;
	reg_entrada[0].Event.KeyEvent.bKeyDown = tecla;

	WriteConsoleInput(entrada, reg_entrada, 1, &escritos);
}

bool GuruRamIngreso::existe_tecla(word tecla, word teclas_salida[])
{
	word *inicio = teclas_salida;

	while (*teclas_salida)
		{
		if (tecla == *teclas_salida)
			return true;

		teclas_salida++;
		}

	return false;
}

void GuruRamIngreso::cargar_ingreso(char* texto_a_ingresar)
{
	texto = texto_a_ingresar;
}

char* GuruRamIngreso::ingresado()
{
	return texto.contenido();
}

GuruBuffer&	GuruRamIngreso::sacar_buffer()
{
	return texto;
}

void GuruRamIngreso::refrescar()
{
	GuruBuffer linea(data_ingreso.largo_control + 2);

	linea.insertar(1, ' ', data_ingreso.largo_control);

	escribir(ingreso_x, ingreso_y, data_ingreso.ColorFrente, data_ingreso.ColorFondo, linea.contenido());

	escribir(ingreso_x, ingreso_y, data_ingreso.ColorFrente, data_ingreso.ColorFondo, texto.contenido());
}
