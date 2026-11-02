#ifndef __GURUMAMEUTIL_H__
#define __GURUMAMEUTIL_H__

#include <GuruMameListInfo.h>
#include <ModeloMame.h>
#include <ModeloLocalRoms.h>

class EncuentraRom {
public:
	EncuentraRom(const rom& copia):_rom(copia){};

	bool existe(vector<rom>& listrom){
		for (vector<rom>::iterator it=listrom.begin();it!=listrom.end();++it){
			if (_rom == *it) return true;
		}

		return false;
	};

	string buscaRom(map<string, vector<rom> >& maprom){
		for (map<string, vector<rom> >::iterator it=maprom.begin();it!=maprom.end();++it){
			for (vector<rom>::iterator it2=it->second.begin();it2!=it->second.end();++it2)
				if (_rom == *it2) return string(it->first);
		}

		return string();
	};
/*
	long buscaRom(map<rom, long>& maprom){
		for (map<rom, long>::iterator it=maprom.begin();it!=maprom.end();++it){
				if (_rom == it->first) return it->second;
		}

		return 0;
	};
*/
	rom buscaHash(vector<rom>& listrom){
		for (vector<rom>::iterator it=listrom.begin();it!=listrom.end();++it){
			if (_rom.comparar_sin_nombre(*it)) return *it;
		}

		return rom();
	};

	pair<string, string> buscaHash(map<string, vector<rom> >& listrom){
		for (map<string, vector<rom> >::iterator it=listrom.begin();it!=listrom.end();++it){
			for (vector<rom>::iterator it2=it->second.begin();it2!=it->second.end();++it2)
				if (_rom.comparar_sin_nombre(*it2)) return make_pair(it->first,it2->nombre);
		}

		return make_pair(string(), string());
	};

private:
	rom _rom;
};

class ExisteRom{
public:
	ExisteRom(const rom& copia):_rom(copia){};

	bool operator()(const rom& rom1){
		return (_rom == rom1);
	}

	bool operator()(const std::pair<string, vector<rom> >& juego){
		vector<rom>::const_iterator it=find_if(juego.second.begin(), juego.second.end(), ExisteRom(_rom));
		if (it!=juego.second.end()) return true;

		return false;
	}

private:
	rom _rom;
};

class ExisteHash{
public:
	ExisteHash(const rom& copia):_rom(copia){};

	bool operator()(const rom& rom1){
		if (_rom.crc != rom1.crc) return false;
		if (_rom.sha1 != rom1.sha1) return false;

		return true;
	}

	bool operator()(const std::pair<string, vector<rom> >& juego){
		vector<rom>::const_iterator it=find_if(juego.second.begin(), juego.second.end(), ExisteHash(_rom));
		if (it!=juego.second.end()) return true;

		return false;
	}

private:
	rom _rom;
};

class ExisteNombre{
public:
	ExisteNombre(const rom& copia):_rom(copia){};

	bool operator()(const rom& rom1){
		if (_rom.nombre != rom1.nombre) return false;

		return true;
	}

	bool operator()(const std::pair<string, vector<rom> >& juego){
		vector<rom>::const_iterator it=find_if(juego.second.begin(), juego.second.end(), ExisteNombre(_rom));
		if (it!=juego.second.end()) return true;

		return false;
	}

private:
	const rom& _rom;
};


class VerificaRoms{
public:
	enum comparacion {
		igual=0,				// Las 2 roms son iguales.
		distinto_nombre,		// Tiene distinto nombre pero sus hash's y tamanio son iguales.
		falta,					// No existe la rom.
		otro_archivo,			// La rom le pertenece a otro juego.
		otro_archivo_nombre};	// La rom le pertenece a otro juego y con otro nombre.

	class estado {
	public:
		estado(const string& nombre_rom, const string& nombre_archivo, enum comparacion estado_comparacion):
				_estado_rom(estado_comparacion), _original(nombre_rom), _archivo(nombre_archivo){};

		string& nombre() {return _original;};
		string& archivo() {return _archivo;};
		comparacion estadoRom() {return _estado_rom;};

	private:
		comparacion _estado_rom;
		string _original;
		string _archivo;
	};

public:
	VerificaRoms(const string& listinfo, const string& romslocales=""):mame(listinfo),misroms(romslocales) {};

	int verificar(const string& juego, vector<rom>& roms2, vector<pair<rom, estado> >& resultado) {
		int incorrectos = 0;

		vector<rom> roms_juego = mame.sacar_roms(juego);

		if (roms_juego.size() == 0) throw string("Juego Inexistente.");

		resultado.clear();
		for (vector<rom>::iterator it=roms_juego.begin();it!=roms_juego.end();++it) {
				vector<rom>::iterator itmame;

				// busco el archivo exacto(nombre, crc, sha1, etc)
				itmame =find_if(roms2.begin(), roms2.end(), ExisteRom(*it));
				if (itmame!=roms2.end()){
					resultado.push_back(make_pair(*it, estado("","", igual)));
					continue;
				}

				// busco por el hash en el mismo archivo, puede tener otro nombre.
				itmame = find_if(roms2.begin(),roms2.end(), ExisteHash(*it));
				if (itmame!=roms2.end()) {
					resultado.push_back(make_pair(*it, estado(itmame->nombre,"", distinto_nombre)));
					incorrectos++;
					continue;
				}

				if (!misroms.bdd().empty()){
					pair<string, string> res;
					res=misroms.buscar_rom("", "", it->tamanio, it->crc, it->sha1);
					// first = nombre del archivo .zip con la ruta incluida.
					// second = nombre de la rom encontrada.

					// pregunto si hay datos donde va el archivo zip donde encontre la rom
					if (!res.first.empty()){
						if (res.second.compare(it->nombre) == 0) // tiene el mismo nombre que el que busco?
							resultado.push_back(make_pair(*it, estado(res.second,res.first, otro_archivo)));
						else
							resultado.push_back(make_pair(*it, estado(res.second,res.first, otro_archivo_nombre)));

						incorrectos++;
						continue;
					}
				}

				resultado.push_back(make_pair(*it, estado("","", falta)));
				incorrectos++;
		}

		return incorrectos;
	};
private:
	ModeloMame			mame;
	ModeloLocalRoms		misroms;
};

#endif
