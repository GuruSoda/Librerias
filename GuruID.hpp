#ifndef __GURUID_H__
#define __GURUID_H__

#include <GuruCompat.h>
#include <map>

#include <GuruAcceso.hpp>

using namespace std;

template<typename T>
class GuruID {
public:
	GuruID(void):_id(0) {};
	virtual ~GuruID(void) {};

	unsigned long id (const T& valor) {
		typename map<T, unsigned long>::iterator it(cache.find(valor));

		if (it != cache.end()) 
			return it->second;
		else 
			return insertar(valor);
	};

	T valor (unsigned long id) {
		typename map<unsigned long, typename map<T, unsigned long>::iterator>::iterator it(reverse_cache.find(id));

		if (it != reverse_cache.end())
			return it->second->first;
		
		return T();
	}

	bool existe (const T& valor) {
		typename map<T, unsigned long>::iterator it;

		if ((it=cache.find(valor)) != cache.end())
			return true;
		else
			return false;
	};

	map<T, unsigned long>& src () {return cache;};
	map<unsigned long, typename map<T, unsigned long>::iterator>& reverse_src() { return reverse_cache; };

protected:
	virtual unsigned long insertar(const T& valor) {
		pair<typename map<T, unsigned long>::iterator, bool> ret(cache.insert(make_pair(valor, ++_id)));

		if (ret.second == false) return id(valor);

		reverse_cache.insert(make_pair(_id, ret.first));

		return _id;
	}

	map<T, unsigned long> cache;
	map<unsigned long, typename map<T, unsigned long>::iterator> reverse_cache;

	unsigned long _id;
};

template<typename T>
class GuruID_threadsafe : public GuruID<T> {
	GuruBloqueo acceso;
public:
	GuruID_threadsafe() :GuruID<T>() {}

	virtual unsigned long insertar(const T& valor) {
		acceso.bloquear();
		unsigned long tmp_id = GuruID<T>::insertar(valor);
		acceso.desbloquear();

		return tmp_id;
	}
};

typedef GuruID<uint8_t>  GuruID8;
typedef GuruID<uint16_t> GuruID16;
typedef GuruID<uint32_t> GuruID32;
typedef GuruID<uint64_t> GuruID64;

#endif
