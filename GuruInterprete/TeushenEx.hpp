#if !defined __TEUSHEN_HPP__
#define __TEUSHEN_HPP__

#include <GuruInterprete/GuruInterprete.hpp>
#include <GuruInterprete/Teushen.hpp>

// TEUSHEN_FULL: Carga las funciones internas y extenas desde librerias.
// TEUSHEN_BASIC: Carga solo las funciones internas.
// TEUSHEN_LIBS: Carga solo las funciones desde las librerias externas.

#if defined TEUSHEN_FULL
#define TEUSHEN_BASIC
#define TEUSHEN_LIBS
#endif

#if defined TEUSHEN_BASIC
#include <GuruInterprete/GuruHashFunciones.hpp> // debe estar primero para no tener conflicto con boost y uint32
#include <GuruInterprete/GuruAlgoFunciones.hpp>
#include <GuruInterprete/GuruArchivoTextoFunciones.hpp>
#include <GuruInterprete/GuruConfigFunciones.hpp>
#include <GuruInterprete/GuruDirectorioFunciones.hpp>
#include <GuruInterprete/GuruEtiquetaFunciones.hpp>
#include <GuruInterprete/GuruSQLiteFunciones.hpp>
#include <GuruInterprete/GuruGenIDFunciones.hpp>
#include <GuruInterprete/GuruMatematicaFunciones.hpp>
#include <GuruInterprete/GuruProtocoloFunciones.hpp>
#include <GuruInterprete/GuruRegExFunciones.hpp>
#include <GuruInterprete/GuruSistemaFunciones.hpp>
#include <GuruInterprete/GuruSocketFunciones.hpp>
#include <GuruInterprete/GuruStringFunciones.hpp>
#include <GuruInterprete/GuruTagFunciones.hpp>
#include <GuruInterprete/GuruTokenFunciones.hpp>
#endif
#if defined TEUSHEN_LIBS
#include <GuruSingleton.hpp>
#include <GuruInterprete/GuruCargaLibs.hpp>
#endif

//
// Lenguaje Teushen con un conjunto de funciones
//
class teushenEx:public teushen {
public:
	teushenEx(const string& dir_libs="") { agregar_librerias_internas(dir_libs); }
	virtual ~teushenEx() { }

	void cargar_desde_librerias(const string& directorio) {
#if defined TEUSHEN_LIBS
		GuruInterprete::GuruCargaLibs	librerias;

		librerias.cargar(GuruDirectorio::comoDirectorio(directorio));

		for (GuruInterprete::GuruCargaLibs::iterator it = librerias.begin(); it != librerias.end(); ++it)
			agregar_funciones(it.funciones());
#endif
	}

private:
	void agregar_librerias_internas(const string& dir_libs="") {
	#if defined TEUSHEN_BASIC
		agregar_funciones(new GuruAlgoFunciones());
		agregar_funciones(new GuruArchivoTextoFunciones());
		agregar_funciones(new GuruConfigFunciones());
		agregar_funciones(new GuruDirectorioFunciones());
		agregar_funciones(new GuruEtiquetaFunciones());
		agregar_funciones(new GuruGenIDFunciones());
		agregar_funciones(new GuruProtocoloFunciones());
		agregar_funciones(new GuruRegExFunciones());
		agregar_funciones(new GuruSistemaFunciones());
		agregar_funciones(new GuruSocketFunciones());
		agregar_funciones(new GuruSQLiteFunciones());
		agregar_funciones(new GuruStringFunciones());
		agregar_funciones(new GuruHashFunciones());
		agregar_funciones(new GuruTagFunciones());
		agregar_funciones(new GuruTokenFunciones());
	#endif

	#if defined TEUSHEN_LIBS
		cargar_desde_librerias(dir_libs);
	#endif

	}
};
#endif
