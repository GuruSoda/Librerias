#if !defined GURUHTTPSERVIDOR_H__
#define GURUHTTPSERVIDOR_H__

#include <string>
#include <vector>
#include <algorithm>
#include <ctype.h>
#include <cstdio>

#include <GuruCompat.h>

#if defined TEUSHEN
	#error No defina TEUSHEN directamente, defina TEUSHEN_BASIC, TEUSHEN_LIBS, TEUSHEN_FULL
#endif

#if defined(TEUSHEN_BASIC) || defined(TEUSHEN_LIBS) || defined(TEUSHEN_FULL)
	#define TEUSHEN
#endif 

#if defined TEUSHEN
	#include <GuruInterprete/GuruPreprocesador.hpp>
	#include <GuruInterprete/Teushen.hpp>
	#include <GuruPool.hpp>
	#include "GuruhttpFunciones.hpp"
#endif

#if defined TEUSHEN_FULL
	#include <GuruInterprete/TeushenEx.hpp>
#endif

#include "GuruhttpInterface.hpp"

#if defined GURUHTTP_LISTAR
	#if !defined GURUHTTP_ARCHIVO_CSS
		#define GURUHTTP_ARCHIVO_CSS "GuruhttpServidor.css"
//		#error Archivo CSS no definido, Defina GURUHTTP_ARCHIVO_CSS con un nombre de archivo.
	#endif
#endif

using namespace std;

class GuruhttpServidor : public SitioWeb
{
public:
	GuruhttpServidor() {};
	virtual ~GuruhttpServidor() {};

	void procesarGet() {
		if (!recurso().info().Existe()) {
		#if defined GURUHTTP_LISTAR
			if (recurso().archivo().compare(GURUHTTP_ARCHIVO_CSS) == 0) {
				GuruDirectorio::GuruArchivo a(GuruDirectorio::propiedades(string(GURUHTTP_ARCHIVO_CSS)));
				if (!a.Existe()) throw GuruhttpExcepcion(404);
				recurso().info(a);
				response()->enviar_archivo();
				return ;
			} else
		#endif
				throw GuruhttpExcepcion(404);
		}

		if (recurso().info().EsArchivo()) {
		#if defined TEUSHEN
			if (request()->recurso().info().sacar_extension().compare(".guru") == 0) { 
				return procesar_archivo_teushen();
			}
		#endif

			return response()->enviar_archivo();

		} else if (recurso().info().EsDirectorio()) {
				if (config()->listar()) {
					string tmp(SacarBusqueda());

					if (tmp.empty()) {
					#if defined GURUHTTP_LISTAR
						ostringstream oss;
						response()->armar_listado_tipo_google(oss);
						enviar_buffer(oss.str(), "text/html");
					#else
						throw GuruhttpExcepcion(404);
					#endif
					} else {
						recurso().info(GuruDirectorio::propiedades(tmp));
						return response()->enviar_archivo();
					}
				} else
					if (!config()->listar()) {
						string tmp(SacarBusqueda());

						if (tmp.empty()) throw GuruhttpExcepcion(404);
						else {
							recurso().info(GuruDirectorio::propiedades(tmp));

						#if defined TEUSHEN
							if (GuruStringAlgo::stringequals(GuruDirectorio::GuruSplitPath(tmp).ext(), string(".guru")))  { 
								return procesar_archivo_teushen();
							}
						#endif
							return response()->enviar_archivo();
						}
					} else throw GuruhttpExcepcion(403);
		} else throw GuruhttpExcepcion(404);
	}

