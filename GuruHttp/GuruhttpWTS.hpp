#ifndef __GURUHTTPWTS_HPP__
#define __GURUHTTPWTS_HPP__

#include <GuruWTS.h>
#include "GuruhttpREST.hpp"

#include <list>
#include <vector>

// /sesiones
// /sesiones/frossi
// /procesos
// /procesos/byusername/frossi
// /procesos/bysessionid/4
// /procesos/byprocessid/34212
// /procesos/user/sesion/nombreproceso/idproceso
// /informacion
// /version
class GuruHttpWTS : public GuruHttpREST {
public:
	GuruHttpWTS() {}
	virtual ~GuruHttpWTS() {}

	virtual void procesarGet() {
		init();

		if (equal(0, "sesiones")) { // /sesiones
			switch (totalsubdir()) {
			case 1: // /sesiones
				sesiones();
				break;
			case 2: // /sesiones/frossi
				break;
			default:
				throw GuruhttpExcepcion(400);
			}
		} else if (equal(0, "informacion")) {
			info();
		} else if (equal(0, "procesos")) { // procesos
			switch (totalsubdir()) {
			case 1: // /procesos
				procesos();
				break;
			case 2: // /procesos/byxxx
				throw GuruhttpExcepcion(400);
				break;
			case 3: // /procesos/byxxx/xyz - procesos(DWORD sessionid=-1, const string& username="", DWORD processid=-1)
				if (subdir(1).compare("byusername") == 0) {
					procesos(-1, subdir(2), -1);
				} else if (subdir(1).compare("bysessionid") == 0) {
					procesos(atoi(subdir(2).c_str()), "", -1);
				} else if (subdir(1).compare("byprocessid") == 0) {
					procesos(-1, "", atoi(subdir(2).c_str()));
				} else throw GuruhttpExcepcion(400);
				break;
			default:
				throw GuruhttpExcepcion(400);
			}
		} else if (equal(0, "version")) {
			version();
		} else 
			throw GuruhttpExcepcion(400);
	};

	void sesiones() {
		GuruWTS wts;

		int campos[] = { WTSUserName, WTSClientAddress, WTSConnectState, WTSSessionId, WTSWinStationName, WTSClientDirectory, NULL };

		vector<GuruWTS::sesion> info_sesiones;
		
		wts.sesiones(info_sesiones, string(), campos);

		Json::Value		root;

		for (vector<GuruWTS::sesion>::iterator it = info_sesiones.begin(), itend = info_sesiones.end(); it != itend; ++it) {

			if (it->WTSUserName().empty()) continue;

			Json::Value juni;

			juni["username"] = it->WTSUserName();
			juni["state"] = wts.estado_a_str(it->WTSConnectState());
			juni["id"] = static_cast<Json::UInt64>(it->WTSSessionId());
			juni["winstationname"] = it->WTSWinStationName();
//			juni["address"] = it->WTSClientAddress();
//			juni["WTSClientDirectory"] = it->WTSClientDirectory();
			

			root.append(juni);
		}

		if (root.empty()) throw GuruhttpExcepcion(404);

		enviar_json(root.toStyledString());
	}

	void procesos(DWORD sessionid=-1, const string& username="", DWORD processid=-1) {

		GuruWTS		wts;
		Json::Value	root;

		vector<GuruWTS::proceso> info_procesos;
		
		wts.procesos(info_procesos, string(), sessionid, username, processid);

		for (vector<GuruWTS::proceso>::iterator it = info_procesos.begin(), itend = info_procesos.end(); it != itend; ++it) {
			Json::Value juni;

			juni["process"] = it->nombre_proceso();
			juni["idsession"] = static_cast<Json::UInt64>(it->id_session());
			juni["idprocess"] = static_cast<Json::UInt64>(it->id_proceso());
			juni["username"] = it->nombre_usuario();

			root.append(juni);
		}

		if (root.empty()) throw GuruhttpExcepcion(404);

		enviar_json(root.toStyledString());
	}

	void info() {
		Json::Value		root;

		root["Ejecutable"] = GuruDirectorio::rutaProceso();
		root["Directorio_Actual"] = GuruDirectorio::directorio_actual();
		root["OS"] = GuruInfo::ver_windows();
		root["Usuario"] = GuruInfo::username();
		root["Computadora"] = GuruInfo::computername();
		root["Compilacion"] = __DATE__ " - " __TIME__;
		root["Version"] = GuruInfo::str_version;
		root["Fecha"] = GuruAlgo::fechaHoraRFC822(time(NULL));

		vector<GuruInfo::unidad> unidades;

		if (!GuruInfo::estadounidades(unidades).empty()) {
			for (vector<GuruInfo::unidad>::iterator it = unidades.begin(), itend = unidades.end(); it != itend; ++it) {
				Json::Value juni;

				juni["Unidad"] = it->nombre;
				juni["Libre"] = it->libre;
				juni["Total"] = it->total;

				root["Unidades"].append(juni);
			}
		}

		{
			GuruInfo::memoria mem;
			estadomemoria(mem);

			Json::Value jmem;

			jmem["Carga"] = mem.cargaMemoria;

			jmem["TotalFisica"] = mem.totalFisica;
			jmem["DisponibleFisica"] = mem.disponibleFisica;

			jmem["TotalPageFile"] = mem.totalPageFile;
			jmem["DisponiblePageFile"] = mem.disponiblePageFile;

			jmem["TotalVirtual"] = mem.totalVirtual;
			jmem["DisponibleVirtual"] = mem.disponibleVirtual;

			root["Memoria"] = jmem;
		}

		enviar_json(root.toStyledString());
	}

	void version() {
		Json::Value		root;

		root["Compilacion"] = __DATE__ " - " __TIME__;
		root["Version"] = GuruInfo::str_version;

		enviar_buffer(root.toStyledString(), "application/json");
	}

};

#endif
