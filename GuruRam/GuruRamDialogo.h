// GuruRamDialogo.h: interface for the GuruRamDialogo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GURURAMDIALOGO_H__A9E6BE5D_9C99_4F68_8B55_B43A5CA20C32__INCLUDED_)
#define AFX_GURURAMDIALOGO_H__A9E6BE5D_9C99_4F68_8B55_B43A5CA20C32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GuruRam.h"

#include "GuruRamMenu.h"
#include "GuruRamLista.h"
#include "GuruRamIngreso.h"

typedef struct
		{
		dword id_control;
		int   tipo_control;

		GuruRam* GuruControl;
		}control;

typedef struct
		{
		dword id_control;
		int   tipo_control;

		byte ColorFrente;
		byte ColorFondo;

		int	x;
		int	y;

		int ancho;
		int alto;

		GuruBuffer	texto;

		}estatico;

class GuruRamDialogo : public GuruRam  
{
public:
	GuruRamDialogo(byte x_dialogo = 0, byte y_dialogo = 0, byte ancho_dialogo = 0, byte alto_dialogo = 0);
	virtual ~GuruRamDialogo();

	void	limites(byte x_dialogo, byte y_dialogo, byte ancho_dialogo, byte alto_dialogo);
	void	marco(struct VENTANA *forma_ventana);

	virtual void	texto (struct ESCRIBIR *data_escribir);
	virtual void	texto (int x, int y, byte ColorFrente, byte ColorFondo, const char *Frase);
	virtual void	texto (int x, int y, char caracter);

	virtual GuruRamMenu* menu(struct MENU *forma_menu, word teclas_salida[] = NULL);
	virtual GuruRamLista* lista(struct LISTA *forma_lista, word teclas_salida[] = NULL);
	virtual GuruRamIngreso* ingresar(struct INGRESAR *forma_ingreso, word teclas_salida[] = NULL);

	byte dialogo_x;
	byte dialogo_y;
	byte dialogo_ancho;
	byte dialogo_alto;

	enum tipo_controles {control_menu, control_lista, control_ingreso, control_texto};
	enum ejecucion_dialogo {ejecutar_dialogo, cortar_ejecucion_dialogo};

//////////////////////////////////////////////////////////////////////
// Para los eventos del dialogo
//////////////////////////////////////////////////////////////////////
	virtual word ejecutar();

private: // metodos
	virtual bool inicializar() {return true;}
	virtual void crear_marco();
	virtual void post_crear_marco() {}
	virtual void mostrar_controles_estaticos();
	virtual void mostrar_controles();
	virtual word pre_ejecutar() {return ejecutar_dialogo;}

//////////////////////////////////////////////////////////////////////
// El retorno de estos mensajes indica:
// true = continua con el ciclo de paseo de los controles.
// false = cancela el cicla de paseo por los controles.
//////////////////////////////////////////////////////////////////////
	virtual bool CuandoEnter() {return false;}
	virtual bool CuandoEscape() {return false;}


private: // atributos
	struct VENTANA		data_ventana;

	GuruListaGenerica	<control>  controles;
	GuruListaGenerica	<estatico> estaticos;

	dword			cant_controles;

	region_video	region_dialogo;
};

#endif // !defined(AFX_GURURAMDIALOGO_H__A9E6BE5D_9C99_4F68_8B55_B43A5CA20C32__INCLUDED_)

//////////////////////////////////////////////////////////////////////
// Como utilizar la clase GuruRamDialogo
// 1) Crear una clase heredada de GuruRamDialogo
//   1.1) El constructor debe ser de la siguiente forma:
//        DialogoServidor::DialogoServidor(GuruRam &copia):GuruRamDialogo(copia)
//   1.2) En el Contructor se debe llamar al metodo limite para saber el ancho
//        y algo del dialogo
// 2) Desde donde se declare la instancia de la clase llamarla con la
//    funcion ejecutar();
//////////////////////////////////////////////////////////////////////
