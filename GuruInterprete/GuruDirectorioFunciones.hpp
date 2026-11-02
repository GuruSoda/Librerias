#ifndef __GURUDIRECTORIOFUNCIONES_HPP__
#define __GURUDIRECTORIOFUNCIONES_HPP__

#include <GuruDirectorio.hpp>
#include <GuruInterprete/GuruInterprete.hpp>
#include <GuruAlgo.hpp>

class GuruDirectorioFunciones: public GuruInterprete::GuruFunciones<GuruDirectorioFunciones> {
	GuruDirectorio::iterator	it;
	GuruDirectorio::iterator	end;

	GuruDirectorio::iterator_recursivo	it_r;
	GuruDirectorio::iterator_recursivo	end_r;

	GuruDirectorio::GuruArchivo actual;
	bool						recursivo;

	std::string					dir_actual;

public:
	GuruDirectorioFunciones () {
		cargar_funcion(this, &GuruDirectorioFunciones::init, "dir.init");
		cargar_funcion(this, &GuruDirectorioFunciones::init_recursivo, "dir.init_recursivo");
		cargar_funcion(this, &GuruDirectorioFunciones::extraer, "dir.extraer");
		cargar_funcion(this, &GuruDirectorioFunciones::directorio, "dir.directorio");
		cargar_funcion(this, &GuruDirectorioFunciones::nombre, "dir.nombre");
		cargar_funcion(this, &GuruDirectorioFunciones::tamanio, "dir.tamanio");
		cargar_funcion(this, &GuruDirectorioFunciones::hora, "dir.hora");
		cargar_funcion(this, &GuruDirectorioFunciones::fecha, "dir.fecha");
		cargar_funcion(this, &GuruDirectorioFunciones::fechahora, "dir.fechahora");
		cargar_funcion(this, &GuruDirectorioFunciones::armarfecha, "dir.armarfecha");
		cargar_funcion(this, &GuruDirectorioFunciones::epoc, "dir.epoc");
		cargar_funcion(this, &GuruDirectorioFunciones::es_dir, "dir.es_dir");
		cargar_funcion(this, &GuruDirectorioFunciones::es_archivo, "dir.es_archivo");
		cargar_funcion(this, &GuruDirectorioFunciones::existe, "dir.existe");
		cargar_funcion(this, &GuruDirectorioFunciones::propiedades, "dir.propiedades");

		cargar_funcion(this, &GuruDirectorioFunciones::copiar, "dir.copiar");
		cargar_funcion(this, &GuruDirectorioFunciones::mover, "dir.mover");
		cargar_funcion(this, &GuruDirectorioFunciones::borrar, "dir.borrar");
	};

	virtual ~GuruDirectorioFunciones() {};

	string init(const vector<string>& param) {
		recursivo = false;
		
		if (param.at(0).empty()) return string("0");

		string directorio,comodin;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: directorio=param.at(i); break;
			case 1: comodin=param.at(i); break;
			}
		}
	
		it=GuruDirectorio::iterator(directorio, comodin);
		actual=*it;

		return string("1");
	}

	string init_recursivo(const vector<string>& param) {
		recursivo=true;

		if (param.at(0).empty()) return string("0");

		string directorio,comodin;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: directorio=param.at(i); break;
			case 1: comodin=param.at(i); break;
			}
		}
	
		it_r=GuruDirectorio::iterator_recursivo(directorio, comodin);
		actual=*it_r;

		return string("1");
	}

	string extraer(const vector<string>& param) {
		if (recursivo) {
			if (it_r != end_r) {
				actual=*it_r;
				dir_actual=it_r.dir();
				++it_r;
				return string("1");
			} else return string("0");
		} else {
			if (it != end) {
				actual=*it;
				++it;
				return string("1");
			} else 
				return string("0");
		}
	}

	string directorio(const vector<string>& param) {
	//	return it_r.dir();
		return dir_actual;
	}

	string nombre(const vector<string>& param) {
		return actual.nombre();
	}

	string tamanio(const vector<string>& param) {
		return GuruStringAlgo::int64astr(actual.tamanio());
	}

	string hora(const vector<string>& param) {
		return GuruAlgo::armarFecha("%H%:%M%:%S%", actual.fecha());
	}

	string fecha(const vector<string>& param) {
		return GuruAlgo::armarFecha("%AA%/%MM%/%DD%", actual.fecha());
	}

	string fechahora(const vector<string>& param) {
		return GuruAlgo::armarFecha("%AA%/%MM%/%DD% %H%:%M%:%S%", actual.fecha());
	}

	string armarfecha(const vector<string>& param) {
		if (param.size() == 1)
			return GuruAlgo::armarFecha(param.at(0), actual.fecha());
		else
			return string();
	}

	string epoc(const vector<string>& param) {
		return GuruStringAlgo::int64astr(actual.fecha());
	}

	string es_dir(const vector<string>& param) {
		return (actual.EsDirectorio()) ? string("1") : string();
	}

	string es_archivo(const vector<string>& param) {
		return (actual.EsArchivo()) ? string("1") : string();
	}

	string existe(const vector<string>& param) {
		return GuruDirectorio::existe(param.at(0)) ? string("1") : string();
	}

	string copiar(const vector<string>& param) {
		if (param.size() != 2) return string("0");

		return GuruDirectorio::copiarArchivo(param.at(0), param.at(1)) ? string("1") : string();
	}

	string mover(const vector<string>& param) {
		if (param.size() != 2) return string("0");

		return GuruDirectorio::moverArchivo(param.at(0), param.at(1)) ? string("1") : string();
	}

	string borrar(const vector<string>& param) {
		if (param.size() != 1) return string("0");

		return GuruDirectorio::borrarArchivo(param.at(0)) ? string("1") : string();
	}

	string propiedades(const vector<string>& param) {
		actual=GuruDirectorio::propiedades(param.at(0));

		if (actual) return string("1");
		else return string();
	}

};

#endif
