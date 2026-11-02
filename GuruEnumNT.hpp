#ifndef __GURUENUMNT_HPP__
#define __GURUENUMNT_HPP__

//////////////////////////////////////////////////////////////////////
//	Link: Netapi32.lib
//////////////////////////////////////////////////////////////////////

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

/* // otra opcion para lo anterior
#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <windows.h>
#include <winsock2.h>
*/

#include <windows.h>
#include <lm.h>

#include <string>

namespace GuruEnumNT {
/*
	Las clases terminadas en "ex" llaman a la funcion enum del operativo, es mas lenta pero suele traer mas informacion.
	Las clases NO terminadas en "ex" son mas rapidas.

	Clases:
		iterator_enum_usuarios
		iterator_enum_usuarios_ex
		iterator_enum_usuarios_en_grupo_ex
		iterator_enum_grupos
		iterator_enum_grupos_ex
		iterator_enum_maquinas
		iterator_enum_server_ex

Ejemplo de uso:
// Ejemplo de rapida carga de usuarios, utiliza la funcion NetQueryDisplayInformation
	GuruEnumNT::iterator_enum_usuarios it("\\\\CD0003");
	GuruEnumNT::iterator_enum_usuarios itfin;

	for (;it!=itfin;++it) wcout << it->usri1_name << endl;

// Ejemplo donde carga mas informacion, pero es mas lento.
	GuruEnumNT::iterator_enum_grupos_ex<GROUP_INFO_0, 0> it("\\\\CD0003");
	GuruEnumNT::iterator_enum_grupos_ex<GROUP_INFO_0, 0> itfin;

	for (;it!=itfin;++it) wcout << it->grpi0_name << endl;

// Ejemplo clasico realizado solo por compatibilidad.
	GuruUsuariosNT<USER_INFO_1, 1> u("\\\\CD0003");

	for (GuruUsuariosNT<USER_INFO_1, 1>::iterator it=u.begin();it!=u.end();++it) wcout << it->usri1_name << endl;
*/

	template <typename T, DWORD nivel_info>
	class iterator_enum {
	public:
		virtual void cargar_info() = 0;

		iterator_enum(const string& server):server(server),ejecutando(true) {
			reg = NULL;
			wservidor[0] = NULL;
			buffer = NULL;
			cargadas = 0;
			total = 0;
			handle = 0;
			procesadas = 0;
			ejecutando = true;

			nivel = nivel_info;
			if (!server.empty())
				MultiByteToWideChar(CP_ACP, 0, server.c_str(), UNCLEN, wservidor, UNCLEN);
			else
				wservidor[0] = NULL;

			procesadas = cargadas = total = handle = 0;
		};

		iterator_enum():ejecutando(false) {
			reg = NULL;
			buffer = NULL;
			wservidor[0] = NULL;
			nivel = nivel_info;
		}

		virtual ~iterator_enum() {
			if (buffer != NULL) NetApiBufferFree(buffer);
		};

		bool operator != (const iterator_enum& it) {
			if (it.ejecutando != this->ejecutando)
				return true;
			else
				return false;
		}

		bool operator == (const iterator_enum& it) {
			if (it.ejecutando == this->ejecutando)
				return true;
			else
				return false;
		}

		virtual void operator++ () {
			if (procesadas < cargadas) {
				reg++;
				procesadas++;
			} else {
				NetApiBufferFree(buffer); buffer = NULL; reg = NULL;
				if (status_enum == ERROR_MORE_DATA)
					cargar_info();
				else
					ejecutando = false;
			}
		}

		T* operator -> () { return reg;} 

	protected:
		string				server;
		DWORD				nivel;
		NET_API_STATUS		status_enum;
		T*					reg;
		WCHAR				wservidor[UNCLEN];
		T*					buffer;
		DWORD				cargadas;
		DWORD				total;
		DWORD				handle;

		DWORD				procesadas;
		bool				ejecutando;
	};

