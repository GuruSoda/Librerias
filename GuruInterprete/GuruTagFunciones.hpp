#ifndef __GURUTAGFUNCIONES_HPP__
#define __GURUTAGFUNCIONES_HPP__

#include <string>

#include <GuruEtiquetaLeer.hpp>

using namespace std;

class GuruTagFunciones: public GuruInterprete::GuruFunciones<GuruTagFunciones> {
	GuruEtiquetaLeer	tag;
public:
	GuruTagFunciones () {
		cargar_funcion(this, &GuruTagFunciones::cargar, "tag.cargar");
		cargar_funcion(this, &GuruTagFunciones::cargarex, "tag.cargarex");
		cargar_funcion(this, &GuruTagFunciones::sacartag, "tag.tag");
	};

	virtual ~GuruTagFunciones() {
	};

	string cargar(const vector<string>& param) {
		if (param.size() == 1) tag.cargar(param.at(0));

		return string();
	}

	string cargarex(const vector<string>& param) {
		return string();
	}

	string sacartag(const vector<string>& param) {
		if (param.size() == 1) return tag[param.at(0)];

		return string();
	}
};

#endif
