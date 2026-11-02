// GuruRamMensaje.cpp: implementation of the GuruRamMensaje class.
//
//////////////////////////////////////////////////////////////////////

#include "GuruRamMensaje.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuruRamMensaje::GuruRamMensaje()
{
	zona_salvada.celdas_salvadas = NULL;
}

GuruRamMensaje::~GuruRamMensaje()
{
	if (zona_salvada.celdas_salvadas)
		GuruRam::destruir(&zona_salvada);
}

void GuruRamMensaje::mostrar(char* frase, char* titulo, dword opciones, struct VENTANA* data_ventana)
{
	if (data_ventana)
		ventana(data_ventana);

	struct VENTANA ventana_mensaje = {0};

	int largo = strlen (frase);

	ventana_mensaje.x				= (ancho_dispositivo / 2) - (largo / 2) - 2;
	ventana_mensaje.y				= (alto_dispositivo/2) - 2;
	ventana_mensaje.Alto			= 5;
	ventana_mensaje.Sombra			= true;
	ventana_mensaje.ColorFrente		= white;
	ventana_mensaje.ColorFondo		= red;
	ventana_mensaje.Ancho			= largo + 4;

	if (titulo)
		{
		ventana_mensaje.LugarTitulo = abajo_centro;
		ventana_mensaje.Titulo = titulo;
		}

	if (zona_salvada.celdas_salvadas)
		recuperar(&zona_salvada);

	salvar(ventana_mensaje.x, ventana_mensaje.y, (ventana_mensaje.Sombra) ? ventana_mensaje.Ancho + 1 : ventana_mensaje.Ancho, (ventana_mensaje.Sombra) ? ventana_mensaje.Alto + 1 : ventana_mensaje.Alto, &zona_salvada);

	ventana(&ventana_mensaje);
	escribir(ventana_mensaje.x + 2, alto_dispositivo / 2, white, red, frase);
}

//////////////////////////////////////////////////////////////////////
// destruir
// Tarea: Destruye la ventana pero no la despinta de la pantalla
//////////////////////////////////////////////////////////////////////
void GuruRamMensaje::destruir()
{
	GuruRam::destruir(&zona_salvada);
}

//////////////////////////////////////////////////////////////////////
// ocultar
// Tarea: recupera el fondo de la ventana del mensaje
//////////////////////////////////////////////////////////////////////
void GuruRamMensaje::ocultar()
{
	recuperar(&zona_salvada);
}
