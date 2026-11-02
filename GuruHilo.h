#if !defined __GURUHILO_H__
#define __GURUHILO_H__

//////////////////////////////////////////////////////////////////////
// Compilar con /MT (Multi-thread)
//////////////////////////////////////////////////////////////////////

#include "GuruCompat.h"
#include "GuruTiposDeDatos.h"

#if defined __GURUWIN32__
#include <windows.h>
#include <process.h>
#include <stddef.h>
#elif defined __GURULINUX__

#endif

class GuruHilo  
{
public:
	GuruHilo();
	virtual ~GuruHilo();

	void	lanzar(unsigned (*direccion_inicio )(void*), void* parametro = NULL);
	bool	activo();
	bool	terminar(dword codigo_retorno=0);
	bool	esperar(dword milisegundos=0);

	void	fin(dword codigo_retorno=0);

	word	thread_id() {return tid;};
#if defined __GURUWIN32__
	HANDLE	handle() {return manipulador;};
#elif defined __GURULINUX__
	dword	handle() {return manipulador;};
#endif

private:
	word		tid;

#if defined __GURUWIN32__
	HANDLE		manipulador;
#elif defined __GURULINUX__
	dword		manipulador;
#endif

};

#endif // !defined __GURUHILO_H__

/*
uintptr_t _beginthread(void( __cdecl *start_address )( void * ), unsigned stack_size, void *arglist);
uintptr_t _beginthreadex(void *security, unsigned stack_size, unsigned ( __stdcall *start_address )( void * ), void *arglist, unsigned initflag, unsigned *thrdaddr);
*/

