#include "GuruHilo.h"

GuruHilo::GuruHilo()
{

}

GuruHilo::~GuruHilo()
{

}

//////////////////////////////////////////////////////////////////////
// La funcion a ser llamada como thread debe ser asi:
// unsigned __stdcall threadfunc(void * param);
//////////////////////////////////////////////////////////////////////
void GuruHilo::lanzar(unsigned (*direccion_inicio )(void*), void* parametro)
{
#if defined __GURUWIN32__
	manipulador = (HANDLE)_beginthreadex(NULL, 0, (unsigned (__stdcall*)(void*)) direccion_inicio, parametro, 0, &tid);
#elif defined __GURULINUX__
#endif
}

bool GuruHilo::activo()
{
#if defined __GURUWIN32__
	return (WaitForSingleObject( (HANDLE)manipulador, 0) == WAIT_OBJECT_0) ? true : false;
#elif defined __GURULINUX__
	return true;
#endif
}

bool GuruHilo::terminar(dword codigo_retorno)
{
#if defined __GURUWIN32__
	return (TerminateThread((HANDLE) manipulador, codigo_retorno) != 0) ? true : false;
#elif defined __GURULINUX__
	return true;
#endif
}

bool GuruHilo::esperar(dword milisegundos)
{
#if defined __GURUWIN32__
	return (WaitForSingleObject( (HANDLE)manipulador, milisegundos) == WAIT_TIMEOUT) ? true : false;
#elif defined __GURULINUX__
	return true;
#endif
}

void GuruHilo::fin(dword codigo_retorno)
{
#if defined __GURUWIN32__
	_endthreadex(codigo_retorno);
#elif defined __GURULINUX__
#endif
}
