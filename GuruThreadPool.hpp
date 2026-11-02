#ifndef __GURUTHREADPOOL_HPP__
#define __GURUTHREADPOOL_HPP__

#include <GuruCompat.h>
#include <GuruAcceso.hpp>

#include <queue>

template <typename T, typename Y>
class GuruThreadPool {
public:
	GuruThreadPool(const size_t capacidad=0):capacidad(capacidad) {}

	typedef T objeto_type;
	typedef Y item_type;

	typedef typename queue<item_type> cola;

	typedef enum {libre, ocupado} estado;

	typedef struct {
		estado _estado;
	#ifdef __GURUWIN32__
		unsigned	tid;
		uintptr_t	h_bt;
	#elif defined __GURULINUX__
		pthread_t	tid;
		int			h_bt; 
	#endif
	} item_pool;

	typedef typename std::list<item_pool> pool;

	void inicializar() {
		for (size_t i=0;i<capacidad;i++) {
			item_pool _item;
		#ifdef __GURUWIN32__
			_item.h_bt = _beginthreadex(NULL, 0, recibe_conexiones, static_cast<void*>(this), 0, &_item.tid);
		#elif defined __GURULINUX__
			_item.h_bt = pthread_create(&_item.tid, NULL, recibe_conexiones, (void*)this);
		#endif

			_pool.push_back(_item);
		}
	}

	item_type sacar_item() {
		acceso_cola.bloquear();
		item_type item = _cola.front();
		_cola.pop();
		acceso_cola.desbloquear();
	}

	static void iniciar_thread (void* param) {

		GuruThreadPool* esto = static_cast<GuruThreadPool*>(param);

		item_type = esto->sacar_item();
	}

	void agregar(item_type& item) {
		acceso_cola.bloquear();
		_cola.push(item);
		acceso_cola.desbloquear();
	}

private:
	cola	_cola;
	pool	_pool;
	size_t	capacidad;

	GuruBloqueo		acceso_cola;
};

#endif
