#ifndef __GURUHTTPJSONALGO_HPP__
#define __GURUHTTPJSONALGO_HPP__

#include <jsoncpp/json.h>
#include <GuruInfo.hpp>

namespace GuruHttpJSONAlgo {

#if defined __GURUWIN32__
	template <typename T>
	T& infoSistemaOperativo(T& root) {
		root["Ejecutable"] = GuruDirectorio::rutaProceso();
		root["Directorio_Actual"] = GuruDirectorio::directorio_actual();
		root["OS"] = GuruInfo::ver_windows();
		root["Usuario"] = GuruInfo::username();
		root["Computadora"] = GuruInfo::computername();
		root["Compilacion"] = __DATE__ " - " __TIME__;
		root["Version"] = GuruInfo::str_version;
		root["FechaRFC822"] = GuruAlgo::fechaHoraRFC822(time(NULL));
		root["FechaEPOCH"] = GuruStringAlgo::itos(time(NULL));

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

		return root;
	}

#endif
}

#endif
