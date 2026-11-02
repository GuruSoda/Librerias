#ifndef __GURUTOKENFUNCIONES_HPP__
#define __GURUTOKENFUNCIONES_HPP__

#include <string>
#include <vector>
#include <stdexcept>
#include <GuruToken.hpp>
#include <GuruAlgo.hpp>

using namespace std;

class GuruTokenFunciones: public GuruInterprete::GuruFunciones<GuruTokenFunciones> {
	GuruTokenEx	tokens;
public:
	GuruTokenFunciones () {
		cargar_funcion(this, &GuruTokenFunciones::extraer, "token.extraer");
		cargar_funcion(this, &GuruTokenFunciones::token, "token.token");
	};

	virtual ~GuruTokenFunciones() {
	};

	static GuruInterprete::IGuruFuncion* ptr_funciones () { 
		return new GuruTokenFunciones;
	};

	string extraer(const vector<string>& param) {
		string cadena;
		string delimitadores;

		try {
			for (unsigned int i=0;i<param.size();i++) {
				switch (i) {
				case 0: cadena=param.at(i); break;
				case 1: delimitadores=param.at(i); break;
				}
			}
			return GuruStringAlgo::itos(tokens.extraer(cadena, delimitadores));
		} catch (std::out_of_range&) {}

		return string();
	}

	// El acceso a los valores comienza en 1.
	string token(const vector<string>& param) {
		if (param.size() == 0) return string();

		int lugar=atoi(param.at(0).c_str());

		if (lugar == 0) return string();

		return tokens[lugar-1];
	}

};

#endif
