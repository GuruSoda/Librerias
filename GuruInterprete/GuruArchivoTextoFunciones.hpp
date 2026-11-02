#ifndef __GURUARCHIVOTXTFUNCIONES_HPP__
#define __GURUARCHIVOTXTFUNCIONES_HPP__

#include <GuruInterprete/GuruInterprete.hpp>
#include <GuruBuffer.hpp>
#include <fstream>
#include <stdexcept>
#include <cstdlib>

using namespace std;

class GuruArchivoTextoFunciones: public GuruInterprete::GuruFunciones<GuruArchivoTextoFunciones> {
	fstream		archivo;
	string 		msj_error;
	GuruBuffer	buffer;
public:
	GuruArchivoTextoFunciones () {
		cargar_funcion(this, &GuruArchivoTextoFunciones::abrirl, "txt.openr");
		cargar_funcion(this, &GuruArchivoTextoFunciones::abrire, "txt.openw");
		cargar_funcion(this, &GuruArchivoTextoFunciones::cerrar, "txt.close");
		cargar_funcion(this, &GuruArchivoTextoFunciones::leer, "txt.read");
		cargar_funcion(this, &GuruArchivoTextoFunciones::escribir, "txt.write");
		cargar_funcion(this, &GuruArchivoTextoFunciones::error, "txt.error");
	};

	virtual ~GuruArchivoTextoFunciones() {};

	string abrirl(const vector<string>& param) {
		string nombre;
		string opciones;

		try {
			for (unsigned int i=0;i<param.size();i++) {
				switch (i) {
				case 0: nombre=param.at(i); break;
				case 1: opciones=param.at(i); break;
				}
			}
			archivo.open(nombre.c_str(), ios_base::in);
			return (!archivo.is_open()) ? string() : string("1");
		} catch (std::out_of_range&) {}

		return string();
	}

	string abrire(const vector<string>& param) {
		string archivo;
		string opciones;

		try {
			for (unsigned int i=0;i<param.size();i++) {
				switch (i) {
				case 0: archivo=param.at(i); break;
				case 1: opciones=param.at(i); break;
				}
			}
		} catch (std::out_of_range&) {}

		return string();
	}

	string leer(const vector<string>& param) {

		uint16_t total=4096;
		total = (param.size() == 1) ? atoi(param.at(0).c_str()) : 4096;

		if (buffer.cantidad_buf() < total)
			buffer.reservar(total);

		archivo.getline(reinterpret_cast<char*>(buffer.buf()), total);

		return (!archivo.eof()) ? string(reinterpret_cast<char*>(buffer.buf())) : string();
	}

	string escribir(const vector<string>& param) {
		if (param.size() != 1) return string();

		archivo << param.at(0).c_str();

		return (archivo.fail()) ? string() : string("1");
	}

	string cerrar(const vector<string>& param) {
		archivo.close();
		msj_error.clear();

		return string();
	}

	string error(const vector<string>& param) {
		return msj_error;
	}

};

#endif
