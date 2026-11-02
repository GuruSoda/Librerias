#ifndef __GURUMATEMATICAFUNCIONES_HPP__
#define __GURUMATEMATICAFUNCIONES_HPP__

#include <GuruInterprete/GuruInterprete.hpp>
#include <GuruAlgo.hpp>
#include <stdlib.h>

using namespace std;

class GuruMatematicaFunciones: public GuruInterprete::GuruFunciones<GuruMatematicaFunciones> {

public:
	GuruMatematicaFunciones () {
		cargar_funcion(this, &GuruMatematicaFunciones::inc, "mat.inc");
		cargar_funcion(this, &GuruMatematicaFunciones::inc, "mat.sum");
		cargar_funcion(this, &GuruMatematicaFunciones::dec, "mat.dec");
		cargar_funcion(this, &GuruMatematicaFunciones::dec, "mat.res");
		cargar_funcion(this, &GuruMatematicaFunciones::mul, "mat.mul");
		cargar_funcion(this, &GuruMatematicaFunciones::div, "mat.div");

	};

	virtual ~GuruMatematicaFunciones() {};

	string inc(const vector<string>& param) {
		if (param.at(0).empty()) return string();

		string variable,nro;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: variable=param.at(i); break;
			case 1: nro=param.at(i); break;
			}
		}

		int64_t int_nro = (nro.empty()) ? 1 : static_cast<int64_t>(_atoi64(nro.c_str()));

		variable=GuruStringAlgo::itos(static_cast<int64_t>(_atoi64(variable.c_str())+int_nro));

		return variable;
	}

	string dec(const vector<string>& param) {
		if (param.at(0).empty()) return string();

		string variable,nro;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: variable=param.at(i); break;
			case 1: nro=param.at(i); break;
			}
		}

		int64_t int_nro = (nro.empty()) ? 1 : static_cast<int64_t>(_atoi64(nro.c_str()));

		variable=GuruStringAlgo::itos(static_cast<int64_t>(_atoi64(variable.c_str())-int_nro));

		return variable;
	}

	string mul(const vector<string>& param) {
		if (param.at(0).empty()) return string();

		string variable,nro;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: variable=param.at(i); break;
			case 1: nro=param.at(i); break;
			}
		}

		int64_t int_nro = (nro.empty()) ? 1 : static_cast<int64_t>(_atoi64(nro.c_str()));

		variable=GuruStringAlgo::itos(static_cast<int64_t>(_atoi64(variable.c_str())*int_nro));

		return variable;
	}

	string div(const vector<string>& param) {
		if (param.at(0).empty()) return string();

		string variable,nro;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: variable=param.at(i); break;
			case 1: nro=param.at(i); break;
			}
		}

		int64_t int_nro = (nro.empty()) ? 1 : static_cast<int64_t>(_atoi64(nro.c_str()));

		if (int_nro > 0)
			variable=GuruStringAlgo::itos(static_cast<int64_t>(_atoi64(variable.c_str())/int_nro));

		return variable;
	}

};

#endif

