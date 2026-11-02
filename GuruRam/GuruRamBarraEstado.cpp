// GuruRamBarraEstado.cpp: implementation of the GuruRamBarraEstado class.
//
//////////////////////////////////////////////////////////////////////

#include "GuruRamBarraEstado.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuruRamBarraEstado::GuruRamBarraEstado()
{

}

GuruRamBarraEstado::~GuruRamBarraEstado()
{

}

void GuruRamBarraEstado::escribir(char *frase, enum lugar donde)
{
	switch (donde)
		{
	case izquierda:
					linea = frase;
					break;
	case centro:
					break;
	case derecha:
					break;
		}

	linea.insertar(linea.longitud_data()+1, ' ', ancho - linea.longitud_data());
	GuruRam::escribir(1, alto, GuruRam::black, GuruRam::white, linea.contenido());
}
