#if !defined __GURUQUEUE_HPP__
#define __GURUQUEUE_HPP__

#include <queue>
#include <GuruAcceso.hpp>

template <typename T>
class GuruQueue_threadsafe:public std::queue<T> {
public:
	GuruQueue_threadsafe():std::queue<T>() {};

	void push(const T& val) {
		acceso.bloquear();
		std::queue<T>::push(val);
		acceso.desbloquear();
	};

	typename std::queue<T>::reference front() {
		acceso.bloquear();
		typename std::queue<T>::reference tmp = std::queue<T>::front();
		acceso.desbloquear();
		return tmp;
	}

	void pop() {
		acceso.bloquear();
		std::queue<T>::pop();
		acceso.desbloquear();
	}

private:
	GuruBloqueo acceso;
};

#endif
