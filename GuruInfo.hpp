#ifndef __GURUINFO_HPP__
#define __GURUINFO_HPP__

//////////////////////////////////////////////////////////////////////
// Link con User32.lib
//////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <windows.h>

#include <Lmcons.h> // GetUserName
#include <Winuser.h> // GetSystemMetrics 

#include <GuruCompat.h>

namespace GuruInfo {

	static string version_app () {
		static std::string str_version;

		if (!str_version.empty()) return str_version;

		static const char* meses[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
		const string fecha(__DATE__);
		
		str_version = fecha.substr(7,4); // anio

		string mes(fecha.substr(0, 3));

		for (int i = 0; i < 12; i++) {
			if (strcmp(mes.c_str(), meses[i]) == 0) {
				if (i < 10) str_version += '0';
				str_version += GuruStringAlgo::itos(i+1);
				break;
			}
		}

		// dia
		int dia = atoi(fecha.substr(4, 2).c_str());

		if (dia < 10) {
			str_version += '0';
			str_version += GuruStringAlgo::itos(dia);
		} else 
			str_version += fecha.substr(4, 2);

		const string hora(__TIME__);

		str_version += hora.substr(0, 2); // hora
		str_version += hora.substr(3, 2); // minuto
		str_version += hora.substr(6, 2); // segundo

		return str_version;
	}

	// para que se ejecute antes del main y ya tener la cadena preparada
	static const std::string str_version = GuruInfo::version_app ();

#if defined __GURUWIN32__
	struct unidad {
		std::string nombre;

		uint64_t total;
		uint64_t libre;
	};

	static vector<struct unidad>& estadounidades (vector<struct unidad>& unidades) {
		unidades.clear();

		DWORD cantidad		= GetLogicalDriveStrings(0, NULL);
		TCHAR* total_unidades		= NULL;

		total_unidades = new TCHAR[cantidad + 1];

		cantidad = GetLogicalDriveStrings(cantidad, total_unidades);

		TCHAR* inicio = total_unidades;

		while (*total_unidades) {
			UINT tipo = GetDriveType(total_unidades);

			if (tipo == DRIVE_FIXED) {
				std::string linea;
				ULARGE_INTEGER ByteLibres, TotalBytes, TotalBytesLibres;

				if (GetDiskFreeSpaceEx(total_unidades, &ByteLibres, &TotalBytes, &TotalBytesLibres)) {
					struct unidad u;
					u.nombre = GuruStringAlgo::actual_string(total_unidades);
					u.total = (uint64_t) TotalBytes.QuadPart;
					u.libre = (uint64_t) TotalBytesLibres.QuadPart;

					unidades.push_back(u);
				}
			}
#if defined __GURUUNICODE__
			total_unidades += wcslen(total_unidades) + 1;
#else
			total_unidades += strlen(total_unidades) + 1;
#endif
		}

		delete[]inicio;

		return unidades;
	}

	struct memoria {
		int cargaMemoria;

		uint64_t totalFisica;
		uint64_t disponibleFisica;
		uint64_t totalPageFile;
		uint64_t disponiblePageFile;
		uint64_t totalVirtual;
		uint64_t disponibleVirtual;

		void clean() {
			cargaMemoria = 0;
			totalFisica = disponibleFisica = totalPageFile = disponiblePageFile = totalVirtual = disponibleVirtual = 0;
		}
	};

	static void estadomemoria (struct memoria& mem) {

		mem.clean();

#if !defined GURUWINNT40
		MEMORYSTATUSEX		memoria;

		memoria.dwLength = sizeof(MEMORYSTATUSEX);

		GlobalMemoryStatusEx(&memoria);

		mem.cargaMemoria = memoria.dwMemoryLoad;

		mem.totalFisica = (uint64_t)memoria.ullTotalPhys;
		mem.disponibleFisica = (uint64_t)memoria.ullAvailPhys;

		mem.totalPageFile = (uint64_t)memoria.ullTotalPageFile;
		mem.disponiblePageFile = (uint64_t)memoria.ullAvailPageFile;

		mem.totalVirtual = (uint64_t)memoria.ullTotalVirtual;
		mem.disponibleVirtual = (uint64_t)memoria.ullAvailVirtual;
#else
		MEMORYSTATUS		memoria;

		memoria.dwLength = sizeof(MEMORYSTATUS);

		GlobalMemoryStatus(&memoria);

		mem.cargaMemoria = memoria.dwMemoryLoad;

		mem.totalFisica = (uint64_t)memoria.dwTotalPhys;
		mem.disponibleFisica = (uint64_t)memoria.dwAvailPhys;

		mem.totalPageFile = (uint64_t)memoria.dwTotalPageFile;
		mem.disponiblePageFile = (uint64_t)memoria.dwAvailPageFile;

		mem.totalVirtual = (uint64_t)memoria.dwTotalVirtual;
		mem.disponibleVirtual = (uint64_t)memoria.dwAvailVirtual;
#endif
	}

	static std::string computername () {
		TCHAR	nombre[MAX_COMPUTERNAME_LENGTH + 1];
		DWORD	tamanio_buffer = MAX_COMPUTERNAME_LENGTH;

		if (GetComputerName(nombre, &tamanio_buffer))
			return GuruStringAlgo::actual_string(nombre);
		else
			return std::string();
	}

	static std::string username () {
		TCHAR	nombre[UNLEN + 1];
		DWORD	tamanio_buffer = UNLEN;

		if (GetUserName(nombre, &tamanio_buffer))
			return GuruStringAlgo::actual_string(nombre);
		else
			return std::string();
	}

	static std::string ver_windows (){
		OSVERSIONINFOEX VersionInfo = { 0 };

		VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

		std::string resp;

		if (GetVersionEx((OSVERSIONINFO*)&VersionInfo)) {
			switch (VersionInfo.dwMajorVersion) {
			case 3: // Windows NT 3.51				
			case 4: // Windows 95 | Windows 98 | Windows Me | Windows NT 4.0
				switch (VersionInfo.dwMinorVersion) {
				case 0: // Windows 95 | Windows NT 4.0
					if (VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
						resp += "Windows 95";
						if (GuruStringAlgo::actual_string(VersionInfo.szCSDVersion).compare("C") == 0)
							resp += " OSR2";
					} else
						resp += "Windows NT 4.0";
					break;
				case 10: // Windows 98
					resp += "Windows 98";

					if (GuruStringAlgo::actual_string(VersionInfo.szCSDVersion).compare("A") == 0)
						resp += " Second Edition";
					break;
				case 90: // Windows Me
					resp += "Windows Me";
					break;
				case 51: // Windows NT 3.51
					resp += "Windows NT 3.51 ";
					break;
				}

				break;
			case 5: // Windows 2000 | Windows XP | Windows Server 2003 family
				switch (VersionInfo.dwMinorVersion) {
				case 0: // Windows 2000
					resp += "Windows 2000";

					if (VersionInfo.wProductType == VER_NT_WORKSTATION)
						resp += " Professional";
					else {
						if (VersionInfo.wSuiteMask & VER_SUITE_DATACENTER)
							resp += " Datacenter Server";
						else if (VersionInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
							resp += " Advanced Server";
						else
							resp += " Server";
					}
					break;

				case 1: // Windows XP
					resp += "Windows XP";

					if (VersionInfo.wSuiteMask & VER_SUITE_PERSONAL)
						resp += " Home Edition";
					else
						resp += " Professional";
					break;

				case 2: // Windows Server 2003 family
					if (GetSystemMetrics(SM_SERVERR2) == 0)
						resp += "Windows Server 2003";
					else
						resp += "Windows Server 2003 R2";
					break;
				}
				break;
			case 6:
				switch (VersionInfo.dwMinorVersion) {
				case 0:
					if (VersionInfo.wProductType == VER_NT_WORKSTATION)
						resp += "Windows Vista";
					else
						resp += "Windows Server 2008";
					break;
				case 1:
					if (VersionInfo.wProductType == VER_NT_WORKSTATION)
						resp += "Windows 7";
					else
						resp += "Windows Server 2008 R2";
					break;
				case 2:
					if (VersionInfo.wProductType == VER_NT_WORKSTATION)
						resp += "Windows 8";
					else
						resp += "Windows Server 2012";
					break;
				case 3:
					if (VersionInfo.wProductType == VER_NT_WORKSTATION)
						resp += "Windows 8.1";
					else
						resp += "Windows Server 2012 R2";
					break;
				}

				break;
			case 10:
				switch (VersionInfo.dwMinorVersion) {
				case 0:
					if (VersionInfo.wProductType == VER_NT_WORKSTATION)
						resp += "Windows 10";
					else
						resp += "Windows Server 2016";
					break;
				}

				break;
			}

			SYSTEM_INFO si = { 0 };
			GetSystemInfo(&si);

			if (VersionInfo.dwMajorVersion >= 5) {
				if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					resp += ", 64bits";
				else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
					resp += ", 32bits";
			}

			if (!GuruStringAlgo::actual_string(VersionInfo.szCSDVersion).empty())
				resp += " (" + std::string(VersionInfo.szCSDVersion) + ")";

			resp += ", Compilacion " + GuruStringAlgo::itos(VersionInfo.dwBuildNumber);

			//		informacion.SistemaOperativo.insertar(1, '"');
			//		informacion.SistemaOperativo.insertar(strlen(informacion.SistemaOperativo.contenido())+1, '"');
		}

		return resp;
	}
#endif // __GURUWIN32__
}

#endif
