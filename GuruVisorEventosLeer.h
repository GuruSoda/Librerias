#ifndef __GURUVISOREVENTOSLEER_H__
#define __GURUVISOREVENTOSLEER_H__

#include "GuruCompat.h"
#include "GuruError.h"
#include "GuruBuffer.h"
#include <windows.h>

#include <string>

using namespace std;

class RegistroVisorEventos
{
	friend class GuruVisorEventosLeer;

public:
	string	computadora;
	string	origen;
	dword	categoria;
	dword	id_suceso;
	string	usuario;	
	string	hora;
	string	fecha;
	dword	cadenas_descripcion;
	GuruBuffer	descripcion;
	dword	fecha_hora_escrito;

	bool	cargado;

public:
	RegistroVisorEventos():cargado(false) {};
};

class GuruVisorEventosLeer
{
public:
	GuruVisorEventosLeer(void);
	virtual ~GuruVisorEventosLeer(void);

	RegistroVisorEventos primer_registro(const char* computadora,const char* nombre_registro);
	RegistroVisorEventos siguiente_registro();

private:
	EVENTLOGRECORD* cargar_registro();
	void armar_registro(EVENTLOGRECORD* registrowin, RegistroVisorEventos& registro);
	bool armar_descripcion(RegistroVisorEventos& registro);

	GuruError	error;
	HANDLE		handle;
};

#endif // !defined __GURUVISOREVENTOSLEER_H__