#ifndef __GURUCACHE_HPP__
#define __GURUCACHE_HPP__

//////////////////////////////////////////////////////////////////////
// GuruCacheLRU:
// Cache basado en la estrategia de reemplazo LRU (least recently used o menos utilizado recientemente)
// Si el parametro capacidad del contructor es 0, es un cache sin algoritmo de limpieza.
// Basado en el siguiente documento:
// http://timday.bitbucket.org/lru.html
// Nota:
//	Tambien existen otros tipos de algoritmos de cache como:
//	MRU: Usado más recientemente
//	LFU: Menos frecuentemente usados
//////////////////////////////////////////////////////////////////////
#include <map>
#include <list>

#include <GuruAcceso.hpp>

using namespace std;

class GuruCacheException: public std::exception 
{
public:
	GuruCacheException() {};
	virtual ~GuruCacheException() throw() {};
};

template <typename K, typename V, typename Compare = std::less<K> >
class GuruCacheLRU {
public:
	GuruCacheLRU(const size_t capacidad=0):capacidad(capacidad) {};

	typedef K key_type;
	typedef V value_type;

	typedef std::list<key_type> llaves;
	typedef std::map<key_type, std::pair<value_type, typename llaves::iterator>, Compare > cache;
	typedef typename std::map<key_type, std::pair<value_type, typename llaves::iterator> >::iterator iterator;

	iterator	begin()		{ return _cache.begin();}
	iterator	end()		{ return _cache.end();}
	size_t		size()		{ return _cache.size();}
	void		clear()		{ _llaves.clear();_cache.clear();}

	cache& data() {return _cache;}

	void insertar(const key_type& k, const value_type& v) {
		if (capacidad != 0 && _cache.size() == capacidad) eliminar();

		typename llaves::iterator it = _llaves.insert(_llaves.end(), k);

		if (it == _llaves.end()) return ;

		if (_cache.insert(std::make_pair(k, std::make_pair(v, it))).second == false) _llaves.erase(it);
	}

	template <typename functor_eliminar>
	void insertar(const key_type& k, const value_type& v, functor_eliminar& func_eliminar) {

		if (capacidad != 0 && _cache.size() == capacidad) eliminar(func_eliminar);

		typename llaves::iterator it = _llaves.insert(_llaves.end(), k);

		if (it == _llaves.end()) return ;

		if (_cache.insert(std::make_pair(k, std::make_pair(v, it))).second == false) _llaves.erase(it);
	}

	value_type& operator () (const key_type& k) {

		typename cache::iterator it = _cache.find(k);

		if (it == _cache.end()) throw GuruCacheException();

		_llaves.splice(_llaves.end(), _llaves, (*it).second.second);

		return (*it).second.first;
	}

protected:
	void eliminar() {

		if (_cache.empty()) return ;

		typename cache::iterator it = _cache.find(_llaves.front());

		_cache.erase(it);
		_llaves.pop_front();
	}

	template <typename functor_eliminar>
	void eliminar(functor_eliminar functor) {

		if (_cache.empty()) return ;

		typename cache::iterator it = _cache.find(_llaves.front());

		functor(it->first, it->second.first);

		_cache.erase(it);
		_llaves.pop_front();
	}

	llaves	_llaves;
	cache	_cache;

	size_t capacidad;	
};

template <typename K, typename V, typename Compare = std::less<K> >
class GuruCacheLFU {
public:
	GuruCacheLFU(const size_t capacidad=0):capacidad(capacidad) {};

	typedef K key_type;
	typedef V value_type;

	typedef std::list<pair<key_type, ::uint32_t> > llaves;
	typedef std::map<key_type, std::pair<value_type, typename llaves::iterator>, Compare> cache;

	struct menorFrecuencia {
		bool operator()(const pair<key_type, ::uint32_t>& a, const pair<key_type, ::uint32_t>& b) {   
			return a.second < b.second;
        }   
    };

	cache& data() {return _cache;}

	size_t	size()	{ return _cache.size();}
	void	clear() { _llaves.clear();_cache.clear();}

	void insertar(const key_type& k, const value_type& v) {
		if (capacidad != 0 && _cache.size() == capacidad) eliminar();

		typename llaves::iterator it = _llaves.insert(_llaves.end(), make_pair(k, static_cast<const ::uint32_t>(0)));

		if (it == _llaves.end()) return ;

		if (_cache.insert(std::make_pair(k, std::make_pair(v, it))).second == false) _llaves.erase(it);
	}

