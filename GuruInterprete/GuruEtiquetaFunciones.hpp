#ifndef __GURUETIQUETAFUNCIONES_HPP__
#define __GURUETIQUETAFUNCIONES_HPP__

#include <GuruEtiquetaLeer.hpp>

using namespace std;

class GuruEtiquetaFunciones: public GuruInterprete::GuruFunciones<GuruEtiquetaFunciones> {
	GuruEtiquetaLeer	et;
public:
	GuruEtiquetaFunciones() {
		cargar_funcion(this, &GuruEtiquetaFunciones::cargar, "etiqueta.cargar");
		cargar_funcion(this, &GuruEtiquetaFunciones::leer, "etiqueta.leer");
	};

	virtual ~GuruEtiquetaFunciones() {
	};

	string cargar(const vector<string>& param) {
		et.cargar(param.at(0));
		return string();
	}

	string leer(const vector<string>& param) {

		return et[param.at(0)];
	}

};

#endif
