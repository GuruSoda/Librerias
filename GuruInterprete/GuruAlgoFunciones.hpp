#ifndef __GURUALGOFUNCIONES_HPP__
#define __GURUALGOFUNCIONES_HPP__

#include <GuruAlgo.hpp>
#include <stdlib.h>

class GuruAlgoFunciones: public GuruInterprete::GuruFunciones<GuruAlgoFunciones> {
public:
	GuruAlgoFunciones () {
		cargar_funcion(this, &GuruAlgoFunciones::leible, "algo.leible");
	};

	virtual ~GuruAlgoFunciones() {
	};

	string leible(const vector<string>& param) {

		if (param.size() != 1) return string();

		float tamanio=static_cast<float>(atof(param.at(0).c_str()));

		if (tamanio==0) return string("0");

		int divisiones=0;

		while (tamanio > static_cast<float>(0)) {
			if ((tamanio/static_cast<float>(1024)) < static_cast<float>(1)) break;

			tamanio/=static_cast<float>(1024);
			divisiones++;
		}

		string tmp(GuruStringAlgo::itoss(tamanio));
		string::size_type lugar;
		// cambio el . por la ,
		if ((lugar=tmp.find('.')) != string::npos) {
			tmp[lugar] = ',';

			// si tengo mas de un digito despues de la coma los borro
			if (lugar<tmp.size()-2) tmp.erase(lugar+2);
		}

		// me fijo cuantas veces dividi el numero.
		switch (divisiones) {
		case 0: tmp+="B"; break;
		case 1: tmp+="K"; break;
		case 2: tmp+="M"; break;
		case 3: tmp+="G"; break;
		case 4: tmp+="T"; break;
		}

		return tmp;
	}

};

#endif
