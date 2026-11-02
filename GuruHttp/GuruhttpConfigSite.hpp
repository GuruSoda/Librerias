#ifndef __GURUHTTPCONFIGSITE_HPP__
#define __GURUHTTPCONFIGSITE_HPP__

#include "GuruhttpREST.hpp"

class GuruHttpConfigSite : public GuruHttpREST {
public:
	GuruHttpConfigSite() {}
	virtual ~GuruHttpConfigSite() {}

	virtual void procesarGet() {
		map <string, vector<pair<string, GuruhttpDirVirtual> > >& a = GuruSingleton<GuruhttpConfig>::instancia()->map_sitios();

		Json::Value infoSitios;

		for (map <string, vector<pair<string, GuruhttpDirVirtual> > >::iterator it = a.begin(), itend = a.end(); it != itend; ++it) {
			Json::Value sitio;

			sitio["host"] = it->first;

			for (vector<pair<string, GuruhttpDirVirtual> >::iterator it2 = it->second.begin(), it2end = it->second.end(); it2 != it2end; ++it2) {
				Json::Value confSitio;

				confSitio["nombre"] = it2->second.sitio();
				confSitio["listar"] = it2->second.listar();
				confSitio["dirApps"] = it2->second.dirApps();
				confSitio["velocidad"] = it2->second.velocidad();
				confSitio["libreria"] = it2->second.lib();
				confSitio["libreria_interfaz"] = it2->second.intefaz_lib();

				Json::Value hosts;
				for (vector<string>::iterator it5 = it2->second.sacarHost().begin(), it5end = it2->second.sacarHost().end(); it5 != it5end; ++it5) {
					hosts.append(*it5);
				}
				confSitio["hosts"] = hosts;

				Json::Value env;
				for (std::map<std::string, std::string>::iterator it3 = it2->second.env().begin(), it3end = it2->second.env().end(); it3 != it3end; ++it3) {
					env[it3->first] = it3->second;
				}
				confSitio["env"] = env;

				Json::Value busquedas; 
				for (vector<string>::iterator it4 = it2->second.sacarBusqueda().begin(), it4end = it2->second.sacarBusqueda().end(); it4 != it4end; ++it4) {
					busquedas.append(*it4);
				}
				confSitio["busqueda"] = busquedas;

				Json::Value logs;
				for (std::map<std::string, std::string>::iterator it6 = it2->second.sacarLog().begin(), it6end = it2->second.sacarLog().end(); it6 != it6end; ++it6) {
					logs[it6->first] = it6->second;
				}
				confSitio["logs"] = logs;

				sitio["sitios"].append(confSitio);
			}

			infoSitios.append(sitio);
		}

		enviar_json(infoSitios.toStyledString());
	};
};

#endif