	class iterator_enum_usuarios:public iterator_enum<NET_DISPLAY_USER, 1> {
	public:
		iterator_enum_usuarios(const string& server):iterator_enum<NET_DISPLAY_USER, 1>(server) {
			cargar_info();
		};
		iterator_enum_usuarios():iterator_enum<NET_DISPLAY_USER, 1>() {};
		virtual ~iterator_enum_usuarios() {};

		void cargar_info() {
			status_enum = NetQueryDisplayInformation(wservidor, nivel, handle, (DWORD)-1, MAX_PREFERRED_LENGTH, &cargadas, (LPVOID*)&buffer);

			if ((status_enum == NERR_Success) || (status_enum == ERROR_MORE_DATA)) {
				reg = buffer;
				procesadas=1;
				handle = reg->usri1_next_index;
			}
		}

		virtual void operator++ () {
			if (procesadas < cargadas) {
				reg++;
				procesadas++;
				handle = reg->usri1_next_index;
			} else {
				NetApiBufferFree(buffer);buffer=NULL;
				if (status_enum == ERROR_MORE_DATA)
					cargar_info();
				else
					ejecutando = false;
			}
		}
	};

	class iterator_enum_grupos:public iterator_enum<NET_DISPLAY_GROUP, 3> {
	public:
		iterator_enum_grupos(const string& server):iterator_enum<NET_DISPLAY_GROUP, 3>(server) {
			cargar_info();
		};
		iterator_enum_grupos():iterator_enum<NET_DISPLAY_GROUP, 3>() {};
		virtual ~iterator_enum_grupos() {};

		void cargar_info() {
			status_enum = NetQueryDisplayInformation(wservidor, nivel, handle, (DWORD)-1, MAX_PREFERRED_LENGTH, &cargadas, (LPVOID*)&buffer);

			if ((status_enum == NERR_Success) || (status_enum == ERROR_MORE_DATA)) {
				reg = buffer;
				procesadas=1;
				handle = reg->grpi3_next_index;
			}
		}

		virtual void operator++ () {
			if (procesadas < cargadas) {
				reg++;
				procesadas++;
				handle = reg->grpi3_next_index;
			} else {
				NetApiBufferFree(buffer);buffer=NULL;
				if (status_enum == ERROR_MORE_DATA)
					cargar_info();
				else
					ejecutando = false;
			}
		}
	};

	class iterator_enum_maquinas:public iterator_enum<NET_DISPLAY_MACHINE, 2> {
	public:
		iterator_enum_maquinas(const string& server):iterator_enum<NET_DISPLAY_MACHINE, 2>(server) {
			cargar_info();
		};
		iterator_enum_maquinas():iterator_enum<NET_DISPLAY_MACHINE, 2>() {};

		void cargar_info() {
			status_enum = NetQueryDisplayInformation(wservidor, nivel, handle, (DWORD)-1, MAX_PREFERRED_LENGTH, &cargadas, (LPVOID*)&buffer);

			if ((status_enum == NERR_Success) || (status_enum == ERROR_MORE_DATA)) {
				reg = buffer;
				procesadas=1;
				handle = reg->usri2_next_index;
			}
		}

		virtual void operator++ () {
			if (procesadas < cargadas) {
				reg++;
				procesadas++;
				handle = reg->usri2_next_index;
			} else {
				NetApiBufferFree(buffer);buffer=NULL;
				if (status_enum == ERROR_MORE_DATA)
					cargar_info();
				else
					ejecutando = false;
			}
		}
	};

	template <typename T, DWORD nivel_info, DWORD tam_buffer=MAX_PREFERRED_LENGTH>
	class iterator_enum_usuarios_ex:public iterator_enum<T, nivel_info> {
	public:
		iterator_enum_usuarios_ex(const string& server):iterator_enum<T, nivel_info>(server) {
			cargar_info();
		};
		iterator_enum_usuarios_ex():iterator_enum<T, nivel_info>() {};

		virtual void cargar_info() {
			status_enum = NetUserEnum (
							wservidor,
							nivel,
  							FILTER_NORMAL_ACCOUNT,
							(LPBYTE*)&buffer,
							tam_buffer,
							&cargadas,
							&total,
							&handle
							);

			if ((status_enum == NERR_Success) || (status_enum == ERROR_MORE_DATA)) {
				reg = buffer;
				procesadas=1;
			}
		}
	};

