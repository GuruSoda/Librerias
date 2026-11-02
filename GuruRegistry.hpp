#if !defined __GURUREGISTRY_HPP__
#define __GURUREGISTRY_HPP__

//////////////////////////////////////////////////////////////////////
// Link con Advapi32.lib
//////////////////////////////////////////////////////////////////////
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <windows.h>


#include <stdlib.h>
#include <vector>

#include <GuruCompat.h>
#include <GuruError.hpp>

using namespace std;

typedef	struct {
			string		nombre;
			uint16_t	cant_valores;
			SYSTEMTIME	fecha;
		}GuruRegistry_Reg;

#ifdef __GURUWIN32__
	#if defined __GURUUNICODE__
		#define GURUREG_CONVERT(x) GuruStringAlgo::ansi_unicode(x)
	#else
		#define GURUREG_CONVERT(x) x
	#endif
#elif defined __GURULINUX__
	#define CONVERT(x) x
#endif

class GuruRegistry {
public:
	GuruRegistry() {};
	virtual ~GuruRegistry() {};

	bool	existe(const std::string& key, const std::string& rama) {
		HKEY	Handle_Key;
		LONG	salida;

		if ((salida = RegOpenKeyEx(hKey(key), GURUREG_CONVERT(rama).c_str(), 0, KEY_READ, &Handle_Key))!=ERROR_SUCCESS) {
			Guru_Reg_Error.descripcion(salida);
			return false;
		} else {
			RegCloseKey(Handle_Key);
			return true;
		}
	};

	bool	escribir(const std::string& key, const std::string& rama, const std::string& valor, const std::string& cadena, uint32_t tipo = REG_SZ) {
		LONG salida;

		acceso = KEY_WRITE;

		if (abrir(key,rama)) {
			if ((salida = RegSetValueEx(Guru_Handle_Key, GURUREG_CONVERT(valor).c_str(), 0, tipo, (const BYTE*)cadena.c_str(), (DWORD)cadena.size())) != ERROR_SUCCESS) {
				Guru_Reg_Error.descripcion(salida);
				return false;
			} else {
				cerrar();
				return true;
			}
		} else
			return false;
	};

	bool	escribir(const std::string& key, const std::string& rama, const std::string& valor,uint32_t numero) {
		LONG	salida;

		acceso = KEY_WRITE;

		if (abrir(key,rama)) {
			if ((salida = RegSetValueEx(Guru_Handle_Key, GURUREG_CONVERT(valor).c_str(), 0, REG_DWORD, (const unsigned char *)&numero, sizeof(DWORD))) != ERROR_SUCCESS) {
				Guru_Reg_Error.descripcion(salida);
				return false;
			} else {
				cerrar();
				return true;
			}
		} else
			return false;
	};

	bool	escribir_encriptado(const std::string& key, const std::string& rama, const std::string& valor, std::string& cadena) {
		char buffer[256];

		cadena = buffer;

		if (!escribir(key, rama, valor, buffer))
			return false;
		else
			return true;
	};

	bool	leer(const std::string& key, const std::string& rama, const std::string& valor, std::string& cadena) {
		char	buffer[255];
		DWORD	tipo_data = REG_MULTI_SZ;
		DWORD	largo_buffer=sizeof(buffer);

		acceso	= KEY_READ; // Permission for read access.

		if (abrir(key,rama)) {
			if (RegQueryValueEx(Guru_Handle_Key, GURUREG_CONVERT(valor).c_str(), 0, &tipo_data, (LPBYTE) buffer, &largo_buffer) != ERROR_SUCCESS) {
				Guru_Reg_Error.descripcion(GetLastError());
				return false;
			} else {
				cadena = buffer;
				cerrar();
				return true;
			}
		} else
			return false;
	};

	bool	leer(const std::string& key, const std::string& rama, const std::string& valor, uint32_t *numero) {
		DWORD	tipo_data = REG_MULTI_SZ;
		DWORD	largo_buffer=sizeof(REG_DWORD);

		acceso	= KEY_READ; // Permission for read access.

		if (abrir(key,rama)) {
			if (RegQueryValueEx(Guru_Handle_Key, GURUREG_CONVERT(valor).c_str(), 0, &tipo_data, (LPBYTE) numero, &largo_buffer) != ERROR_SUCCESS) {
				Guru_Reg_Error.descripcion(GetLastError());
				return false;
			} else {
				cerrar();
				return true;
			}
		} else
			return false;
	};

