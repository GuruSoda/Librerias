#ifndef __GURUSINGLETON_H__
#define __GURUSINGLETON_H__

template <class T>
class GuruSingleton 
{
public:
	static T* instancia(){
		return singleton ? singleton : (singleton = new T);
	}

protected:
	GuruSingleton(void) {} ;
	virtual ~GuruSingleton(void) {};

	static T* singleton;
};

template <class T>
T* GuruSingleton<T>::singleton = NULL;

template <class T>
class GuruSingletonEx 
{
public:
	static T& instancia() {
		static T unico;
		return unico;
	}

protected:
	GuruSingletonEx(void) {} ;
	virtual ~GuruSingletonEx(void) {};
};

#endif
