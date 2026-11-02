#ifndef __GURUCONFIGFUNCIONES_HPP__
#define __GURUCONFIGFUNCIONES_HPP__

#include <map>
#include <string>
#include <stdexcept>
#include <GuruConfigLeer.h>
#include <functional>
#include <iterator>

using namespace std;

class GuruConfigFunciones: public GuruInterprete::GuruFunciones<GuruConfigFunciones> {
	map<string, string> conf_seccion;
	GuruConfigLeer	conf;
public:
	GuruConfigFunciones() {
		cargar_funcion(this, &GuruConfigFunciones::leer, "config.leer");
		cargar_funcion(this, &GuruConfigFunciones::seccion, "config.seccion");
		cargar_funcion(this, &GuruConfigFunciones::secciones, "config.secciones");
		cargar_funcion(this, &GuruConfigFunciones::valor, "config.seccion.valor");
	};

	virtual ~GuruConfigFunciones() {
	};

	string leer(const vector<string>& param) {
		if (param.size() != 3 && param.size() != 4) return string();

		string archivo,seccion,entrada,val_default;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: archivo=param.at(i); break;
			case 1: seccion=param.at(i); break;
			case 2: entrada=param.at(i); break;
			case 3: val_default=param.at(i); break;
			}
		}

		#ifdef __GURUWIN32__
			if (archivo.find('/') != string::npos)
//				std::replace_if(archivo.begin(), archivo.end(),std::bind2nd(std::equal_to<char>(), '/') , '\\');

				std::replace_copy(archivo.begin(), archivo.end(), std::back_inserter(archivo), '/', '\\');
		#endif

		conf.abrir(archivo);

		string tmp(conf.leer(seccion, entrada, val_default));

		conf.cerrar();

		return tmp;
	}

	string valor(const vector<string>& param) {
		if (param.size() != 1) return string();

		map<string, string>::iterator it(conf_seccion.find(param.at(0)));

		return (it == conf_seccion.end()) ? string() : it->second;
	}

	string seccion(const vector<string>& param) {
		if (param.size() != 2) return string();

		conf.abrir(param.at(0));

		return (conf.leer_seccion(param.at(1), conf_seccion).size() == 0) ? "" : GuruStringAlgo::itos(conf_seccion.size());
	}

	string secciones(const vector<string>& param) {
		if (param.size() != 1) return string();

		vector<string> ls_secciones;

		try {
			conf.abrir(param.at(0));

			conf.secciones(ls_secciones);

			string str_secciones;

			vector<string>::iterator it=ls_secciones.begin();
			str_secciones +=  *it++;

			for (;it!=ls_secciones.end();++it)
				str_secciones += " | " + *it;

			conf.cerrar();
			return str_secciones;
		} catch (GuruConfigExcepcion&) {
			return string();
		}
	}
};

#endif
