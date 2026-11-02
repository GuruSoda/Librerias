#ifndef __GURUBUFFERALGO_HPP__
#define __GURUBUFFERALGO_HPP__

#include <string>
#include <fstream>

#include <GuruDirectorio.hpp>
#include <GuruBuffer.hpp>
#include <GuruString_algo.hpp>

namespace GuruBufferAlgo {
	template <typename T>
	T& archivo_buffer(const std::string& archivo, T& buffer) {

		buffer.clear_data();

		GuruDirectorio::GuruArchivo prop(GuruDirectorio::propiedades(archivo));

		if (!prop.Existe()) return buffer;

		ifstream arch(archivo, std::ios::in | std::ios::binary);

		if (!arch.is_open()) return buffer;

		buffer.reservar(prop.tamanio() + 2);

		if (arch.read(buffer.buf(), prop.tamanio()))
			buffer.cantidad_data(prop.tamanio());

		if (arch.good())
			buffer.cantidad_data(prop.tamanio());

		arch.close();

		return buffer;
	}

	template <typename T>
	T& buffer_archivo(T& buffer, const std::string& archivo) {

		ofstream arch(archivo.c_str(), std::ios::out | std::ios::binary);

		if (!arch.is_open()) return buffer;

		if (!arch.write(buffer.buf(), buffer.cantidad_data()))
			return buffer;

		arch.close();

		return buffer;
	}

	template <typename T>
	T* buffer_archivo(T* buffer, const std::string& archivo, size_t tam) {

		ofstream arch(archivo, std::ios::out | std::ios::binary);

		if (!arch.is_open()) return buffer;

		if (!arch.write(buffer, tam))
			return buffer;

		arch.close();

		return buffer;
	}

	class GuruBufferCharStream:public GuruBufferChar {
	public:
		GuruBufferCharStream() {}
		GuruBufferCharStream(size_t tam) { reservar(tam); }
		virtual ~GuruBufferCharStream() {}

		GuruBufferCharStream& operator << (const std::string& dato) {

			agregar(dato.c_str(), dato.length());

			return *this;
		}

		GuruBufferCharStream& operator << (const char* dato) {

			agregar(dato, strlen(dato));

			return *this;
		}

		GuruBufferCharStream& operator << (uint64_t dato) {

			operator<<(GuruStringAlgo::int64astr(dato));

			return *this;
		}

		void FinChar() { *ptr_fin_data = '\0'; ptr_fin_data++; }

		const char* BufferChar() { return buf(); }
	};
} 

#endif
