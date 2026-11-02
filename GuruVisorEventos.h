#if !defined __GURUVISOREVENTOS_H__
#define __GURUVISOREVENTOS_H__

#include "GuruCompat.h"

#include <Windows.h>
#include <Stdio.h>
#include <time.h>

#include "GuruLista.h"
#include "GuruError.h"
#include "GuruRegistry.h"
#include "GuruTiposDeDatos.h"

#define					GURUVISOREVENTOS_TAMANIO_BUFFER			64

enum	tipo_registro {Error, Warning, Information, Success_Audit, Failure_Audit, Todos, Ninguno, Desconocido};

class GuruVisorEventos  
{
public:
	GuruVisorEventos();
	virtual ~GuruVisorEventos();

	bool	abrir(char *evento);
	bool	abrir(char *nombre_pc,char *evento_requerido);
	dword	cantidad_entradas();
	bool	sacar_linea();
	char*	nombre_pc();
	char*	origen();
	char*	tipo(char *buffer);
	tipo_registro	tipo();
	char*	categoria();
	char*	id();		
	char*	suceso();	
	char*	usuario();
	char*	descripcion();

	char*	hora();
	char*	fecha();

	HANDLE	handle_visor_eventos();

	void	lectua_desde_el_primero(bool forma = true);

	const char*	mensaje_error();
	dword	numero_error();

// miembros
private:
	const char*	sacar_descripcion_reg(char *descripcion[], dword id);
	char*	reemplazo_entorno(char *source);

	char*	cargar_buffer_info(char *nueva_info);

// datos
private:

	dword	tamanio_buffer_info;

	dword	tipo_lectura;

	HANDLE	hl;

	EVENTLOGRECORD	*registro;
	char			*buffer_info;
	GuruError		GuruVisorEventos_error;

	char	evento[50];
	char	pc[25];
};

#endif // !defined __GURUVISOREVENTOS_H__
