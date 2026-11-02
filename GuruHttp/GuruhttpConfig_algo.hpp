#ifndef __GURUHTTPCONFIG_ALGO_HPP__
#define __GURUHTTPCONFIG_ALGO_HPP__

#include <GuruConfigLeer.h>
#include <GuruDirectorio.hpp>
#include <GuruAlgo.hpp>
#include <GuruSingleton.hpp>
#include <GuruDirectorio.hpp>
#include <GuruVariable.hpp>
#include <GuruLog.hpp>

#include <GuruHttp/GuruhttpServidor.hpp>
#include <GuruHttp/GuruhttpServidorJSON.hpp>

#include <GuruInterprete/GuruCargaLibs.hpp>

namespace GuruHttpConfigAlgo {

	static int id=1;

//////////////////////////////////////////////////////////////////////
// Configuracion del Servicio HTTP
//////////////////////////////////////////////////////////////////////
	template <typename T>
	void cargar_conf_http(const T& archivo) {

		GuruConfigLeer conf(archivo);

		vector<string> secciones;

		conf.secciones(secciones);

		for (vector<string>::iterator it=secciones.begin();it!=secciones.end();++it) {
			GuruhttpConf	confhttp;

			confhttp.puerto(atoi(conf.leer(*it, "puerto", "80").c_str()));
			confhttp.interfaz(conf.leer(*it, "interfaz", "0.0.0.0"));
			confhttp.sitios(conf.leer(*it, "sitios", "..\\sitios\\"));

			confhttp.ponerVelocidad(atoi(conf.leer(*it, "velocidad", "0").c_str()));
			confhttp.agregar_direccion_rechazar(conf.leer(*it, "denegar", "0.0.0.0"));
			confhttp.agregar_direccion_aceptar(conf.leer(*it, "aceptar", "255.255.255.255"));

			confhttp.log_error(conf.leer(*it, "log_error", "error.log"));
			
			if (conf.leer(*it, "debug", "0").compare("1") == 0)
				GuruLog::init(GuruLog::level_debug);
			

			confhttp.nombre(*it);

			GuruSingleton<GuruhttpConfig>::instancia()->agregarConfHttp(*it, confhttp);
#if defined TEUSHEN_LIBS
			GuruSingleton<GuruInterprete::GuruCargaLibs>::instancia()->cargar(GuruDirectorio::comoDirectorio(conf.leer(*it, "teushen", "..\\libs\\")));
//			GuruSingleton<teushen::GuruCargaLibs>::instancia()->directorio_libs(GuruDirectorio::comoDirectorio(conf.leer(*it, "teushen", "..\\libs\\")));
#endif
		}
	}

//////////////////////////////////////////////////////////////////////
// Configuracion de los sitios
//////////////////////////////////////////////////////////////////////
	GuruhttpInterface* sitioDesdeLibreria(GuruhttpDirVirtual& dir) {
		GuruhttpInterface* sitio=NULL;

	#if defined __GURUWIN32__
		typedef GuruhttpInterface* (WINAPI* ptr_func_sitio) (const std::map<std::string, std::string>&);
		typedef bool (WINAPI* ptr_func_init) (GuruhttpDirVirtual&);
	#elif defined __GURULINUX__
		typedef GuruhttpInterface* (*ptr_func_sitio) (const std::map<std::string, std::string>&);
		typedef bool (*ptr_func_init) (GuruhttpDirVirtual&);
	#endif

	#if defined __GURUWIN32__
		HMODULE modulo = LoadLibraryEx(dir.lib().c_str(), NULL, 0);
	#elif defined __GURULINUX__
		void* modulo = dlopen(dir.lib().c_str(), RTLD_LAZY);
	#endif

		if (modulo) {
		#if defined __GURUWIN32__
			ptr_func_sitio func_sitio = (ptr_func_sitio)GetProcAddress(modulo, dir.intefaz_lib().c_str());
		#elif defined __GURULINUX__
			ptr_func_sitio func_sitio = (ptr_func_sitio)dlsym(modulo, dir.intefaz_lib().c_str());
		#endif

			if (func_sitio) {
				sitio = func_sitio(dir.env());
			#if defined __GURUWIN32__
				ptr_func_init func_init = (ptr_func_init)GetProcAddress(modulo, "init");
			#elif defined __GURULINUX__
				ptr_func_init func_init = (ptr_func_init)dlsym(modulo, "init");
			#endif

				GURULOG_DEBUG("Modulo y funcion Cargado...");

				if (func_init) { 
					GURULOG_DEBUG("Por Ejecutar el init...");
					bool init = func_init(dir);
					return init ? sitio : NULL;
				}
			} else 
				throw GuruConfigExcepcion(GuruError().descripcion());
		} else
				throw GuruConfigExcepcion(GuruError().descripcion());

		return sitio;
	}

