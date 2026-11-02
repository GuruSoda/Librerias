#ifndef __GURUETIQUETALEER_HPP__
#define __GURUETIQUETALEER_HPP__

#include <GuruCompat.h>

#include <map>
#include <string>
#include <vector>
#include <GuruString_algo.hpp>

using namespace std;

class GuruEtiquetaLeer
{
public:
	GuruEtiquetaLeer() {};
	GuruEtiquetaLeer(const string& registro):registro(registro) {};
	GuruEtiquetaLeer(const vector<string>& registro) {cargar(registro);};
	GuruEtiquetaLeer(int argc, char *argv[]) {cargar(argc, argv);};

	virtual ~GuruEtiquetaLeer() {};

	string operator [] (const string& campo_a_buscar) {

		caracter=registro.begin();

		enum tipo_caracter token = sacar_token();

		while (token != fin_cadena) {
			if (token == simbolo) {
				if (GuruStringAlgo::stringequals(buffer, campo_a_buscar)) {
					token = sacar_token();

					if (token == cadena || token == valor)
						return buffer;

					break;
				}
			}

			token = sacar_token();
		}

		return string("");
	}

	void	cargar(const string& registro) { this->registro = registro; }

	void	cargar(const vector<string>& nuevo_registro) {
		for (vector<string>::const_iterator it=nuevo_registro.begin();it!=nuevo_registro.end();++it)
			registro += *it + " ";
	}

	void	cargar(int argc, char* argv[]) {
		for (int i=1;i<argc;i++) {
			registro += argv[i];
			if ((i+1) != argc) registro += " ";
		}
	}

	string&	sacar_registro() {return registro;};

protected:
	enum tipo_caracter {simbolo, cadena, valor, fin_cadena, desconocido};

	enum	tipo_caracter sacar_token () {

		buffer.clear();

		while (caracter != registro.end() && isspace(*caracter))
			caracter++;

		if (caracter == registro.end())
			return fin_cadena;

		switch (*caracter) {
		case '\n': 
		case '\0': return fin_cadena;
		case ':' : 
		case '=' :
				   caracter++;

				   while (caracter != registro.end() && isspace(*caracter))
						caracter++;
					
				   if (caracter == registro.end())
						return fin_cadena;

				   switch (*caracter) {
				   case '\"': caracter++;
							  return sacar_cadena();
				   default:   return sacar_valor();
					}
		case '\"': caracter++;
				   return sacar_cadena();
		default  : return sacar_simbolo();
		}
	}

	enum	tipo_caracter sacar_cadena() {
		while (caracter != registro.end() && *caracter != '\"' && *caracter != '\n' && *caracter != '\0') {
			buffer += *caracter;

			caracter++;
		}

		if (caracter == registro.end())
			return cadena;

		switch (*caracter) {
		case '\"': caracter++;
		case '\n': 
		case '\0': return cadena;
		default:
				return desconocido;
		}
	}

	enum	tipo_caracter sacar_simbolo() {
		while (caracter != registro.end() && *caracter != '\n' && *caracter != '\0' && *caracter != ':' && *caracter != ' ' && *caracter != '=') {
			buffer += *caracter;

			caracter++;
		}

		if (caracter == registro.end())
			return fin_cadena;

		switch (*caracter) {
		case ':': 
		case '=': return simbolo;
		case ' ': return desconocido;
		case '\n': 
		case '\0': return fin_cadena;
		default:
				return desconocido;
		}
	}

	enum	tipo_caracter sacar_valor() {
		while (caracter != registro.end() && *caracter != ' ' && *caracter != '\n' && *caracter != '\0' ) {
			buffer += *caracter;

			caracter++;
		}

		return valor;
	}

	string	buffer;		// almacena lo sacado por el parser
	string	registro;	// almacena al cadena cruda (con los campos y valores "tag:pepe" o "tag=pepe")

	string::iterator caracter;
};

class GuruEtiquetaLeerEx: public GuruEtiquetaLeer {
public:
	GuruEtiquetaLeerEx() {};
	GuruEtiquetaLeerEx(const string& registro):GuruEtiquetaLeer(registro) {cargar_registro();};
	GuruEtiquetaLeerEx(const vector<string>& registro):GuruEtiquetaLeer(registro) {cargar_registro();};
	GuruEtiquetaLeerEx(int argc, char *argv[]):GuruEtiquetaLeer(argc, argv) {cargar_registro();};

	string operator [] (const string& etiqueta_a_buscar) {

		map<string, string, GuruStringAlgo::ci_less>::iterator it = eti_val.find(etiqueta_a_buscar);

		if (it == eti_val.end()) return string();

		return it->second;
	}

	void cargar(const string& etiqueta, const string& valor) {
		eti_val.insert(make_pair(etiqueta, valor));
	}

private:
	map<string, string, GuruStringAlgo::ci_less>& cargar_registro() {
		caracter=registro.begin();

		enum tipo_caracter token = sacar_token();

		while (token != fin_cadena) {
			if (token == simbolo) {

				string etiqueta(buffer);

				token = sacar_token();

				if (token == cadena || token == valor) {
					eti_val.insert(make_pair(etiqueta, buffer));
				}
			}

			token = sacar_token();
		}

		return eti_val;
	}

	map<string, string, GuruStringAlgo::ci_less> eti_val; // indice=etiquetas
};

#endif