	template <typename functor_eliminar>
	void insertar(const key_type& k, const value_type& v, functor_eliminar& func) {
		if (capacidad != 0 && _cache.size() == capacidad) eliminar(func);

		typename llaves::iterator it = _llaves.insert(_llaves.end(), make_pair(k, static_cast<uint32_t>(0)));

		if (it == _llaves.end()) return ;

		if (_cache.insert(std::make_pair(k, std::make_pair(v, it))).second == false) _llaves.erase(it);
	}

	value_type& operator () (const key_type& k) {

		typename cache::iterator it = _cache.find(k);

		if (it == _cache.end()) throw GuruCacheException();

		it->second.second->second++;

		return (*it).second.first;
	}

protected:
	void eliminar() {

		if (_cache.empty()) return ;

		_llaves.sort(menorFrecuencia());

		typename cache::iterator it = _cache.find(_llaves.front().first);

		_cache.erase(it);
		_llaves.pop_front();
	}

	template <typename functor_eliminar>
	void eliminar(functor_eliminar& functor) {

		if (_cache.empty()) return ;

		_llaves.sort(menorFrecuencia());

		typename cache::iterator it = _cache.find(_llaves.front().first);

		functor(it->first, it->second.first);

		_cache.erase(it);
		_llaves.pop_front();
	}

	llaves	_llaves;
	cache	_cache;

	size_t capacidad;	
};

template <typename K, typename V, typename Compare = std::less<K> >
class GuruCacheLRUEX: public GuruCacheLRU<K,V,Compare> {
public:
	GuruCacheLRUEX(const size_t capacidad=0):GuruCacheLRU<K, V, Compare>(capacidad) {};

	void clear() {
		bloqueo.bloquear();
		GuruCacheLRU<K,V,Compare>::clear();
		bloqueo.desbloquear();
	}


	void insertar(const typename GuruCacheLRU<K,V,Compare>::key_type& k, const typename GuruCacheLRU<K,V,Compare>::value_type& v) {
		bloqueo.bloquear();
		GuruCacheLRU<K,V,Compare>::insertar(k,v);
		bloqueo.desbloquear();
	}

	template <typename functor_eliminar>
	void insertar(const typename GuruCacheLRU<K,V,Compare>::key_type& k, const typename GuruCacheLRU<K,V,Compare>::value_type& v, functor_eliminar& functor) {
		bloqueo.bloquear();
		GuruCacheLRU<K,V,Compare>::insertar(k,v, functor);
		bloqueo.desbloquear();
	}

	typename GuruCacheLRU<K,V,Compare>::value_type& operator () (const typename GuruCacheLRU<K,V,Compare>::key_type& k) {
		bloqueo.bloquear();
		try {
			typename GuruCacheLRU<K,V,Compare>::value_type& a(GuruCacheLRU<K,V,Compare>::operator()(k));
			bloqueo.desbloquear();
			return a;
		} catch (GuruCacheException&) {
			bloqueo.desbloquear();
			throw ;
		}
	}


protected:
	GuruBloqueo bloqueo;
};

template <typename K, typename V, typename Compare = std::less<K> >
class GuruCacheLFUEX: public GuruCacheLFU<K,V,Compare> {
public:
	GuruCacheLFUEX(const size_t capacidad=0):GuruCacheLFU<K,V,Compare>(capacidad) {};

	void clear() {
		bloqueo.bloquear();
		GuruCacheLFU<K,V,Compare>::clear();
		bloqueo.desbloquear();
	}

	void insertar(const typename GuruCacheLFU<K,V,Compare>::key_type& k, const typename GuruCacheLFU<K,V,Compare>::value_type& v) {
		bloqueo.bloquear();
		GuruCacheLFU<K,V,Compare>::insertar(k,v);
		bloqueo.desbloquear();
	}

	typename GuruCacheLFU<K,V,Compare>::value_type& operator () (const typename GuruCacheLFU<K,V,Compare>::key_type& k) {
		bloqueo.bloquear();
		try {
			typename GuruCacheLFU<K,V,Compare>::value_type& a(GuruCacheLFU<K,V,Compare>::operator()(k));
			bloqueo.desbloquear();
			return a;
		} catch (GuruCacheException&) {
			bloqueo.desbloquear();
			throw ;
		}
	}

	size_t size() {
		bloqueo.bloquear();
		size_t a=GuruCacheLFU<K,V,Compare>::_cache.size();
		bloqueo.desbloquear();
		return a;
	}

protected:
	GuruBloqueo bloqueo;
};

#endif
