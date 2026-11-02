#ifndef __GURUHTTPSESSION_HPP__
#define __GURUHTTPSESSION_HPP__

#include <GuruAcceso.hpp>
#include <GuruString_algo.hpp>
#include <string>

class httpSessiones {
public:
	httpSessiones() {};
	virtual ~httpSessiones() {};

	string nueva() {
		map<string, string> datasesion;
		string id_sesion(GuruStringAlgo::rand_str());

		acceso.bloquear();

		sesiones.insert(make_pair(id_sesion, datasesion));

		acceso.desbloquear();

		return id_sesion;
	}

	map<std::string, std::string>& data_sesion(std::string id, map<std::string, std::string>& data) {
		data.clear();

		map<std::string, map<std::string, std::string> >::iterator it=sesiones.find(id);

		if (it == sesiones.end()) return data;

		data = it->second;

		return data;
	};

	void agregar(const string& id, const string& key, const string& value) {
		acceso.bloquear();

		map<std::string, map<std::string, std::string> >::iterator it=sesiones.find(id);

		if (it != sesiones.end())
			it->second.insert(make_pair(key, value));

		acceso.desbloquear();
	}

private:
	// cada string representa un id(sesion) con sus variables y valores
	map<std::string, map<std::string, std::string> > sesiones;

	GuruBloqueo	acceso;
};

#endif
