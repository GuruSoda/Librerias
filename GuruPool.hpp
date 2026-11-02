#ifndef __GURUPOOL_HPP__
#define __GURUPOOL_HPP__

#include <list>

#include <GuruAcceso.hpp>

template <typename T>
class GuruPool {
public:
	GuruPool(const size_t capacidad=0):_capacidad(capacidad) {};
	virtual ~GuruPool() {
		vaciar();
	};

	typedef T item_pool;
	typedef enum {tomado, libre} estado;

	typedef std::list<pair<item_pool*, estado> > pool;
	typedef typename pool::iterator iterator;

	iterator	begin()		{ return _pool.begin();}
	iterator	end()		{ return _pool.end();}
	size_t		size()		{ return _pool.size();}
	size_t		capacidad()	{ return _capacidad;}
	void 		capacidad(size_t a)	{ _capacidad=a;}

	size_t		ocupado() {
		size_t total = 0;
		for (iterator it = _pool.begin(); it != _pool.end(); ++it) if (it->second == tomado) total++;

		return total;
	}

	virtual void agregar(item_pool* item) {
		_pool.push_back(make_pair(item, libre));
	};

	virtual item_pool*	obtener() {
		for (iterator it=_pool.begin();it!=_pool.end();++it) {
			if (it->second == libre) {
				it->second = tomado;
				_pool.splice(_pool.end(), _pool, it);
				return it->first;
			}
		}

		if (_capacidad != 0 && _pool.size() == _capacidad) return NULL;

		item_pool* item = new item_pool;
		if (item) _pool.push_back(make_pair(item, tomado));
		return item;
	};

	virtual void liberar(item_pool* item) {

		if (!item) return ;

		for (iterator it = _pool.end(), itend=_pool.begin(); it != itend ; ) {
			--it;
			if (it->first == item) {
				it->second = libre;
				_pool.splice(_pool.begin(), _pool, it);
				break;
			}
		}
	};

	virtual void vaciar () {
		for (iterator it=_pool.begin();it!=_pool.end();++it) {
			delete it->first;
		}

		_pool.clear();
	};

private:
	size_t	_capacidad;
	pool	_pool;
};

template <typename T>
class GuruPool_threadsafe:public GuruPool<T> {
public:
	GuruPool_threadsafe(const size_t capacidad=0):GuruPool<T>(capacidad) {};

	size_t		capacidad()	{ return GuruPool<T>::capacidad();}

	size_t		ocupado() {
		acceso.bloquear();
		size_t total = GuruPool<T>::ocupado();
		acceso.desbloquear();

		return total;
	}
	void capacidad(size_t a)	{ 
		acceso.bloquear();
		GuruPool<T>::capacidad(a);
		acceso.desbloquear();
	}

	void agregar(typename GuruPool<T>::item_pool* item) {
		acceso.bloquear();
		GuruPool<T>::agregar(item);
		acceso.desbloquear();
	};

	typename GuruPool<T>::item_pool* obtener() {
		acceso.bloquear();
		typename GuruPool<T>::item_pool* tmp = GuruPool<T>::obtener();
		acceso.desbloquear();

		return tmp;
	};

	void liberar(typename GuruPool<T>::item_pool* item) {
		acceso.bloquear();
		GuruPool<T>::liberar(item);
		acceso.desbloquear();
	};

	void vaciar() {
		acceso.bloquear();
		GuruPool<T>::vaciar();
		acceso.desbloquear();
	};

private:
	GuruBloqueo acceso;
};

#endif
