#ifndef __GURUMARCA_HPP__
#define __GURUMARCA_HPP__

#include <fstream>
#include <string>

#include <GuruError.hpp>

//////////////////////////////////////////////////////////////////////
// Ejemplo de uso:
// 1)
//	GuruMarca a("form.html", "salidahtml.html", "<?Guru", "?>");
//
//	for (GuruMarca::iterator it=a.begin();it!=a.end();++it) {
//		cout << (*it).marca_encontrada() << endl;
//		a << "<!-- -->";
//	}
/////////////////////////////////////
// 2)
//	a.abrir_archivos();
//
//	while (a.sacar_marca() != GuruMarca::fin_archivo) {
//		cout << a.marca_encontrada() << endl;
//	}
//////////////////////////////////////////////////////////////////////
namespace GuruBuscaMarca {

	enum opciones {antes, despues, salida};

	template <typename T>
	bool ir_hasta_marca(istream& entrada, ostream& salida, const T& marca, opciones opt=GuruBuscaMarca::antes | !GuruBuscaMarca::salida) {

		ios::int_type	caracter;
		ios::pos_type	posicion_inicial=entrada.tellg();

		bool opt_salida=(opt & GuruBuscaMarca::salida) ? true : false;
		bool opt_despues=(opt & GuruBuscaMarca::despues) ? true : false;

		while (!entrada.eof()) {

			if (entrada.peek() != EOF)
				caracter = entrada.get();
			else 
				break;

			if (entrada.eof()) break;

			if (caracter == *marca.begin()) {
				string::const_iterator it;
				::uint32_t lugar=1;

				for (it=marca.begin();it!=marca.end();++it,lugar++) {
					if (*it!=caracter) break;

					if (entrada.peek() != EOF)
						caracter = entrada.get();
					else 
						break;
				}

				if (it == marca.end()) { // encontre la cadena
					entrada.unget();

					if (opt_despues) {
						if (opt_salida)
							salida << marca;
					} else
						entrada.seekg(static_cast<std::ios::off_type>(static_cast<const ::int32_t>(0)-static_cast<const ::int32_t>(marca.length())), std::ios_base::cur);

					return true;
				} else { // no es la cadena, retrocedo hasta el primero encontrado.
					entrada.seekg(static_cast<std::ios::off_type>(static_cast<const ::int32_t>(0)-static_cast<const ::int32_t>(lugar)), std::ios_base::cur);
					while (lugar>0) {
						if (entrada.peek() != EOF)
							caracter = entrada.get();
						else 
							break;

						if (opt_salida)
							salida.put(caracter);
						lugar--;
					}
				}

			} else {
				if (opt_salida)
					salida.put(caracter);
			}
		}

		ios::pos_type posicion_actual=entrada.tellg();

		if (entrada.peek() ==  EOF) {
			entrada.unget();
			entrada.seekg(posicion_inicial, std::ios_base::beg);
			posicion_actual=entrada.tellg();
			return false;
		}

		if (posicion_inicial < posicion_actual) {
			entrada.seekg(static_cast<std::ios::off_type>(static_cast<const ::int32_t>(posicion_actual)-static_cast<const ::int32_t>(posicion_inicial)), std::ios_base::cur);
			posicion_actual=entrada.tellg();

		}

		return false;
	}
}

class GuruMarca {
public:
	GuruMarca(istream& entrada, ostream& salida, const string& inicio, const string& fin):entrada(entrada),salida(salida), str_marca_inicio(inicio), str_marca_fin(fin), estado(desconocido) {};
	virtual ~GuruMarca() {};

	enum tag {marca, marca_inicio, marca_fin, fin_archivo, desconocido};

	enum GuruMarca::tag sacar_data() {
		enum tag	estado;
		if ((estado=sacar_hasta_marca_inicio()) != marca_inicio) return estado;

		if ((estado=sacar_hasta_fin_marca()) != marca_fin) return estado;

		return marca;
	}

	enum GuruMarca::tag sacar_hasta_marca_inicio() {

		estado=fin_archivo;

		while (!entrada.eof()) {
			sacar_caracter();

			if (entrada.eof()) break;

			if (caracter == *str_marca_inicio.begin()) {
				string::const_iterator it;
				int lugar=1;

				for (it=str_marca_inicio.begin();it!=str_marca_inicio.end();++it,lugar++) {
					if (*it!=caracter) break;

					sacar_caracter();
				}

				if (it == str_marca_inicio.end()) { // encontre la cadena
					poner_caracter();
					return marca_inicio;
				} else { // no es la cadena, retrocedo hasta el primero encontrado.
					lugar=-lugar;
					entrada.seekg(lugar, std::ios_base::cur);
					while (lugar<=0) {
						salida.put(sacar_caracter());
						lugar++;
					}
				}

			} else salida.put(caracter);
		}

		return estado;
	};

	enum GuruMarca::tag sacar_hasta_fin_marca() {
		buffer.clear();

		while (!entrada.eof()) {
			sacar_caracter();
			buffer+=caracter;

			if (entrada.eof()) break;

			if (caracter == *str_marca_fin.begin()) {
				string::const_iterator it;
				int lugar=1;

				for (it=str_marca_fin.begin();it!=str_marca_fin.end();++it,lugar++) {
					if (*it!=caracter) break;

					sacar_caracter();
				}

				if (it == str_marca_fin.end()) { // encontre la cadena
					poner_caracter();
					buffer.erase(buffer.end()-1); // tengo el primer caracter del fin de marca.
					return marca_fin;
					break;
				} else { // no es la cadena, retrocedo hasta el primero encontrado.
					lugar=-lugar;
					entrada.seekg(lugar, std::ios_base::cur);
				}

			}
		}

		if (entrada.eof()) return fin_archivo;
		else return desconocido;
	}

	int sacar_caracter() {

		if (entrada.peek() != EOF) caracter = entrada.get();

		switch (caracter) {
		case EOF:   break;
		}
		return caracter;
	};

	int poner_caracter() {
		entrada.unget();
		entrada.unget();
		caracter = entrada.get();

		return caracter;
	};

	string marca_encontrada() {
		return buffer;
	}

	enum tag estado_marca() {
		return estado;
	}

	GuruMarca& operator << (const string& datos) {
		salida << datos;

		return *this;
	}
/*
	class iterator {
	public:
		iterator(GuruMarca& esto, enum tag estado=marca):esto(esto),estado(estado) {

			if (estado==marca) {
				// esto.abrir_archivos();
				estado = esto.sacar_marca();
			}
		};
		virtual ~iterator() {};

		bool operator != (iterator& it) { 
			if (it.esto.estado_marca() != estado) return false;
			else return true;
		}

		void operator++() {
			esto.sacar_marca();
		}

		GuruMarca& operator*() {
			return esto;
		}

		GuruMarca& operator->() {
			return esto;
		}

	protected:
		GuruMarca& esto;
		enum tag estado;
	};

	iterator begin() {return iterator(*this);};
	iterator end() {return iterator(*this, fin_archivo);};
*/
protected:
	istream&		entrada;
	ostream&		salida;

	string		str_marca_inicio;
	string		str_marca_fin;

	int			caracter;
	string		buffer;

	enum tag	estado;
};

#endif
