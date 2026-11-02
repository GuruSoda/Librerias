#if !defined __GURUCMD_H__
#define __GURUCMD_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <GuruError.hpp>

class GuruCmd {
public:
	GuruCmd() {};
	virtual ~GuruCmd() {};

	void	terminar_cmd();

	static	void	leer_salida(void *yo);
	static	void	leer_salida_error(void *yo);

	static	void	controlar_cmd(void *yo);

	bool	enviar_comando(char *comando);
	virtual	void	on_salida_cmd(char *texto);
	virtual	void	on_salida_error_cmd(char *texto);
	virtual	void	on_terminar_cmd();

	bool	Ejecutando_Cmd;
	void	cargar_cmd();

private:

	HANDLE	ejecutar_cmd();
	char*	nombre_interprete_comandos(char *aplicacion);

	static	void enviar_senial(void *yo);


// para el redireccionamiento de los pipies

	// para la aplicacion a redireccionar
	HANDLE		entrada_hijo, salida_hijo, error_hijo;

// para la entrada desde el programa
	HANDLE		Entrada_Std, Salida_Std, Error_Std;

// para los thread que controlan las entradas salidas y error
	HANDLE		thread_salida;
	HANDLE		thread_control;
	HANDLE		thread_error;

	HANDLE		handle_cmd;

	HANDLE		handle_evento;

//	log			logeo;

	std::string	mensaje;
	GuruError	error;
};

#endif // !defined(AFX_GURUCMD_H__04C154A3_01B9_11D5_8FA1_000102696F41__INCLUDED_)
