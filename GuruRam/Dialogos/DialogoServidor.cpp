// DialogoServidor.cpp: implementation of the DialogoServidor class.
//
//////////////////////////////////////////////////////////////////////

#include "DialogoServidor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DialogoServidor::DialogoServidor()
{
	limites(10, 6, 50, 7);

	texto(5, 2, white, blue,"Nombre Servidor:");

	struct INGRESAR ingreso = {0, 5, 3, black, white, 40, 80};
	servidor = ingresar(&ingreso);

	texto(alineado_centrado, dialogo_alto - 1, white, blue,"[ESC=Cancelar - ENTER=Aceptar]");
}

DialogoServidor::~DialogoServidor()
{

}

void DialogoServidor::nombre_servidor(char* nombre_servidor)
{
	servidor->cargar_ingreso(nombre_servidor);
}

char* DialogoServidor::nombre_servidor()
{
	return servidor->ingresado();
}
