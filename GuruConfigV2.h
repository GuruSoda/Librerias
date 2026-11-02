#ifndef __GURUCONFIGV2_H__
#define __GURUCONFIGV2_H__

// definiciones para el comportamiento:
// Para que pueda cargar un campo libre, ejem nada={98asd98fuasdfnasdfasdpf908asdfasd}
// GURUCONFIG_CAMPOLIBRE
//
// 04/10/2016
// * Una cadena empieza con " y termina con ", antes podia terminar con un enter.
// * Un valor de una llave empieza despues del = y termina en el ;
// * El campo libre ahora es optativo, se requiere un etiqueta del preprocesador.

#include <GuruCompat.h>
#include <GuruError.hpp>

#include <iostream> // cout

#include <fstream>
#include <string>

#if defined __GURUWIN32__
#elif defined __GURULINUX__
#include <stdlib.h>
#include <stdint.h>
#endif

using namespace std;

class GuruConfigExcepcion:public std::exception
{
public:
	GuruConfigExcepcion(const string& msj):mensaje(msj) {};
	~GuruConfigExcepcion() throw() {};

	virtual const char* what() const throw() { return mensaje.c_str();};

protected:
	string mensaje;
};

class GuruConfig
{
public:
	GuruConfig(const string& archivo_conf=""):nombre_archivo(archivo_conf) {};
	virtual ~GuruConfig(void) { if (handle_archivo.is_open()) cerrar();};

	void	abrir(const string& nombre);
	void	cerrar();
protected:
	enum   tipo_caracter  {error, fin_archivo, valor, campo_libre, cadena, simbolo, subseccion, enter, comentario, abre_parentesis, cierra_parentesis, abre_llave, cierra_llave, abre_corchetes, cierra_corchete, igual, punto_y_coma};

	enum	tipo_caracter sacar_hasta_enter();
	enum    tipo_caracter sacar_hasta_punto_y_coma();
	enum    tipo_caracter sacar_hasta_cierra_parenteris();
	enum	tipo_caracter sacar_hasta_fin_seccion();
	enum    tipo_caracter sacar_token();
	enum    tipo_caracter sacar_cadena();
	enum    tipo_caracter sacar_simbolo();
#if defined GURUCONFIG_CAMPOLIBRE
	enum    tipo_caracter sacar_campo_libre();
	enum	tipo_caracter ignorar_campo_libre();
#endif
	enum    tipo_caracter sacar_bloque(int iniciobloque, int finbloque);

	int     sacar_caracter();
	int     poner_caracter();
	void	agregar_caracter(int caracter);

	bool	siguiente_simbolo();

	void	abrir();

	string	buffer;
	int		caracter;
	uint32_t	fila;				// fila que esta procesando
	uint32_t	columna;			// columna que esta procesando
	enum tipo_caracter token;

	uint32_t	lugar_actual;		// byte del archivo que esta procesando
	uint32_t	inicio_simbolo;		// bytes del archivo donde empieza el simbolo procesado
	uint32_t	inicio_valor;		// bytes del archivo donde comienza el valor que se proceso

	ifstream	handle_archivo;
	string		nombre_archivo;
};

#endif