	template <typename T, DWORD nivel_info, DWORD tam_buffer = MAX_PREFERRED_LENGTH>
	class iterator_enum_usuarios_en_grupo_ex:public iterator_enum<T, nivel_info> {
		WCHAR	wgrupo[256];
	public:
		iterator_enum_usuarios_en_grupo_ex(const string& server, const string& grupo) :iterator_enum<T, nivel_info>(server) {
			if (!grupo.empty())
				MultiByteToWideChar(CP_ACP, 0, grupo.c_str(), -1, wgrupo, 256);
			else
				wgrupo[0] = NULL;

			cargar_info();
		};
		iterator_enum_usuarios_en_grupo_ex() :iterator_enum<T, nivel_info>() {};

		virtual void cargar_info() {

			status_enum = NetGroupGetUsers(wservidor, wgrupo, nivel, (LPBYTE*)&buffer, tam_buffer, &cargadas, &total, reinterpret_cast<PDWORD_PTR>(&handle));

			if ((status_enum == NERR_Success) || (status_enum == ERROR_MORE_DATA)) {
				reg = buffer;
				procesadas = 1;
			} else ejecutando = false;

		}
	};

	template <typename T, DWORD nivel_info, DWORD tam_buffer=MAX_PREFERRED_LENGTH>
	class iterator_enum_grupos_ex:public iterator_enum<T, nivel_info> {
	public:
		iterator_enum_grupos_ex(const string& server):iterator_enum<T, nivel_info>(server) {
			cargar_info();
		};
		iterator_enum_grupos_ex():iterator_enum<T, nivel_info>() {};

		virtual void cargar_info() {
			status_enum = NetGroupEnum (
							wservidor,
							nivel,
							(LPBYTE*)&buffer,
							tam_buffer,
							&cargadas,
							&total,
							&handle
							);

			if ((status_enum == NERR_Success) || (status_enum == ERROR_MORE_DATA)) {
				reg = buffer;
				procesadas=1;
			}
		}
	};

	template <typename T, DWORD nivel_info, DWORD tipo_servidor=SV_TYPE_WORKSTATION|SV_TYPE_SERVER, DWORD tam_buffer=MAX_PREFERRED_LENGTH>
	class iterator_enum_server_ex:public iterator_enum<T, nivel_info> {
	public:
		iterator_enum_server_ex(const string& server, const string& dominio):iterator_enum<T, nivel_info>(server) {
			if (!dominio.empty())
				MultiByteToWideChar(CP_ACP, 0, dominio.c_str(), -1, wdominio, DNLEN);
			else
				wdominio[0] = NULL;

			cargar_info();
		};

		iterator_enum_server_ex():iterator_enum<T, nivel_info>() {};

		virtual void cargar_info() {
			status_enum = NetServerEnum (
							wservidor,
							nivel_info,
							(LPBYTE*)&buffer,
							tam_buffer,
							&cargadas,
							&total,
							tipo_servidor,
							wdominio,
							&handle
							);

			if ((status_enum == NERR_Success) || (status_enum == ERROR_MORE_DATA)) {
				reg = buffer;
				procesadas=1;
			}
		}
	protected:
		WCHAR	wdominio[DNLEN];
	};

	template <typename T, DWORD nivel_info, DWORD tam_buffer = MAX_PREFERRED_LENGTH>
	class iterator_enum_share_ex :public iterator_enum<T, nivel_info> {
	public:
		iterator_enum_share_ex(const string& server):iterator_enum<T, nivel_info>(server) {
			cargar_info();
		};
		iterator_enum_share_ex() :iterator_enum<T, nivel_info>() {};

		virtual void cargar_info() {
			status_enum = NetShareEnum(wservidor, nivel, (LPBYTE*)&buffer, tam_buffer, &cargadas, &total, &handle);

			if ((status_enum == NERR_Success) || (status_enum == ERROR_MORE_DATA)) {
				reg = buffer;
				procesadas = 1;
			}
		}
	};

}

#endif
