#ifndef __GURUGENIDFUNCIONES_HPP__
#define __GURUGENIDFUNCIONES_HPP__

#include <GuruAlgo.hpp>
#include <GuruID.hpp>
#include <map>
#include <string>

class GuruGenIDFunciones: public GuruInterprete::GuruFunciones<GuruGenIDFunciones> {
	GuruID<std::string> contenedor;
	std::map<string, unsigned long>::iterator it;
	std::pair<string, unsigned long> par;
public:
	GuruGenIDFunciones () {
		cargar_funcion(this, &GuruGenIDFunciones::id, "genid.id");
		cargar_funcion(this, &GuruGenIDFunciones::listar, "genid.listar");
		cargar_funcion(this, &GuruGenIDFunciones::siguiente, "genid.++");
		cargar_funcion(this, &GuruGenIDFunciones::llave, "genid.llave");
		cargar_funcion(this, &GuruGenIDFunciones::valor, "genid.valor");
	};

	virtual ~GuruGenIDFunciones() {
	};

	string id(const vector<string>& param) {

		if (param.size() != 1) return string();

		return GuruStringAlgo::itos(contenedor.id(param.at(0)));
	}

	string listar(const vector<string>& param) {
		it = contenedor.src().begin();
		
		return string();
	}

	string siguiente(const vector<string>& param) {
		
		if (it == contenedor.src().end()) return string();


		par.first = it->first;
		par.second = it->second;

		++it;
		
		return string("1");
	}

	string llave(const vector<string>& param) {
		return par.first;
	}

	string valor(const vector<string>& param) {
		return GuruStringAlgo::itos(par.second);
	}

};

#endif
