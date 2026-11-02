#ifndef __GURUMAMELISTINFO_H__
#define __GURUMAMELISTINFO_H__

#include <GuruCompat.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

class rom
{
public:
	rom():tamanio(0),crc("00000000"),sha1("0000000000000000000000000000000000000000") {};
	rom(const rom& copia) {
		this->nombre = copia.nombre;
		this->tamanio = copia.tamanio;
		this->crc = copia.crc;
		this->sha1 = copia.sha1;
		this->unido = copia.unido;
	};

	bool operator == (const rom& rom2) const {
/*
		cout << "Comparando == " << this->nombre << " con " << rom2.nombre << endl;
		cout << "Comparando == " << this->tamanio << " con " << rom2.tamanio << endl;
		cout << "Comparando == " << this->crc << " con " << rom2.crc << endl;
		cout << "Comparando == " << this->sha1 << " con " << rom2.sha1 << endl;
*/
		if (this->nombre != rom2.nombre) return false;
		if (this->tamanio != rom2.tamanio) return false;
		if (this->crc != rom2.crc) return false;
		if (this->sha1 != rom2.sha1) return false;

//		cout << "Iguales..." << endl;
		return true;
	};

	bool operator != (const rom& rom2) const {
		if (this->nombre == rom2.nombre) return false;
		if (this->tamanio == rom2.tamanio) return false;
		if (this->crc == rom2.crc) return false;
		if (this->sha1 == rom2.sha1) return false;

		return true;
	};

	bool operator > (const rom& rom2) const {
//		cout << "Comparando > " << this->nombre << " con " << rom2.nombre << endl;
		// if (this->nombre < rom2.nombre) return false;
		return (this->nombre > rom2.nombre);
		// return true;
	}

	bool operator < (const rom& rom2) const {
//		cout << "Comparando < " << this->nombre << " con " << rom2.nombre << endl;
		// if (this->nombre > rom2.nombre) return false;
		return (this->sha1 < rom2.sha1);
//		return (this->nombre < rom2.nombre);
//		if (this->tamanio == rom2.tamanio) return false;
//		if (this->crc > rom2.crc) return false;
//		if (this->sha1 > rom2.sha1) return false;

		// return true;
	};

	bool comparar_sin_nombre (const rom& rom2) const {
		if (this->tamanio != rom2.tamanio) return false;
		if (this->crc != rom2.crc) return false;
		if (this->sha1 != rom2.sha1) return false;

		return true;
	};

	bool comparar_nombre (const rom& rom2) const {
		if (this->nombre != rom2.nombre) return false;

		return true;
	};

	void clear() {
		nombre.clear();
		tamanio = 0;
		crc.clear();
		sha1.clear();
		unido.clear();
	}

	friend class GuruMameListInfo;

	string		nombre;
	::uint64_t	tamanio;
	string		crc;
	string		sha1;
	string		unido;
};

class juego
{
public:
	juego():anio("0"){};
	juego(const juego& copia){
		nombre = copia.nombre;
		descripcion = copia.descripcion;
		fabricante = copia.fabricante;
		anio = copia.anio;
		rom_de = copia.rom_de;
		clon_de = copia.clon_de;

		lista_roms = copia.lista_roms;
	};

	bool operator == (juego& juego2) {

		for (vector<rom>::iterator it=lista_roms.begin();it!=lista_roms.end();++it){
			bool existe = false;
			for (vector<rom>::iterator it2=juego2.lista_roms.begin();it2!=juego2.lista_roms.end();++it2){
//				cout << endl << "it :" << it->nombre << endl;
//				cout << "it2 :" << it2->nombre << endl;
				if (*it == *it2) {
//					cout << "Iguales ... " << endl;
					existe = true;
					break;
				}
				// else cout << "Distintos ... " << endl;
			}
			if (!existe) return false;
		}

		if (this->nombre != juego2.nombre) return false;
		if (this->descripcion != juego2.descripcion) return false;
		if (this->fabricante != juego2.fabricante) return false;
		if (this->anio != juego2.anio) return false;
		if (this->rom_de != juego2.rom_de) return false;
		if (this->clon_de != juego2.clon_de) return false;

		return true;
	}

	void clear() {
		anio.clear();
		clon_de.clear();
		descripcion.clear();
		fabricante.clear();
		nombre.clear();
		rom_de.clear();

		lista_roms.clear();
	}

	string nombre;
	string descripcion;
	string fabricante;
	string anio;
	string rom_de;
	string clon_de;

	vector<rom> lista_roms;

	friend class GuruMameListInfo;
};

class GuruMameListInfo
{
private:
	//	enum	entrada			{game , name, description, year, manufacturer, cloneof, romof, sampleof, rom, disk, biosset, sample, chip, video, input, dipswitch, driver, sound, resource, fin_game, fin_resource};
		enum	tipo_caracter	{error, fin_archivo, simbolo, abre_parentesis, cierra_parentesis, cadena};

public:
	GuruMameListInfo(const string& ruta_listinfo, bool cargar_a_memoria = true);
	~GuruMameListInfo(void);

	bool abrir();
	void cerrar();

	juego sacar_juego();
	vector<juego>& listinfo2list();
	map<string, juego>& listinfo2map();
	map<string, vector<rom> >& listinfo2rommap();

	class iterator;
	friend class iterator;

	iterator end() { return iterator(*this, true); }
	iterator begin() { return iterator(*this); }

	class iterator	{
	public:
/*begin*/iterator(GuruMameListInfo& MameListInfo):_listinfo(MameListInfo) {_juego=_listinfo.sacar_juego();token=_listinfo.token;};
/*end*/	 iterator(GuruMameListInfo& MameListInfo, bool):_listinfo(MameListInfo),token(fin_archivo) {};

		bool operator!=(const iterator& it2) const {
			if (this->token != it2.token) return true;
			else return false;
		}

		juego* operator->() const {
			return (juego*)&_juego;
		}

		juego& operator*() {
			return _juego;
		}

		void operator++() {
			_juego = _listinfo.sacar_juego();
			token  = _listinfo.token;
		}

	protected:
		GuruMameListInfo& _listinfo;
		enum GuruMameListInfo::tipo_caracter token;
		juego _juego;
	};

private:
	int	 sacar_caracter();
	enum GuruMameListInfo::tipo_caracter sacar_token();
	enum GuruMameListInfo::tipo_caracter sacar_cadena();
	enum GuruMameListInfo::tipo_caracter sacar_simbolo();
	void agregar_caracter(int caracter);
	void ignorar_seccion();
//	void armar_registro_de_simbolos(enum entrada tipo_entrada);

	bool cargar_a_memoria();

	bool	en_memoria;
	string	buffer;			// Almacena el token sacado del archivo
	char*	inicio_archivo;
	int		caracter;		// Almacena el caracter leido desde el archivo
	char*	archivo;
	string	_nombre_archivo_listinfo;
	ifstream _archivo_listinfo;
	enum tipo_caracter token;

	vector<juego> lista_juegos;
	map<string,juego> mapa_juegos;
	map<string, vector<rom> > mapa_roms;
};

#endif