	std::map<std::string, std::string>& env_map(const string& env, std::map<std::string, std::string>& var) {
		GuruTokenEx x(",");

		var.clear();

		size_t total = x.extraer(env);
		if (total > 0) {
			GuruTokenEx y("=");

			for (size_t i = 0; i < total; i++) {
				if (y.extraer(x[i]) > 0) {
					var.insert(make_pair(y[0], y[1]));
				}
			}
		}

		return var;
	}

	std::map<std::string, std::string>& cargar_atributos_extra(const string& str_atributos, std::map<std::string, std::string>& atributosExtra) {
		GuruTokenEx x(",");

		atributosExtra.clear();

		size_t total = x.extraer(str_atributos);
		if (total > 0) {
			GuruTokenEx y(":");

			for (size_t i = 0; i < total; i++) {
				if (y.extraer(x[i]) > 0) {
					atributosExtra.insert(make_pair(y[0], y[1]));
				}
			}
		}

		return atributosExtra;
	}

	template <typename T>
	void cargar_conf_sitio (const T& archivo) {
		GuruConfigLeer conf(archivo);

		try {
			for (GuruConfigLeer::iterator it = conf.begin(); it != conf.end(); ++it) {

				GURULOG_DEBUG("Sitio: " + it.seccion());
				GuruhttpDirVirtual	dir;

				GuruVariableMap var;

//				if (it.seccion().compare("catalogadorws") == 0)
//					int a = 1;

//				map<string, string> env; // para variables de entorno definidas en el archivo de configuracion del sitio

				if (!it.entrada("env").empty())
					env_map(it.entrada("env"), dir.env());

				// si no esta definida la entrada temp la creo
				if (it.entrada("temp").empty()) {
					dir.env("temp", string("{temp}") + DIVISOR_DIRECTORIO + string("%sitio%") + DIVISOR_DIRECTORIO);
				} else 
					dir.env("temp", it.entrada("temp"));

				// cargo una nueva variable de entorno con el nombre del sitio
				dir.env("sitio", it.seccion());

				// inserto todas las variables y sus valores
				var.insertar(it.entradas());
				var.insertar(dir.env());

//				var.reemplazar(); // reemplazo todas las variables internas.

				// para cambiar las variables de entorno del sistema operativo
				GuruVariableEntorno ent('{', '}');
				// modifico todas las entradas de la seccion que se esta leyendo
				// para modificar las variables de entorno/contexto de la aplicacion
				for (map<string, string>::iterator it2 = it.entradas().begin(), it2end = it.entradas().end(); it2 != it2end; ++it2) {
					it2->second = var.reemplazar(it2->second); // variables de contexto/entorno de la aplicacion.
					it2->second = ent.reemplazar(it2->second); // variables de entorno del Sistema Operativo.
				}

				for (map<string, string>::iterator it = dir.env().begin(), itend = dir.env().end(); it != itend; ++it) {
					it->second = var.reemplazar(it->second); // variables de contexto/entorno de la aplicacion.
					it->second = ent.reemplazar(it->second); // variables de entorno del Sistema Operativo.
				}

				dir.sitio(it.seccion());

				GuruToken tk(" ,;|");
				vector<string> tokens;
				if (!tk.extraer(it.entrada("host", "*"), tokens).empty())
					dir.agregarHost(tokens);

				dir.dirApps(GuruDirectorio::comoDirectorio(it.entrada("directorio", ".")));

				if (it.entrada("listado", "interno").compare("externo") == 0)
					dir.listar(true);
				else
					dir.listar(false);

				if (!tk.extraer(it.entrada("busqueda", "index.html|main.html"), tokens).empty())
					dir.agregarBusqueda(tokens);

				if (!tk.extraer(it.entrada("multi-log", ""), tokens).empty()) {
					vector<string> multitokens;
					for (vector<string>::iterator it2=tokens.begin();it2!=tokens.end();++it2) {
						if (!tk.extraer(it.entrada(*it2, ""), multitokens).empty())
							dir.agregarLog(multitokens.at(0), multitokens.at(1));
					}
				} else {
					if (tk.extraer(it.entrada("log", ""), tokens).size() == 2)
						dir.agregarLog(tokens.at(0), tokens.at(1));

					if (tk.extraer(it.entrada("log1", ""), tokens).size() == 2)
						dir.agregarLog(tokens.at(0), tokens.at(1));

					if (tk.extraer(it.entrada("log2", ""), tokens).size() == 2)
						dir.agregarLog(tokens.at(0), tokens.at(1));

					if (tk.extraer(it.entrada("log3", ""), tokens).size() == 2)
						dir.agregarLog(tokens.at(0), tokens.at(1));

					if (tk.extraer(it.entrada("log-error", ""), tokens).size() == 2)
						dir.agregarLog(tokens.at(0), tokens.at(1));
				}

				if (!it.entrada("atributos").empty())
					cargar_atributos_extra(it.entrada("atributos"), dir.sacarAtributosExtra());

				dir.velocidad(atoi(it.entrada("velocidad", "0").c_str()));

				dir.id(id++);

				dir.intefaz_lib(it.entrada("interfaz"));
				dir.lib(it.entrada("libreria"));

				// Para saber que interfaz se va a utilizar.
				if (!dir.intefaz_lib().empty() && !dir.lib().empty()) {
					GuruhttpInterface* ifz = sitioDesdeLibreria(dir);

					if (ifz) {
						dir.interfaz(ifz);
					} else {
						cerr << "Error leyendo intefaz " << it.entrada("interfaz") << " de la libreria " << it.entrada("libreria") << " del sitio " << it.seccion() << endl;
						throw GuruConfigExcepcion("intefaz y libreria deben existir juntas.");
						continue;
					}
				} else {
					std::string e(it.entrada("listar", "no"));

					if (GuruStringAlgo::stringequals(e, string("si")) || GuruStringAlgo::stringequals(e, string("html"))) {
						dir.interfaz(new GuruhttpSitio<GuruhttpServidor>());
						dir.listar(true);
					} else if (GuruStringAlgo::stringequals(e, string("json"))) {
						dir.interfaz(new GuruhttpSitio<GuruhttpServidorJSON>());
						dir.listar(true);
					} else if (GuruStringAlgo::stringequals(e, string("no"))) {
						dir.interfaz(new GuruhttpSitio<GuruhttpServidor>());
						dir.listar(false);
					}
				} 

				GuruSingleton<GuruhttpConfig>::instancia()->nuevo_sitio(dir);

				GURULOG_DEBUG("Sitio Cargado Correctamente: " + it.seccion());
			}
		}catch (GuruConfigExcepcion& e) {
			cerr << "Error Cargando Sitio:" << e.what() << endl;
			GURULOG_DEBUG("Error Cargando Sitio: " + string(e.what()));
		}
	}

	template <typename T>
	void cargar_sitios (const T& directorio) {
		GuruDirectorio::iterator itfinal;
		for (GuruDirectorio::iterator it(directorio, "*.conf");it!=itfinal;++it) {
			cargar_conf_sitio(directorio + it->nombre());
		}
	}
}

#endif