	void procesarHead() {

		if (!recurso().info().Existe()) {
	#if defined GURUHTTP_LISTAR
			if (recurso().archivo().compare(GURUHTTP_ARCHIVO_CSS) == 0) {
				GuruDirectorio::GuruArchivo a(GuruDirectorio::propiedades(string(GURUHTTP_ARCHIVO_CSS)));
				if (!a.Existe()) throw GuruhttpExcepcion(404);
				recurso().info(a);
				response()->enviar_header_archivo();
				return ;
			} else
	#endif
				throw GuruhttpExcepcion(404);
		}

		if (recurso().info().EsArchivo()) {
		#if defined TEUSHEN
			if (recurso().info().sacar_extension().compare(".guru") == 0) { 
				return procesar_archivo_teushen();
			}
		#endif

			return response()->enviar_header_archivo();

		} else if (recurso().info().EsDirectorio()) {
			if (config()->listar()) {
				string tmp(SacarBusqueda());

				if (tmp.empty()) {
				#if defined GURUHTTP_LISTAR
					ostringstream oss;
					response()->armar_listado_tipo_google(oss);
					response()->enviar_header_pagina(oss.str());
				#else
					throw GuruhttpExcepcion(404);
				#endif
				} else {
					recurso().info(GuruDirectorio::propiedades(tmp));
					return response()->enviar_header_archivo();
				}
			} else 
				if (!config()->listar()) {

				string tmp(SacarBusqueda());

				if (tmp.empty()) throw GuruhttpExcepcion(404);
				else {
					recurso().info(GuruDirectorio::propiedades(tmp));

				#if defined TEUSHEN
					if (GuruStringAlgo::stringequals(GuruDirectorio::GuruSplitPath(tmp).ext(), string(".guru")))  { 
						return procesar_archivo_teushen();
					}
				#endif

					return response()->enviar_header_archivo();
				}
			} else throw GuruhttpExcepcion(403);
		} else throw GuruhttpExcepcion(404);

	}

	string SacarBusqueda() {
		if (config() == NULL) return string();

		vector<string>& busquedas=request()->config()->sacarBusqueda();

		for (vector<string>::iterator it=busquedas.begin();it!=busquedas.end();++it)
			if (GuruDirectorio::existe(request()->recurso().ruta_absoluta() + *it))
				return string(recurso().ruta_absoluta() + *it);

		return string();
	}

#if defined TEUSHEN
	void procesar_archivo_teushen() {
		ostringstream oss;
		
		string script_original(recurso().ruta_absoluta());

		procesar(recurso().ruta_absoluta(), oss);

		// La ruta absoluta solo es modificada cuando el escript pide bajar un archivo 
		// y no enviar la salida del procesamiento del script.
		if (recurso().ruta_absoluta().compare(script_original) == 0)
			response()->enviar(oss.str());
		else
			response()->enviar_archivo();
	}

	ostream& procesar(const string& archivo_guru, ostream& salida) {

		static GuruPool_threadsafe<teushenEx> pool;
		teushenEx* interprete = pool.obtener();

		interprete->limpiar();

		GuruhttpFunciones	funchttp(request());

		interprete->agregar_funciones(&funchttp);

		ifstream ifs(archivo_guru.c_str(), ios_base::binary);

		if (!ifs.is_open()) throw GuruhttpExcepcion(403);

		ControlFin_instruccion control_interprete("<?Guru", "?>");

		interprete->agregar_control(GuruInterprete::fin_analisis, &control_interprete);

		bool ok=true;

		try {
			interprete->interpretar(ifs, salida, 0);
		} catch (GuruInterprete::Interprete::InterpreteException& e) {
			cout << "Error:" << e.what() << endl;
//			cout << "Interpretado:" << salida.str() << endl;
			ok=false;
		} catch (...) { // para que quede atrapado cualquier otra excepcion.
		}

		interprete->borrar_funciones(&funchttp);
		pool.liberar(interprete);

		ifs.close();

		if (!ok) throw GuruhttpExcepcion(500);

		return salida;
	}

#endif

	void mostrar_recurso() {
		cout << "Directorio:" << recurso().directorio() << endl;
		cout << "SubDirectorio:" << recurso().subdirectorio() << endl;
		cout << "Archivo:" << recurso().archivo() << endl;
		cout << "Ruta Relativa:" << recurso().ruta_relativa() << endl;
	}

	void procesarPost() {
		procesarGet();
	};
};

#endif
