#ifndef __GURUSTRINGFUNCIONES_HPP__
#define __GURUSTRINGFUNCIONES_HPP__

#include <GuruInterprete/GuruInterprete.hpp>
#include <GuruAlgo.hpp>
#include <GuruString_algo.hpp>

using namespace std;

class GuruStringFunciones: public GuruInterprete::GuruFunciones<GuruStringFunciones> {

public:
	GuruStringFunciones () {
		cargar_funcion(this, &GuruStringFunciones::concatenar, "str.concat");
		cargar_funcion(this, &GuruStringFunciones::substr, "str.substr");
		cargar_funcion(this, &GuruStringFunciones::trim, "str.trim");
		cargar_funcion(this, &GuruStringFunciones::del, "str.delete");
		cargar_funcion(this, &GuruStringFunciones::equals, "str.equals");
		cargar_funcion(this, &GuruStringFunciones::find, "str.find");
		cargar_funcion(this, &GuruStringFunciones::length, "str.length");
	};

	virtual ~GuruStringFunciones() {};

	string concatenar(const vector<string>& param) {

		string tmp;

		for (vector<string>::const_iterator it=param.begin();it!=param.end();++it)
			tmp+=*it;

		return tmp;
	}

	string substr(const vector<string>& param) {
		if (param.at(0).empty()) return string();

		string cadena,inicio,total;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: cadena=param.at(i); break;
			case 1: inicio=param.at(i); break;
			case 2: total=param.at(i); break;
			}
		}

		string::size_type int_total = (total.empty()) ? string::npos : atoi(total.c_str());

		return cadena.substr(atoi(inicio.c_str())-1, int_total);
	}

	string trim(const vector<string>& param) {

		if (param.size() != 1) return string();

		return GuruStringAlgo::trim_copy(param.at(0));
	}

	string del(const vector<string>& param) {
		if (param.at(0).empty()) return string();

		string cadena,inicio,total;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: cadena=param.at(i); break;
			case 1: inicio=param.at(i); break;
			case 2: total=param.at(i); break;
			}
		}

		string::size_type int_total = (total.empty()) ? string::npos : atoi(total.c_str());

		cadena.erase(atoi(inicio.c_str()), int_total);

		return cadena;
	}

	string equals(const vector<string>& param) {
		return string();
	}

	string find(const vector<string>& param) {
		if (param.at(0).empty()) return string();

		string cadena,caracter,desde;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: cadena=param.at(i); break;
			case 1: caracter=param.at(i); break;
			case 2: desde=param.at(i); break;
			}
		}

		string::size_type int_desde = (desde.empty()) ? 0 : atoi(desde.c_str());

		string::size_type lugar=cadena.find(caracter.at(0), int_desde+1);

		return GuruStringAlgo::itos(lugar+1);
	}

	string length(const vector<string>& param) {
		if (param.size() == 0) return string("0");

		return GuruStringAlgo::itos(param.at(0).length());
	}

};

#endif