	bool	leer_encriptado(const std::string& key, const std::string& rama, const std::string& valor, std::string& cadena) {
		string buffer;

		if (!leer(key, rama, valor, buffer))
			return false;
		else {
			desencriptar(buffer,cadena);
			return true;
		}
	};

	bool	guardar_archivo(const std::string& key, const std::string& rama, const std::string& valor, const std::string& nombre_archivo) {
		char*	data;
		HANDLE	archivo;
		DWORD	cant_bytes,leidos=0;
		bool	salida = true;

		archivo = CreateFile(GURUREG_CONVERT(nombre_archivo).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);

		if (archivo == INVALID_HANDLE_VALUE) {
			Guru_Reg_Error.descripcion();
			salida = false;
		} else {
			cant_bytes = GetFileSize(archivo, NULL);

			data = new char [cant_bytes+1];

			if (abrir(key,rama)) {
				if (ReadFile(archivo, data, cant_bytes, &leidos, NULL)) {
					if (RegSetValueEx(Guru_Handle_Key, GURUREG_CONVERT(valor).c_str(), 0, REG_BINARY, (const unsigned char *) data, cant_bytes) != ERROR_SUCCESS) {
						Guru_Reg_Error.descripcion(GetLastError());
						salida = false;
					} else cerrar();
				}
			} else salida = false;

			delete [] data;
			}

		return salida;
	};

	bool	recuperar_archivo(const std::string& key, const std::string& rama, const std::string& valor, const std::string& nombre_archivo) {
		bool	salida = true;
		char*	data;
		HANDLE	archivo;
		DWORD	cant_bytes,escritos=0,tipo_dato;

		DWORD	SubKeys;             // number of subkeys
		DWORD	MaxSubKeyLen;        // longest subkey name
		DWORD	MaxClassLen;         // longest class string
		DWORD	Values;              // number of value entries
		DWORD	MaxValueNameLen;     // longest value name
		DWORD	MaxValueLen;         // longest value data

		if (abrir(key,rama)) {
			if (RegQueryInfoKey(Guru_Handle_Key,
							NULL,					// class buffer
							NULL,					// size of class buffer
							NULL,					// reserved
							&SubKeys,				// number of subkeys
							&MaxSubKeyLen,			// longest subkey name
							&MaxClassLen,			// longest class string
							&Values,				// number of value entries
							&MaxValueNameLen,		// longest value name
							&MaxValueLen,			// longest value data *
							NULL,					// descriptor length
							NULL
							) == ERROR_SUCCESS)
				{

				archivo = CreateFile(GURUREG_CONVERT(nombre_archivo).c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);

				if (archivo == INVALID_HANDLE_VALUE) {
					Guru_Reg_Error.descripcion();
					salida = false;
				} else {
					cant_bytes = MaxValueLen;

					data = new char [cant_bytes+1];

					if (RegQueryValueEx(Guru_Handle_Key, GURUREG_CONVERT(valor).c_str(), 0, &tipo_dato, (unsigned char *)data, &cant_bytes) == ERROR_SUCCESS) {
						if (WriteFile(archivo, data, cant_bytes, &escritos, NULL) == 0) {
							Guru_Reg_Error.descripcion(GetLastError());
							salida = false;
						}
					} else {
						Guru_Reg_Error.descripcion(GetLastError());
						salida = false;
					}

					delete [] data;
					CloseHandle(archivo);
				}

				cerrar();
			}
		}

		return salida;
	};

