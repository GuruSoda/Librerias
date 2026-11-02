#ifndef __GURUASYNC_H__
#define __GURUASYNC_H__

#include <GuruCompat.h>
#include <GuruQueue.hpp>

#if defined __GURUWIN32__
	#include <process.h>
#elif defined __GURULINUX__
//////////////////////////////////////////////////////////////////////
//	Link: pthread
//////////////////////////////////////////////////////////////////////
	#include <pthread.h>
	#include <semaphore.h>
#endif

using namespace std;

template <typename T, typename Y>
class GuruAsync
{
	typedef bool (T::*TMetodo)(const Y& variable);

	T*		Clase;
	TMetodo	Metodo;

	GuruQueue_threadsafe<Y>		cola;
	string			nombre;

#ifdef __GURUWIN32__
	HANDLE	hescribir;
	HANDLE	ejecutando;
	HANDLE	HThreadAsync;
#elif defined __GURULINUX__
	sem_t	hescribir;
	sem_t	ejecutando;
	pthread_t HThreadAsync;
#endif
public:
	GuruAsync(T* laclase, TMetodo elmetodo):Clase(laclase),Metodo(elmetodo) {
		init();
	};

	virtual ~GuruAsync(void) {
	#ifdef __GURUWIN32__
		SetEvent(ejecutando);
		WaitForSingleObject(HThreadAsync, INFINITE);
		CloseHandle(HThreadAsync);
	#elif defined __GURULINUX__
		sem_post(&ejecutando);
		pthread_join(HThreadAsync, NULL);
	#endif
	};

	void init() {
	#ifdef __GURUWIN32__
		hescribir=CreateSemaphore(NULL, 0, 1000000, NULL);
		ejecutando=CreateEvent( NULL, FALSE,  FALSE,  NULL);

		HThreadAsync = CreateThread(NULL, 0, ThreadAsync, static_cast<void*>(this), 0, NULL);
	#elif defined __GURULINUX__
		sem_init(&hescribir, 0, 0);
		sem_init(&ejecutando, 0, 0);

		pthread_create(&HThreadAsync, NULL, ThreadAsync, (void*)this);
	#endif
	}

	virtual void Async(const Y& data) {

		cola.push(data);

	#ifdef __GURUWIN32__
		ReleaseSemaphore(hescribir, 1, NULL);
	#elif defined __GURULINUX__
		sem_post(&hescribir);
	#endif
	}

#ifdef __GURUWIN32__
	static DWORD WINAPI ThreadAsync(LPVOID llamador) {
#elif defined __GURULINUX__
	static void* ThreadAsync(void* llamador) {
#endif
		GuruAsync*	esto = static_cast<GuruAsync*>(llamador);
	#ifdef __GURUWIN32__
		HANDLE eventos[2];
	#elif defined __GURULINUX__
   		struct timeval espera; espera.tv_sec = 1; espera.tv_usec = 250000;
		int valor_escritura, valor_ejecutando;
		int esperas=0;
	#endif

		do {
		#ifdef __GURUWIN32__
			eventos[0] = esto->ejecutando;
			eventos[1] = esto->hescribir;
			if (WaitForMultipleObjects(2, eventos, FALSE, INFINITE) == WAIT_OBJECT_0) break;
		#elif defined __GURULINUX__
			valor_escritura=0; valor_ejecutando=0;
			while (sem_getvalue(&esto->ejecutando, &valor_escritura) == 0 && sem_getvalue(&esto->ejecutando, &valor_ejecutando) == 0) {
				if (valor_escritura || valor_ejecutando) break;
				
				espera.tv_sec = 0; espera.tv_usec = 250000;
      			select(0, 0, 0, 0, &espera);
			}

			if (valor_ejecutando) break;
		#endif

			Y& data = esto->cola.front();

			bool ok = ((esto->Clase)->*esto->Metodo)(data);

			// si devuelve false, el elemento es colocado nuevamente en el cola.
			if (!ok) esto->Async(data);

			esto->cola.pop();

		} while(8);

	#ifdef __GURUWIN32__
		CloseHandle(esto->hescribir);
		CloseHandle(esto->ejecutando);
		_endthreadex(0);
	#elif defined __GURULINUX__
		sem_destroy(&esto->ejecutando);
		sem_destroy(&esto->hescribir);
		pthread_exit(0);
	#endif

		return 0;
	}
};

#endif
