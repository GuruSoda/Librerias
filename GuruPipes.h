// GuruPipes.h: interface for the GuruPipes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GURUPIPES_H__CB74B2C3_25B0_11D4_B04F_00104BC85F60__INCLUDED_)
#define AFX_GURUPIPES_H__CB74B2C3_25B0_11D4_B04F_00104BC85F60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AUTOR_H
	#define		__AUTOR_H
	static char autor[] = "Autor: Fabricio D. Rossi (El Guru)";
#endif


#define					BUFFER_SALIDA			4096
#define					BUFFER_ENTRADA			4096

#define					INTERVALO_SCAN_PIPES	250 // un cuarto de segundo
//#define					ESPERA_PIPES			15000

typedef	struct
		{
		char usuario[25];
		char dominio[25];
		char clave[52];
		}data_validar;

#include <windows.h>
#include "GuruError.h"

class GuruPipes  
{
public:
	GuruPipes();
	virtual ~GuruPipes();

	BOOL	abrir(char const *nombre);
	BOOL	crear(const char *nombre);
	BOOL	leer(void *buffer,DWORD a_leer);
	BOOL	leer_cadena(char *buffer,DWORD a_leer);
	BOOL	escribir(void *buffer,DWORD a_escribir);
	BOOL	escribir_cadena(const char *buffer);

	BOOL	esperar(DWORD milisegundos = 1);
	BOOL	conectar();
	void	cortar();
	char *	mensaje_error();
	void	liberar();
	HANDLE	nuevo_handle(HANDLE nuevo);
	HANDLE	handle_pipe();

	void	ver_cmd();

	BOOL	acceso(char *grupo);

	void	escuchar(char *nuevo_pipe);
	static	void tomar_entradas_pipes(GuruPipes *esto);
	static	void nueva_conexion(GuruPipes *llamador);	
	void	aceptada(GuruPipes *nueva,GuruPipes *actual);

	virtual BOOL aceptar_entrada();
	virtual void main_pipes();


protected:

	char	*nombre_pipe;			// nombre del pipe \\.\PIPE\xxxx
	char	*msg_error;				// ultimo mensaje de error
	HANDLE	pipe;					// handle del pipe que esta escuchando
	DWORD	modo_apertura;			
	DWORD	modo_pipe;
	DWORD	tiempo_espera;
	GuruError	error;
	BOOL	escuchar_entradas;		// si se esta escuchando por entradas al pipe
	BOOL	soy_servidor_pipe;
	HANDLE	handle_escuchar_entradas; // handle al thread que esta escuchando las entradas
};

#endif // !defined(AFX_GURUPIPES_H__CB74B2C3_25B0_11D4_B04F_00104BC85F60__INCLUDED_)
