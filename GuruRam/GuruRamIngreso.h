// GuruRamIngreso.h: interface for the GuruRamIngreso class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GURURAMINGRESO_H__183CFCE1_39B7_49B6_AFBE_A3EEE568F53B__INCLUDED_)
#define AFX_GURURAMINGRESO_H__183CFCE1_39B7_49B6_AFBE_A3EEE568F53B__INCLUDED_

#include "GuruRam.h"
#include "..\GuruBuffer.h"

#define     Lo(Tecla)   *((byte*)(&Tecla)+1)
#define     Hi(Tecla)   *((byte*)(&Tecla))

typedef struct INGRESAR
	{
	unsigned int id;

	int  x;               // Posicion x
	int  y;               // Posicion y

	byte ColorFrente;     // Color del caracter
	byte ColorFondo;      // Color del fondo

	int  largo_control;   // Cuan largo es el control.
	int  largo_buffer;    // Largo del texto a guardar.

	int  tipo;			  // Tipo de caracteres a ingresar

//	char *cadena;   // Frase a escribir
	}INGRESAR;


class GuruRamIngreso : public GuruRam  
{
public:
	GuruRamIngreso(struct INGRESAR *data_ingreso = NULL);
	virtual ~GuruRamIngreso();

//////////////////////////////////////////////////////////////////////
// Para el ingreso de datos
//////////////////////////////////////////////////////////////////////
	word	ingresar(struct INGRESAR *forma_ingreso = NULL, word teclas_salida[] = NULL);
	word	leer_tecla();
	void	escribir_tecla(word tecla);

	void		cargar_ingreso(char* texto_a_ingresar);
	char*		ingresado();
	GuruBuffer&	sacar_buffer();

	void	refrescar();

	bool	existe_tecla(word tecla, word teclas_salida[]);

	enum tipo_ingreso {total, numeros, letras, clave};

private:
	GuruBuffer texto;

	struct INGRESAR data_ingreso;

	int		ingreso_x;
	int		ingreso_y;

	int		pos_cursor;
};

#endif // !defined(AFX_GURURAMINGRESO_H__183CFCE1_39B7_49B6_AFBE_A3EEE568F53B__INCLUDED_)