	vector<GuruRegistry_Reg>& ls_lista_valores(const string& key, const string& rama, vector<GuruRegistry_Reg>& valores) {
		uint16_t cant_valores=0;

		GuruValores.clear();

		if (!abrir(key.c_str(), rama.c_str())) {
			Guru_Reg_Error.descripcion();
			return GuruValores;
		}

		uint16_t	indice=0;
		TCHAR		nombre[255];
		DWORD		tamanio_nombre=sizeof(nombre);

		TCHAR		clase[255];
		DWORD		tamanio_clase=sizeof(clase);

		FILETIME	fecha;

		GuruRegistry_Reg	reg_gurureg;

		long	salida;


		do{
			salida = RegEnumKeyEx(Guru_Handle_Key, indice, nombre, &tamanio_nombre,	NULL, clase, &tamanio_clase, &fecha);

			if (salida != ERROR_SUCCESS) {
				Guru_Reg_Error.descripcion();
				break;
			}

			cant_valores++;
			indice++;
#if defined __GURUUNICODE__
			reg_gurureg.nombre = GuruStringAlgo::unicode_ansi(nombre);
#else
			reg_gurureg.nombre = nombre;
#endif

			FileTimeToSystemTime(&fecha,&reg_gurureg.fecha);

			GuruValores.push_back(reg_gurureg);

			tamanio_nombre=sizeof(nombre);
			tamanio_clase=sizeof(clase);

		}while (salida == ERROR_SUCCESS);

		cerrar();

		return GuruValores;
	};

	const std::string	mensaje_error() { return Guru_Reg_Error.mensaje(); }

private: // (miembros)

	bool	abrir (const std::string& key, const std::string& rama) {
		DWORD	disposicion;
		long	salida;

		salida = RegCreateKeyEx(hKey(key), GURUREG_CONVERT(rama).c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, acceso, NULL, &Guru_Handle_Key, &disposicion);

		if (salida != ERROR_SUCCESS) {
			Guru_Reg_Error.descripcion(salida);

			salida = REG_CREATED_NEW_KEY;
			salida = REG_OPENED_EXISTING_KEY;

			return false;
		} else
			return true;
	};

	bool	cerrar() {
		long	salida;

		if ((salida = RegCloseKey(Guru_Handle_Key)) != ERROR_SUCCESS) {
			Guru_Reg_Error.descripcion(salida);
			return false;
		} else
			return true;
	};

	void	encriptar(const std::string& cadena_limpia, std::string& str_encriptada) {
		char	*cadena_temp = new char [(cadena_limpia.size()*8)+1];
		char	caracter[10];
		char	cadena_encriptada[256];

		strcpy(cadena_encriptada, str_encriptada.c_str());

		strcpy(cadena_encriptada, cadena_limpia.c_str());

		strrev(cadena_encriptada);

		strcpy(cadena_temp,"");

		for (size_t i=0;i<cadena_limpia.size();i++) {
			sprintf(caracter,"%04d",cadena_encriptada[i]);
			strcat(cadena_temp,caracter);

			sprintf(caracter,"%02d",(char)(rand() % 5) + 32);
			strcat(cadena_temp,caracter);
		}

		strcat(cadena_temp,"\n");

		str_encriptada = cadena_temp;

		delete [] cadena_temp;
	};

	void	desencriptar(const std::string& str_encriptada, std::string& cadena_limpia) {
		char *	cadena_temp = new char [cadena_limpia.size()+1];
		int		caracter,i=0;
		char	cadena_encriptada[256];
		char*	inicio = &cadena_encriptada[0];

		while (*inicio) {
			strncpy(cadena_temp,cadena_encriptada,4);
			cadena_temp[4] = '\0';

			caracter = atoi(cadena_temp);

			cadena_limpia[i++] = (char) caracter;
			inicio += 6;
		}

//		_strrev(cadena_limpia);

		delete [] cadena_temp;
	};

	HKEY	hKey (const std::string& key) {
		if (key.compare("HKLM") == 0)
			return HKEY_LOCAL_MACHINE;
		else if (key.compare("HKCU") == 0)
				return HKEY_CURRENT_USER;
		else if (key.compare("HKCC") == 0)
				return HKEY_CURRENT_CONFIG;
		else if (key.compare("HKU") == 0)
				return HKEY_USERS;
		else if (key.compare("HKCR") == 0)
				return HKEY_CLASSES_ROOT;
		else
			return NULL;
	};

private: // (data)

	HKEY				Guru_Handle_Key;
	GuruError			Guru_Reg_Error;
	vector<GuruRegistry_Reg>	GuruValores;
	REGSAM				acceso;
};

#endif
