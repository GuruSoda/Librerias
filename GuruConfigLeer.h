#ifndef __GURUCONFIGLEER_H__
#define __GURUCONFIGLEER_H__

#include <GuruConfigV2.h>
#include <GuruAlgo.hpp>
#include <map>
#include <vector>

class GuruConfigLeer:public GuruConfig
{
public:
	GuruConfigLeer(const string& archivo_conf=""):GuruConfig(archivo_conf) {};
	virtual ~GuruConfigLeer(void) {};

	uint32_t total_secciones();

	vector<string>& secciones(vector<string>& listado);
	string	leer (const string& seccion, const string& entrada, const string& val_default="");
	string	leer_seccion(map<string, string>& simbolo_valor);
	map<string, string>& leer_seccion(const string& seccion, map<string, string>& simbolo_valor);

	class iterator {
	public:
		iterator(GuruConfigLeer* conf):conf(conf) {
			conf->abrir();
			nombre_seccion = conf->leer_seccion(simbolo_valor);
		};

		iterator(GuruConfigLeer* conf, enum tipo_caracter token):conf(conf), token_iterator(token) {
		};

		bool operator!=(const iterator it2) {
			if (conf->token != it2.token_iterator) return true;
			else return false;
		}

		void operator++() {
			nombre_seccion = conf->leer_seccion(simbolo_valor);
		}

		string& entrada(const string& simbolo, const string& val_default="") {
			map<string, string>::iterator it(simbolo_valor.find(simbolo));

			if (it != simbolo_valor.end())
				return it->second;
			else 
				return const_cast<string&>(val_default);
		}

		string& seccion() {
			return nombre_seccion;
		}

		map<string, string>& entradas() { return simbolo_valor; }

		virtual ~iterator(){};
	private:
		GuruConfigLeer*		conf;
		map<string, string>	simbolo_valor;
		enum tipo_caracter	token_iterator;
		string				nombre_seccion;
	};

	iterator begin() {return iterator(this);};
	iterator end() {return iterator(this, fin_archivo);};
};

#endif
