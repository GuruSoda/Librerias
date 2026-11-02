#ifndef __GURUBUFFER2_H__
#define __GURUBUFFER2_H__

#include <GuruCompat.h>

#include <string.h>
#include <cstdlib>

#if defined __GURUWIN32__
#elif defined __GURULINUX__
#include <stdint.h>
#endif

template <typename T>
class basic_GuruBuffer
{
public:
	basic_GuruBuffer() {limpiar();};
	basic_GuruBuffer(size_t bytes) {limpiar(); reservar(bytes);};
	virtual ~basic_GuruBuffer(){
		liberar();
	}

	basic_GuruBuffer<T>&	reservar(size_t total) {
		liberar();

//		ptr_inicio_buffer = new T[total];
		ptr_inicio_buffer = static_cast<T*>(std::calloc(total, sizeof(T)));

		if (ptr_inicio_buffer == NULL) return *this;

		tam_buffer=total;

		ptr_fin_buffer=(ptr_inicio_buffer+tam_buffer);
		ptr_inicio_data = ptr_fin_data = ptr_inicio_buffer;

		return *this;
	}

	void liberar(){
		if (ptr_inicio_buffer != NULL) 
		//	delete [] ptr_inicio_buffer;
			std::free(ptr_inicio_buffer);

		ptr_inicio_buffer=NULL;

		tam_buffer=0;
		tam_data=0;
	}

	void clear_data() {
		tam_data=0;
		ptr_fin_data = ptr_inicio_data = ptr_inicio_buffer;

	}

	inline T*			buf() {return ptr_inicio_buffer;};
	inline const T*		const_buf() {return ptr_inicio_buffer;};
	inline uint64_t		cantidad_data() {return tam_data;};
	inline void			cantidad_data(uint64_t t) {tam_data=t;}
	inline uint64_t		cantidad_buf() {return tam_buffer;};
	inline const T*		begin_data() { return ptr_inicio_data; };
	inline const T*		end_data() { return ptr_fin_data; };

	basic_GuruBuffer<T>&	asignar(const T* buf, uint64_t tam){
		if (tam < tam_buffer && ptr_inicio_buffer != NULL) {
			memcpy( ptr_inicio_buffer, buf, static_cast<size_t>(tam));

			tam_data=tam;

			ptr_fin_data=(ptr_inicio_buffer+tam_data);
			ptr_inicio_data=ptr_inicio_buffer;

		}

		return *this;
	}

	basic_GuruBuffer<T>&	asignar(const T dato, size_t lugar){
		if (lugar < tam_buffer && ptr_inicio_buffer != NULL) {
			ptr_inicio_buffer[lugar] = dato;
		}

		return *this;
	}

	basic_GuruBuffer<T>& agregar(const T* dato, size_t total) {

		if (ptr_inicio_buffer == NULL) reservar(total + 2);

		if ((tam_data + total) <= tam_buffer) {
			memcpy(ptr_fin_data, dato, static_cast<size_t>(total));
			tam_data += total;
			ptr_fin_data += total;
		} else {
			ptr_inicio_buffer = (T*)realloc(static_cast<void*>(ptr_inicio_buffer), static_cast<size_t>(tam_data) + total);
			
			ptr_inicio_data = ptr_inicio_buffer;
			ptr_fin_data = ptr_inicio_data;
			ptr_fin_data += tam_data;

			memcpy(ptr_fin_data, dato, static_cast<size_t>(total));

			ptr_fin_data += total;

			tam_data += total;
			tam_buffer += total;

			ptr_fin_buffer = (ptr_inicio_buffer + tam_buffer);
		}

		return *this;
	}

	friend basic_GuruBuffer<T>& operator << (basic_GuruBuffer<T>& buffer, const T& dato) {
		if (!buffer.ptr_inicio_data && !buffer.ptr_fin_data) {
			buffer.ptr_inicio_data = buffer.ptr_fin_data = buffer.ptr_inicio_buffer;
		} else if (buffer.tam_data) buffer.ptr_fin_data++;

		memcpy(buffer.ptr_fin_data, const_cast<T*>(&dato), sizeof(T));
//		buffer.ptr_fin_data++;

		buffer.tam_data++;

		return buffer;
	}

	class iterator {
	public:
		iterator(basic_GuruBuffer<T>& esto, bool alfinal):esto(esto) {
			if (alfinal) {
				ptr_actual = esto.ptr_fin_data;
				ptr_inicio = esto.ptr_inicio_data;
				ptr_fin    = esto.ptr_fin_data;
				posicion_actual=esto.tam_data;
			} else {
				ptr_actual	= esto.ptr_inicio_data;
				ptr_inicio	= esto.ptr_inicio_data;
				ptr_fin		= esto.ptr_fin_data;
				posicion_actual=0;
			}

			
		}
		virtual ~iterator() {}

		bool operator != (const iterator& it) {
			if (this->posicion_actual != it.posicion_actual) return true;
			else return false;
		}

		void operator++ () {
			if (posicion_actual < esto.tam_data) {ptr_actual++;posicion_actual++;}
		}

		void operator++ (int) {
			if (posicion_actual < esto.tam_data) {ptr_actual++;posicion_actual++;}
		}

		void operator-- () {
			if (posicion_actual > 0) {ptr_actual--;posicion_actual--;}
		}

		void operator-- (int) {
			if (posicion_actual > 0) {ptr_actual--;posicion_actual--;}
		}

		T operator* () { return *ptr_actual;}

		iterator& operator << (T& dato) {
			ptr_fin++;
			ptr_fin = dato;
			esto.tam_data+=sizeof(T);

			return *this;
		}

	protected:
		basic_GuruBuffer<T>& esto;

		T*			ptr_actual;			// puntero al byte actual de extraccion
		T*			ptr_inicio;			// puntero al inicio del buffer
		T*			ptr_fin;			// puntero al final del buffer
		::uint64_t	posicion_actual;	// posicion en bytes dentro del buffer
	};

	iterator begin() {return iterator(*this, false);};
	iterator end()   {return iterator(*this, true);};

protected:
	void limpiar() {
		ptr_inicio_buffer=NULL;
		ptr_fin_buffer=NULL;
		ptr_inicio_data=NULL;
		ptr_fin_data=NULL;
		tam_buffer=0;
		tam_data=0;
	};

	T*			ptr_inicio_buffer;
	T*			ptr_fin_buffer;

	T*			ptr_inicio_data;
	T*			ptr_fin_data;

	uint64_t	tam_buffer;
	uint64_t	tam_data;
};

typedef basic_GuruBuffer<uint8_t>	GuruBuffer;
typedef basic_GuruBuffer<char>		GuruBufferChar;
typedef basic_GuruBuffer<wchar_t>	GuruBufferWChar;

///////////////////////////////////////////////////////////////////////
// Ejemplo de como utilizar iterator de GuruBuffer
///////////////////////////////////////////////////////////////////////
/*
GuruBuffer buf;
char* info = "Soda Stereo";

buf.reservar(120);
buf.asignar(reinterpret_cast<uint8_t*>(info), strlen("Soda Stereo"));

for (GuruBuffer::iterator it=buf.begin();it!=buf.end();++it) {
	cout << *it;
}

cout << endl;

basic_GuruBuffer<string> buf2;

buf2.reservar(15);

for (basic_GuruBuffer<string>::iterator it2=buf2.begin();it2!=buf2.end();++it2) {
	cout << *it2;
}
*/

#endif
