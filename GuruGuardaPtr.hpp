#ifndef __GURUGUARDAPTR_HPP__
#define __GURUGUARDAPTR_HPP__

#include <map>
#include <string>

template<typename K, typename V>
class GuruGuardaPtr {
public:
	virtual ~GuruGuardaPtr<K,V>(void) {};

	typedef K key_type;
	typedef V value_type;

	static GuruGuardaPtr<key_type, value_type>& instancia() {
		static GuruGuardaPtr<key_type, value_type> unico;
		return unico;
	}

	static value_type* ptr(const key_type& nombre) {
		GuruGuardaPtr<key_type, value_type>& _instancia = GuruGuardaPtr<key_type, value_type>::instancia();

		typename std::map<key_type, value_type*>::iterator lugar(_instancia._ptr.find(nombre));

		if (lugar == _instancia._ptr.end())
			return NULL;
		else
			return lugar->second;
	}

	inline static void ptr(const key_type& nombre, value_type* ptr) {
		GuruGuardaPtr<key_type, value_type>::instancia().ptr().insert(std::make_pair(nombre, ptr));
	}

	std::map<key_type, value_type*>& ptr() { return _ptr; }

private:
	GuruGuardaPtr(void) {};

	std::map<key_type, value_type*> _ptr;
};

typedef GuruGuardaPtr<std::string, void> GuruGuardaPtrStr;

#endif
