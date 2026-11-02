#ifndef __GURUCARGALIBS_H__
#define __GURUCARGALIBS_H__

#include <GuruCompat.h>
#include <GuruDirectorio.hpp>
#include <GuruString_algo.hpp>
#include <GuruInterprete/GuruInterprete.hpp>
#include <map>
#include <string>

#if defined __GURUWIN32__
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef _WINSOCKAPI_
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
	#endif

	#include <windows.h>

#elif defined __GURULINUX__
	#include <dlfcn.h>
#endif

using namespace std;

namespace GuruInterprete {

	class GuruCargaLibs {
	public:
		GuruCargaLibs() {};

	#if defined __GURUWIN32__
		typedef GuruInterprete::IGuruFuncion* (WINAPI* ptr_funciones) ();
	#elif defined __GURULINUX__
		typedef GuruInterprete::IGuruFuncion* (*ptr_funciones) ();
	#endif

		struct lib {
	#if defined __GURUWIN32__
			HMODULE modulo;
	#elif defined __GURULINUX__
			void* modulo;
	#endif
			ptr_funciones inicializacion;
		};


		virtual ~GuruCargaLibs() {
			for (map<string, struct lib>::iterator it=librerias.begin();it!=librerias.end();++it) {
	#if defined __GURUWIN32__
				FreeLibrary(it->second.modulo);
	#elif defined __GURULINUX__
				dlclose(it->second.modulo);
	#endif
	//			cout << "Liberando " << it->first << endl;
			}
		};

		void cargarLibreria(const string& nombre) {

	#if defined __GURUWIN32__
		#if defined __GURUUNICODE__
			HMODULE modulo = LoadLibraryEx(GuruStringAlgo::ansi_unicode(nombre).c_str(), NULL, 0);
		#else
			HMODULE modulo=LoadLibraryEx(nombre.c_str(),NULL,0);
#		endif
	#elif defined __GURULINUX__
			void* modulo=dlopen(nombre.c_str(), RTLD_LAZY);
	#endif

			if (modulo) {
				ptr_funciones init;
	#if defined __GURUWIN32__
				init=(ptr_funciones)GetProcAddress(modulo, "ptr_funciones");
	#elif defined __GURULINUX__
				init = (ptr_funciones)dlsym(modulo, "ptr_funciones");
	#endif

				if (init) {
					struct lib data;

					data.modulo = modulo;
					data.inicializacion=init;

					librerias.insert(make_pair(nombre, data));
				}
			}
		}

		void descargarLibreria(const string& nombre) {
			map<string, struct lib>::iterator it(librerias.find(nombre));

			if (it == librerias.end()) return ;

	#if defined __GURUWIN32__
			FreeLibrary(it->second.modulo);
	#elif defined __GURULINUX__
			dlclose(it->second.modulo);
	#endif
			librerias.erase(it);
		}

		::uint16_t cargar(const string& dir="") {

//			if (!librerias.empty()) return static_cast<uint16_t>(librerias.size());
			string directorio;

			if (dir.empty() && directorio_default.empty()) return 0;

			if (dir.empty()) directorio=directorio_default;
			else directorio=dir;

			::uint16_t total=0;
			GuruDirectorio::iterator itfinal;

			for (GuruDirectorio::iterator it(directorio, "func*.dll");it!=itfinal;++it) {
	#if defined __GURUWIN32__
		#if defined __GURUUNICODE__
				HMODULE modulo = LoadLibraryEx(GuruStringAlgo::ansi_unicode(directorio + it->nombre()).c_str(), NULL, 0);
		#else
				HMODULE modulo=LoadLibraryEx((string(directorio) + string(it->nombre())).c_str(),NULL,0);
		#endif
	#elif defined __GURULINUX__
				void* modulo=dlopen((string(directorio) + string(it->nombre())).c_str(), RTLD_LAZY);
	#endif

				if (modulo) {
					ptr_funciones init;
	#if defined __GURUWIN32__
					init=(ptr_funciones)GetProcAddress(modulo, "ptr_funciones");
	#elif defined __GURULINUX__
					init = (ptr_funciones)dlsym(modulo, "ptr_funciones");
	#endif
					if (init) {
						struct lib data;

						data.modulo = modulo;
						data.inicializacion=init;

						librerias.insert(make_pair(it->nombre(), data));
						total+=1;
					}
				} else cout << GuruError().descripcion() << endl;
			}

			return total;
		};

		void descargar() {
			for (map<string, struct lib>::iterator it=librerias.begin(),end=librerias.end();it!=end;++it){
				#if defined __GURUWIN32__
						FreeLibrary(it->second.modulo);
				#elif defined __GURULINUX__
						dlclose(it->second.modulo);
				#endif
//						librerias.erase(it);
			}
		}

		void cargarModulo (const string& nombre, ptr_funciones ptr) {
			struct lib data;

			data.modulo = NULL;
			data.inicializacion=ptr;

			librerias.insert(make_pair(nombre, data));
		}

		vector<GuruInterprete::IGuruFuncion*>& cargar_funciones (vector<GuruInterprete::IGuruFuncion*>& func) {

			for (map<string, struct lib>::iterator it=librerias.begin(),end=librerias.end();it!=end;++it)
				func.push_back((it->second.inicializacion)());

			return func;
		}

		void directorio_libs(const string& dir) {directorio_default=dir;};
		string& directorio_libs() {return directorio_default;};

		class iterator {
		public:
			iterator(GuruCargaLibs& esto, map<string, struct lib>::iterator it):esto(esto),it(it) {};
			virtual ~iterator() {};

			ptr_funciones funcion_init() { return it->second.inicializacion;};
			GuruInterprete::IGuruFuncion* funciones() { return (it->second.inicializacion)();};
			string libreria() {return it->first;};

			bool operator != (const iterator& it1) { return (it1.it != it) ? true : false; }

			void operator++ () { ++it; }

		protected:
			GuruCargaLibs& esto;
			map<string, struct lib>::iterator it;
		};

		iterator begin() { return iterator(*this, librerias.begin());};
		iterator end() { return iterator(*this, librerias.end());};

	protected:
		map<string, struct lib> librerias;
		string					directorio_default;
	};
}
#endif
