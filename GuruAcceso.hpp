#if !defined _GURUACCESO_H__
#define _GURUACCESO_H__

//////////////////////////////////////////////////////////////////////
// Link:
// Windows: Kernel32.lib
// Linux: phread.lib
//////////////////////////////////////////////////////////////////////
#include "GuruCompat.h"

#ifdef __GURUWIN32__
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef _WINSOCKAPI_
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
	#endif

	#include <windows.h>
#elif defined __GURULINUX__
	#include <pthread.h>
#endif

class GuruAcceso
{
public:
	GuruAcceso(int cantidad_permisos=0){
		if (!cantidad_permisos){
			mutex_acceso = NULL;
			total_accesos = 0;
		} else inicializar(cantidad_permisos);
	};

	virtual ~GuruAcceso(){
		for (int i=0;i<total_accesos;i++)
			destruir_acceso(i);

		delete[] mutex_acceso;
	};

	void inicializar(int cantidad_accesos){
	#ifdef __GURUWIN32__
		mutex_acceso = new CRITICAL_SECTION [cantidad_accesos];
	#elif defined __GURULINUX__
		mutex_acceso = new pthread_mutex_t [cantidad_accesos];
	#endif

		for (int i=0;i<cantidad_accesos;i++)
			crear_acceso(i);
	};

	void esperar_acceso(int acceso){
	#ifdef __GURUWIN32__
		EnterCriticalSection(&mutex_acceso[acceso]);
	#elif defined __GURULINUX__
		pthread_mutex_lock(&mutex_acceso[acceso]);
	#endif
	}

	void permitir_acceso(int acceso){
	#ifdef __GURUWIN32__
		LeaveCriticalSection(&mutex_acceso[acceso]);
	#elif defined __GURULINUX__
		pthread_mutex_unlock(&mutex_acceso[acceso]);
	#endif
	}

private:
	bool crear_acceso(int acceso){
		if (total_accesos <= acceso) {
	#ifdef __GURUWIN32__
			InitializeCriticalSection(&mutex_acceso[acceso]);
	#elif defined __GURULINUX__
			pthread_mutex_init(&mutex_acceso[acceso], NULL);
	#endif
			return true;
		}

		return false;
	}

	bool destruir_acceso(int acceso){
		if (total_accesos <= acceso) {
	#ifdef __GURUWIN32__
			DeleteCriticalSection(&mutex_acceso[acceso]);
	#elif defined __GURULINUX__
			pthread_mutex_destroy(&mutex_acceso[acceso]);
	#endif
			return true;
		}

		return false;
	};

#ifdef __GURUWIN32__
	CRITICAL_SECTION* mutex_acceso;
#elif defined __GURULINUX__
	pthread_mutex_t* mutex_acceso;
#endif
	int total_accesos;
};

class GuruBloqueo {
public:
	GuruBloqueo(){crear_acceso();};
	virtual ~GuruBloqueo(){destruir_acceso();};

	inline void bloquear(){
	#ifdef __GURUWIN32__
		EnterCriticalSection(&mutex_acceso);
	#elif defined __GURULINUX__
		pthread_mutex_lock(&mutex_acceso);
	#endif
	}

	inline void desbloquear(){
	#ifdef __GURUWIN32__
		LeaveCriticalSection(&mutex_acceso);
	#elif defined __GURULINUX__
		pthread_mutex_unlock(&mutex_acceso);
	#endif
	}

private:
	inline void crear_acceso(){

	#ifdef __GURUWIN32__
			InitializeCriticalSection(&mutex_acceso);
	#elif defined __GURULINUX__
			pthread_mutex_init(&mutex_acceso, NULL);
	#endif
	}

	inline void destruir_acceso(){

	#ifdef __GURUWIN32__
			DeleteCriticalSection(&mutex_acceso);
	#elif defined __GURULINUX__
			pthread_mutex_destroy(&mutex_acceso);
	#endif
	};

#ifdef __GURUWIN32__
	CRITICAL_SECTION mutex_acceso;
#elif defined __GURULINUX__
	pthread_mutex_t mutex_acceso;
#endif
};

#endif
