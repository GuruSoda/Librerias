#if !defined __GURUVISOREVENTOSESCRIBIR_H__
#define __GURUVISOREVENTOSESCRIBIR_H__

#include "GuruCompat.h"

#include <Windows.h>
#include "GuruRegistry.hpp"

#define					APLICACION					1
#define					SEGURIDAD					2
#define					SISTEMA						3

class GuruVisorEventosEscribir
{
public:
	GuruVisorEventosEscribir();
	GuruVisorEventosEscribir(const char *nombre_reg_servicio);

	virtual ~GuruVisorEventosEscribir();

	void	registrar_aplicacion(char *nombre_registracion);
	void	registrar_seguridad(char *nombre_registracion);
	void	registrar_sistema(char *nombre_registracion);

	void	EscribirEvento(WORD tipo, DWORD id, const char *parametros[]);

private:

	void	registrar(int lugar, char *nombre_registracion);

private:
	char	nombre_servicio[255];
};

#endif // !defined(AFX_GURUVISOREVENTOSESCRIBIR_H__D3BCB6A5_91DB_4054_9764_CBE9E915323E__INCLUDED_)
